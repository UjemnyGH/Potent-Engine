#version 450 core

layout(location = 0) out vec4 oPos;
layout(location = 1) out vec4 oNor;
layout(location = 2) out vec4 oAlbedo;

uniform sampler2DArray uTextureArray[32];

in vec4 vCol;
in vec2 vTexC;
in float vTexId;
in vec3 vNor;
in vec4 vPos;

void main() {
	int texId = int(ceil(vTexId));

	vec4 result = vec4(1.0, 1.0, 1.0, 1.0);

	if(texId < 32) {
		result = texture(uTextureArray[texId], vec3(vTexC.xy, 1));
	}

	result *= vCol;

	oAlbedo.rgb = vec3(result.rgb);

	if(texId < 32) {
		oAlbedo.a = texture(uTextureArray[texId], vec3(vTexC.xy, 1)).a;
	}
	else {
		oAlbedo.a = 0.1f;
	}

	oPos = vec4(vPos.xyz, 1.0);
	oNor = vec4(normalize(vNor), 1.0);
}