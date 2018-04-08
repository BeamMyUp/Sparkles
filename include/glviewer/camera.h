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

	Camera(GLfloat aspect, glm::vec3* upVector = nullptr, glm::vec3* initialPosition = nullptr, glm::vec3* initalTarget = nullptr);
	Camera(const glm::mat4& transformMatrix, GLfloat fovy, GLfloat aspect); 
	~Camera() = default;

	virtual void move(CameraDirection displacement, GLfloat deltaTime); 
	virtual void rotate(GLdouble xoffset, GLdouble yoffset, GLboolean constrainPitch = true);
	virtual void zoom(GLdouble yoffest);
	virtual void saveWorldCoordinates(); 
	virtual glm::mat4 GetViewMatrix();

	virtual GLfloat getFovy() const
	{
		return m_fovy; 
	}
	 
	virtual GLfloat getAspect() const
	{
		return m_aspect;
	}

	virtual void setFovy(GLfloat fovy) { m_fovy = fovy; }
	virtual void setAspect(GLfloat aspect) { m_aspect = aspect; }

	virtual glm::mat4 getLocalMatrix() const { return m_transform; }
	virtual void setLocalMatrix(const glm::mat4& aMatrix) { m_transform = aMatrix; }

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
};

VIEWER_NAMESPACE_END