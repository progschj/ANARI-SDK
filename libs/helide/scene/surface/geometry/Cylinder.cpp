// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Cylinder.h"
// std
#include <numeric>

namespace helide {

Cylinder::Cylinder(HelideGlobalState *s) : Geometry(s)
{
  m_embreeGeometry =
      rtcNewGeometry(s->embreeDevice, RTC_GEOMETRY_TYPE_CONE_LINEAR_CURVE);
}

void Cylinder::commit()
{
  Geometry::commit();

  cleanup();

  m_index = getParamObject<Array1D>("primitive.index");
  m_radius = getParamObject<Array1D>("primitive.radius");
  m_vertexPosition = getParamObject<Array1D>("vertex.position");
  m_vertexAttributes[0] = getParamObject<Array1D>("vertex.attribute0");
  m_vertexAttributes[1] = getParamObject<Array1D>("vertex.attribute1");
  m_vertexAttributes[2] = getParamObject<Array1D>("vertex.attribute2");
  m_vertexAttributes[3] = getParamObject<Array1D>("vertex.attribute3");
  m_vertexAttributes[4] = getParamObject<Array1D>("vertex.color");

  if (!m_vertexPosition) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'vertex.position' on cylinder geometry");
    return;
  }

  if (m_index)
    m_index->addCommitObserver(this);
  m_vertexPosition->addCommitObserver(this);

  const float *radius = m_radius ? m_radius->beginAs<float>() : nullptr;
  m_globalRadius = getParam<float>("radius", 1.f);

  const auto numCylinders =
      m_index ? m_index->size() : m_vertexPosition->size() / 2;

  {
    auto *vr = (float4 *)rtcSetNewGeometryBuffer(embreeGeometry(),
        RTC_BUFFER_TYPE_VERTEX,
        0,
        RTC_FORMAT_FLOAT4,
        sizeof(float4),
        numCylinders * 2);

    if (m_index) {
      const auto *begin = m_index->beginAs<uint2>();
      const auto *end = m_index->endAs<uint2>();
      const auto *v = m_vertexPosition->beginAs<float3>();
      uint32_t cID = 0;
      std::for_each(begin, end, [&](const uint2 &idx) {
        vr[cID + 0] =
            float4(v[idx.x], radius ? radius[cID / 2] : m_globalRadius);
        vr[cID + 1] =
            float4(v[idx.y], radius ? radius[cID / 2] : m_globalRadius);
        cID += 2;
      });
    } else {
      const auto *begin = m_vertexPosition->beginAs<float3>();
      const auto *end = m_vertexPosition->endAs<float3>();
      uint32_t rID = 0;
      std::transform(begin, end, vr, [&](const float3 &v) {
        return float4(v, radius ? radius[rID++ / 2] : m_globalRadius);
      });
    }
  }

  {
    auto *idx = (uint32_t *)rtcSetNewGeometryBuffer(embreeGeometry(),
        RTC_BUFFER_TYPE_INDEX,
        0,
        RTC_FORMAT_UINT,
        sizeof(uint32_t),
        numCylinders);
    std::iota(idx, idx + numCylinders, 0);
    std::transform(idx, idx + numCylinders, idx, [](auto &i) { return i * 2; });
  }

  rtcCommitGeometry(embreeGeometry());
}

float4 Cylinder::getAttributeValueAt(
    const Attribute &attr, const Ray &ray) const
{
  if (attr == Attribute::NONE)
    return DEFAULT_ATTRIBUTE_VALUE;

  auto attrIdx = static_cast<int>(attr);
  auto *attributeArray = m_vertexAttributes[attrIdx].ptr;
  if (!attributeArray)
    return Geometry::getAttributeValueAt(attr, ray);

  auto idx = m_index ? *(m_index->dataAs<uint32_t>() + ray.primID) : ray.primID;

  auto a = readAttributeArrayAt(attributeArray, idx + 0);
  auto b = readAttributeArrayAt(attributeArray, idx + 1);

  return a + (b - a) * ray.u;
}

void Cylinder::cleanup()
{
  if (m_index)
    m_index->removeCommitObserver(this);
  if (m_vertexPosition)
    m_vertexPosition->removeCommitObserver(this);
}

} // namespace helide
