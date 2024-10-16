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

void main() {
	gl_Position = uProjection * uView * uTransform * iPos;

	vCol = iCol;
	vTexC = iTexC;
	vTexId = iTexId;
}