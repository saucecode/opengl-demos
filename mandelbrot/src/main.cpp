#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include <iostream>
#include <map>
#include <thread>
#include <chrono>
#include <string>

#include "Magic.hpp"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int SCREEN_BPP = 32;

const int FRAMES_PER_SECOND = 60;

GLFWwindow *window;
std::map<int, bool> keyboard;

GLuint vao, vbo, ebo;
GLuint shaderProgram;
bool useDoublePrecision = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if(action == GLFW_PRESS) keyboard[key] = true;
	if(action == GLFW_RELEASE) keyboard[key] = false;
}

bool init(char *fragFilename) {
	glfwInit();
	if(useDoublePrecision) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	}else {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	}
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fragment Shader Fractals", nullptr, nullptr);
    glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }
    
    printf ("glGetString (GL_VERSION) returns %s\n", glGetString (GL_VERSION));
    printf("Loading fragment shader from: %s\n", fragFilename);
    printf("Will treat as %s precision", useDoublePrecision ? "double" : "single");
    
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	glfwSetKeyCallback(window, key_callback);

	const char *vsSource = loadFile("default.vert");
	const char *fsSource = loadFile(fragFilename);
	int success;
	char shaderLog[1024];

	GLuint vertexShader, fragShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vsSource, nullptr);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if(!success) {
		glGetShaderInfoLog(vertexShader, 1024, nullptr, shaderLog);
		std::cout << "Vertex shader error: " << shaderLog << std::endl;
	}

	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fsSource, nullptr);
	glCompileShader(fragShader);
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);

	if(!success) {
		glGetShaderInfoLog(fragShader, 1024, nullptr, shaderLog);
		std::cout << "Fragment shader error: " << shaderLog << std::endl;
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// setup VAO

	float vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f
	};

	uint32_t indices[] = {
		0,1,2, 0,2,3
	};

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	return true;
}

void update(float dt) {
	static int iterations = 100;
	static double zoom = 2, camera[2] = {0, 0};
	if(keyboard[GLFW_KEY_E]) {
		std::cout << "Iterations: " << ++iterations << " Zoom: " << 1.0/zoom << " Camera: " << camera[0] << ", " << camera[1] << "\n";
	}
	if(keyboard[GLFW_KEY_Q]) std::cout << --iterations << "\n";
	if(keyboard[GLFW_KEY_W]) camera[1] += 0.5 * dt * zoom;
	if(keyboard[GLFW_KEY_S]) camera[1] -= 0.5 * dt * zoom;
	if(keyboard[GLFW_KEY_D]) camera[0] += 0.5 * dt * zoom;
	if(keyboard[GLFW_KEY_A]) camera[0] -= 0.5 * dt * zoom;
	
	if(keyboard[GLFW_KEY_X]) {
		zoom *= 0.9 * std::max(1.0f, dt * 10);
	}
	if(keyboard[GLFW_KEY_Z]) {
		zoom /= 0.9 * std::max(1.0f, dt * 10);
	}
	if(zoom > 1/0.25) zoom = 1/0.25;

	glUseProgram(shaderProgram);
	
	glUniform2f(glGetUniformLocation(shaderProgram, "viewport"), SCREEN_WIDTH, SCREEN_HEIGHT);
	glUniform1i(glGetUniformLocation(shaderProgram, "maxIterations"), iterations);
	
	if(useDoublePrecision) {
		glUniform2d(glGetUniformLocation(shaderProgram, "zoom"), zoom, zoom);
		glUniform2d(glGetUniformLocation(shaderProgram, "camera"), camera[0], camera[1]);
	}else {
		glUniform2f(glGetUniformLocation(shaderProgram, "zoom"), zoom, zoom);
		glUniform2f(glGetUniformLocation(shaderProgram, "camera"), camera[0], camera[1]);
	}
	
	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int main( int argc, char *argv[] ) {
	
	if(argc < 2) {
		std::cout << "Usage: " << argv[0] << " <shader file> [single|double]\n\n";
		std::cout << "Precision defaults to single.\n";
		std::cout << "double precision works on double precision shaders only, and requires OpenGL 4.0.\n\n";
		std::cout << "Example:    " << argv[0] << " shaders/mandelbrot2x.fs double\n";
		return 1;
	}
	
	bool running = true;
	if(argc == 3) useDoublePrecision = strcmp(argv[2], "double") == 0;
	
	if(!init(argv[1]))
		return 1;

	float dt = 1.0 / FRAMES_PER_SECOND;
	float currentTime = glfwGetTime();
	while(running){
		if(keyboard[GLFW_KEY_ESCAPE]) {
			running = false;
			break;
		}
		
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		dt = glfwGetTime() - currentTime;
		update(dt);
		currentTime = glfwGetTime();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
