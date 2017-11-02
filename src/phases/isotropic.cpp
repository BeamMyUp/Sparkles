#include <nori/phases/isotropic.h>
#include <nori/warp/warp.h>

NORI_NAMESPACE_BEGIN

IsotropicPhase::IsotropicPhase(const PropertyList &list) {

}

float IsotropicPhase::eval(const Vector3f& wo, const Vector3f& wi) const {
	return INV_FOURPI; 
}

float IsotropicPhase::sample(SampleQueryRecord &sRec, Sampler* sampler) const {
	Warp::WarpQueryRecord wRec;
	Warp::warp(wRec, Warp::EWarpType::EUniformSphere, sampler->next2D());

	sRec.sample.v = wRec.warpedPoint;
	sRec.pdf = wRec.pdf;

	return sRec.pdf;
}

float IsotropicPhase::pdf() const {
	return Warp::pdf(Warp::EWarpType::EUniformSphere, Point3f()); 
}

std::string IsotropicPhase::toString() const {
	return tfm::format(
		"Isotropic");
}

NORI_REGISTER_CLASS(IsotropicPhase, "isotropic");
NORI_NAMESPACE_END