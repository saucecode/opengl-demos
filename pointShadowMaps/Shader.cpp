#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"

#include <string>
#include <iostream>
#include <cstdio>
#include <cstring>

Shader::Shader(std::string vertexShaderSourceFilename, std::string fragmentShaderSourceFilename){
	this->vertexShaderSource = readFileContents(vertexShaderSourceFilename);
	this->fragmentShaderSource = readFileContents(fragmentShaderSourceFilename);
	this->useGeometryShader = false;
}

Shader::Shader(std::string vertexShaderSourceFilename, std::string fragmentShaderSourceFilename, std::string geometryShaderSourceFilename){
	this->vertexShaderSource = readFileContents(vertexShaderSourceFilename);
	this->fragmentShaderSource = readFileContents(fragmentShaderSourceFilename);
	this->geometryShaderSource = readFileContents(geometryShaderSourceFilename);
	this->useGeometryShader = true;
}

void Shader::prepareShader(){

	const GLchar *vertexSource = vertexShaderSource.c_str();
	const GLchar *fragmentSource = fragmentShaderSource.c_str();
	const GLchar *geometrySource = geometryShaderSource.c_str();

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);

	char buffer[512];
	glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
	std::cout << "Vertex shader compilation output: (Success: " << (status == GL_TRUE) << ")\n" << std::string(buffer) << "\n--------\n";

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	std::memset(buffer, '\0', 512);
	glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
	std::cout << "Fragment shader compilation output: (Success: " << (status == GL_TRUE) << ")\n" << std::string(buffer) << "\n--------\n";

	if(useGeometryShader){
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometrySource, NULL);
		glCompileShader(geometryShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
		std::memset(buffer, '\0', 512);
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
		std::cout << "Geometry shader compilation output: (Success: " << (status == GL_TRUE) << ")\n" << std::string(buffer) << "\n--------\n";
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	if(useGeometryShader) glAttachShader(shaderProgram, geometryShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");

	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	std::memset(buffer, '\0', 512);
	glGetProgramInfoLog(shaderProgram, 512, NULL, buffer);
	std::cout << "Shader linking output: (Success: " << (status == GL_TRUE) << ")\n" << std::string(buffer) << "\n--------\n";

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if(useGeometryShader) glDeleteShader(geometryShader);
}

void Shader::setVec3(std::string uniform, glm::vec3 vector){
	setVec3(uniform, vector.x, vector.y, vector.z);
}

void Shader::setVec3(std::string uniform, float x, float y, float z){
	GLuint uniformLocation = glGetUniformLocation(this->shaderProgram, uniform.c_str());
	glUniform3f(uniformLocation, x,y,z);
}

void Shader::setFloat(std::string uniform, float butterfly){
	GLuint uniformLocation = glGetUniformLocation(this->shaderProgram, uniform.c_str());
	glUniform1f(uniformLocation, butterfly);
}

void Shader::setInt(std::string uniform, int value){
	GLuint uniformLocation = glGetUniformLocation(this->shaderProgram, uniform.c_str());
	glUniform1i(uniformLocation, value);
}

void Shader::setMat4(std::string uniform, glm::mat4 matrix){
	GLuint uniformLocation = glGetUniformLocation(this->shaderProgram, uniform.c_str());
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::useProgram(){
	glUseProgram(this->shaderProgram);
}

std::string Shader::readFileContents(std::string path){
	std::FILE *fp = std::fopen(path.c_str(), "r");

	std::string contents;
    std::fseek(fp, 0, SEEK_END);
    contents.resize(std::ftell(fp));
    std::rewind(fp);
    std::fread(&contents[0], 1, contents.size(), fp);
    std::fclose(fp);
    return(contents);
}
