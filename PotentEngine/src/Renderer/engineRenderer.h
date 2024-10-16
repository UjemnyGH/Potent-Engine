#pragma once
#ifndef _POTENT_ENGINE_RENDERER_
#define _POTENT_ENGINE_RENDERER_

#include "engineRendererData.h"

namespace potent {
	const int TEXTURE_INDICES[32] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };

	const char* DEBUG_DRAW_VERTEX_SHADER =
		"#version 450 core\n"
		"uniform mat4 uProjection;\n"
		"uniform mat4 uView;\n"
		"uniform mat4 uTransform;\n"
		"layout(location = 0) in vec4 iPos;\n"
		"void main() {\ngl_Position = uProjection * uView * uTransform * iPos;\n}\n";

	const char* DEBUG_DRAW_FRAGMNET_SHADER =
		"#version 450 core\n"
		"out vec4 oCol;\n"
		"void main() {\noCol = vec4(0.0, 1.0, 0.0, 1.0);\n}\n";

	class Renderer {
	private:
		ShaderProgram mShaderProgram;
		std::vector<Shader*> mShaders;

	public:
		std::string name;

		// Get renderer shader program pointer
		ShaderProgram* operator()() { return &mShaderProgram; }

		// Attach shaders to shader program
		void attachShader(Shader* shader) {
			mShaders.push_back(shader);
		}

		// Clear shaders from shaders queue
		void clearShaders() {
			mShaders.clear();
		}

		// Relink all shaders in queue
		void relinkShaderProgram() {
			mShaderProgram.~ShaderProgram();

			if (!mShaderProgram.created) {
				for (Shader* shader : mShaders) {
					mShaderProgram.attach(shader);
				}

				mShaderProgram.link();
			}
		}

		// Use max amount of textures (32)  with mipmaps
		void useMaxTexture() {
			mShaderProgram.use();

			glUniform1iv(glGetUniformLocation(mShaderProgram.id, "uTexture"), 32, TEXTURE_INDICES);

			mShaderProgram.unuse();
		}

		// Use max amount of texture arrays (32) without mipmaps but with more storage
		void useMaxTextureArray() {
			mShaderProgram.use();

			glUniform1iv(glGetUniformLocation(mShaderProgram.id, "uTextureArray"), 32, TEXTURE_INDICES);

			mShaderProgram.unuse();
		}

		// Render data
		void render(RenderData* pRenderData, RMat projectionMatrix, RMat viewMatrix, RMat transformMatrix = RMat::Identity()) {
			mShaderProgram.use();
			pRenderData->vertexArray.bind();

			for (int i = 0; i < 32; i++) {
				if (pRenderData->texturesPtr[i] != nullptr) {
					pRenderData->texturesPtr[i]->textureBuffer.bindUnit(i);
				}
			}

			glUniformMatrix4fv(glGetUniformLocation(mShaderProgram.id, "uProjection"), 1, 0, projectionMatrix.m);
			glUniformMatrix4fv(glGetUniformLocation(mShaderProgram.id, "uView"), 1, 0, viewMatrix.m);
			glUniformMatrix4fv(glGetUniformLocation(mShaderProgram.id, "uTransform"), 1, 0, transformMatrix.m);

			glDrawArrays(GL_TRIANGLES, 0, pRenderData->normalsOffset);

			pRenderData->vertexArray.unbind();
			mShaderProgram.unuse();
		}
	};

	void debugDrawPoint(RVec point, RMat projection, RMat view, RMat transform = RMat::Identity()) {
		std::vector<float> p = { point.x, point.y, point.z, point.w };

		Shader vs, fs;
		vs.loadFromMemory(DEBUG_DRAW_VERTEX_SHADER, GL_VERTEX_SHADER);
		fs.loadFromMemory(DEBUG_DRAW_FRAGMNET_SHADER, GL_FRAGMENT_SHADER);
		ShaderProgram sp;
		sp.attach(&vs);
		sp.attach(&fs);
		sp.link();

		VArray vao;
		VBuffer vbo;
		
		sp.use();
		vao.bind();

		vbo.bindPlace(3, 0);
		vbo.bindData(p);

		glUniformMatrix4fv(glGetUniformLocation(sp.id, "uProjection"), 1, 0, projection.m);
		glUniformMatrix4fv(glGetUniformLocation(sp.id, "uView"), 1, 0, view.m);
		glUniformMatrix4fv(glGetUniformLocation(sp.id, "uTransform"), 1, 0, transform.m);

		glDrawArrays(GL_POINTS, 0, 1);

		vao.unbind();
		sp.unuse();

		vs.~Shader();
		fs.~Shader();
		sp.~ShaderProgram();
		vao.~VArray();
		vbo.~VBuffer();
	}

	void debugDrawModel(RenderObjectData* model, RMat projection, RMat view, bool transposeTransform = 1) {
		Shader vs, fs;
		vs.loadFromMemory(DEBUG_DRAW_VERTEX_SHADER, GL_VERTEX_SHADER);
		fs.loadFromMemory(DEBUG_DRAW_FRAGMNET_SHADER, GL_FRAGMENT_SHADER);
		ShaderProgram sp;
		sp.attach(&vs);
		sp.attach(&fs);
		sp.link();

		VArray vao;
		VBuffer vbo;

		sp.use();
		vao.bind();

		vbo.bindPlace(3, 0);
		vbo.bindData(model->meshData.vertices);

		glUniformMatrix4fv(glGetUniformLocation(sp.id, "uProjection"), 1, 0, projection.m);
		glUniformMatrix4fv(glGetUniformLocation(sp.id, "uView"), 1, 0, view.m);
		glUniformMatrix4fv(glGetUniformLocation(sp.id, "uTransform"), 1, transposeTransform, model->transform.GetTransform().m);

		glDrawArrays(GL_LINES, 0, model->meshData.vertices.size() / 3);

		vao.unbind();
		sp.unuse();

		vs.~Shader();
		fs.~Shader();
		sp.~ShaderProgram();
		vao.~VArray();
		vbo.~VBuffer();
	}

	void debugDrawLine(RVec start, RVec end, RMat projection, RMat view, RMat transform = RMat::Identity()) {
		std::vector<float> p = { 
			start.x, start.y, start.z,
			end.x, end.y, end.z
		};

		Shader vs, fs;
		vs.loadFromMemory(DEBUG_DRAW_VERTEX_SHADER, GL_VERTEX_SHADER);
		fs.loadFromMemory(DEBUG_DRAW_FRAGMNET_SHADER, GL_FRAGMENT_SHADER);
		ShaderProgram sp;
		sp.attach(&vs);
		sp.attach(&fs);
		sp.link();

		VArray vao;
		VBuffer vbo;

		sp.use();
		vao.bind();

		vbo.bindPlace(3, 0);
		vbo.bindData(p);

		glUniformMatrix4fv(glGetUniformLocation(sp.id, "uProjection"), 1, 0, projection.m);
		glUniformMatrix4fv(glGetUniformLocation(sp.id, "uView"), 1, 0, view.m);
		glUniformMatrix4fv(glGetUniformLocation(sp.id, "uTransform"), 1, 0, transform.m);

		glDrawArrays(GL_LINES, 0, 2);

		vao.unbind();
		sp.unuse();

		vs.~Shader();
		fs.~Shader();
		sp.~ShaderProgram();
		vao.~VArray();
		vbo.~VBuffer();
	}

	void debugDrawTriangle(RVec t1, RVec t2, RVec t3, RMat projection, RMat view, RMat transform = RMat::Identity()) {
		std::vector<float> p = {
			t1.x, t1.y, t1.z,
			t2.x, t2.y, t2.z,
			t3.x, t3.y, t3.z
		};

		Shader vs, fs;
		vs.loadFromMemory(DEBUG_DRAW_VERTEX_SHADER, GL_VERTEX_SHADER);
		fs.loadFromMemory(DEBUG_DRAW_FRAGMNET_SHADER, GL_FRAGMENT_SHADER);
		ShaderProgram sp;
		sp.attach(&vs);
		sp.attach(&fs);
		sp.link();

		VArray vao;
		VBuffer vbo;

		sp.use();
		vao.bind();

		vbo.bindPlace(3, 0);
		vbo.bindData(p);

		glUniformMatrix4fv(glGetUniformLocation(sp.id, "uProjection"), 1, 0, projection.m);
		glUniformMatrix4fv(glGetUniformLocation(sp.id, "uView"), 1, 0, view.m);
		glUniformMatrix4fv(glGetUniformLocation(sp.id, "uTransform"), 1, 0, transform.m);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		vao.unbind();
		sp.unuse();

		vs.~Shader();
		fs.~Shader();
		sp.~ShaderProgram();
		vao.~VArray();
		vbo.~VBuffer();
	}
}

#endif