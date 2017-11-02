#include <nori/integrators/integrator.h>
#include <nori/core/scene.h>
#include <nori/samplers/sampler.h>

NORI_NAMESPACE_BEGIN

const nori::Emitter* Integrator::chooseOneLight(const Scene &scene, Sampler *sampler) {
	int nLights = scene.getEmitters().size();
	if (nLights == 0)
		return nullptr;

	int lightNum = std::min(static_cast<int>(sampler->next1D() * nLights), nLights - 1);
	const Emitter* light = scene.getEmitters()[lightNum];

	return light;
}

NORI_NAMESPACE_END