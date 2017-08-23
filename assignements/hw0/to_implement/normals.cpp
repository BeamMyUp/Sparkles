#include <nori/integrators/normals.h>
#include <nori/core/scene.h>
#include <nori/shapes/shape.h>

NORI_NAMESPACE_BEGIN

NormalIntegrator::NormalIntegrator(const PropertyList &props) {
}

Color3f NormalIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
	// ECSE689: Add calculations to display an object's normals and return the result

	return Color3f(0.f, 1.f, 0.f);
}

std::string NormalIntegrator::toString() const {
	return tfm::format(
		"NormalIntegrator[]"
		);
}

NORI_REGISTER_CLASS(NormalIntegrator, "normals");
NORI_NAMESPACE_END