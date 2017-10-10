#pragma once

#include <nori/integrators/integrator.h>
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
	uint32_t m_nSamples1;			//<< Number of samples for measure 1
	uint32_t m_nSamples2;			//<< Number of samples for measure 2
	EMeasure m_measure1;			//<< Measure 1 used for MIS
	EMeasure m_measure2;			//<< Measure 2 used for MIS (Warning : no compatibility tests between measures is made)
	Warp::EWarpType m_warpType1;	//<< Warp type used for measure 1 (if needed)
	Warp::EWarpType m_warpType2;	//<< Warp type used for measure 2 (if needed)
};

NORI_NAMESPACE_END