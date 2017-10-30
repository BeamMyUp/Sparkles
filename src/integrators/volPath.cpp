#include <nori/integrators/volPath.h>
#include <nori/core/scene.h>
#include <nori/shapes/shape.h>
#include <nori/emitters/emitter.h>
#include <nori/bsdfs/bsdf.h>
#include <nori/mediums/phaseFunction.h>
#include <nori/mediums/homogeneous.h>

NORI_NAMESPACE_BEGIN

VolumePathIntegrator::VolumePathIntegrator(const PropertyList &props){
	m_medium = new HomogeneousMedium(props);

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
	Emitter* const light = scene->getEmitters()[0];
	
	SampleQueryRecord emitterSampling;
	light->sample(emitterSampling, EMeasure::ESolidAngle, sampler->next2D(), &x); 

	Intersection lightZ;
	Ray3f lightRay(x, emitterSampling.sample.v); 
	scene->rayIntersect(lightRay, lightZ);

	Color3f tr_tz = m_medium->Tr(lightZ.t); 
	Color3f Li = tr_tz * light->getRadiance(); 
	Color3f Ls = /*m_medium->getPhaseFunction()->eval(wo, wi)*/ INV_FOURPI * tr_ot * Li;
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

Color3f VolumePathIntegrator::uniformSampleOneLight(const Scene &scene, Sampler *sampler, bool handleMedia) const {
	//int nLights = scene.getEmitters().size(); 
	//if (nLights == 0) 
	//	return Color3f(0.f);
	//
	//int lightNum = std::min(static_cast<int>(sampler->next1D() * nLights), nLights - 1);
	//const Emitter* light = scene.getEmitters()[lightNum];
	//
	//Point2f uLight = sampler->next2D();
	//Point2f uScattering = sampler->next2D(); 
	//
	//return nLights * estimateDirect

	return Color3f(); 
}

std::string VolumePathIntegrator::toString() const {
	return tfm::format(
		"VolumePathIntegrator[]"
	);
}

NORI_REGISTER_CLASS(VolumePathIntegrator, "volume");
NORI_NAMESPACE_END
