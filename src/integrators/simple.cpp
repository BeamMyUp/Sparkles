#include <nori/integrators/simple.h>
#include <nori/shapes/shape.h>
#include <nori/core/scene.h>
#include <nori/emitters/emitter.h>
#include <nori/bsdfs/bsdf.h>

NORI_NAMESPACE_BEGIN

SimpleIntegrator::SimpleIntegrator(const PropertyList &props) {

}

Color3f SimpleIntegrator::Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const{
	Color3f Li(0.0f);

	// Query if the ray intersects an object
	Intersection its;
	if (!scene->rayIntersect(ray, its))
		return Li;

	// Find the corresponding normal and BSDF where the ray intersects
	Vector3f n = its.shFrame.n;
	const BSDF* bsdf = its.shape->getBSDF();

	for (const auto &emitter : scene->getEmitters()) {
		// Query the light's radiance and 
		EmitterQueryRecord eqr;
		Color3f Le = emitter->eval(eqr, its.p); 

		// If point is visible from light source
		Ray3f visibilityRay(its.p, eqr.wi); 
		if (!scene->rayIntersect(visibilityRay)) { 
			BSDFQueryRecord bqr(its.toLocal(eqr.wi), its.toLocal(-ray.d), ESolidAngle); 

			float cosTheta = zeroClamp(n.dot(visibilityRay.d.normalized()));

			Li += Le * bsdf->eval(bqr) * cosTheta; 
		}
	}

	return Li;
}

std::string SimpleIntegrator::toString() const {
	return tfm::format(
		"SimpleIntegrator[]"
	);
}

NORI_REGISTER_CLASS(SimpleIntegrator, "simple");
NORI_NAMESPACE_END
