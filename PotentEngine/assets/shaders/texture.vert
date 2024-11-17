#version 450 core

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uTransform;

layout(location = 0) in vec4 iPos;
layout(location = 1) in vec3 iNor;
layout(location = 2) in vec2 iTexC;
layout(location = 3) in vec4 iCol;
layout(location = 4) in float iTexId;

out VS_OUT {
	vec4 vCol;
	vec2 vTexC;
	float vTexId;
	vec3 vNor;
	vec4 vPos;

	mat4 vProjection;
	mat4 vView;
	mat4 vTransform;
} vs_out;

void main() {
	vs_out.vCol = iCol;
	vs_out.vTexC = iTexC;
	vs_out.vTexId = iTexId;
	vs_out.vNor = transpose(inverse(mat3(uTransform))) * iNor;
	vs_out.vPos = uTransform * vec4(iPos.xyz, 1.0);

	vs_out.vProjection = uProjection;
	vs_out.vView = uView;
	vs_out.vTransform = uTransform;

	gl_Position = uProjection * uView * uTransform * iPos;
}