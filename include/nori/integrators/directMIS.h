#pragma once

#include <nori/integrators/direct.h>
#include <nori/warp/warp.h>
#include <nori/warp/mis.h>

NORI_NAMESPACE_BEGIN

class DirectMISIntegrator : public Integrator {
public:
	/**
	* \brief Sample the incident radiance along a ray
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

	DirectMISIntegrator(const PropertyList &props);

protected:
	MIS m_mis; 
	DirectIntegrator* m_integrator1;
	DirectIntegrator* m_integrator2; 
};

NORI_NAMESPACE_END