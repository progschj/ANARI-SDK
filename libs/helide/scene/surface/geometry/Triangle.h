// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Geometry.h"

namespace helide {

struct Triangle : public Geometry
{
  Triangle(HelideGlobalState *s);

  void commit() override;

  float4 getAttributeValueAt(
      const Attribute &attr, const Ray &ray) const override;

 private:
  void cleanup();

  helium::IntrusivePtr<Array1D> m_index;
  helium::IntrusivePtr<Array1D> m_vertexPosition;
  std::array<helium::IntrusivePtr<Array1D>, 5> m_vertexAttributes;
};

} // namespace helide
