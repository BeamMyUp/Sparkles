#pragma once

#include <nori/emitters/emitter.h>
#include <nori/shapes/shape.h>

NORI_NAMESPACE_BEGIN

class AreaLight : public Emitter {
public:

	/// Returns the radiance arriving at the queried point from the emitter, assuming no occlusion
	virtual void eval(EmitterQueryRecord& outERec, const Point3f& p, const Point3f* const ref = nullptr) const override;

	/// Sample an Emitter according to the measure given in argument
	virtual void sample(SampleQueryRecord& sqr, EMeasure measure, const Point2f &sample, const Point3f* x = nullptr) const override;

	/// Returns the pdf of a 3D point on the emitter according to EMeasure
	virtual float pdf(EMeasure measure, const Point3f& sample, const Point3f* const x = nullptr) const override;

	/// Return if emitter is an area light
	virtual bool isArea() const override { return true; }

	/// Set object's emitters and emitter's shape
	virtual void setShape(Shape* shape) { m_shape = shape; }

	/// Return a brief string summary of the instance (for debugging purpose)
	virtual std::string toString() const override;

	AreaLight(const PropertyList& propList);

protected:
	Shape* m_shape;

};

NORI_NAMESPACE_END