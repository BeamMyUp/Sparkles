#pragma once

#include <nori/bsdfs/bsdf.h>
#include <nori/emitters/emitter.h>
#include <nori/shapes/sphere.h>
#include <nori/core/common.h>

NORI_NAMESPACE_BEGIN

Sphere::Sphere(const PropertyList &propList)
	: m_center(propList.getPoint("center", Point3f()))
	, m_radius(propList.getFloat("radius", 1.f)){

	m_invSurfaceArea = 1.f / (4.f * M_PI * m_radius * m_radius); 
}

void Sphere::calculateBoundingBox(){
	// ECSE689: Calculate m_bbox value
}

bool Sphere::rayIntersect(const Ray3f &ray_, float &outT, IntersectionQueryRecord* IQR /*= nullptr*/) const{
	// ECSE689 : Implement Ray sphere intersection and modify outT with the result
	
	return false; 
}

void Sphere::updateIntersection(const Ray3f &ray, Intersection &its, const IntersectionQueryRecord* IQR /*= nullptr*/) const {
	// ECSE689 : update the intersection structure its with the final information. Use ray.maxt for its.t.
}

void Sphere::sampleArea(SampleQueryRecord& outSQR, const Point2f &sample) const {
	throw NoriException("Sphere::sampleArea is not yet implemented");
}

void Sphere::sampleSolidAngle(SampleQueryRecord& outSQR, const Point2f &sample, const Point3f& x) const {
	throw NoriException("Sphere::sampleSolidAngle is not yet implemented");
}

float Sphere::pdfArea(const Point3f &sample) const {
	throw NoriException("Sphere::pdfArea is not yet implemented");
	return 0.f;
}

float Sphere::pdfSolidAngle(const Point3f &sample, const Point3f& x) const {
	throw NoriException("Sphere::pdfSolidAngle is not yet implemented");
	return 0.f;
}

std::string Sphere::toString() const {
	return tfm::format(
		"Sphere[\n"
		"  center = (%f, %f, %f) \n"
		"  radius = %f \n"
		"  bsdf = %s,\n"
		"  emitter = %s\n"
		"]",
		m_center.x(), m_center.y(), m_center.z(),
		m_radius,
		m_bsdf ? indent(m_bsdf->toString()) : std::string("null"),
		m_emitter ? indent(m_emitter->toString()) : std::string("null")
	);
}

NORI_REGISTER_CLASS(Sphere, "sphere");
NORI_NAMESPACE_END