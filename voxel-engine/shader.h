#pragma once

#include "stdafx.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

std::string ReadShaderFile(const GLchar* path)
{
	std::string shaderCode;
	std::ifstream shaderFile;

	shaderFile.exceptions(std::ifstream::badbit);
	try
	{
		shaderFile.open(path);
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n" << std::endl;
	}

	return (shaderCode);
}

GLuint Shader(const GLchar *vertexPath, const GLchar *fragmentPath)
{
	std::string vTemp = ReadShaderFile(vertexPath);
	std::string fTemp = ReadShaderFile(fragmentPath);
	const GLchar *vShaderCode = vTemp.c_str();
	const GLchar *fShaderCode = fTemp.c_str();

	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];

	// Compile vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << vertexPath << std::endl << fragmentPath << std::endl;
		std::cout << "ERROR::SHADER::VERTEX_COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Compile fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << vertexPath << std::endl << fragmentPath << std::endl;
		std::cout << "ERROR::SHADER::FRAGMENT_COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, vertex);
	glAttachShader(ShaderProgram, fragment);
	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ShaderProgram, 512, NULL, infoLog);
		std::cout << vertexPath << std::endl << fragmentPath << std::endl;
		std::cout << "ERROR::SHADER::PROGRAM_LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return (ShaderProgram);
}