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

#include <nori/accelerators/accel.h>
#include <nori/accelerators/bvh.h>
#include <nori/core/scene.h>
#include <nori/core/bitmap.h>
#include <nori/integrators/integrator.h>
#include <nori/samplers/sampler.h>
#include <nori/shapes/shape.h>
#include <nori/cameras/camera.h>
#include <nori/cameras/perspective.h>
#include <nori/emitters/emitter.h>
#include <glviewer/camera.h>

NORI_NAMESPACE_BEGIN

Scene::Scene(const PropertyList & propList) {
	std::string accel = propList.getString("accelerator", "bvh");

	if (accel == "bvh")
		m_accel = new BVH();
	else
		m_accel = new Accel(); 
}

Scene::~Scene() {
    delete m_accel;
    delete m_sampler;
    delete m_camera;
    delete m_integrator;
}

bool Scene::rayIntersect(const Ray3f &ray, Intersection &its) const {
	return m_accel->rayIntersect(ray, its, false);
}

bool Scene::rayIntersect(const Ray3f &ray) const {
	Intersection its; // Unused
	return m_accel->rayIntersect(ray, its, true);
}

const BoundingBox3f& Scene::getBoundingBox() const {
	return m_accel->getBoundingBox();
}

void Scene::activate() {
    m_accel->build();

    if (!m_integrator)
        throw NoriException("No integrator was specified!");
    if (!m_camera)
        throw NoriException("No camera was specified!");
    
    if (!m_sampler) {
        /* Create a default (independent) sampler */
        m_sampler = static_cast<Sampler*>(
            NoriObjectFactory::createInstance("independent", PropertyList()));
    }

    cout << endl;
    cout << "Configuration: " << toString() << endl;
    cout << endl;
}

void Scene::addChild(NoriObject *obj) {
    switch (obj->getClassType()) {
        case EClassType::EShape: {
                Shape* shape = static_cast<Shape *>(obj);
                m_accel->addShape(shape);
                m_shapes.push_back(shape);

				if (shape->isEmitter())
					m_emitters.push_back(shape->getEmitter()); 
            }
            break;
        
        case EClassType::EEmitter: {
                Emitter *emitter = static_cast<Emitter *>(obj);
				m_emitters.push_back(emitter);
            }
            break;

        case EClassType::ESampler:
            if (m_sampler)
                throw NoriException("There can only be one sampler per scene!");
            m_sampler = static_cast<Sampler *>(obj);
            break;

        case EClassType::ECamera:
            if (m_camera)
                throw NoriException("There can only be one camera per scene!");
            m_camera = static_cast<Camera *>(obj);
            break;
        
        case EClassType::EIntegrator:
            if (m_integrator)
                throw NoriException("There can only be one integrator per scene!");
            m_integrator = static_cast<Integrator *>(obj);
            break;

        default:
            throw NoriException("Scene::addChild(<%s>) is not supported!",
                classTypeName(obj->getClassType()));
    }
}

void Scene::resetCamera(const viewer::Camera* const camera) {
	m_camera->resetCamera(camera);
}

std::string Scene::toString() const {
    std::string meshes;
    for (size_t i=0; i<m_shapes.size(); ++i) {
        meshes += std::string("  ") + indent(m_shapes[i]->toString(), 2);
        if (i + 1 < m_shapes.size())
            meshes += ",";
        meshes += "\n";
    }

    return tfm::format(
        "Scene[\n"
        "  integrator = %s,\n"
        "  sampler = %s\n"
        "  camera = %s,\n"
        "  meshes = {\n"
        "  %s  }\n"
        "]",
        indent(m_integrator->toString()),
        indent(m_sampler->toString()),
        indent(m_camera->toString()),
        indent(meshes, 2)
    );
}

NORI_REGISTER_CLASS(Scene, "scene");
NORI_NAMESPACE_END
