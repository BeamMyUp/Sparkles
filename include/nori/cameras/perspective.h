#pragma once

/*
This file is part of Nori, a simple educational ray tracer

Copyright (c) 2015 by Wenzel Jakob

Nori is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License Version 3
as published by the Free Software Foundation.

Nori is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <nori/cameras/camera.h>
#include <nori/core/rfilter.h>
#include <nori/core/math.h>
#include <nori/warp/warp.h>
#include <Eigen/Geometry>

NORI_NAMESPACE_BEGIN

/**
* \brief Perspective camera with depth of field
*
* This class implements a simple perspective camera model. It uses an
* infinitesimally small aperture, creating an infinite depth of field.
*/
class PerspectiveCamera : public Camera {
public:

	PerspectiveCamera(const PropertyList &propList) {
		/* Width and height in pixels. Default: 720p */
		m_outputSize.x() = propList.getInteger("width", 1280);
		m_outputSize.y() = propList.getInteger("height", 720);
		m_invOutputSize = m_outputSize.cast<float>().cwiseInverse();

		/* Specifies an optional camera-to-world transformation. Default: none */
		m_cameraToWorld = propList.getTransform("toWorld", Transform());

		/* Horizontal field of view in degrees */
		m_fov = propList.getFloat("fov", 30.0f);

		/* Near and far clipping planes in world-space units */
		m_nearClip = propList.getFloat("nearClip", 1e-4f);
		m_farClip = propList.getFloat("farClip", 1e4f);

		m_rfilter = NULL;
	}

	void activate();

	Color3f sampleRay(Ray3f &ray,
		const Point2f &samplePosition,
		const Point2f &apertureSample) const;

	void addChild(NoriObject *obj);

	/// Return a human-readable summary
	std::string toString() const;

	float getFOV() const { return m_fov; }
	float getNearClip() const { return m_nearClip; }
	float getFarClip() const { return m_farClip; }

private:
	Vector2f m_invOutputSize;
	Transform m_sampleToCamera;
	Transform m_cameraToWorld;
	float m_fov;
	float m_nearClip;
	float m_farClip;
};

NORI_NAMESPACE_END
