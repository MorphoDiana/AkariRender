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

#ifndef AKARIRENDER_CAMERA_H
#define AKARIRENDER_CAMERA_H

#include <Akari/Render/Geometry.hpp>
#include <Akari/Core/Component.h>
#include <Akari/Core/Film.h>

namespace Akari {
    struct CameraSample {
        vec2 p_film;
        vec2 p_lens;
        Ray primary;
        Float weight = 1;
    };

    class Camera : public Component {
    public:
        [[nodiscard]] virtual bool IsProjective() const { return false; }

        virtual void GenerateRay(const vec2 &u1, const vec2 &u2, const ivec2 &raster, CameraSample &sample) const = 0;

        [[nodiscard]] virtual std::shared_ptr<Film> GetFilm() const =0 ;
    };


}
#endif // AKARIRENDER_CAMERA_H