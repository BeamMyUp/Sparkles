#pragma once

#include <nori/shapes/shape.h>

NORI_NAMESPACE_BEGIN

class Sphere : public Shape {
public:

	// Calculate/Update Axis Aligned Bounding Box
	void calculateBoundingBox() override;

	// Compute the quadratic solution of a sphere
	bool quadratic(double a, double b, double c, double& t0, double& t1) const;

	// Return whether a ray intersects with the sphere or not
	bool rayIntersect(const Ray3f &ray_, Intersection &its, bool shadowRay) const override;

	/// Return a human-readable summary of this instance
	std::string toString() const override;

	/// Create a default unit sphere
	Sphere(const PropertyList &propList);

protected:
	std::string m_name;
	float       m_radius; 
	Vector3f    m_center; 
};

NORI_NAMESPACE_END
