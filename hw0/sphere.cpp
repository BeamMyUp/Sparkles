#pragma once

#include <nori/bsdfs/bsdf.h>
#include <nori/emitters/emitter.h>
#include <nori/shapes/sphere.h>

NORI_NAMESPACE_BEGIN

Sphere::Sphere(const PropertyList &propList)
	: m_center(propList.getPoint("center", Point3f()))
	, m_radius(propList.getFloat("radius", 1.f)){

}

void Sphere::calculateBoundingBox(){
	// ECSE689: Calculate m_bbox value
}

bool Sphere::rayIntersect(const Ray3f &ray_, Intersection &its, bool shadowRay) const{
	// ECSE689 : Implement Ray sphere intersection and fill its with the result
	
	return false; 
}

bool Sphere::quadratic(double a, double b, double c, double& t0, double& t1) const {
	// ECSE689: calculate the zeros t0 and t1 of the quadratic function defined by a^2x + bx + c

	return false;
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