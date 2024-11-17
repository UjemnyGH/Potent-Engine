#version 450 core

layout(location = 0) out vec4 oPos;
layout(location = 1) out vec4 oNor;
layout(location = 2) out vec4 oAlbedo;

uniform sampler2DArray uTextureArray[32];

in GS_OUT {
	vec4 vCol;
	vec2 vTexC;
	float vTexId;
	vec3 vNor;
	vec4 vPos;

	mat4 vProjection;
	mat4 vView;
	mat4 vTransform;
} fs_in;

/*in vec4 vCol;
in vec2 vTexC;
in float vTexId;
in vec3 vNor;
in vec4 vPos;*/

void main() {
	int texId = int(ceil(fs_in.vTexId));

	vec4 result = vec4(1.0, 1.0, 1.0, 1.0);

	if(texId < 32) {
		result = texture(uTextureArray[texId], vec3(fs_in.vTexC.xy, 0));
	}

	result *= fs_in.vCol;

	oAlbedo.rgb = vec3(result.rgb);

	if(texId < 32) {
		oAlbedo.a = texture(uTextureArray[texId], vec3(fs_in.vTexC.xy, 0)).a;
	}
	else {
		oAlbedo.a = 0.1f;
	}

	oPos = vec4(fs_in.vPos.xyz, 1.0);
	oNor = vec4(normalize(fs_in.vNor), 1.0);
}