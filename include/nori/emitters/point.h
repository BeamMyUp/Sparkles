#pragma once

#include <nori/emitters/emitter.h>

NORI_NAMESPACE_BEGIN

class PropertyList;

class PointLight : public Emitter {
public: 

	/// Returns the point light's position
	const Point3f& getPosition() const { return m_position; }

	/// Returns the radiance arriving at the queried point from the emitter, assuming no occlusion
	/** Point lights' radiance is actually their Radiant Intensity. Therefore, a unit conversion must happen*/
	virtual Color3f eval(EmitterQueryRecord& eqr, const Point3f& p) const;
	
	/// Return a brief string summary of the instance (for debugging purpose)
	virtual std::string toString() const override;

	PointLight(const PropertyList& propList); 

protected:
	Point3f m_position;
};

NORI_NAMESPACE_END