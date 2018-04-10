#define GLM_SWIZZLE

#include "glviewer/camera.h"
#include "glm/gtc/matrix_transform.hpp"

VIEWER_NAMESPACE_BEGIN

Camera::Camera(GLfloat aspect, GLfloat nearPlane, GLfloat farPlane, glm::vec3* upVector, glm::vec3* initialPosition, glm::vec3* initialTarget)
	: m_mvtSpeed(5.0f)
	, m_rotSpeed(0.001f)
	, m_fovy(45.0f)
	, m_aspect(aspect)
	, responsibleForDelete(true)
	, m_near(nearPlane)
	, m_far(farPlane)
{
	if (upVector)
		m_worldUp = *upVector;
	else
		m_worldUp = glm::vec3(0, 1, 0);

	glm::vec3 axisX, axisZ, position;

	if (initialPosition)
		position = *initialPosition;
	else
		position = glm::vec3(0, 0, 0);
	
	glm::vec3 target(0, 0, 0); 
	if (initialTarget)
		target = *initialTarget;

	axisZ = glm::normalize(target - position);
	assert(std::abs(glm::dot(axisZ, m_worldUp)) != 1 && "WorldUp and Cam's Z are colinear. No possible way to determine Cam's base");

	axisX = glm::normalize(glm::cross(m_worldUp, axisZ));

	m_transform[0] = glm::vec4(axisX, 0.0);
	m_transform[1] = glm::vec4(glm::normalize(glm::cross(axisZ, axisX)), 0.0);
	m_transform[2] = glm::vec4(axisZ, 0.0);
	m_transform[3] = glm::vec4(position, 1.0);
	 
	//computePitchAndYaw();
}

Camera::Camera(const glm::mat4& toWorld, GLfloat fovy, GLfloat aspect, GLfloat nearPlane, GLfloat farPlane)
	: m_mvtSpeed(15.0f)
	, m_rotSpeed(0.001f)
	, m_fovy(fovy)
	, m_aspect(aspect)
	, responsibleForDelete(false)
	, m_near(nearPlane)
	, m_far(farPlane)
{
	m_transform = toWorld;
	m_worldUp = toWorld[1].xyz(); 


}

glm::mat4 Camera::GetViewMatrix() const
{
	// If inheratage of transform is implemented, change 
	// m_transform in these calculations for the world transform. 
	glm::vec3 worldPosition = m_transform[3].xyz(); 
	glm::vec3 worldCenter = worldPosition + m_transform[2].xyz();
	glm::vec3 worldUp = m_transform[1].xyz(); 

	return glm::lookAt(worldPosition, worldCenter, worldUp);
}

void Camera::move(CameraDirection direction, GLfloat deltaTime)
{
	GLfloat velocity = m_mvtSpeed * deltaTime; 

	if (direction == CameraDirection::eForward)
		m_transform[3] += m_transform[2] * velocity;
	if (direction == CameraDirection::eBackward)
		m_transform[3] -= m_transform[2] * velocity;
	if (direction == CameraDirection::eLeft)
		m_transform[3] += m_transform[0] * velocity;
	if (direction == CameraDirection::eRight)
		m_transform[3] -= m_transform[0] * velocity;
}

void Camera::rotate(GLdouble xoffset, GLdouble yoffset, GLboolean constrainPitch /* = true */)
{
	float yaw = -(float)(xoffset * m_rotSpeed);
	float pitch = -(float)(yoffset * m_rotSpeed);

	glm::mat4 matrix = m_transform; 

	glm::mat4 yawMat;
	yawMat = glm::rotate(yawMat, yaw, glm::vec3(0, 1, 0));

	glm::vec4 position = matrix[3]; 

	matrix =  yawMat * matrix;
	matrix[3] = position; 
	
	matrix = glm::rotate(matrix, pitch, glm::vec3(1, 0, 0));

	m_transform = matrix;
}

void Camera::zoom(GLdouble yoffset)
{
	if (m_fovy >= 1.0f && m_fovy <= 45.0f)
		m_fovy -= yoffset; 
	if (m_fovy <= 1.0f)
		m_fovy = 1.0f;
	if (m_fovy >= 45.0f)
		m_fovy = 45.0f; 
}

void Camera::saveWorldCoordinates()
{
	m_worldUp = m_transform[1].xyz();
}

VIEWER_NAMESPACE_END