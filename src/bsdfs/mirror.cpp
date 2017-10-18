/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob

    Nori is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Nori is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <nori/bsdfs/bsdf.h>
#include <nori/core/frame.h>

NORI_NAMESPACE_BEGIN

/// Ideal mirror BRDF
class Mirror : public BSDF {
public:
    Mirror(const PropertyList &) { }

    Color3f eval(const BSDFQueryRecord &) const {
        /* Discrete BRDFs always evaluate to zero in Nori */
        return Color3f(0.0f);
    }

    float pdf(const BSDFQueryRecord &) const {
        /* Discrete BRDFs always evaluate to zero in Nori */
        return 0.0f;
    }

    Color3f sample(BSDFQueryRecord &bRec, SampleQueryRecord& sRec, const Point2f &sample) const {
        if (Frame::cosTheta(bRec.wo) <= 0) 
            return Color3f(0.0f);

        // Reflection in local coordinates
        bRec.wi = Vector3f(
            -bRec.wo.x(),
            -bRec.wo.y(),
             bRec.wo.z()
        );
        bRec.measure = EMeasure::EDiscrete;

        /* Relative index of refraction: no change */
        bRec.eta = 1.0f;

		sRec.sample.v = bRec.wi;
		sRec.pdf = pdf(bRec); 

        return Color3f(1.0f);
    }

    std::string toString() const {
        return "Mirror[]";
    }
};

NORI_REGISTER_CLASS(Mirror, "mirror");
NORI_NAMESPACE_END
