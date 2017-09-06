#include <nori/integrators/simple.h>
#include <nori/shapes/shape.h>
#include <nori/core/scene.h>
#include <nori/emitters/emitter.h>
#include <nori/bsdfs/bsdf.h>

NORI_NAMESPACE_BEGIN

SimpleIntegrator::SimpleIntegrator(const PropertyList &props) {

}

Color3f SimpleIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const{
	// ECSE689: Implement a simple Direct illumination integrator
	
	return Color3f();
}

std::string SimpleIntegrator::toString() const {
	return tfm::format(
		"SimpleIntegrator[]"
	);
}

NORI_REGISTER_CLASS(SimpleIntegrator, "simple");
NORI_NAMESPACE_END
