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

#ifndef AKARIRENDER_REFLECTION_H
#define AKARIRENDER_REFLECTION_H

#include <Akari/Render/BSDF.h>
namespace Akari {
    class Fresnel {
      public:
        virtual Spectrum Evaluate(Float cosThetaI) const = 0;
    };

    class AKR_EXPORT FresnelNoOp : public Fresnel {
      public:
        [[nodiscard]] Spectrum Evaluate(Float cosThetaI) const override;
    };

    class AKR_EXPORT FresnelConductor : public Fresnel {
        Spectrum etaI, etaT, k;

      public:
        FresnelConductor(const Spectrum &etaI, const Spectrum &etaT, const Spectrum &k)
            : etaI(etaI), etaT(etaT), k(k) {}
        [[nodiscard]] Spectrum Evaluate(Float cosThetaI) const override;
    };

    class AKR_EXPORT FresnelDielectric : public Fresnel {
        Float etaI, etaT;

      public:
        FresnelDielectric(const Float &etaI, const Float &etaT) : etaI(etaI), etaT(etaT) {}
        [[nodiscard]] Spectrum Evaluate(Float cosThetaI) const override;
    };
    inline Float FrDielectric(Float cosThetaI, Float etaI, Float etaT) {
        bool entering = cosThetaI > 0.f;
        if (!entering) {
            std::swap(etaI, etaT);
            cosThetaI = std::abs(cosThetaI);
        }
        Float sinThetaI = std::sqrt(std::fmax(0.0f, 1 - cosThetaI * cosThetaI));
        Float sinThetaT = etaI / etaT * sinThetaI;
        Float cosThetaT = std::sqrt(std::fmax(0.0f, 1 - sinThetaT * sinThetaT));

        Float Rpar = ((etaT * cosThetaI) - (etaI * cosThetaT)) / ((etaT * cosThetaI) + (etaI * cosThetaT));
        Float Rper = ((etaI * cosThetaI) - (etaT * cosThetaT)) / ((etaI * cosThetaI) + (etaT * cosThetaT));
        return 0.5 * (Rpar * Rpar + Rper * Rper);
    }

    // https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
    inline Spectrum FrConductor(Float cosThetaI, const Spectrum &etaI, const Spectrum &etaT, const Spectrum &k) {
        float CosTheta2 = cosThetaI * cosThetaI;
        float SinTheta2 = 1 - CosTheta2;
        Spectrum Eta = etaT / etaI;
        Spectrum Etak = k / etaI;
        Spectrum Eta2 = Eta * Eta;
        Spectrum Etak2 = Etak * Etak;

        Spectrum t0 = Eta2 - Etak2 - SinTheta2;
        Spectrum a2plusb2 = sqrt(t0 * t0 + 4.0f * Eta2 * Etak2);
        Spectrum t1 = a2plusb2 + CosTheta2;
        Spectrum a = sqrt(0.5f * (a2plusb2 + t0));
        Spectrum t2 = 2.0f * a * cosThetaI;
        Spectrum Rs = (t1 - t2) / (t1 + t2);

        Spectrum t3 = CosTheta2 * a2plusb2 + SinTheta2 * SinTheta2;
        Spectrum t4 = t2 * SinTheta2;
        Spectrum Rp = Rs * (t3 - t4) / (t3 + t4);

        return 0.5 * (Rp + Rs);
    }

    class AKR_EXPORT LambertianReflection : public BSDFComponent {
        Spectrum R;

      public:
        explicit LambertianReflection(const Spectrum &R)
            : BSDFComponent(BSDFType(BSDF_DIFFUSE | BSDF_REFLECTION)), R(R) {}
        [[nodiscard]] Spectrum Evaluate(const vec3 &wo, const vec3 &wi) const override;
    };
    class AKR_EXPORT SpecularReflection : public BSDFComponent {
        Spectrum R;
        const Fresnel *fresnel = nullptr;

      public:
        explicit SpecularReflection(const Spectrum &R, const Fresnel *fresnel)
            : BSDFComponent(BSDFType(BSDF_SPECULAR | BSDF_REFLECTION)), R(R), fresnel(fresnel) {}
        [[nodiscard]] Float EvaluatePdf(const vec3 &wo, const vec3 &wi) const override { return 0; }
        [[nodiscard]] Spectrum Evaluate(const vec3 &wo, const vec3 &wi) const override { return Spectrum(0); }
        Spectrum Sample(const vec2 &u, const vec3 &wo, vec3 *wi, Float *pdf) const override;
    };
} // namespace Akari

#endif // AKARIRENDER_REFLECTION_H
