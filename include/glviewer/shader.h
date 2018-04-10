#pragma once 

#include <gl/glew.h>
#include "GLfW/glfw3.h"
#include <nori/core/common.h>

#include <string>

VIEWER_NAMESPACE_BEGIN

class Shader{

private:
	std::string m_code;

	GLenum m_type;
	GLuint m_shaderId;

public:
	bool compile();

	void setCode(const std::string& code);

	GLuint getShaderId();

protected:
	Shader(GLenum type, const std::string& filename);
	Shader(GLenum type);
};

VIEWER_NAMESPACE_END