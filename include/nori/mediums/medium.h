#pragma once

#include <nori/core/object.h>

NORI_NAMESPACE_BEGIN


class Medium : public NoriObject {

public:

	/**
	* \brief Return the type of object (i.e. Mesh/Emitter/etc.)
	* provided by this instance
	* */
	EClassType getClassType() const override { return EClassType::EMedium; }

	virtual Color3f Tr(float t) const = 0;

	/// Sample an Emitter according to the measure given in argument and eval for the resulting point
	virtual void sample(SampleQueryRecord& sRec, const Ray3f &ray, Sampler& sampler, float& outT) const = 0;

	/// Returns the pdf of a 3D point on the emitter according to EMeasure
	virtual float pdf(EMeasure measure, const Point3f& sample, const Point3f* const x = nullptr) const = 0;

	const PhaseFunction* const getPhaseFunction() const { return m_phaseFunction; }

protected:
	PhaseFunction* m_phaseFunction; 
};

NORI_NAMESPACE_END