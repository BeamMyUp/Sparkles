#include <nori/mediums/homogeneous.h>
#include <nori/samplers/sampler.h>
#include <nori/phases/phaseFunction.h>

NORI_NAMESPACE_BEGIN

HomogeneousMedium::HomogeneousMedium(const PropertyList &props) 
	: m_sigmaA(props.getColor("sigmaA", 0.0f))
	, m_sigmaS(props.getColor("sigmaS", 0.8f)){
	m_sigmaT = m_sigmaS + m_sigmaA;
}

Color3f HomogeneousMedium::Tr(float t) const {
	Color3f tr(0.f);
	for (int i = 0; i < 3; ++i)
		tr[i] = std::exp(-m_sigmaT[i] * t);

	return tr; 
}

void HomogeneousMedium::sample(SampleQueryRecord& sRec, const Ray3f &ray, Sampler& sampler, float& outT) const {
	// /!\ Ignore mediumInteractions for now and assume the medium takes the full scene /!\

	// Sample a channel and distance along the ray (PBRT3-894)
	int channel = std::min(static_cast<int>(sampler.next1D() * 3), 2); // 3 : number of channels (Spectrum::nSamples), 2 : nChannels-1 
	float t = -std::log(1 - sampler.next1D()) / m_sigmaT[channel]; 
	bool sampledMedium = t < ray.maxt; 

	// Compute the transmittance and sampling density (PBRT3-894)
	Color3f tr = Tr(t);

	// Return weighting factor for scattering from homogeneous medium (PBRT3-894)
	Color3f density = sampledMedium ? m_sigmaT * tr : tr;
	
	float pdf = 0.f;
	for (int i = 0; i < 3; ++i)
		pdf += density[i];

	pdf *= 1.f / 3.f; 
	sRec.sample.c = sampledMedium ? tr * m_sigmaS : tr;
	sRec.pdf = pdf; 
	outT = t;
}

/// Returns the pdf of a 3D point on the emitter according to EMeasure
float HomogeneousMedium::pdf(EMeasure measure, const Point3f& sample, const Point3f* const x /*= nullptr*/) const {
	return 0;
}

std::string HomogeneousMedium::toString() const {
	return tfm::format(
		"HomogeneousMedium[]"
	);
}

NORI_REGISTER_CLASS(HomogeneousMedium, "homogeneous");
NORI_NAMESPACE_END