#pragma once

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

NORI_NAMESPACE_BEGIN

/**
* \brief Diffuse / Lambertian BRDF model
*/
class Diffuse : public BSDF {
public:
	Diffuse(const PropertyList &propList);

	/// Evaluate the BRDF model
	Color3f eval(const BSDFQueryRecord &bRec) const;

	/// Compute the density of \ref sample() wrt. solid angles
	float pdf(const BSDFQueryRecord &bRec) const;

	/// Draw a a sample from the BRDF model
	Color3f sample(BSDFQueryRecord &bRec, SampleQueryRecord& sRec, const Point2f &sample) const;

	// TODO: Remove - not needed with the BSDF type enum
	bool isDiffuse() const { return true; }

	EBSDFType getBSDFType() const override { return EBSDFType::EDiffuse; }
	Color3f albedo() const { return m_albedo; }

	/// Return a human-readable summary
	std::string toString() const;

	EClassType getClassType() const { return EClassType::EBSDF; }
private:
	Color3f m_albedo;
};

NORI_NAMESPACE_END

