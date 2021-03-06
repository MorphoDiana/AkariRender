// MIT License
//
// Copyright (c) 2019 椎名深雪
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef AKARIRENDER_SAMPLING_HPP
#define AKARIRENDER_SAMPLING_HPP
#include <algorithm>
#include <Akari/Core/Math.h>

namespace Akari {
    inline vec2 ConcentricSampleDisk(const vec2 &u) {
        vec2 uOffset = 2.f * u - vec2(1, 1);
        if (uOffset.x == 0 && uOffset.y == 0)
            return vec2(0, 0);

        Float theta, r;
        if (std::abs(uOffset.x) > std::abs(uOffset.y)) {
            r = uOffset.x;
            theta = Pi4 * (uOffset.y / uOffset.x);
        } else {
            r = uOffset.y;
            theta = Pi2 - Pi4 * (uOffset.x / uOffset.y);
        }
        return r * vec2(std::cos(theta), std::sin(theta));
    }

    inline vec3 CosineHemisphereSampling(const vec2 &u) {
        auto uv = ConcentricSampleDisk(u);
        auto r = dot(uv, uv);
        auto h = std::sqrt(std::max(0.0f, 1 - r));
        return vec3(uv.x, h, uv.y);
    }
    inline Float CosineHemispherePDF(Float cosTheta){
        return cosTheta * InvPi;
    }
}
#endif // AKARIRENDER_SAMPLING_HPP
