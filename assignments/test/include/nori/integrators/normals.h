#pragma once

#include <nori/integrators/integrator.h>

NORI_NAMESPACE_BEGIN

class NormalIntegrator : public Integrator {
public:

	/**
	* \brief returns the normal when an intersection is found
	*
	* \param scene
	*    A pointer to the underlying scene
	* \param sampler
	*    A pointer to a sample generator
	* \param ray
	*    The ray in question
	* \return
	*    The normal of the first shape hit by the ray
	*/
	Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const;

	/// Return a brief string summary of the instance (for debugging purpose)
	std::string toString() const;

	NormalIntegrator(const PropertyList &props);
};

NORI_NAMESPACE_END