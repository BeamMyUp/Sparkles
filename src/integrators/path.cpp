#include <nori/integrators/path.h>

NORI_NAMESPACE_BEGIN

PathIntegrator::PathIntegrator(const PropertyList &props){

}

Color3f PathIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
	return Color3f(0.f);
}

std::string PathIntegrator::toString() const {
	return tfm::format(
		"PathIntegrator[]"
	);
}

NORI_REGISTER_CLASS(PathIntegrator, "path");
NORI_NAMESPACE_END