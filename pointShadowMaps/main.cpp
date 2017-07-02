#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL/SOIL.h>

#include <thread>
#include <iostream>
#include <cmath>
#include <vector>

#include "Shader.hpp"
#include "Mesh.hpp"
#include "Light.hpp"

class Program {
private:
	GLFWwindow *window;

	Shader *shader, *depthShader;

	GLint posAttrib, normalAttrib, texAttrib;
	GLint uniView, uniProjection, uniColor, uniCameraPosition;

	GLuint tex, whiteTex;


	Mesh *meshFloor;
	Mesh *cube, *meshWall, *meshGuard;

	Light *starLight;
	std::vector<Light*> sceneLights;

	float yaw, pitch, speed;
	glm::vec3 camera, direction;

	glm::mat4 viewMatrix, projectionMatrix;

	int frameCounter, framerate;
	float dt;
	double previousTime[2];

	int WINDOW_WIDTH, WINDOW_HEIGHT;

public:
	Program(GLFWwindow* window){
		this->window = window;
		glfwGetFramebufferSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);

		shader = new Shader("camera.vs", "camera.fs");
		shader->prepareShader();
		shader->useProgram();
		// NOTE -- You have to declare as many elements as are in the NR_POINT_LIGHTS in the fragment shader, or else scene renders black.
		shader->setInt("depthMaps[0]", 1);
		shader->setInt("depthMaps[1]", 2);
		shader->setInt("depthMaps[2]", 3);
		shader->setInt("depthMaps[3]", 4);
		shader->setInt("tex", 0);

		starLight = new Light(glm::vec3(0.0f, 0.0f, 0.0f));
		sceneLights.push_back(starLight);
		sceneLights.push_back(new Light(glm::vec3(0,0,0.5f)));

		sceneLights.push_back(new Light(glm::vec3(0,-11.f,1.7f)));
		sceneLights[2]->diffuse = glm::vec3(1.0f,1.0f,0.0f) * 0.3f;
		sceneLights[2]->specular = glm::vec3(0.0f);
		sceneLights[2]->setFarPlane(15.0f);

		depthShader = new Shader("depth.vs", "depth.fs", "depth.gs");
		depthShader->prepareShader();

		posAttrib = glGetAttribLocation(shader->getShaderProgram(), "position");
		normalAttrib = glGetAttribLocation(shader->getShaderProgram(), "normal");
		texAttrib = glGetAttribLocation(shader->getShaderProgram(), "texcoord");

		uniView = glGetUniformLocation(shader->getShaderProgram(), "view");
		uniProjection = glGetUniformLocation(shader->getShaderProgram(), "projection");
		uniColor = glGetUniformLocation(shader->getShaderProgram(), "color");

		uniCameraPosition = glGetUniformLocation(shader->getShaderProgram(), "cameraPosition");


		glGenTextures(1, &tex);
		glGenTextures(1, &whiteTex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		int width, height;
		unsigned char* image = SOIL_load_image("image.png", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);

		glBindTexture(GL_TEXTURE_2D, whiteTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		float solidWhitePixel[] = {1.f,1.f,1.f};
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_FLOAT, solidWhitePixel);


		float vertices3[] = {
	        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
	        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
	        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
	        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
	        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
	        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

	        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
	        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
	        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
	        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
	        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
	        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

	        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
	        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
	        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

	        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
	        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
	        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

	        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
	        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
	        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
	        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
	        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
	        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,

	        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
	        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
	        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
	        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
	        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
	        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f
		};

		GLuint elements3[] = {
			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35
		};

		meshFloor = new Mesh(vertices3, sizeof(vertices3), elements3, sizeof(elements3), posAttrib, normalAttrib, texAttrib);
		meshFloor->setScale(70,70,0.2f);

		meshGuard = new Mesh(vertices3, sizeof(vertices3), elements3, sizeof(elements3), posAttrib, normalAttrib, texAttrib);
		meshGuard->setScale(10.0f,0.2f,10.0f);
		meshGuard->setPosition(0,-10,0);

		cube = new Mesh(vertices3, sizeof(vertices3), elements3, sizeof(elements3), posAttrib, normalAttrib, texAttrib);
		meshWall = new Mesh(vertices3, sizeof(vertices3), elements3, sizeof(elements3), posAttrib, normalAttrib, texAttrib);
		meshWall->setPosition(-3, 2, 3);
		meshWall->setScale(1,1,6);


		camera = glm::vec3(-1.f, -1.f, 1.2f);
		direction = glm::vec3(-1.0f,0,0);
		yaw = 0;
		pitch = 0;
		speed = 3.0f;


		projectionMatrix = glm::perspective(glm::radians(90.0f), WINDOW_WIDTH/1.0f/WINDOW_HEIGHT, 0.1f, 100.0f);

		frameCounter = 0;
		framerate = 0;
		previousTime[0] = glfwGetTime();

		for(Light *light : sceneLights)
			light->prepareShadowMap();

		shader->setInt("pointLightCount", sceneLights.size());
	}

	void update(){
		calculateFramerate();
		checkKeyboardInput();

		if(pitch > glm::radians(89.f)) pitch = glm::radians(89.f);
		if(pitch < -glm::radians(89.f)) pitch = -glm::radians(89.f);

		shader->setInt("pointLightCount", sceneLights.size());
	}

	void calculateFramerate(){
		dt = glfwGetTime() - previousTime[1];
		previousTime[1] = glfwGetTime();
		frameCounter++;
		if(previousTime[1] - previousTime[0] >= 1.0){
			framerate = frameCounter;
			frameCounter = 0;
			previousTime[0] = previousTime[1];
			std::cout << "Recorded FPS: " << framerate << "\n";
		}
	}

	void checkKeyboardInput(){
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);

		if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
			yaw += 0.05f;
		}
		if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
			yaw -= 0.05f;
		}
		if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
			pitch += 0.05f;
		}
		if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
			pitch -= 0.05f;
		}
		if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
			camera.x += direction.x * dt * speed;
			camera.y += direction.y * dt * speed;
		}
		if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
			camera.x += -direction.x * dt * speed;
			camera.y += -direction.y * dt * speed;
		}
		if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
			camera.x += -direction.y * dt * speed;
			camera.y += direction.x * dt * speed;
		}
		if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
			camera.x += direction.y * dt * speed;
			camera.y += -direction.x * dt * speed;
		}

	}

	void render(){
		for(Light *light : sceneLights){
			light->prepareRender(depthShader);
			glCullFace(GL_FRONT);
			renderSceneDefault(*depthShader, true);
			glCullFace(GL_BACK);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader->useProgram();

		// matrices for the camera
		projectionMatrix = glm::perspective(glm::radians(90.0f), WINDOW_WIDTH/1.0f/WINDOW_HEIGHT, 0.1f, 100.0f);
		viewMatrix = glm::lookAt(camera, camera+direction, glm::vec3(0.0f, 0.0f, 1.0f));

		for(unsigned int i=0; i<sceneLights.size(); i++){
			glActiveTexture(GL_TEXTURE1 + i);
			glBindTexture(GL_TEXTURE_CUBE_MAP, sceneLights[i]->cubeMapTex);
		}
		shader->setMat4("view", viewMatrix);
		shader->setMat4("projection", projectionMatrix);
		shader->setVec3("color", 1,1,1);
		shader->setVec3("cameraPosition", camera);

		for(unsigned int i=0; i<sceneLights.size(); i++){
			shader->setVec3("lights[" + std::to_string(i) + "].ambient", sceneLights[i]->getAmbient());
			shader->setVec3("lights[" + std::to_string(i) + "].diffuse", sceneLights[i]->getDiffuse());
			shader->setVec3("lights[" + std::to_string(i) + "].specular", sceneLights[i]->getSpecular());
			shader->setVec3("lights[" + std::to_string(i) + "].position", sceneLights[i]->getPosition());
			shader->setFloat("lights[" + std::to_string(i) + "].farPlane", sceneLights[i]->getFarPlane());
		}

		renderSceneDefault(*shader, false);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	void renderSceneDefault(Shader activeShader, bool shadowPass){
		if(shadowPass){/* suppress warnings */}
		direction.x = cos(yaw);
		direction.y = sin(yaw);
		direction.z = tan(pitch);

		sceneLights[0]->setPosition(6 * cos(previousTime[1]), 6 * sin(previousTime[1]), 5.f);
		int i=0;
		for(Light *light : sceneLights){
			activeShader.setVec3("lights[" + std::to_string(i) + "].position", light->getPosition());
			i++;
		}

		cube->setPosition(0,0,1.5f);
		cube->setOrientation(previousTime[1] * 0.5f, glm::vec3(0,1,1));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		cube->render(activeShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, whiteTex);
		// if(!shadowPass) meshLight->render(activeShader);


		meshFloor->setPosition(0,0,0);
		meshFloor->render(activeShader);
		meshFloor->setPosition(0,0,8);
		meshFloor->render(activeShader);
		meshWall->render(activeShader);
		meshGuard->render(activeShader);
	}
};

int main() {

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "OpenGL", nullptr, nullptr); // Windowed

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	std::cout << "We have " << GL_MAX_TEXTURE_UNITS << " texture units." << "\n";

	Program program(window);

	glEnable(GL_DEPTH_TEST);

	while(!glfwWindowShouldClose(window)) {
		program.update();
		program.render();
		std::this_thread::sleep_for(std::chrono::milliseconds(8));
	}

    glfwTerminate();

	return 0;
}
