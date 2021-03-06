#version 400 core
out vec4 FragColor;

uniform vec2 viewport;
uniform int maxIterations;

uniform dvec2 zoom;
uniform dvec2 camera;

void main()
{
	dvec2 c = dvec2( (gl_FragCoord.x/viewport.x - 0.5), (gl_FragCoord.y/viewport.y - 0.5))*zoom + camera;
	dvec2 Z = c;
	
	// bulb checking optimization
	// https://en.wikipedia.org/wiki/Mandelbrot_set#Cardioid_/_bulb_checking
	double q = (c.x - 0.25)*(c.x - 0.25) + c.y*c.y;
	if(q*(q+c.x-0.25) < c.y*c.y/4.0) {
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);
		return;
	}
	
	int iterations = 0;
	while(dot(Z, Z) <= 3 && iterations < maxIterations) {
		Z = dvec2(Z.x*Z.x - Z.y*Z.y, 2*Z.x*Z.y) + c;
		iterations += 1;
	}
	float intensity = iterations / 1.0 / maxIterations;
	FragColor = vec4(intensity, intensity, intensity, 1.0f);
}
