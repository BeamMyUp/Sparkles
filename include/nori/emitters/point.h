#pragma once

#include <nori/emitters/emitter.h>

NORI_NAMESPACE_BEGIN

class PropertyList;

class PointLight : public Emitter {
public: 

	/// Returns the radiance arriving at the queried point from the emitter, assuming no occlusion
	/** Point lights' radiance is actually their Radiant Intensity. Therefore, a unit conversion must happen*/
	virtual void eval(EmitterQueryRecord& outERec, const Point3f& p, const Point3f* const samplePoint = nullptr) const override;
	
	/// Sample an Emitter according to the measure given in argument
	virtual void sample(SampleQueryRecord& sqr, EMeasure measure, const Point2f &sample, const Point3f* const x = nullptr) const override;

	/// Returns the pdf of a 3D point on the emitter according to EMeasure
	virtual float pdf(EMeasure measure, const Point3f& sample, const Point3f* const x = nullptr) const override;

	/// Returns the point light's position
	const Point3f& getPosition() const { return m_position; }

	/// Return a brief string summary of the instance (for debugging purpose)
	virtual std::string toString() const override;

	PointLight(const PropertyList& propList); 

protected:
	Point3f m_position;
};

NORI_NAMESPACE_END