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

#include <nori/bsdfs/diffuse.h>
#include <nori/core/frame.h>
#include <nori/warp/warp.h>

NORI_NAMESPACE_BEGIN

Diffuse::Diffuse(const PropertyList &propList) {
    m_albedo = propList.getColor("albedo", Color3f(0.5f));
}

/// Evaluate the BRDF model
Color3f Diffuse::eval(const BSDFQueryRecord &bRec) const {
    /* This is a smooth BRDF -- return zero if the measure
        is wrong, or when queried for illumination on the backside */
    if (bRec.measure != EMeasure::ESolidAngle
        || Frame::cosTheta(bRec.wi) <= 0
        || Frame::cosTheta(bRec.wo) <= 0)
        return Color3f(0.0f);

    /* The BRDF is simply the albedo / pi */
    return m_albedo * INV_PI;
}

/// Compute the density of \ref sample() wrt. solid angles
float Diffuse::pdf(const BSDFQueryRecord &bRec) const {
    /* This is a smooth BRDF -- return zero if the measure
        is wrong, or when queried for illumination on the backside */
    if (bRec.measure != EMeasure::ESolidAngle
        || Frame::cosTheta(bRec.wi) <= 0
        || Frame::cosTheta(bRec.wo) <= 0)
        return 0.0f;


    /* Importance sampling density wrt. solid angles:
        cos(theta) / pi.

        Note that the directions in 'bRec' are in local coordinates,
        so Frame::cosTheta() actually just returns the 'z' component.
    */
    return INV_PI * Frame::cosTheta(bRec.wi);
}

/// Draw a a sample from the BRDF model
Color3f Diffuse::sample(BSDFQueryRecord &bRec, SampleQueryRecord& sRec, const Point2f &sample) const {
    if (Frame::cosTheta(bRec.wo) <= 0)
        return Color3f(0.0f);

    bRec.measure = EMeasure::ESolidAngle;

    /* Warp a uniformly distributed sample on [0,1]^2
        to a direction on a cosine-weighted hemisphere */
	Warp::WarpQueryRecord wqr; 
	Warp::warp(wqr, Warp::EWarpType::ECosineHemisphere, sample); 
    bRec.wi = sRec.sample.v = wqr.warpedPoint;

	sRec.pdf = pdf(bRec);

    /* Relative index of refraction: no change */
    bRec.eta = 1.0f;

    /* eval() / pdf() * cos(theta) = albedo. There
        is no need to call these functions. */
    return m_albedo;
}

/// Return a human-readable summary
std::string Diffuse::toString() const {
    return tfm::format(
        "Diffuse[\n"
        "  albedo = %s\n"
        "]", m_albedo.toString());
}

NORI_REGISTER_CLASS(Diffuse, "diffuse");
NORI_NAMESPACE_END
