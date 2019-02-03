#version 400 core
out vec4 FragColor;
in vec4 outColor;

uniform vec2 viewport;
uniform int maxIterations;

uniform dvec2 zoom;
uniform dvec2 camera;

void main()
{
	dvec2 c = dvec2(-0.8, 0.156);
	dvec2 Z = dvec2( (gl_FragCoord.x/viewport.x - 0.5), (gl_FragCoord.y/viewport.y - 0.5))*zoom + camera;
	
	int iterations = 0;
	while(dot(Z, Z) <= 3 && iterations < maxIterations) {
		Z = dvec2(Z.x*Z.x - Z.y*Z.y, 2*Z.x*Z.y) + c;
		iterations += 1;
	}
	float intensity = iterations / 1.0 / maxIterations;
	FragColor = vec4(intensity, intensity, intensity, 1.0f);
}
