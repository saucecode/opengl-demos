#version 150
in vec3 Color;
in vec3 Normal;
in vec2 Texcoord;
in vec3 FragPos;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float farPlane;
};

#define NR_POINT_LIGHTS 4
uniform Light lights[NR_POINT_LIGHTS];
uniform Material material;

uniform vec3 cameraPosition;
out vec4 outColor;
uniform sampler2D tex;
uniform samplerCube depthMaps[NR_POINT_LIGHTS];

uniform int pointLightCount;

float lengthSquared(vec3 a, vec3 b){
	return dot(a - b, a - b);
}


float shadowCalculation(vec3 fragPos, vec3 lightPos, float farPlane, samplerCube depthMap, vec3 normal, vec3 lightDir){
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // use the light to fragment vector to sample from the depth map
    float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= farPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // now test for shadows
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	// float bias = 0.05;
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

vec3 calculatePointLight(Light light, samplerCube depthMap, vec3 normal, vec3 fragPos, vec3 viewDir){
	float distanceSquared = lengthSquared(light.position, FragPos);
	float attenuation = 1.0 / (1.0 + 0.14 * sqrt(distanceSquared) + 0.07 * distanceSquared);
	vec3 ambient = material.ambient * light.ambient * attenuation;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = (material.specular * spec) * light.specular * attenuation;

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = (diff * material.diffuse) * light.diffuse * attenuation;

	float shadow = shadowCalculation(fragPos, light.position, light.farPlane, depthMap, norm, lightDir);

	return ambient + (1.0 - shadow)*(diffuse + specular);
}

void main(){
	vec3 result = vec3(0.03);
	vec3 viewDir = normalize(cameraPosition - FragPos);

	for(int i=0; i<pointLightCount; i++)
		result += calculatePointLight(lights[i], depthMaps[i], Normal, FragPos, viewDir);

	outColor = texture(tex, Texcoord) * vec4(Color*result, 1.0);
}
