#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <nori/core/common.h>


VIEWER_NAMESPACE_BEGIN

//Shader program contains many shaders
class ShaderProgram
{
public:
	ShaderProgram(std::string vertexShaderName, std::string fragmentShaderName);
	GLuint getId(){ return m_programId; }

private:

	GLuint m_programId;
	bool link();
	bool compileShader(GLenum shaderType, const std::string& filename, GLuint& outShaderId); 
};

VIEWER_NAMESPACE_END