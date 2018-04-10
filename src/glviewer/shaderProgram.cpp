#include "glviewer/shaderProgram.h"
#include <cassert>
#include <fstream>

VIEWER_NAMESPACE_BEGIN

bool ShaderProgram::compileShader(GLenum type, const std::string& filename, GLuint& outShaderId)
{
	outShaderId = glCreateShader(type);

	//read file and extract code
	std::ifstream file(filename);

	if (!file.is_open())
	{
		std::string errorMessage = "ERROR::SHADER::FILE CAN'T BE READ\n";
		throw std::runtime_error(errorMessage);
	}


	std::string shaderCode = std::string((std::istreambuf_iterator<char>(file)),
										std::istreambuf_iterator<char>());

	const char* code = shaderCode.c_str();
	glShaderSource(outShaderId, 1, &code, NULL);
	glCompileShader(outShaderId);

	GLint isCompiled = GL_FALSE;

	GLchar infoLog[512];
	glGetShaderiv(outShaderId, GL_COMPILE_STATUS, &isCompiled);
	if (!isCompiled)
	{
		glGetShaderInfoLog(outShaderId, 512, nullptr, infoLog);
		std::string errorMessage = "ERROR::SHADER::COMPILATION_FAILED\n";
		errorMessage.append(infoLog).append("\n");
		throw std::runtime_error(errorMessage);
	}

	return isCompiled;
}


bool ShaderProgram::link()
{
	GLint isLinked = GL_FALSE;

	glLinkProgram(m_programId);
	glGetProgramiv(m_programId, GL_LINK_STATUS, &isLinked);

	GLchar infoLog[512];
	if (!isLinked)
	{
		glGetProgramInfoLog(m_programId, 512, nullptr, infoLog);
		std::string errorMessage = "ERROR::SHADER::PROGRAM::LINK_FAILED\n";
		errorMessage.append(infoLog).append("\n");
		throw std::runtime_error(errorMessage);
	}

	return isLinked;

}

ShaderProgram::ShaderProgram(std::string vertexShaderName, std::string fragmentShaderName) 
{
	m_programId = glCreateProgram();

	assert(vertexShaderName.size() > 0, "Attempted to create a Shader program without valid vertex shader filename");
	assert(vertexShaderName.size() > 0, "Attempted to create a Shader program without valid fragment shader filename ");

	GLuint vertexShader = 0, fragmentShader = 0;
	bool c1 = compileShader(GL_VERTEX_SHADER, vertexShaderName, vertexShader);
	bool c2 = compileShader(GL_FRAGMENT_SHADER, fragmentShaderName, fragmentShader);

	assert(c1 && c2);

	//Attach shaders
	glAttachShader(m_programId, vertexShader);
	glAttachShader(m_programId, fragmentShader);

	bool isLinked = link();
	assert(isLinked);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	m_isValid = true;
}

VIEWER_NAMESPACE_END