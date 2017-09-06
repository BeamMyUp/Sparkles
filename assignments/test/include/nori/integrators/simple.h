#pragma once

#include <nori/integrators/integrator.h>

NORI_NAMESPACE_BEGIN

class SimpleIntegrator : public Integrator {
public:

	/**
	* \brief Direct Illumination Integrator computing reflected radiance for diffuse surfaces
	*
	* \param scene
	*    A pointer to the underlying scene
	* \param sampler
	*    A pointer to a sample generator
	* \param ray
	*    The ray in question
	* \return
	*    Estimate of the radiance in the direction given
	*/
	virtual Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const override;

	/// Return a brief string summary of the instance (for debugging purpose)
	std::string toString() const override;

	SimpleIntegrator(const PropertyList &props);
};

NORI_NAMESPACE_END
