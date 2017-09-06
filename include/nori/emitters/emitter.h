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

#pragma once

#include <nori/shapes/object.h>

NORI_NAMESPACE_BEGIN

class PropertyList;

struct EmitterQueryRecord {
	Vector3f wi; //< Normalized incident direction of light
};

/**
 * \brief Superclass of all emitters
 */
class Emitter : public NoriObject {
public:

    /**
     * \brief Return the type of object (i.e. Mesh/Emitter/etc.) 
     * provided by this instance
     * */
    EClassType getClassType() const override { return EEmitter; }

	/// Returns the emitter's radiance
	virtual const Color3f& getRadiance() const { return m_radiance; }

	/// Return if the emitter's an area light
	virtual bool isArea() const { return false; }

	/// Returns the radiance arriving at the queried point from the emitter, assuming no occlusion
	virtual Color3f eval(EmitterQueryRecord& eqr, const Point3f& p) const = 0;

	/// Sample an Emitter according to the measure given in argument
	virtual void sample(SampleQueryRecord& esqr, EMeasure measure, const Point2f &sample) const = 0; 

	/// Return a brief string summary of the instance (for debugging purpose)
	virtual std::string toString() const override;


	Emitter(const PropertyList& propList); 

protected:
	Color3f m_radiance; 
};

NORI_NAMESPACE_END
