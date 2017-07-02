#version 150
in vec3 position;
in vec3 normal;
in vec2 texcoord;

out vec3 Normal;
out vec3 Color;
out vec2 Texcoord;
out vec3 FragPos;

uniform vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	gl_Position = projection * view * model * vec4(position, 1.0);
	Color = color;
	Texcoord = texcoord;
	FragPos = vec3(model * vec4(position, 1.0));
	Normal = mat3(transpose(inverse(model))) * normal;
}
