#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT {
	vec4 vCol;
	vec2 vTexC;
	float vTexId;
	vec3 vNor;
	vec4 vPos;

	mat4 vProjection;
	mat4 vView;
	mat4 vTransform;
} gs_in[];

out GS_OUT {
	vec4 vCol;
	vec2 vTexC;
	float vTexId;
	vec3 vNor;
	vec4 vPos;

	mat4 vProjection;
	mat4 vView;
	mat4 vTransform;
} gs_out;

void main() {
	for(int i = 0; i < 3; i++) {
		gs_out.vCol = gs_in[i].vCol;
		gs_out.vTexC = gs_in[i].vTexC;
		gs_out.vTexId = gs_in[i].vTexId;
		gs_out.vNor = gs_in[i].vNor;
		gs_out.vPos = gs_in[i].vPos;

		gs_out.vProjection = gs_in[i].vProjection;
		gs_out.vView = gs_in[i].vView;
		gs_out.vTransform = gs_in[i].vTransform;

		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}

	EndPrimitive();
}