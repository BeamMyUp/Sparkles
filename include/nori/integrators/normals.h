#pragma once

#include <nori/integrators/integrator.h>

NORI_NAMESPACE_BEGIN

class NormalIntegrator : public Integrator {
public:
	NormalIntegrator(const PropertyList &props);

	Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const;

	std::string toString() const;
};

NORI_NAMESPACE_END