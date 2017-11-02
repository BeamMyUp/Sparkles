#include <nori/integrators/volPath.h>
#include <nori/core/scene.h>
#include <nori/shapes/shape.h>
#include <nori/emitters/emitter.h>
#include <nori/bsdfs/bsdf.h>
#include <nori/mediums/phaseFunction.h>
#include <nori/mediums/homogeneous.h>

NORI_NAMESPACE_BEGIN

VolumePathIntegrator::VolumePathIntegrator(const PropertyList &props)
	: m_medium(nullptr){

}

VolumePathIntegrator::~VolumePathIntegrator() {
	delete m_medium;
	m_medium = nullptr;
}

Color3f VolumePathIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
	Ray3f _ray(ray); 
	
	Intersection its;
	bool intersects = scene->rayIntersect(_ray, its);

	if (intersects) {
		_ray.maxt = its.t;
	}

	// Sample the participating medium, if present (901)
	SampleQueryRecord mediumSampling;
	float t; 
	m_medium->sample(mediumSampling, _ray, *sampler, t);
	Color3f tr_ot = mediumSampling.sample.c / mediumSampling.pdf;

	if (t > _ray.maxt && intersects) {
		// Surface rendering

		if (its.shape->isEmitter()) {
			Color3f Le = its.shape->getEmitter()->getRadiance();
			return tr_ot * Le;
		}

		// TODO: normal surface intersection... 
	}

	Point3f x = _ray.o + _ray.d * t;
	const Emitter* const light = chooseOneLight(*scene, sampler);
	
	SampleQueryRecord emitterSampling;
	light->sample(emitterSampling, EMeasure::ESolidAngle, sampler->next2D(), &x); 

	Intersection lightZ;
	Ray3f lightRay(x, emitterSampling.sample.v); 
	scene->rayIntersect(lightRay, lightZ);

	Vector3f wo = -ray.d;
	Vector3f wi(0.f); 

	Color3f tr_tz = m_medium->Tr(lightZ.t); 
	Color3f Li = tr_tz * light->getRadiance(); 
	Color3f Ls = m_medium->getPhaseFunction()->eval(wo, wi) * tr_ot * Li;
	Color3f L = Ls /  emitterSampling.pdf;

	return L;

		//SampleQueryRecord phaseSampling;
		//m_medium->getPhaseFunction()->sample(phaseSampling, sampler);

		// Possibly terminate the path with Russian roulette (879)
		//{
		//	if (bounces > 3) {
		//		float q = std::max(0.05f, 1 - throughput.y());
		//		if (sampler->next1D() < q)
		//			break;
		//		throughput /= 1 - q; 
		//	}
		//}
	//}

	//return L;
}

const nori::Emitter* VolumePathIntegrator::chooseOneLight(const Scene &scene, Sampler *sampler) const {
	int nLights = scene.getEmitters().size(); 
	if (nLights == 0)
		return nullptr;
	
	int lightNum = std::min(static_cast<int>(sampler->next1D() * nLights), nLights - 1);
	const Emitter* light = scene.getEmitters()[lightNum];

	return light; 
}

void VolumePathIntegrator::addChild(NoriObject *child) {
	switch (child->getClassType()) {
	case EClassType::EMedium:
		if (m_medium)
			throw NoriException(
				"Medium: tried to register multiple medium instances!");
		m_medium = static_cast<Medium *>(child);
		break;

	default:
		throw NoriException("Shape::addChild(<%s>) is not supported!",
			classTypeName(child->getClassType()));
	}
};

std::string VolumePathIntegrator::toString() const {
	return tfm::format(
		"VolumePathIntegrator[]"
	);
}

NORI_REGISTER_CLASS(VolumePathIntegrator, "volume");
NORI_NAMESPACE_END
