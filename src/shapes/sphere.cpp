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
	Vector3f rad = Vector3f(m_radius, m_radius, m_radius);

	Vector3f min = m_center - rad;
	Vector3f max = m_center + rad;

	m_bbox = BoundingBox3f(min, max);
}

bool Sphere::rayIntersect(const Ray3f &ray_, Intersection &its, bool shadowRay) const{
	Ray3f ray = ray_;

	// Calculate quadratic coefficients
	double a = ray.d.dot(ray.d);
	double b = 2.f * (ray.d.dot(ray.o - m_center));
	double c = (ray.o - m_center).dot(ray.o - m_center) - m_radius*m_radius;

	double t0, t1;

	// Check for a quadratic solution. If none, no intersection
	if (!quadratic(a, b, c, t0, t1))
		return false;

	// If the intersection is outside the ray's min and max, no intersection
	if (t0 > ray.maxt || t1 < ray.mint)
		return false;

	// try to get a solution inside the ray's min and max 
	float tHit = (float)t0;
	if (t0 < ray.mint)
	{
		tHit = (float)t1;
		if (tHit > ray.maxt)
			return false;
	}

	// Intersection found
	Vector3f itsp = ray(tHit);

	its.t = tHit;
	its.p = itsp;
	its.shape = this;

	// Calculate normal at the intersection point
	Vector3f normal = (itsp - m_center).normalized();

	its.shFrame = Frame(normal);
	its.geoFrame = Frame(normal);

	// Calculate UVs at the intersection point
	double theta = acos(itsp.z() / m_radius);
	double zRad = std::sqrt(itsp.x()*itsp.x() + itsp.y()*itsp.y());
	double cosPhi = itsp.x() / zRad;
	double phi = acos(cosPhi);

	its.uv = Point2f((float)phi * INV_TWOPI, (float)theta * INV_PI);

	return true;
}

bool Sphere::quadratic(double a, double b, double c, double& t0, double& t1) const {
	// Linear case
	if (a == 0) {
		if (b != 0) {
			t0 = t1 = -c / b;
			return true;
		}
	}

	// Find quadratic discriminant
	double discrim = b*b - 4.0f * a*c;

	if (discrim < 0.0)
		return false;

	double rootDiscrim = sqrt(discrim);

	// Compute quadratic t values
	double q;
	if (b < 0.0)
		q = -0.5 * (b - rootDiscrim);

	else
		q = -0.5 * (b + rootDiscrim);

	t0 = q / a;
	t1 = c / q;

	if (t0 > t1)
		std::swap(t0, t1);

	return true;
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