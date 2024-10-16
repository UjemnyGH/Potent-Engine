#version 450 core

uniform sampler2D uTexture[32];

in vec4 vCol;
in vec2 vTexC;
in float vTexId;

out vec4 oCol;

void main() {
	int texId = int(vTexId);

	vec4 result = vec4(1.0, 1.0, 1.0, 1.0);

	if(texId < 32) {
		result = texture(uTexture[texId], vTexC);
	}

	result = result * vCol;

	if(result.w < 0.1) discard;

	oCol = result;
}