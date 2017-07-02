# point shadow maps

An implementation of shadow maps for *only* point lights using OpenGL 3.2. Supports an arbitrary numbers of point lights.

## The technique

We have a scene with a bunch of meshes and a bunch of point lights. For each point light, we create a six sided cube map. This normally requires the program to render the scene six times per point light. The program utilizes a geometry shader (depth.gs) in order to do generate all six sides in one render pass. Each render pass for the point light generates a depthMap, and can be calculated every frame in real time.

When rendering the scene, the 'camera' shader takes each Light's depthMap and determines if a fragment is in shade or light as part of the lighting calculation.

The number of points lights you can have is limited by your GPU. It is the value given by `GL_MAX_TEXTURE_UNITS` and is printed when the program starts. Each point light has a cubemap, and so takes up 6 texture units.

The resolution of the shadowmap can be set in Light.hpp.