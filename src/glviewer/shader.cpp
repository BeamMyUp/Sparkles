#include "glviewer/shader.h"

#include <iostream>
#include <fstream>

VIEWER_NAMESPACE_BEGIN

Shader::Shader(GLenum type) 
	: m_shaderId(glCreateShader(type))
	, m_type(type)
{
}

Shader::Shader(GLenum type, const std::string& filename) 
	: m_shaderId(glCreateShader(type))
	, m_type(type)
{
	//read file and extract code
	std::ifstream file(filename);

	if (!file.is_open())
	{
		std::cout << "could not create shader: file can't be opened" << std::endl;
		return;
	}

	m_code = std::string((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
}

void Shader::setCode(const std::string& code)
{
	m_code = code;
}

bool Shader::compile()
{
	const char* code = m_code.c_str();
	glShaderSource(m_shaderId, 1, &code, NULL);
	glCompileShader(m_shaderId);

	GLint isCompiled = GL_FALSE;

	GLchar infoLog[512]; 
	glGetShaderiv(m_shaderId, GL_COMPILE_STATUS, &isCompiled);
	if (!isCompiled)
	{
		glGetShaderInfoLog(m_shaderId, 512, nullptr, infoLog);
		std::string errorMessage = "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"; 
		errorMessage.append(infoLog).append("\n");
		throw std::runtime_error(errorMessage);
	}
	
	return isCompiled; 
}

GLuint Shader::getShaderId()
{
	return m_shaderId;
}

VIEWER_NAMESPACE_END