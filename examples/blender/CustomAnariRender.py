import bpy
import array
import gpu
import math
from gpu_extras.presets import draw_texture_2d
import numpy as np
from anari import *

prefixes = {
    lib.ANARI_SEVERITY_FATAL_ERROR : "FATAL",
    lib.ANARI_SEVERITY_ERROR : "ERROR",
    lib.ANARI_SEVERITY_WARNING : "WARNING",
    lib.ANARI_SEVERITY_PERFORMANCE_WARNING : "PERFORMANCE",
    lib.ANARI_SEVERITY_INFO : "INFO",
    lib.ANARI_SEVERITY_DEBUG : "DEBUG"
}

def anari_status(device, source, sourceType, severity, code, message):
    print('[%s]: '%prefixes[severity]+message)

status_handle = ffi.new_handle(anari_status) #something needs to keep this handle alive


class ANARIRenderEngine(bpy.types.RenderEngine):
    # These three members are used by blender to set up the
    # RenderEngine; define its internal name, visible name and capabilities.
    bl_idname = "ANARI"
    bl_label = "Anari"
    bl_use_preview = True
    bl_use_shading_nodes_custom=False

    # Init is called whenever a new render engine instance is created. Multiple
    # instances may exist at the same time, for example for a viewport and final
    # render.
    def __init__(self):
        dummy = gpu.types.GPUFrameBuffer()
        dummy.bind()

        self.scene_data = None
        self.draw_data = None

        self.debug = anariLoadLibrary('debug', status_handle)
        self.library = anariLoadLibrary('visgl', status_handle)

        self.nested = anariNewDevice(self.library, 'default')
        anariCommitParameters(self.nested, self.nested)

        self.device = anariNewDevice(self.debug, 'debug')
        anariSetParameter(self.device, self.device, 'wrappedDevice', ANARI_DEVICE, self.nested)
        #anariSetParameter(device, device, 'traceMode', ANARI_STRING, 'code')
        anariCommitParameters(self.device, self.device)

        self.frame = None
        self.camera = None
        self.perspective = anariNewCamera(self.device, 'perspective')
        self.ortho = anariNewCamera(self.device, 'orthographic')
        self.world = anariNewWorld(self.device)
        self.renderer = anariNewRenderer(self.device, 'default')
        anariCommitParameters(self.device, self.renderer)

        self.default_material = anariNewMaterial(self.device, 'matte')
        anariCommitParameters(self.device, self.default_material)

        self.meshes = dict()
        self.lights = dict()
        self.arrays = dict()
        self.images = dict()
        self.samplers = dict()

        self.scene_instances = []


    # When the render engine instance is destroyed, this is called. Clean up any
    # render engine data here, for example stopping running render threads.
    def __del__(self):
        pass

    def anari_frame(self, width=0, height=0):
        if not self.frame:
            self.frame = anariNewFrame(self.device)
            anariSetParameter(self.device, self.frame, 'channel.color', ANARI_DATA_TYPE, ANARI_UFIXED8_VEC4)

        if width != 0 and height != 0:
            anariSetParameter(self.device, self.frame, 'size', ANARI_UINT32_VEC2, [width, height])

        return self.frame

    scene_data = dict()


    def extract_camera(self, depsgraph, width, height, region_view=None, space_view=None):
        scene = depsgraph.scene

        camera = None
        transform = None
        fovx = 1
        zoom = 1

        if region_view:
            zoom = 4 / ((math.sqrt(2) + region_view.view_camera_zoom / 50) ** 2)
            if region_view.view_perspective == 'CAMERA':
                if space_view and space_view.use_local_camera:
                    camera = region_view.camera
                else:
                    camera = scene.objects['Camera']
        else:
            camera = scene.objects['Camera']

        
        if camera:
            transform = camera.matrix_world            
            if camera.data.type == "ORTHO":
                fovx = 0
                zoom = camera.data.ortho_scale
            else:
                fovx = camera.data.angle_x
                zoom = 1
        else:
            transform = region_view.view_matrix.inverted()
            if region_view.view_perspective == "ORTHO":
                fovx = 0
                zoom = 1.0275 * region_view.view_distance * 35 / space_view.lens
            else:
                fovx = 2.0 * math.atan(36 / space_view.lens)
                zoom = 1


        if fovx > 0:
            self.camera = self.perspective
            fovy = 2.0*math.atan(math.tan(0.5*fovx)/width*height*zoom)
            anariSetParameter(self.device, self.camera, 'fovy', ANARI_FLOAT32, fovy)
        else:
            self.camera = self.ortho
            anariSetParameter(self.device, self.camera, 'height', ANARI_FLOAT32, zoom/width*height)

            
        cam_transform = transform.transposed()
        cam_pos = cam_transform[3][0:3]
        cam_view = (-cam_transform[2])[0:3]
        cam_up = cam_transform[1][0:3]

        anariSetParameter(self.device, self.camera, 'aspect', ANARI_FLOAT32, width/height)
        anariSetParameter(self.device, self.camera, 'position', ANARI_FLOAT32_VEC3, cam_pos)
        anariSetParameter(self.device, self.camera, 'direction', ANARI_FLOAT32_VEC3, cam_view)
        anariSetParameter(self.device, self.camera, 'up', ANARI_FLOAT32_VEC3, cam_up)

        anariCommitParameters(self.device, self.camera)

        return self.camera

    def get_mesh(self, name):
        if name in self.meshes:
            return self.meshes[name]
        else:
            mesh = anariNewGeometry(self.device, 'triangle')
            surface = anariNewSurface(self.device)
            group = anariNewGroup(self.device)
            instance = anariNewInstance(self.device)
            material = self.default_material

            anariSetParameter(self.device, surface, 'geometry', ANARI_GEOMETRY, mesh)
            anariSetParameter(self.device, surface, 'material', ANARI_MATERIAL, material)
            anariCommitParameters(self.device, surface)

            surfaces = ffi.new('ANARISurface[]', [surface])
            array = anariNewArray1D(self.device, surfaces, ANARI_SURFACE, 1)
            anariSetParameter(self.device, group, 'surface', ANARI_ARRAY1D, array)
            anariCommitParameters(self.device, group)

            anariSetParameter(self.device, instance, 'group', ANARI_GROUP, group)
            anariCommitParameters(self.device, instance)

            result = (mesh, material, surface, group, instance)
            self.meshes[name] = result
            return result

    def get_light(self, name, subtype):
        if name in self.lights and self.lights[name][1] == subtype:
            return self.lights[name][0]
        else:
            light = anariNewLight(self.device, subtype)
            self.lights[name] = (light, subtype)
            return light


    def update_array(self, name, atype, data, components):
        elements = data.size//components
        if name in self.arrays:
            (array, oldtype, oldelements) = self.arrays[name]
            if atype == oldtype and elements == oldelements:
                # update in place
                ptr = anariMapArray(self.device, array)
                ffi.memmove(ptr, ffi.from_buffer(data), data.size*data.itemsize) 
                anariUnmapArray(self.device, array)
                return (array, False)

        array = anariNewArray1D(self.device, ffi.from_buffer(data), atype, elements)
        self.arrays[name] = (array, atype, elements)
        return (array, True)


    def mesh_to_geometry(self, objmesh, name):
        objmesh.calc_loop_triangles()
        objmesh.calc_normals_split()

        #flatten to triangle soup
        flat_vertices = []
        flat_normals = []
        flat_uvs = []
        flat_colors = []
        for i, tri in enumerate(objmesh.loop_triangles):
            for j in range(0, 3):
                idx = tri.vertices[j]
                loop = tri.loops[j]
                flat_vertices += objmesh.vertices[idx].co[:]
                flat_normals += objmesh.loops[loop].normal[:]
                if objmesh.uv_layers.active:
                    flat_uvs += objmesh.uv_layers.active.uv[loop].vector[:]
                if objmesh.color_attributes:
                    flat_colors += objmesh.color_attributes[0].data[loop].color[:]

        vertices = np.array(flat_vertices, dtype=np.float32)
        normals = np.array(flat_normals, dtype=np.float32)
        uvs = np.array(flat_uvs, dtype=np.float32)
        colors = np.array(flat_colors, dtype=np.float32)

        (mesh, material, surface, group, instance) = self.get_mesh(name)

        require_commit = False

        (array, replaced) = self.update_array(name+'_vertex.position', ANARI_FLOAT32_VEC3, vertices, 3)
        if replaced:
            anariSetParameter(self.device, mesh, 'vertex.position', ANARI_ARRAY1D, array)
            require_commit = True

        (array, replaced) = self.update_array(name+'_vertex.normal', ANARI_FLOAT32_VEC3, normals, 3)
        if replaced:
            anariSetParameter(self.device, mesh, 'vertex.normal', ANARI_ARRAY1D, array)
            require_commit = True

        if flat_uvs:
            (array, replaced) = self.update_array(name+'_vertex.attribute0', ANARI_FLOAT32_VEC2, uvs, 2)
            if replaced:
                anariSetParameter(self.device, mesh, 'vertex.attribute0', ANARI_ARRAY1D, array)
                require_commit = True

        if flat_colors:
            (array, replaced) = self.update_array(name+'_vertex.color', ANARI_FLOAT32_VEC4, colors, 4)
            if replaced:
                anariSetParameter(self.device, mesh, 'vertex.color', ANARI_ARRAY1D, array)
                require_commit = True

        if require_commit:
            anariCommitParameters(self.device, mesh)

        return (mesh, material, surface, group, instance)


    def image_handle(self, image):
        if image in self.images:
            return self.images[image]
        else:
            image.update()
            if image.has_data:
                atype = [ANARI_FLOAT32, ANARI_FLOAT32_VEC2, ANARI_FLOAT32_VEC3, ANARI_FLOAT32_VEC4][image.channels-1]
                pixbuf = np.array(image.pixels, dtype=np.float32)
                pixels = anariNewArray2D(self.device, ffi.from_buffer(pixbuf), atype, image.size[0], image.size[1])
                self.images[image] = pixels
                return pixels
            else:
                return None

    def sampler_handle(self, material, paramname):
        key = (material, paramname)
        if key in self.samplers:
            return self.samplers[key]
        else:
            sampler = anariNewSampler(self.device, "image2D")
            self.samplers[key] = sampler
            return sampler

    def parse_source_node(self, material, paramname, atype, input):
        if input.links:
            link = input.links[0]
            node = link.from_node
            if node.type == 'VERTEX_COLOR':
                anariSetParameter(self.device, material, paramname, ANARI_STRING, 'color')
                return
            elif node.type == 'TEX_IMAGE' and node.image:
                image = node.image
                pixels = self.image_handle(image)
                if pixels:
                    sampler = self.sampler_handle(material, paramname)
                    anariSetParameter(self.device, sampler, 'image', ANARI_ARRAY2D, pixels)
                    anariSetParameter(self.device, sampler, 'inAttribute', ANARI_STRING, "attribute0")
                    if link.from_socket.name == "Alpha":
                        #swizzle alpha into first position
                        anariSetParameter(self.device, sampler, 'outTransform', ANARI_FLOAT32_MAT4, [0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0])
                        
                    # todo set filter and repeat modes here
                    anariCommitParameters(self.device, sampler)
                    anariSetParameter(self.device, material, paramname, ANARI_SAMPLER, sampler)
                    return
                else:
                    print("no pixels")

        # use the default value if none of the previous ones worked
        if atype == ANARI_FLOAT32:
            anariSetParameter(self.device, material, paramname, atype, input.default_value)
        elif atype == ANARI_FLOAT32_VEC2:
            anariSetParameter(self.device, material, paramname, atype, input.default_value[:2])
        elif atype == ANARI_FLOAT32_VEC3:
            anariSetParameter(self.device, material, paramname, atype, input.default_value[:3])
        elif atype == ANARI_FLOAT32_VEC4:
            anariSetParameter(self.device, material, paramname, atype, input.default_value[:4])
        else:
            anariSetParameter(self.device, material, paramname, atype, input.default_value[:])

    def parse_material(self, material):
        nodes = material.node_tree.nodes

        #look for an output node
        out = None
        for n in nodes:
            if n.type == 'OUTPUT_MATERIAL':
                out = n

        if not out:
            return self.default_material

        shader = out.inputs['Surface'].links[0].from_node
        if shader.type == 'BSDF_PRINCIPLED':
            material = anariNewMaterial(self.device, 'physicallyBased')
            self.parse_source_node(material, 'baseColor', ANARI_FLOAT32_VEC3, shader.inputs['Base Color'])
            anariSetParameter(self.device, material, "alphaMode", ANARI_STRING, "opaque")
            self.parse_source_node(material, 'opacity', ANARI_FLOAT32, shader.inputs['Alpha'])
            self.parse_source_node(material, 'metallic', ANARI_FLOAT32, shader.inputs['Metallic'])
            self.parse_source_node(material, 'roughness', ANARI_FLOAT32, shader.inputs['Roughness'])
            anariCommitParameters(self.device, material)
            return material


    def read_scene(self, depsgraph):

        bg_color = depsgraph.scene.world.color[:]+(1.0,)
        anariSetParameter(self.device, self.renderer, 'background', ANARI_FLOAT32_VEC4, bg_color)
        anariCommitParameters(self.device, self.renderer)

        scene_instances = []

        #for update in depsgraph.updates:
        #    obj = update.id
        for obj in depsgraph.objects:
            objmesh = None
            if obj.type in {'MESH', 'CURVE', 'SURFACE', 'FONT', 'META'}:
                objmesh = obj.to_mesh()
            else:
                continue
                
            name = obj.name
            (mesh, material, surface, group, instance) = self.mesh_to_geometry(objmesh, name)

            objmaterial = self.default_material
            if obj.material_slots:
                objmaterial = self.parse_material(obj.material_slots[0].material)

            if objmaterial != material:
                material = objmaterial
                anariSetParameter(self.device, surface, 'material', ANARI_MATERIAL, material)
                anariCommitParameters(self.device, surface)
                self.meshes[name] = (mesh, material, surface, group, instance)


            #flatten the matrix
            transform = [x for v in obj.matrix_world.transposed() for x in v]
            anariSetParameter(self.device, instance, 'transform', ANARI_FLOAT32_MAT4, transform)
            anariCommitParameters(self.device, instance)

            scene_instances.append(instance)

        instances = ffi.new('ANARIInstance[]', scene_instances)
        array = anariNewArray1D(self.device, instances, ANARI_SURFACE, len(scene_instances))
        anariSetParameter(self.device, self.world, 'instance', ANARI_ARRAY1D, array)

        scene_lights = []
        for key, obj in depsgraph.objects.items():
            if obj.hide_render or obj.type != 'LIGHT':
                continue

            name = obj.name

            transform = obj.matrix_world.transposed()
            direction = (-transform[2])[0:3]
            position = transform[3][0:3]

            if obj.data.type == 'POINT':
                light = self.get_light(name, 'point')
                anariSetParameter(self.device, light, 'color', ANARI_FLOAT32_VEC3, obj.data.color[:])
                #blender uses total watts while anari uses watts/sr
                anariSetParameter(self.device, light, 'intensity', ANARI_FLOAT32, obj.data.energy/(4.0*math.pi))
                anariSetParameter(self.device, light, 'position', ANARI_FLOAT32_VEC3, position)
                anariCommitParameters(self.device, light)
                scene_lights.append(light)
            elif obj.data.type == 'SUN':
                light = self.get_light(name, 'directional')
                anariSetParameter(self.device, light, 'color', ANARI_FLOAT32_VEC3, obj.data.color[:])
                anariSetParameter(self.device, light, 'irradiance', ANARI_FLOAT32, obj.data.energy)
                anariSetParameter(self.device, light, 'direction', ANARI_FLOAT32_VEC3, direction)
                anariCommitParameters(self.device, light)
                scene_lights.append(light)


        lights = ffi.new('ANARILight[]', scene_lights)
        array = anariNewArray1D(self.device, lights, ANARI_LIGHT, len(scene_lights))
        anariSetParameter(self.device, self.world, 'light', ANARI_ARRAY1D, array)

        anariCommitParameters(self.device, self.world)


    # This is the method called by Blender for both final renders (F12) and
    # small preview for materials, world and lights.
    def render(self, depsgraph):
        scene = depsgraph.scene
        scale = scene.render.resolution_percentage / 100.0
        width = int(scene.render.resolution_x * scale)
        height = int(scene.render.resolution_y * scale)

        self.extract_camera(depsgraph, width, height)

        bg_color = depsgraph.scene.world.color[:]+(1.0,)
        anariSetParameter(self.device, self.renderer, 'background', ANARI_FLOAT32_VEC4, bg_color)
        anariCommitParameters(self.device, self.renderer)

        self.read_scene(depsgraph)


        frame = self.anari_frame(width, height)
        anariSetParameter(self.device, frame, 'renderer', ANARI_RENDERER, self.renderer)
        anariSetParameter(self.device, frame, 'camera', ANARI_CAMERA, self.camera)
        anariSetParameter(self.device, frame, 'world', ANARI_WORLD, self.world)
        anariCommitParameters(self.device, frame)

        anariRenderFrame(self.device, frame)
        anariFrameReady(self.device, frame, ANARI_WAIT)
        void_pixels, frame_width, frame_height, frame_type = anariMapFrame(self.device, frame, 'channel.color')

        unpacked_pixels = ffi.buffer(void_pixels, frame_width*frame_height*4)
        pixels = np.array(unpacked_pixels).astype(np.float32)*(1.0/255.0)
        rect = pixels.reshape((width*height, 4))
        anariUnmapFrame(self.device, frame, 'channel.color')

        # Here we write the pixel values to the RenderResult
        result = self.begin_result(0, 0, width, height)
        layer = result.layers[0].passes["Combined"]
        layer.rect = rect
        self.end_result(result)



    # For viewport renders, this method gets called once at the start and
    # whenever the scene or 3D viewport changes. This method is where data
    # should be read from Blender in the same thread. Typically a render
    # thread will be started to do the work while keeping Blender responsive.
    def view_update(self, context, depsgraph):
        region = context.region
        scene = depsgraph.scene

        width = region.width
        height = region.height

        self.read_scene(depsgraph)

        frame = self.anari_frame(width, height)
        self.extract_camera(depsgraph, width, height, region_view=context.region_data, space_view=context.space_data)

        anariSetParameter(self.device, frame, 'renderer', ANARI_RENDERER, self.renderer)
        anariSetParameter(self.device, frame, 'camera', ANARI_CAMERA, self.camera)
        anariSetParameter(self.device, frame, 'world', ANARI_WORLD, self.world)
        anariCommitParameters(self.device, frame)



    # For viewport renders, this method is called whenever Blender redraws
    # the 3D viewport. The renderer is expected to quickly draw the render
    # with OpenGL, and not perform other expensive work.
    # Blender will draw overlays for selection and editing on top of the
    # rendered image automatically.
    def view_draw(self, context, depsgraph):
        region = context.region
        scene = depsgraph.scene

        width = region.width
        height = region.height


        self.extract_camera(depsgraph, width, height, region_view=context.region_data, space_view=context.space_data)

        frame = self.anari_frame(width, height)
        anariSetParameter(self.device, frame, 'camera', ANARI_CAMERA, self.camera)
        anariCommitParameters(self.device, frame)


        anariRenderFrame(self.device, frame)
        anariFrameReady(self.device, frame, ANARI_WAIT)
        void_pixels, frame_width, frame_height, frame_type = anariMapFrame(self.device, frame, 'channel.color')

        unpacked_pixels = ffi.buffer(void_pixels, frame_width*frame_height*4)
        pixels = np.array(unpacked_pixels).astype(np.float32)*(1.0/255.0)
        rect = pixels.reshape((width*height, 4))
        anariUnmapFrame(self.device, frame, 'channel.color')

        self.gpupixels = gpu.types.Buffer('FLOAT', width * height * 4, pixels)
        self.gputexture = gpu.types.GPUTexture((width, height), format='RGBA16F', data=self.gpupixels)
        # Bind shader that converts from scene linear to display space,
        gpu.state.blend_set('ALPHA_PREMULT')
        self.bind_display_space_shader(scene)

        draw_texture_2d(self.gputexture, (0, 0), self.gputexture.width, self.gputexture.height)

        self.unbind_display_space_shader()
        gpu.state.blend_set('NONE')


# RenderEngines also need to tell UI Panels that they are compatible with.
# We recommend to enable all panels marked as BLENDER_RENDER, and then
# exclude any panels that are replaced by custom panels registered by the
# render engine, or that are not supported.
def get_panels():
    exclude_panels = {
        'VIEWLAYER_PT_filter',
        'VIEWLAYER_PT_layer_passes',
    }

    panels = []
    for panel in bpy.types.Panel.__subclasses__():
        if hasattr(panel, 'COMPAT_ENGINES') and ('BLENDER_RENDER' in panel.COMPAT_ENGINES or 'BLENDER_EEVEE' in panel.COMPAT_ENGINES):
            if panel.__name__ not in exclude_panels:
                panels.append(panel)

    return panels


def register():
    # Register the RenderEngine
    bpy.utils.register_class(ANARIRenderEngine)

    for panel in get_panels():
        panel.COMPAT_ENGINES.add('ANARI')


def unregister():
    bpy.utils.unregister_class(ANARIRenderEngine)

    for panel in get_panels():
        if 'ANARI' in panel.COMPAT_ENGINES:
            panel.COMPAT_ENGINES.remove('ANARI')


if __name__ == "__main__":
    register()
