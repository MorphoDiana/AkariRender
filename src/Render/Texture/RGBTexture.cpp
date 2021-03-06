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

#include <Akari/Core/Plugin.h>
#include <Akari/Core/Spectrum.h>
#include <Akari/Plugins/RGBTexture.h>
#include <Akari/Render/Texture.h>

namespace Akari {
    class RGBTexture final : public Texture {
      public:
        Spectrum rgb{};
        RGBTexture() = default;
        explicit RGBTexture(const vec3 &rgb) : rgb(rgb) {}
        AKR_SER(rgb)
        AKR_DECL_COMP(RGBTexture, "RGBTexture")
        Spectrum Evaluate(const ShadingPoint &sp) const override { return rgb; }
        Float AverageLuminance() const override { return rgb.Luminance(); }
        void TraverseProperties(PropertyVisitor &visitor) override {
            Component::TraverseProperties(visitor);
            DefaultTraversal traversal(visitor);
            traversal.Traverse("color", rgb);
        }
    };

    AKR_EXPORT std::shared_ptr<Texture> CreateRGBTexture(const vec3 &rgb) { return std::make_shared<RGBTexture>(rgb); }
    AKR_EXPORT_COMP(RGBTexture, "Texture")
} // namespace Akari