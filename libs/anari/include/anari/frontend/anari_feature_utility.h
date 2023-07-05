// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

// This file was generated by generate_feature_utility.py
// Don't make changes to this directly

#pragma once
#include <anari/anari.h>
typedef struct {
   int ANARI_KHR_CAMERA_OMNIDIRECTIONAL;
   int ANARI_KHR_CAMERA_ORTHOGRAPHIC;
   int ANARI_KHR_CAMERA_PERSPECTIVE;
   int ANARI_KHR_GEOMETRY_CONE;
   int ANARI_KHR_GEOMETRY_CURVE;
   int ANARI_KHR_GEOMETRY_CYLINDER;
   int ANARI_KHR_GEOMETRY_QUAD;
   int ANARI_KHR_GEOMETRY_SPHERE;
   int ANARI_KHR_GEOMETRY_TRIANGLE;
   int ANARI_KHR_LIGHT_DIRECTIONAL;
   int ANARI_KHR_LIGHT_POINT;
   int ANARI_KHR_LIGHT_SPOT;
   int ANARI_KHR_MATERIAL_MATTE;
   int ANARI_KHR_MATERIAL_TRANSPARENT_MATTE;
   int ANARI_KHR_SAMPLER_IMAGE1D;
   int ANARI_KHR_SAMPLER_IMAGE2D;
   int ANARI_KHR_SAMPLER_IMAGE3D;
   int ANARI_KHR_SAMPLER_PRIMITIVE;
   int ANARI_KHR_SAMPLER_TRANSFORM;
   int ANARI_KHR_SPATIAL_FIELD_STRUCTURED_REGULAR;
   int ANARI_KHR_VOLUME_TRANSFER_FUNCTION1D;
   int ANARI_KHR_LIGHT_RING;
   int ANARI_KHR_LIGHT_QUAD;
   int ANARI_KHR_LIGHT_HDRI;
   int ANARI_KHR_MATERIAL_PHYSICALLY_BASED;
   int ANARI_KHR_FRAME_CONTINUATION;
   int ANARI_KHR_AUXILIARY_BUFFERS;
   int ANARI_KHR_AREA_LIGHTS;
   int ANARI_KHR_STOCHASTIC_RENDERING;
   int ANARI_KHR_TRANSFORMATION_MOTION_BLUR;
   int ANARI_KHR_ARRAY1D_REGION;
   int ANARI_KHR_RENDERER_AMBIENT_LIGHT;
   int ANARI_KHR_RENDERER_BACKGROUND_COLOR;
   int ANARI_KHR_RENDERER_BACKGROUND_IMAGE;
   int ANARI_EXP_VOLUME_SAMPLE_RATE;
} ANARIFeatures;
int anariGetDeviceFeatureStruct(ANARIFeatures *features, ANARILibrary library, const char *deviceName);
int anariGetObjectFeatureStruct(ANARIFeatures *features, ANARIDevice device, const char *objectName, ANARIDataType objectType);
int anariGetInstanceFeatureStruct(ANARIFeatures *features, ANARIDevice device, ANARIObject object);
#ifdef ANARI_FEATURE_UTILITY_IMPL
#include <string.h>
static int feature_hash(const char *str) {
   static const uint32_t table[] = {0x4f4e0001u,0x42410002u,0x53520003u,0x4a490004u,0x605f0005u,0x4c450006u,0x5958000du,0x0u,0x0u,0x0u,0x0u,0x0u,0x49480023u,0x5150000eu,0x605f000fu,0x57560010u,0x504f0011u,0x4d4c0012u,0x56550013u,0x4e4d0014u,0x46450015u,0x605f0016u,0x54530017u,0x42410018u,0x4e4d0019u,0x5150001au,0x4d4c001bu,0x4645001cu,0x605f001du,0x5352001eu,0x4241001fu,0x55540020u,0x46450021u,0x1000022u,0x80000022u,0x53520024u,0x605f0025u,0x57410026u,0x5652003cu,0x0u,0x42410073u,0x0u,0x0u,0x535200a5u,0x464500b7u,0x0u,0x0u,0x0u,0x0u,0x4a4900fcu,0x42410131u,0x0u,0x0u,0x0u,0x0u,0x46450167u,0x55410199u,0x53520210u,0x0u,0x504f022au,0x53450040u,0x0u,0x0u,0x59580063u,0x4241004eu,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x42410057u,0x605f004fu,0x4d4c0050u,0x4a490051u,0x48470052u,0x49480053u,0x55540054u,0x54530055u,0x1000056u,0x8000001bu,0x5a590058u,0x32310059u,0x4544005au,0x605f005bu,0x5352005cu,0x4645005du,0x4847005eu,0x4a49005fu,0x504f0060u,0x4f4e0061u,0x1000062u,0x8000001eu,0x4a490064u,0x4d4c0065u,0x4a490066u,0x42410067u,0x53520068u,0x5a590069u,0x605f006au,0x4342006bu,0x5655006cu,0x4746006du,0x4746006eu,0x4645006fu,0x53520070u,0x54530071u,0x1000072u,0x8000001au,0x4e4d0074u,0x46450075u,0x53520076u,0x42410077u,0x605f0078u,0x514f0079u,0x534d007bu,0x4645009au,0x4f4e0081u,0x0u,0x0u,0x0u,0x0u,0x5554008fu,0x4a490082u,0x45440083u,0x4a490084u,0x53520085u,0x46450086u,0x44430087u,0x55540088u,0x4a490089u,0x504f008au,0x4f4e008bu,0x4241008cu,0x4d4c008du,0x100008eu,0x80000000u,0x49480090u,0x504f0091u,0x48470092u,0x53520093u,0x42410094u,0x51500095u,0x49480096u,0x4a490097u,0x44430098u,0x1000099u,0x80000001u,0x5352009bu,0x5453009cu,0x5150009du,0x4645009eu,0x4443009fu,0x555400a0u,0x4a4900a1u,0x575600a2u,0x464500a3u,0x10000a4u,0x80000002u,0x424100a6u,0x4e4d00a7u,0x464500a8u,0x605f00a9u,0x444300aau,0x504f00abu,0x4f4e00acu,0x555400adu,0x4a4900aeu,0x4f4e00afu,0x565500b0u,0x424100b1u,0x555400b2u,0x4a4900b3u,0x504f00b4u,0x4f4e00b5u,0x10000b6u,0x80000019u,0x504f00b8u,0x4e4d00b9u,0x464500bau,0x555400bbu,0x535200bcu,0x5a5900bdu,0x605f00beu,0x554300bfu,0x5a4f00d1u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x565500eau,0x0u,0x515000eeu,0x535200f4u,0x4f4e00dcu,0x0u,0x0u,0x0u,0x0u,0x0u,0x535200dfu,0x0u,0x0u,0x0u,0x4d4c00e3u,0x464500ddu,0x10000deu,0x80000003u,0x575600e0u,0x464500e1u,0x10000e2u,0x80000004u,0x4a4900e4u,0x4f4e00e5u,0x454400e6u,0x464500e7u,0x535200e8u,0x10000e9u,0x80000005u,0x424100ebu,0x454400ecu,0x10000edu,0x80000006u,0x494800efu,0x464500f0u,0x535200f1u,0x464500f2u,0x10000f3u,0x80000007u,0x4a4900f5u,0x424100f6u,0x4f4e00f7u,0x484700f8u,0x4d4c00f9u,0x464500fau,0x10000fbu,0x80000008u,0x484700fdu,0x494800feu,0x555400ffu,0x605f0100u,0x54440101u,0x4a490111u,0x0u,0x0u,0x0u,0x4544011cu,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x504f0120u,0x56550125u,0x4a490129u,0x5150012du,0x53520112u,0x46450113u,0x44430114u,0x55540115u,0x4a490116u,0x504f0117u,0x4f4e0118u,0x42410119u,0x4d4c011au,0x100011bu,0x80000009u,0x5352011du,0x4a49011eu,0x100011fu,0x80000017u,0x4a490121u,0x4f4e0122u,0x55540123u,0x1000124u,0x8000000au,0x42410126u,0x45440127u,0x1000128u,0x80000016u,0x4f4e012au,0x4847012bu,0x100012cu,0x80000015u,0x504f012eu,0x5554012fu,0x1000130u,0x8000000bu,0x55540132u,0x46450133u,0x53520134u,0x4a490135u,0x42410136u,0x4d4c0137u,0x605f0138u,0x554d0139u,0x42410141u,0x0u,0x0u,0x49480146u,0x0u,0x0u,0x0u,0x53520156u,0x55540142u,0x55540143u,0x46450144u,0x1000145u,0x8000000cu,0x5a590147u,0x54530148u,0x4a490149u,0x4443014au,0x4241014bu,0x4d4c014cu,0x4d4c014du,0x5a59014eu,0x605f014fu,0x43420150u,0x42410151u,0x54530152u,0x46450153u,0x45440154u,0x1000155u,0x80000018u,0x42410157u,0x4f4e0158u,0x54530159u,0x5150015au,0x4241015bu,0x5352015cu,0x4645015du,0x4f4e015eu,0x5554015fu,0x605f0160u,0x4e4d0161u,0x42410162u,0x55540163u,0x55540164u,0x46450165u,0x1000166u,0x8000000du,0x4f4e0168u,0x45440169u,0x4645016au,0x5352016bu,0x4645016cu,0x5352016du,0x605f016eu,0x4341016fu,0x4e4d0171u,0x4241017eu,0x43420172u,0x4a490173u,0x46450174u,0x4f4e0175u,0x55540176u,0x605f0177u,0x4d4c0178u,0x4a490179u,0x4847017au,0x4948017bu,0x5554017cu,0x100017du,0x8000001fu,0x4443017fu,0x4c4b0180u,0x48470181u,0x53520182u,0x504f0183u,0x56550184u,0x4f4e0185u,0x45440186u,0x605f0187u,0x4a430188u,0x504f018fu,0x0u,0x0u,0x0u,0x0u,0x0u,0x4e4d0194u,0x4d4c0190u,0x504f0191u,0x53520192u,0x1000193u,0x80000020u,0x42410195u,0x48470196u,0x46450197u,0x1000198u,0x80000021u,0x4e4d01adu,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x424101deu,0x0u,0x0u,0x0u,0x504f01fdu,0x515001aeu,0x4d4c01afu,0x464501b0u,0x535201b1u,0x605f01b2u,0x554901b3u,0x4e4d01bfu,0x0u,0x0u,0x0u,0x0u,0x0u,0x0u,0x535201ccu,0x0u,0x0u,0x0u,0x535201d5u,0x424101c0u,0x484701c1u,0x464501c2u,0x343101c3u,0x454401c6u,0x454401c8u,0x454401cau,0x10001c7u,0x8000000eu,0x10001c9u,0x8000000fu,0x10001cbu,0x80000010u,0x4a4901cdu,0x4e4d01ceu,0x4a4901cfu,0x555401d0u,0x4a4901d1u,0x575601d2u,0x464501d3u,0x10001d4u,0x80000011u,0x424101d6u,0x4f4e01d7u,0x545301d8u,0x474601d9u,0x504f01dau,0x535201dbu,0x4e4d01dcu,0x10001ddu,0x80000012u,0x555401dfu,0x4a4901e0u,0x424101e1u,0x4d4c01e2u,0x605f01e3u,0x474601e4u,0x4a4901e5u,0x464501e6u,0x4d4c01e7u,0x454401e8u,0x605f01e9u,0x545301eau,0x555401ebu,0x535201ecu,0x565501edu,0x444301eeu,0x555401efu,0x565501f0u,0x535201f1u,0x464501f2u,0x454401f3u,0x605f01f4u,0x535201f5u,0x464501f6u,0x484701f7u,0x565501f8u,0x4d4c01f9u,0x424101fau,0x535201fbu,0x10001fcu,0x80000013u,0x444301feu,0x494801ffu,0x42410200u,0x54530201u,0x55540202u,0x4a490203u,0x44430204u,0x605f0205u,0x53520206u,0x46450207u,0x4f4e0208u,0x45440209u,0x4645020au,0x5352020bu,0x4a49020cu,0x4f4e020du,0x4847020eu,0x100020fu,0x8000001cu,0x42410211u,0x4f4e0212u,0x54530213u,0x47460214u,0x504f0215u,0x53520216u,0x4e4d0217u,0x42410218u,0x55540219u,0x4a49021au,0x504f021bu,0x4f4e021cu,0x605f021du,0x4e4d021eu,0x504f021fu,0x55540220u,0x4a490221u,0x504f0222u,0x4f4e0223u,0x605f0224u,0x43420225u,0x4d4c0226u,0x56550227u,0x53520228u,0x1000229u,0x8000001du,0x4d4c022bu,0x5655022cu,0x4e4d022du,0x4645022eu,0x605f022fu,0x55540230u,0x53520231u,0x42410232u,0x4f4e0233u,0x54530234u,0x47460235u,0x46450236u,0x53520237u,0x605f0238u,0x47460239u,0x5655023au,0x4f4e023bu,0x4443023cu,0x5554023du,0x4a49023eu,0x504f023fu,0x4f4e0240u,0x32310241u,0x45440242u,0x1000243u,0x80000014u};
   uint32_t cur = 0x42410000u;
   for(int i = 0;cur!=0;++i) {
      uint32_t idx = cur&0xFFFFu;
      uint32_t low = (cur>>16u)&0xFFu;
      uint32_t high = (cur>>24u)&0xFFu;
      uint32_t c = (uint32_t)str[i];
      if(c>=low && c<high) {
         cur = table[idx+c-low];
      } else {
         break;
      }
      if(cur&0x80000000u) {
         return cur&0xFFFFu;
      }
      if(str[i]==0) {
         break;
      }
   }
   return -1;
}
static void fillFeatureStruct(ANARIFeatures *features, const char *const *list) {
    memset(features, 0, sizeof(ANARIFeatures));
    for(const char *const *i = list;*i!=NULL;++i) {
        switch(feature_hash(*i)) {
            case 0: features->ANARI_KHR_CAMERA_OMNIDIRECTIONAL = 1; break;
            case 1: features->ANARI_KHR_CAMERA_ORTHOGRAPHIC = 1; break;
            case 2: features->ANARI_KHR_CAMERA_PERSPECTIVE = 1; break;
            case 3: features->ANARI_KHR_GEOMETRY_CONE = 1; break;
            case 4: features->ANARI_KHR_GEOMETRY_CURVE = 1; break;
            case 5: features->ANARI_KHR_GEOMETRY_CYLINDER = 1; break;
            case 6: features->ANARI_KHR_GEOMETRY_QUAD = 1; break;
            case 7: features->ANARI_KHR_GEOMETRY_SPHERE = 1; break;
            case 8: features->ANARI_KHR_GEOMETRY_TRIANGLE = 1; break;
            case 9: features->ANARI_KHR_LIGHT_DIRECTIONAL = 1; break;
            case 10: features->ANARI_KHR_LIGHT_POINT = 1; break;
            case 11: features->ANARI_KHR_LIGHT_SPOT = 1; break;
            case 12: features->ANARI_KHR_MATERIAL_MATTE = 1; break;
            case 13: features->ANARI_KHR_MATERIAL_TRANSPARENT_MATTE = 1; break;
            case 14: features->ANARI_KHR_SAMPLER_IMAGE1D = 1; break;
            case 15: features->ANARI_KHR_SAMPLER_IMAGE2D = 1; break;
            case 16: features->ANARI_KHR_SAMPLER_IMAGE3D = 1; break;
            case 17: features->ANARI_KHR_SAMPLER_PRIMITIVE = 1; break;
            case 18: features->ANARI_KHR_SAMPLER_TRANSFORM = 1; break;
            case 19: features->ANARI_KHR_SPATIAL_FIELD_STRUCTURED_REGULAR = 1; break;
            case 20: features->ANARI_KHR_VOLUME_TRANSFER_FUNCTION1D = 1; break;
            case 21: features->ANARI_KHR_LIGHT_RING = 1; break;
            case 22: features->ANARI_KHR_LIGHT_QUAD = 1; break;
            case 23: features->ANARI_KHR_LIGHT_HDRI = 1; break;
            case 24: features->ANARI_KHR_MATERIAL_PHYSICALLY_BASED = 1; break;
            case 25: features->ANARI_KHR_FRAME_CONTINUATION = 1; break;
            case 26: features->ANARI_KHR_AUXILIARY_BUFFERS = 1; break;
            case 27: features->ANARI_KHR_AREA_LIGHTS = 1; break;
            case 28: features->ANARI_KHR_STOCHASTIC_RENDERING = 1; break;
            case 29: features->ANARI_KHR_TRANSFORMATION_MOTION_BLUR = 1; break;
            case 30: features->ANARI_KHR_ARRAY1D_REGION = 1; break;
            case 31: features->ANARI_KHR_RENDERER_AMBIENT_LIGHT = 1; break;
            case 32: features->ANARI_KHR_RENDERER_BACKGROUND_COLOR = 1; break;
            case 33: features->ANARI_KHR_RENDERER_BACKGROUND_IMAGE = 1; break;
            case 34: features->ANARI_EXP_VOLUME_SAMPLE_RATE = 1; break;
            default: break;
        }
    }
}
int anariGetDeviceFeatureStruct(ANARIFeatures *features, ANARILibrary library, const char *deviceName) {
    const char *const *list = (const char *const *)anariGetDeviceFeatures(library, deviceName);
    if(list) {
        fillFeatureStruct(features, list);
        return 0;
    } else {
        return 1;
    }
}
int anariGetObjectFeatureStruct(ANARIFeatures *features, ANARIDevice device, const char *objectName, ANARIDataType objectType) {
    const char *const *list = (const char *const *)anariGetObjectInfo(device, objectName, objectType, "feature", ANARI_STRING_LIST);
    if(list) {
        fillFeatureStruct(features, list);
        return 0;
    } else {
        return 1;
    }
}
int anariGetInstanceFeatureStruct(ANARIFeatures *features, ANARIDevice device, ANARIObject object) {
    const char *const *list = NULL;
    anariGetProperty(device, object, "feature", ANARI_STRING_LIST, &list, sizeof(list), ANARI_WAIT);
    if(list) {
        fillFeatureStruct(features, list);
        return 0;
    } else {
        return 1;
    }
}
#endif
