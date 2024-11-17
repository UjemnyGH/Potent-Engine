#version 450 core

struct DirectionalLightData {
	float direction[4];
	float ambient[4];
	float diffuse[4];
	float specular[4];
};

struct PointLightData {
	// point and spot light
	float position[4];

	float ambient[4];
	float diffuse[4];
	float specular[4];

	// point/spot light
	float constant;
	float linear;
	float quadratic;
};

struct SpotLightData {
	float direction[4];
	// point and spot light
	float position[4];
	float ambient[4];
	float diffuse[4];
	float specular[4];

	// point/spot light
	float constant;
	float linear;
	float quadratic;
	// spotlight
	float cutOff;
	float outerCutOff;
};

// 0 - position (RGB)
// 1 - normal (RGB)
// 2 - diffuse (RGB) specular (A)
uniform sampler2D uTexture[32];
uniform vec4 uViewPosition;
uniform int uMaxDirectionalLights;
uniform int uMaxPointLights;
uniform int uMaxSpotLights;

float gamma = 1.0 / 2.2;

layout(std430, binding = 0) buffer directionalLight {
	DirectionalLightData dirLightData[];
};

layout(std430, binding = 1) buffer pointLight {
	PointLightData pointLightData[];
};

layout(std430, binding = 2) buffer spotLight {
	SpotLightData spotLightData[];
};

in vec2 vTexCoords;

out vec4 oColor;

vec3 calculateDirectionalLight(DirectionalLightData light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(-vec3(light.direction[0], light.direction[1], light.direction[2]));

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	// 64.0 represents shininess
	float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 64.0);

	vec3 ambient = vec3(light.ambient[0], light.ambient[1], light.ambient[2]) * vec3(texture(uTexture[2], vTexCoords).xyz);
	vec3 diffuse = vec3(light.diffuse[0], light.diffuse[1], light.diffuse[2]) * diff * vec3(texture(uTexture[2], vTexCoords).xyz);
	vec3 specular = vec3(light.specular[0], light.specular[1], light.specular[2]) * spec * texture(uTexture[2], vTexCoords).w;

	return (ambient + (diffuse + specular) * vec3(diff));
}

vec3 calculatePointLight(PointLightData light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(vec3(light.position[0], light.position[1], light.position[2]) - fragPos.xyz);
	
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 64.0);

	float distance = length(vec3(light.position[0], light.position[1], light.position[2]) - fragPos);
	float attenuation = pow(1.0 / (light.constant + light.linear * distance), gamma);// + light.quadratic * (distance * distance));

	vec3 ambient = vec3(light.ambient[0], light.ambient[1], light.ambient[2]) * vec3(texture(uTexture[2], vTexCoords).xyz);
	vec3 diffuse = vec3(light.diffuse[0], light.diffuse[1], light.diffuse[2]) * diff * vec3(texture(uTexture[2], vTexCoords).xyz);
	vec3 specular = vec3(light.specular[0], light.specular[1], light.specular[2]) * spec * texture(uTexture[2], vTexCoords).w;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return ambient + (diffuse + specular) * vec3(diff);
}

vec3 calculateSpotLight(SpotLightData light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(vec3(light.position[0], light.position[1], light.position[2]) - fragPos);
	
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 64.0);

	float distance = length(vec3(light.position[0], light.position[1], light.position[2]) - fragPos);
	float attenuation = pow(1.0 / (light.constant + light.linear * distance), gamma);// + light.quadratic * (distance * distance));

	float theta = dot(lightDir, normalize(-vec3(light.direction[0], light.direction[1], light.direction[2])));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = vec3(light.ambient[0], light.ambient[1], light.ambient[2]) * vec3(texture(uTexture[2], vTexCoords).xyz);
	vec3 diffuse = vec3(light.diffuse[0], light.diffuse[1], light.diffuse[2]) * diff * vec3(texture(uTexture[2], vTexCoords).xyz);
	vec3 specular = vec3(light.specular[0], light.specular[1], light.specular[2]) * spec * texture(uTexture[2], vTexCoords).w;
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return ambient + (diffuse + specular) * vec3(diff);
}

void main() {	
	vec3 fragPos = texture(uTexture[0], vTexCoords).xyz;
	vec3 normal = texture(uTexture[1], vTexCoords).xyz;
	vec3 diffuse = texture(uTexture[2], vTexCoords).xyz;
	float specular = texture(uTexture[2], vTexCoords).w;

	// + or - (look what works best)
	vec3 viewDir = -normalize(uViewPosition.xyz - fragPos);
	vec3 norm = normalize(normal);

	vec3 result = vec3(0.0, 0.0, 0.0);

	if(uMaxDirectionalLights == 0 && uMaxPointLights == 0 && uMaxSpotLights == 0) {
		result = diffuse;
	}
	
	for(int i = 0; i < uMaxDirectionalLights; i++) {
		result += calculateDirectionalLight(dirLightData[i], norm, viewDir);
	}

	for(int i = 0; i < uMaxPointLights; i++) {
		result += calculatePointLight(pointLightData[i], norm, fragPos, viewDir);
	}

	for(int i = 0; i < uMaxSpotLights; i++) {
		result += calculateSpotLight(spotLightData[i], norm, fragPos, viewDir);
	}
	// / float(uMaxDirectionalLights + uMaxPointLights + uMaxSpotLights)
	oColor = vec4(pow(result, vec3(gamma)), 1.0);
}