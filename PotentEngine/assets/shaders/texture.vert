#version 450 core

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uTransform;

layout(location = 0) in vec4 iPos;
layout(location = 1) in vec3 iNor;
layout(location = 2) in vec2 iTexC;
layout(location = 3) in vec4 iCol;
layout(location = 4) in float iTexId;

out vec4 vCol;
out vec2 vTexC;
out float vTexId;
out vec3 vNor;
out vec4 vPos;

void main() {
	vCol = iCol;
	vTexC = iTexC;
	vTexId = iTexId;
	vNor = transpose(inverse(mat3(uTransform))) * iNor;
	vPos = uTransform * iPos;

	gl_Position = uProjection * uView * uTransform * iPos;
}