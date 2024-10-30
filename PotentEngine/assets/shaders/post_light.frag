#version 450 core

struct LightingData {
	vec4 direction;
	// point and spot light
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	// point light
	float constant;
	float linear;
	float quadratic;
	// spotlight
	float cutOff;
	float outerCutOff;
	// light type
	float lightType;
};

// 0 - position (RGB)
// 1 - normal (RGB)
// 2 - diffuse (RGB) specular (A)
uniform sampler2D uTexture[32];
uniform vec4 uViewPosition;
uniform float uMaxLights;

layout(std430, binding = 1) buffer lightData {
	LightingData lightingData[];
};

in vec2 vTexCoords;

out vec4 oColor;
uniform int modifier;

vec3 calculateDirectionalLight(LightingData light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(vec3(-light.direction.xyz));

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	// 16.0 represents shininess
	float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 128.0);

	vec3 ambient = vec3(light.ambient.xyz) * vec3(texture(uTexture[2], vTexCoords).rgb);
	vec3 diffuse = vec3(light.diffuse.xyz) * diff * vec3(texture(uTexture[2], vTexCoords).rgb);
	vec3 specular = vec3(light.specular.xyz) * spec * texture(uTexture[2], vTexCoords).a;

	return (ambient + diffuse + specular);
}

void main() {
	vec3 fragPos = texture(uTexture[0], vTexCoords).rgb;
	vec3 normal = texture(uTexture[1], vTexCoords).rgb;
	vec3 diffuse = texture(uTexture[2], vTexCoords).rgb;
	float specular = texture(uTexture[2], vTexCoords).a;

	vec3 viewDir = normalize(uViewPosition.xyz - fragPos);
	vec3 norm = normalize(normal);

	/*vec3 result = diffuse;
	int diffR = int(result.x * modifier);
	int diffG = int(result.y * modifier);
	int diffB = int(result.z * modifier);

	result = vec3(float(diffR) / modifier, float(diffG) / modifier, float(diffB) / modifier);*/

	vec3 result = calculateDirectionalLight(lightingData[0], norm, viewDir);

	//oColor = vec4(diffuse, 1.0);
	oColor = vec4(result, 1.0);
}