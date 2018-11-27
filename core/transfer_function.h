// ======================================================================== //
// Copyright 2009-2018 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "common.h"

namespace oidn {

  // Color transfer function
  class TransferFunction
  {
  public:
    virtual ~TransferFunction() = default;

    virtual float forward(float x) const = 0;
    virtual float reverse(float x) const = 0;
  };

  class LinearTransferFunction : public TransferFunction
  {
  public:
    __forceinline float forward(float x) const override { return x; }
    __forceinline float reverse(float x) const override { return x; }
  };

  class SrgbTransferFunction : public TransferFunction
  {
  public:
    __forceinline float forward(float x) const override
    {
      return pow(x, 1.f/2.2f);
    }

    __forceinline float reverse(float x) const override
    {
      return pow(x, 2.2f);
    }
  };

  // HDR = log + sRGB
  class HdrTransferFunction : public TransferFunction
  {
  private:
    static constexpr float scale = 0.16604764621f; // 1/log2(64+1)
    static constexpr float rcpScale = 1.f / scale;

    float exposure;
    float rcpExposure;

  public:
    HdrTransferFunction(float exposure = 1.f)
    {
      setExposure(exposure);
    }

    void setExposure(float exposure)
    {
      this->exposure = exposure;
      this->rcpExposure = 1.f / exposure;
    }

    __forceinline float forward(float x) const override
    {
      x *= exposure;
      return pow(log2(x+1.f) * scale, 1.f/2.2f);
    }

    __forceinline float reverse(float x) const override
    {
      const float y = pow(x, 2.2f);
      return (exp2(y * rcpScale) - 1.f) * rcpExposure;
    }
  };

} // namespace oidn
