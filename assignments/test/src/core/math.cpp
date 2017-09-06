#include <nori/core/math.h>
#include <nori/core/vector.h>

#include <Eigen/Geometry>

NORI_NAMESPACE_BEGIN 

Vector3f sphericalDirection(float theta, float phi) {
	float sinTheta, cosTheta, sinPhi, cosPhi;

	sincosf(theta, &sinTheta, &cosTheta);
	sincosf(phi, &sinPhi, &cosPhi);

	return Vector3f(
		sinTheta * cosPhi,
		sinTheta * sinPhi,
		cosTheta
	);
}

Point2f sphericalCoordinates(const Vector3f &v) {
	Point2f result(
		std::acos(v.z()),
		std::atan2(v.y(), v.x())
	);
	if (result.y() < 0)
		result.y() += 2 * M_PI;
	return result;
}

void coordinateSystem(const Vector3f &a, Vector3f &b, Vector3f &c) {
	if (std::abs(a.x()) > std::abs(a.y())) {
		float invLen = 1.0f / std::sqrt(a.x() * a.x() + a.z() * a.z());
		c = Vector3f(a.z() * invLen, 0.0f, -a.x() * invLen);
	}
	else {
		float invLen = 1.0f / std::sqrt(a.y() * a.y() + a.z() * a.z());
		c = Vector3f(0.0f, a.z() * invLen, -a.y() * invLen);
	}
	b = c.cross(a);
}

float fresnel(float cosThetaI, float extIOR, float intIOR) {
	float etaI = extIOR, etaT = intIOR;

	if (extIOR == intIOR)
		return 0.0f;

	/* Swap the indices of refraction if the interaction starts
	at the inside of the object */
	if (cosThetaI < 0.0f) {
		std::swap(etaI, etaT);
		cosThetaI = -cosThetaI;
	}

	/* Using Snell's law, calculate the squared sine of the
	angle between the normal and the transmitted ray */
	float eta = etaI / etaT,
		sinThetaTSqr = eta*eta * (1 - cosThetaI*cosThetaI);

	if (sinThetaTSqr > 1.0f)
		return 1.0f;  /* Total internal reflection! */

	float cosThetaT = std::sqrt(1.0f - sinThetaTSqr);

	float Rs = (etaI * cosThetaI - etaT * cosThetaT)
		/ (etaI * cosThetaI + etaT * cosThetaT);
	float Rp = (etaT * cosThetaI - etaI * cosThetaT)
		/ (etaT * cosThetaI + etaI * cosThetaT);

	return (Rs * Rs + Rp * Rp) / 2.0f;
}

bool quadratic(double a, double b, double c, double& t0, double& t1) {
	// ECSE689: calculate the zeros t0 and t1 of the quadratic function defined by a^2x + bx + c

	return false;
}

float distSquared(Point3f p, Point3f q){
	float x = (q.x() - p.x());
	float y = (q.y() - p.y());
	float z = (q.z() - p.z());

	return x * x + y * y + z * z;
}

NORI_NAMESPACE_END