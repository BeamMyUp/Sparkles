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

#include <nori/cameras/perspective.h>
#include <glviewer/viewer.h>
#include <glviewer/camera.h>
#include <glm/gtc/type_ptr.hpp>

NORI_NAMESPACE_BEGIN

void PerspectiveCamera::activate() {
    float aspect = m_outputSize.x() / (float) m_outputSize.y();

    /* Project vectors in camera space onto a plane at z=1:
        *
        *  xProj = cot * x / z
        *  yProj = cot * y / z
        *  zProj = (far * (z - near)) / (z * (far-near))
        *  The cotangent factor ensures that the field of view is 
        *  mapped to the interval [-1, 1].
        */
    float recip = 1.0f / (m_farClip - m_nearClip),
            cot = 1.0f / std::tan(degToRad(m_fov / 2.0f));

    Eigen::Matrix4f perspective;
    perspective <<
        cot, 0,   0,   0,
        0, cot,   0,   0,
        0,   0,   m_farClip * recip, -m_nearClip * m_farClip * recip,
        0,   0,   1,   0;

    /**
        * Translation and scaling to shift the clip coordinates into the
        * range from zero to one. Also takes the aspect ratio into account.
        */
    m_sampleToCamera = Transform( 
        Eigen::DiagonalMatrix<float, 3>(Vector3f(-0.5f, -0.5f * aspect, 1.0f)) *
        Eigen::Translation<float, 3>(-1.0f, -1.0f/aspect, 0.0f) * perspective).inverse();

    /* If no reconstruction filter was assigned, instantiate a Gaussian filter */
    if (!m_rfilter)
        m_rfilter = static_cast<ReconstructionFilter *>(
            NoriObjectFactory::createInstance("gaussian", PropertyList()));

	viewer::Viewer::getInstance().setCamera(m_cameraToWorld, *this);
}

Color3f PerspectiveCamera::sampleRay(Ray3f &ray,
        const Point2f &samplePosition,
        const Point2f &apertureSample) const {
    /* Compute the corresponding position on the 
        near plane (in local camera space) */
    Point3f nearP = m_sampleToCamera * Point3f(
        samplePosition.x() * m_invOutputSize.x(),
        samplePosition.y() * m_invOutputSize.y(), 0.0f);

    /* Turn into a normalized ray direction, and
        adjust the ray interval accordingly */
    Vector3f d = nearP.normalized();
    float invZ = 1.0f / d.z();

    ray.o = m_cameraToWorld * Point3f(0, 0, 0);
    ray.d = m_cameraToWorld * d;
    ray.mint = m_nearClip * invZ;
    ray.maxt = m_farClip * invZ;
    ray.update();

    return Color3f(1.0f);
}

void PerspectiveCamera::addChild(NoriObject *obj) {
    switch (obj->getClassType()) {
		case EClassType::EReconstructionFilter:
            if (m_rfilter)
                throw NoriException("Camera: tried to register multiple reconstruction filters!");
            m_rfilter = static_cast<ReconstructionFilter *>(obj);
            break;

        default:
            throw NoriException("Camera::addChild(<%s>) is not supported!",
                classTypeName(obj->getClassType()));
    }
}


void PerspectiveCamera::resetCamera(const viewer::Camera* const camera) {
	m_fov = camera->getFovy(); 
	Eigen::Matrix4f mat(glm::value_ptr(camera->GetViewMatrix())); 
	m_cameraToWorld = Transform(mat); 
}

std::string PerspectiveCamera::toString() const {
    return tfm::format(
        "PerspectiveCamera[\n"
        "  cameraToWorld = %s,\n"
        "  outputSize = %s,\n"
        "  fov = %f,\n"
        "  clip = [%f, %f],\n"
        "  rfilter = %s\n"
        "]",
        indent(m_cameraToWorld.toString(), 18),
        m_outputSize.toString(),
        m_fov,
        m_nearClip,
        m_farClip,
        indent(m_rfilter->toString())
    );
}

NORI_REGISTER_CLASS(PerspectiveCamera, "perspective");
NORI_NAMESPACE_END
