#ifndef sxde_SHADER_HPP
#define sxde_SHADER_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
private:

	GLuint vertexShader, fragmentShader, geometryShader, shaderProgram;
	std::string vertexShaderSource, fragmentShaderSource, geometryShaderSource;

	std::string readFileContents(std::string path);

	bool useGeometryShader;

public:

	Shader(std::string vertexShaderSourceFilename, std::string fragmentShaderSourceFilename);
	Shader(std::string vertexShaderSourceFilename, std::string fragmentShaderSourceFilename, std::string geometryShaderSourceFilename);
	void prepareShader();
	void useProgram();

	void setVec3(std::string uniform, glm::vec3 vector);
	void setVec3(std::string uniform, float x, float y, float z);
	void setMat4(std::string uniform, glm::mat4 matrix);
	void setFloat(std::string uniform, float butterfly);
	void setInt(std::string uniform, int i);

	inline GLuint getShaderProgram(){ return shaderProgram; }
};

#endif
