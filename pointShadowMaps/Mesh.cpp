#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.hpp"
#include "Shader.hpp"

Mesh::Mesh(float *vertices, int sizeofVertices, GLuint *elements, int sizeofElements, GLuint positionAttribute, GLuint normalAttribute, GLuint texcoordAttribute){
	this->color = glm::vec3(1.f, 1.f, 1.f);

	this->material = {
		glm::vec3(1,1,1),
		glm::vec3(1,1,1),
		glm::vec3(1,1,1),
		32.0f
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeofVertices, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeofElements, elements, GL_STATIC_DRAW);

	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(normalAttribute);
	glVertexAttribPointer(texcoordAttribute, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(texcoordAttribute);

	this->sizeofElements = sizeofElements;

}

void Mesh::render(Shader shader){
	glUniformMatrix4fv(glGetUniformLocation(shader.getShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(position * orientation * scale));
	glBindVertexArray(vao);

	shader.setVec3("material.ambient", material.ambient);
	shader.setVec3("material.diffuse", material.diffuse);
	shader.setVec3("material.specular", material.specular);
	shader.setFloat("material.shininess", material.shininess);

	glDrawElements(GL_TRIANGLES, sizeofElements, GL_UNSIGNED_INT, 0);
}

void Mesh::setPosition(float x, float y, float z){
	position = glm::translate(glm::vec3(x,y,z));
	vecPosition.x = x;
	vecPosition.y = y;
	vecPosition.z = z;
}

void Mesh::setScale(float sx, float sy, float sz){
	scale = glm::scale(glm::vec3(sx, sy, sz));
}

void Mesh::setOrientation(float angle, glm::vec3 axis){
	orientation = glm::rotate(angle, axis);
}
