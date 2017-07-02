#ifndef sxde_MESH_HPP
#define sxde_MESH_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Shader;

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

class Mesh {
public:

	Mesh(float *vertices, int sizeofVertices, GLuint *elements, int sizeofElements, GLuint positionAttribute, GLuint normalAttribute, GLuint texcoordAttribute);
	void render(Shader shader);

	void setPosition(float x, float y, float z);
	void setScale(float sx, float sy, float sz);
	void setOrientation(float angle, glm::vec3 axis);

	inline void setMaterial(Material mat){ material = mat; }

	inline glm::vec3 getPosition(){ return vecPosition; };

private:

	GLuint vao, ebo, vbo;
	glm::vec3 color;
	glm::vec3 vecPosition;
	int sizeofElements;
	glm::mat4 position, orientation, scale;

	Material material;
};

#endif
