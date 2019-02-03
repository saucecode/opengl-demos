# Fragment Shader Fractals

A simple Mandelbrot set and Julia set explorer, implemented using a GLSL fragment shader, at single and double precision.

Requires a dedicated GPU. Requires GLFW3, and a C++11 compiler. Uses GLAD to load OpenGL functions, and the `glad/` folder included contains all that is needed - no need to mess with GLEW and company.

```
	$ make -j
	$ ./fragfractal mandelbrot2x.frag double
	
	$ ./fragfractal mandelbrot.frag
```

Use WASD to move the camera around. Use X and Z to zoom in and out. Use E and Q to increase and decrease the iterations/granularity of the render.

[Here's a short video of the result](https://www.youtube.com/watch?v=lERBDkf8PkM).

## On Precision

This program includes fragment shaders written for versions `400 core` and `330 core`. Prior to OpenGL 4.0, GLSL shaders had no double precision types by default. You can take note of the huge increase in quality between the two at high zoom levels.

## On shader file extensions

I had to look up what the file extensions for the shaders should be. There's no 'official' answer, but the Khronos Glslang reference compiler uses extensions `.frag` and `.vert`, as well as others as appropriate. I like it, and I'll be using it from now on.

https://www.khronos.org/opengles/sdk/tools/Reference-Compiler/
