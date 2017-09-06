#pragma once

#include <nori/emitters/emitter.h>

NORI_NAMESPACE_BEGIN

class DirectionalLight : public Emitter {
public:

	/// Returns the radiance arriving at the queried point from the emitter, assuming no occlusion
	virtual Color3f eval(EmitterQueryRecord& eqr, const Point3f &p) const;

	/// Sample an Emitter according to the measure given in argument
	virtual void sample(SampleQueryRecord& sqr, EMeasure measure, const Point2f &sample) const override;

	/// Returns the point light's position
	const Vector3f& getDirection() const { return m_direction; }

	/// Return a brief string summary of the instance (for debugging purpose)
	virtual std::string toString() const override;

	DirectionalLight(const PropertyList& propList);

protected:
	Vector3f m_direction;
};

NORI_NAMESPACE_END