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

#ifndef AKARIRENDER_BSDF_H
#define AKARIRENDER_BSDF_H

#include <Akari/Core/Spectrum.h>
#include <Akari/Core/Sampling.hpp>

namespace Akari {
    enum BSDFType : int {
        BSDF_NONE = 0u,
        BSDF_REFLECTION = 1u << 0u,
        BSDF_TRANSMISSION = 1u << 1u,
        BSDF_DIFFUSE = 1u << 2u,
        BSDF_GLOSSY = 1u << 3u,
        BSDF_SPECULAR = 1u << 4u,
        BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION,
    };


    inline Float CosTheta(const vec3 &w) { return w.y; }

    inline Float AbsCosTheta(const vec3 &w) { return std::abs(CosTheta(w)); }

    inline Float Cos2Theta(const vec3 &w) { return w.y * w.y; }

    inline Float Sin2Theta(const vec3 &w) { return 1 - Cos2Theta(w); }

    inline Float SinTheta(const vec3 &w) { return std::sqrt(std::fmax(0.0f, Sin2Theta(w))); }

    inline Float Tan2Theta(const vec3 &w) { return Sin2Theta(w) / Cos2Theta(w); }

    inline Float TanTheta(const vec3 &w) { return std::sqrt(std::fmax(0.0f, Tan2Theta(w))); }

    inline bool SameHemisphere(const vec3 &wo, const vec3 &wi) { return wo.y * wi.y >= 0; }

    inline vec3 Reflect(const vec3 &w, const vec3 &n) { return -1.0f * w + 2.0f * dot(w, n) * n; }

    struct BSDFSample {
        const vec3 &wo;
        Float u0{};
        vec2 u{};
        vec3 wi{};
        Spectrum f{};
        Float pdf;
        BSDFType sampledType = BSDF_NONE;
        explicit BSDFSample(const vec3 &wo) : wo(wo), pdf(-1) {}
    };

    class BSDFComponent {
      public:
        explicit BSDFComponent(BSDFType type) : type(type) {}
        const BSDFType type;
        [[nodiscard]] virtual Float EvaluatePdf(const vec3 &wo, const vec3 &wi) const {
            return AbsCosTheta(wi) * InvPi;
        }
        [[nodiscard]] virtual Spectrum Evaluate(const vec3 &wo, const vec3 &wi) const = 0;
        virtual void Sample(BSDFSample &sample) const {
            sample.wi = CosineHemisphereSampling(sample.wo);
            if(!SameHemisphere(sample.wi,sample.wo)){
                sample.wi.y *= -1;
            }
            sample.pdf = AbsCosTheta(sample.wi) * InvPi;
            sample.f = Evaluate(sample.wo, sample.wi);
        }
        [[nodiscard]] bool IsDelta() const { return ((uint32_t)type & (uint32_t)BSDF_SPECULAR) != 0; }
        [[nodiscard]] bool MatchFlag(BSDFType flag) const { return ((uint32_t)type & (uint32_t)flag) != 0; }
    };
    class BSDF {
        constexpr static int MaxBSDF = 8;
        std::array<const BSDFComponent *, MaxBSDF> components{};
        int nComp = 0;
        const CoordinateSystem frame;
        vec3 Ng;

      public:
        BSDF(const vec3 &Ng, const vec3 &Ns) : frame(Ns), Ng(Ng) {}
        void AddComponent(const BSDFComponent *comp) { components[nComp++] = comp; }
        [[nodiscard]] Float EvaluatePdf(const vec3 &wo, const vec3 &wi) const {
            Float pdf = 0;
            int cnt = 0;
            for (int i = 0; i < nComp; i++) {
                auto *comp = components[i];
                if (!comp->IsDelta()) {
                    cnt++;
                    pdf += comp->EvaluatePdf(wo, wi);
                }
            }
            if (cnt > 1) {
                pdf /= (float)cnt;
            }
            return pdf;
        }
        [[nodiscard]] auto LocalToWorld(const vec3 &w) const { return frame.LocalToWorld(w); }
        [[nodiscard]] auto WorldToLocal(const vec3 &w) const { return frame.WorldToLocal(w); }
        [[nodiscard]] Spectrum Evaluate(const vec3 &wo, const vec3 &wi) const {
            auto woW = LocalToWorld(wo);
            auto wiW = LocalToWorld(wi);
            Spectrum f(0);
            for (int i = 0; i < nComp; i++) {
                auto *comp = components[i];
                if (!comp->IsDelta()) {
                    auto reflect = (dot(woW, Ng) * dot(wiW, Ng) > 0);
                    if ((reflect && comp->MatchFlag(BSDF_REFLECTION)) ||
                        (!reflect && comp->MatchFlag(BSDF_TRANSMISSION))) {
                        f += comp->Evaluate(wo, wi);
                    }
                }
            }
            return f;
        }
        void Sample(BSDFSample &sample) const {
            if (nComp == 0) {
                return;
            }
            int selected = std::clamp(int(sample.u0 * (float)nComp), 0, nComp - 1);
            sample.u0 = std::min(sample.u0 * (float)nComp - (float)selected, 1.0f - Eps);
            {
                auto *comp = components[selected];
                comp->Sample(sample);
                sample.sampledType = comp->type;
                if (comp->IsDelta()) {
                    return;
                }
            }
            auto &f = sample.f;
            auto &wo = sample.wo;
            auto &wi = sample.wi;
            auto woW = LocalToWorld(wo);
            auto wiW = LocalToWorld(wi);
            int cnt = 0;
            for (int i = 0; i < nComp; i++) {
                if (i == selected)
                    continue;
                auto *comp = components[i];
                if (!comp->IsDelta()) {
                    auto reflect = (dot(woW, Ng) * dot(wiW, Ng) > 0);
                    if ((reflect && comp->MatchFlag(BSDF_REFLECTION)) ||
                        (!reflect && comp->MatchFlag(BSDF_TRANSMISSION))) {
                        f += comp->Evaluate(wo, wi);
                    }
                    sample.pdf += comp->EvaluatePdf(wo, wi);
                    cnt++;
                }
            }
            if (cnt > 1) {
                f /= (float)cnt;
            }
        }
    };


} // namespace Akari
#endif // AKARIRENDER_BSDF_H