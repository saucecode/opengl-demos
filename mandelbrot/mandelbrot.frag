#version 330 core
out vec4 FragColor;

uniform vec2 viewport;
uniform int maxIterations;

uniform vec2 zoom;
uniform vec2 camera;

void main()
{
	vec2 c = vec2( (gl_FragCoord.x/viewport.x - 0.5), (gl_FragCoord.y/viewport.y - 0.5))*zoom + camera;
	vec2 Z = c;
	
	// bulb checking optimization
	// https://en.wikipedia.org/wiki/Mandelbrot_set#Cardioid_/_bulb_checking
	float q = (c.x - 0.25)*(c.x - 0.25) + c.y*c.y;
	if(q*(q+c.x-0.25) < c.y*c.y/4.0) {
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);
		return;
	}
	
	int iterations = 0;
	while(dot(Z, Z) <= 3 && iterations < maxIterations) {
		Z = vec2(Z.x*Z.x - Z.y*Z.y, 2*Z.x*Z.y) + c;
		iterations += 1;
	}
	float intensity = iterations / 1.0 / maxIterations;
	FragColor = vec4(intensity, intensity, intensity, 1.0f);
}
