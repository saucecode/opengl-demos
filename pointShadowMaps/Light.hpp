#ifndef sxde_LIGHT_HPP
#define sxde_LIGHT_HPP

#include <glm/glm.hpp>

class Shader;

class Light {
private:

	glm::vec3 position;
	GLuint depthMapFBO;
	float farPlane;

	const unsigned int SHADOW_WIDTH = 512, SHADOW_HEIGHT = 512;

public:

	GLuint cubeMapTex;
	glm::vec3 ambient, diffuse, specular; // colors

	Light(glm::vec3 position);

	void prepareShadowMap();
	void prepareRender(Shader *depthShader);

	inline void setPosition(glm::vec3 position){ this->position = position; };
	inline void setPosition(float x, float y, float z){ position.x = x; position.y = y; position.z = z; };
	inline void setFarPlane(float farPlane){ this->farPlane = farPlane; }

	inline glm::vec3 getPosition(){ return position; }
	inline glm::vec3 getAmbient(){ return ambient; }
	inline glm::vec3 getDiffuse(){ return diffuse; }
	inline glm::vec3 getSpecular(){ return specular; }
	inline float getFarPlane(){ return farPlane; }
};

#endif
