#pragma once

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <nori/core/common.h>

VIEWER_NAMESPACE_BEGIN

class Camera
{
public:

	enum CameraDirection
	{
		eForward = 0x01,
		eBackward = 0x02,
		eLeft = 0x04,
		eRight = 0x08
	};

	Camera(GLfloat aspect, GLfloat nearPlane, GLfloat farPlane, glm::vec3* upVector = nullptr, glm::vec3* initialPosition = nullptr, glm::vec3* initalTarget = nullptr);
	Camera(const glm::mat4& toWorld, GLfloat fovy, GLfloat aspect, GLfloat nearPlane, GLfloat farPlane);
	~Camera() = default;

	void move(CameraDirection displacement, GLfloat deltaTime); 
	void rotate(GLdouble xoffset, GLdouble yoffset, GLboolean constrainPitch = true);
	void zoom(GLdouble yoffest);
	void saveWorldCoordinates(); 
	glm::mat4 GetViewMatrix() const;

	GLfloat getFovy() const { return m_fovy; }
	GLfloat getAspect() const { return m_aspect;}
	GLfloat getNear() const { return m_near; }
	GLfloat getFar() const { return m_far; }

	void setFovy(GLfloat fovy) { m_fovy = fovy; }

	glm::mat4 getLocalMatrix() const { return m_transform; }
	void setLocalMatrix(const glm::mat4& aMatrix) { m_transform = aMatrix; }

protected:

	glm::vec3 m_worldUp; 
	glm::vec3 m_worldRight; 

	// Cameras axis in world coordinates
	glm::mat4 m_transform; 
	bool responsibleForDelete; 

	GLfloat m_mvtSpeed; 
	GLfloat m_rotSpeed;
	GLfloat m_aspect; 
	GLfloat m_fovy;  
	GLfloat m_near;
	GLfloat m_far;
};

VIEWER_NAMESPACE_END