#include <nori/integrators/volPath.h>

#include <nori/integrators/directMIS.h>
#include <nori/core/scene.h>
#include <nori/shapes/shape.h>
#include <nori/emitters/emitter.h>
#include <nori/bsdfs/bsdf.h>

NORI_NAMESPACE_BEGIN

VolumePathIntegrator::VolumePathIntegrator(const PropertyList &props) {

}

Color3f VolumePathIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
	return Color3f(0.f);
}

std::string VolumePathIntegrator::toString() const {
	return tfm::format(
		"VolumePathIntegrator[]"
	);
}

NORI_REGISTER_CLASS(VolumePathIntegrator, "volume");
NORI_NAMESPACE_END
