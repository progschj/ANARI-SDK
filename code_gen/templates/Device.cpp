// This file was generated by $generator from $template

#include "$prefixDevice.h"
#include "anari/backend/Library.h"
#include <cstdarg>
#include <cstdint>

// debug interface
#include "anari/ext/debug/DebugObject.h"

namespace anari {
namespace $namespace {

template <typename T>
void writeToVoidP(void *_p, T v)
{
  T *p = (T *)_p;
  *p = v;
}

int $prefixDevice::deviceImplements(const char *extension) {
  return 0;
}

void * $prefixDevice::mapArray(ANARIArray handle) {
  if(auto obj = fromHandle<ArrayObjectBase*>(handle)) {
    return obj->map();
  } else {
    return nullptr;
  }
}
void $prefixDevice::unmapArray(ANARIArray handle) {
  if(auto obj = fromHandle<ArrayObjectBase*>(handle)) {
    obj->unmap();
  }
}

anari::debug_device::ObjectFactory *getDebugFactory();

int $prefixDevice::getProperty(ANARIObject handle,
    const char *name,
    ANARIDataType type,
    void *mem,
    uint64_t size,
    ANARIWaitMask mask) {
  if(handle == this_device()) {
    if(std::strncmp(name, "debugObjects", 12) == 0 && type == ANARI_FUNCTION_POINTER) {
      writeToVoidP(mem, getDebugFactory);
      return 1;
    } else if(std::strncmp(name, "version", 7) == 0 && type == ANARI_INT32) {
      writeToVoidP<std::int32_t>(mem, 1); // TODO: set this to actual value
      return 1;
    } else if(std::strncmp(name, "geometryMaxIndex", 16) == 0 && type == ANARI_UINT64) {
      writeToVoidP<std::uint64_t>(mem, UINT32_MAX); // TODO: set this to actual value
      return 1;
    } else {
      return 0;
    }
  } else if(auto obj = fromHandle<ObjectBase*>(handle)) {
    return obj->getProperty(name, type, mem, size, mask);
  } else {
    return 0;
  }
}

void $prefixDevice::setParameter(ANARIObject handle,
    const char *name,
    ANARIDataType type,
    const void *mem) {
  if(handle == this_device()) {
    staging.set(name, type, mem);
  } else if(auto obj = fromHandle<ObjectBase*>(handle)) {
    obj->set(name, type, mem);
  }
}

void $prefixDevice::unsetParameter(ANARIObject handle, const char *name) {
  if(handle == this_device()) {
    staging.unset(name);
  } else if(auto obj = fromHandle<ObjectBase*>(handle)) {
    obj->unset(name);
  }
}

void $prefixDevice::commit(ANARIObject handle) {
  if(handle == this_device()) {
    current = staging;
    if(current.statusCallback.get(ANARI_STATUS_CALLBACK, &statusCallback)) {
      statusCallbackUserData = nullptr;
      current.statusCallbackUserData.get(ANARI_VOID_POINTER, &statusCallbackUserData);
    } else {
      statusCallback = defaultStatusCallback();
      statusCallbackUserData = defaultStatusCallbackUserPtr();
    }
  } else if(auto obj = fromHandle<ObjectBase*>(handle)) {
    obj->commit();
  }
}

void $prefixDevice::release(ANARIObject handle) {
  if(handle == this_device()) {
    if(refcount.fetch_sub(1) == 1) {
      delete this;
    }
  } else if(auto obj = fromHandle<ObjectBase*>(handle)) {
    obj->release();
  }
}
void $prefixDevice::retain(ANARIObject handle) {
  if(handle == this_device()) {
    refcount++;
  } else if(auto obj = fromHandle<ObjectBase*>(handle)) {
    obj->retain();
  }
}
void $prefixDevice::releaseInternal(ANARIObject handle) {
  if(auto obj = fromHandle<ObjectBase*>(handle)) {
    obj->releaseInternal();
  }
}
void $prefixDevice::retainInternal(ANARIObject handle) {
  if(auto obj = fromHandle<ObjectBase*>(handle)) {
    obj->retainInternal();
  }
}

const void* $prefixDevice::frameBufferMap(ANARIFrame handle, const char *channel) {
  if(auto obj = fromHandle<FrameObjectBase*>(handle)) {
    return obj->mapFrame(channel);
  } else {
    return 0;
  }
}

void $prefixDevice::frameBufferUnmap(ANARIFrame handle, const char *channel) {
if(auto obj = fromHandle<FrameObjectBase*>(handle)) {
    obj->unmapFrame(channel);
  }
}

void $prefixDevice::renderFrame(ANARIFrame handle) {
  if(auto obj = fromHandle<FrameObjectBase*>(handle)) {
    obj->renderFrame();
  }
}
int $prefixDevice::frameReady(ANARIFrame handle, ANARIWaitMask mask) {
  if(auto obj = fromHandle<FrameObjectBase*>(handle)) {
    return obj->frameReady(mask);
  } else {
    return 0;
  }
}
void $prefixDevice::discardFrame(ANARIFrame handle) {
  if(auto obj = fromHandle<FrameObjectBase*>(handle)) {
    obj->discardFrame();
  }
}

/////////////////////////////////////////////////////////////////////////////
// Helper/other functions and data members
/////////////////////////////////////////////////////////////////////////////

$prefixDevice::$prefixDevice() : refcount(1), staging(this_device()), current(this_device()) {
  objects.emplace_back(nullptr); // reserve the null index for the null handle
}
$prefixDevice::~$prefixDevice() {

}

// query functions
const char ** query_object_types(ANARIDataType type);
const ANARIParameter * query_params(ANARIDataType type, const char *subtype);

// internal "api" functions
void anariRetainInternal(ANARIDevice d, ANARIObject handle) {
  reinterpret_cast<$prefixDevice*>(d)->retainInternal(handle);
}
void anariReleaseInternal(ANARIDevice d, ANARIObject handle) {
  reinterpret_cast<$prefixDevice*>(d)->releaseInternal(handle);
}
void anariDeleteInternal(ANARIDevice d, ANARIObject handle) {
  reinterpret_cast<$prefixDevice*>(d)->deallocate(handle);
}
void anariReportStatus(ANARIDevice handle,
    ANARIObject source,
    ANARIDataType sourceType,
    ANARIStatusSeverity severity,
    ANARIStatusCode code,
    const char *format, ...) {

  if($prefixDevice *d = deviceHandle<$prefixDevice*>(handle)) {
    if(d->statusCallback) {
      va_list arglist;
      va_list arglist_copy;
      va_start(arglist, format);
      va_copy(arglist_copy, arglist);
      int count = std::vsnprintf(nullptr, 0, format, arglist);
      va_end(arglist);

      std::vector<char> formattedMessage(count+1);

      va_start(arglist_copy, format);
      std::vsnprintf(formattedMessage.data(), count+1, format, arglist_copy);
      va_end( arglist_copy );

      d->statusCallback(d->statusCallbackUserData, d->this_device(),
        source, sourceType, severity, code, formattedMessage.data());
    }
  }
}

} // namespace $namespace
} // namespace anari


static char deviceName[] = "$namespace";

extern "C" DEVICE_INTERFACE ANARI_DEFINE_LIBRARY_NEW_DEVICE(
    $namespace, subtype)
{
  if (subtype == std::string("default") || subtype == std::string("$namespace"))
    return (ANARIDevice) new anari::$namespace::$prefixDevice();
  return nullptr;
}

extern "C" DEVICE_INTERFACE ANARI_DEFINE_LIBRARY_INIT($namespace)
{

}

extern "C" DEVICE_INTERFACE ANARI_DEFINE_LIBRARY_GET_DEVICE_SUBTYPES(
    $namespace, libdata)
{
  static const char *devices[] = {deviceName, nullptr};
  return devices;
}

extern "C" DEVICE_INTERFACE ANARI_DEFINE_LIBRARY_GET_OBJECT_SUBTYPES(
    $namespace, libdata, deviceSubtype, objectType)
{
  return anari::$namespace::query_object_types(objectType);
}

extern "C" DEVICE_INTERFACE ANARI_DEFINE_LIBRARY_GET_OBJECT_PARAMETERS(
    $namespace, libdata, deviceSubtype, objectSubtype, objectType)
{
  return anari::$namespace::query_params(objectType, objectSubtype);
}

extern "C" DEVICE_INTERFACE ANARI_DEFINE_LIBRARY_GET_PARAMETER_PROPERTY(
    $namespace,
    libdata,
    deviceSubtype,
    objectSubtype,
    objectType,
    parameterName,
    parameterType,
    propertyName,
    propertyType)
{
  return nullptr;
}
