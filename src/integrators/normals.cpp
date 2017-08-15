#include <nori/integrators/normals.h>
#include <nori/core/scene.h>
#include <nori/shapes/shape.h>

NORI_NAMESPACE_BEGIN

NormalIntegrator::NormalIntegrator(const PropertyList &props) {
}

Color3f NormalIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
	// Find the surface that is visible in the requested direction
	Intersection its;
	if (!scene->rayIntersect(ray, its))
		return Color3f(0.0f);

	// Return the component-wise absolute value of the shading normal as a color
	Normal3f n = its.shFrame.n.cwiseAbs();

	return Color3f(n.x(), n.y(), n.z());
}

std::string NormalIntegrator::toString() const {
	return tfm::format(
		"NormalIntegrator[]"
		);
}

NORI_REGISTER_CLASS(NormalIntegrator, "normals");
NORI_NAMESPACE_END