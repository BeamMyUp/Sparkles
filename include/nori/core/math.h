#pragma once

#include <nori/core/common.h>

/* "Ray epsilon": relative error threshold for ray intersection computations */
#define Epsilon 1e-4f

/* A few useful constants */
#undef M_PI

#define M_PI         3.14159265358979323846f
#define INV_PI       0.31830988618379067154f
#define INV_TWOPI    0.15915494309189533577f
#define INV_FOURPI   0.07957747154594766788f
#define SQRT_TWO     1.41421356237309504880f
#define INV_SQRT_TWO 0.70710678118654752440f

NORI_NAMESPACE_BEGIN

/// Convert radians to degrees
inline float radToDeg(float value) { return value * (180.0f / M_PI); }

/// Convert degrees to radians
inline float degToRad(float value) { return value * (M_PI / 180.0f); }

#if !defined(_GNU_SOURCE)
/// Emulate sincosf using sinf() and cosf()
inline void sincosf(float theta, float *_sin, float *_cos) {
	*_sin = sinf(theta);
	*_cos = cosf(theta);
}
#endif

/// Clamp value to non negative value
inline float zeroClamp(float value) {
	return std::max(0.0f, value);
}

/// Absolute value of the dot product
inline float absDot(Eigen::Vector3f u, Eigen::Vector3f v) {
	return std::abs(u.dot(v));
}

/// Square root variant that gracefully handles arguments < 0 that are due to roundoff errors
inline float safeSqrt(float value) {
	return std::sqrt(std::max(0.0f, value));
}

/// Simple floating point clamping function
inline float clamp(float value, float min, float max) {
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else return value;
}

/// Simple integer clamping function
inline int clamp(int value, int min, int max) {
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else return value;
}

/// Linearly interpolate between two values
inline float lerp(float t, float v1, float v2) {
	return ((float)1 - t) * v1 + t * v2;
}

/// Always-positive modulo operation
inline int mod(int a, int b) {
	int r = a % b;
	return (r < 0) ? r + b : r;
}

/// Compute a direction for the given coordinates in spherical coordinates
extern Vector3f sphericalDirection(float theta, float phi);

/// Compute a direction for the given coordinates in spherical coordinates
extern Point2f sphericalCoordinates(const Vector3f &dir);

/**
* \brief Calculates the unpolarized fresnel reflection coefficient for a
* dielectric material. Handles incidence from either side (i.e.
* \code cosThetaI<0 is allowed).
*
* \param cosThetaI
*      Cosine of the angle between the normal and the incident ray
* \param extIOR
*      Refractive index of the side that contains the surface normal
* \param intIOR
*      Refractive index of the interior
*/
extern float fresnel(float cosThetaI, float extIOR, float intIOR);

/**
* \brief Returns if the quadratic equation ax^2 + bx + c is solvable for x
*
* \param[out] t0
*     The first zero found
*
* \param[out] t1
*     The second zero found
*/
extern bool quadratic(double a, double b, double c, double& t0, double& t1);

/**
* \brief Returns the squared distance of the segment pq
*/
extern float squaredDist(Point3f p, Point3f q);

NORI_NAMESPACE_END