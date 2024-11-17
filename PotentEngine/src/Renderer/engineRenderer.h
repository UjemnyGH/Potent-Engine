#pragma once
#ifndef _POTENT_ENGINE_RENDERER_
#define _POTENT_ENGINE_RENDERER_

#include "engineRendererData.h"
#include "../Physics/engineCollisions.h"

namespace potent {
	const int TEXTURE_INDICES[32] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };

	const MeshRawData SQUARE_MESH = {
		{
				1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
				1.0f, -1.0f, 0.0f,

			-1.0f,  1.0f, 0.0f,
				1.0f, -1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f
		},
		{
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f
		},
		{
			1.0f, 1.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,

			0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
		}
	};

	class Renderer {
	private:
		ShaderProgram mShaderProgram;
		std::vector<Shader*> mShaders;
		bool mUseTextureArrays = false;

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
			mUseTextureArrays = false;
			mShaderProgram.use();

			glUniform1iv(glGetUniformLocation(mShaderProgram.id, "uTexture"), 32, TEXTURE_INDICES);

			mShaderProgram.unuse();
		}

		// Use max amount of texture arrays (32) without mipmaps but with more storage
		void useMaxTextureArray() {
			mUseTextureArrays = true;
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
					if (mUseTextureArrays) {
						pRenderData->texturesPtr[i]->textureArrayBuffer.bindUnit(i);
					}
					else {
						pRenderData->texturesPtr[i]->textureBuffer.bindUnit(i);
					}
				}
			}

			glUniformMatrix4fv(glGetUniformLocation(mShaderProgram.id, "uProjection"), 1, 0, projectionMatrix.m);
			glUniformMatrix4fv(glGetUniformLocation(mShaderProgram.id, "uView"), 1, 0, viewMatrix.m);
			glUniformMatrix4fv(glGetUniformLocation(mShaderProgram.id, "uTransform"), 1, 0, transformMatrix.m);

			glDrawArrays(GL_TRIANGLES, 0, pRenderData->normalsOffset / 3);

			pRenderData->vertexArray.unbind();
			mShaderProgram.unuse();
		}
	};

	// Post process renderer (wrap of potent::Renderer with additional functionalities)
	class PostProcessRenderer : public Renderer {
	private:
		GeometryBuffer mStageGeometryBuffer;
		RenderObjectData mSquareObject;
		RenderData mStageRender;
		std::array<Texture, 32> mStageTextureData;
		std::array<std::uint32_t, 32> mStageTextureOrder = {
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
			16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
		};

		bool mCreated = false;

	public:
		// Just a constructor
		PostProcessRenderer() {
			mSquareObject.makeModel(SQUARE_MESH);
			mSquareObject.name = "POST_PROCESS_SQUARE_MESH";

			mStageRender.name = "POST_PROCESS_RENDER";
			mStageRender.meshData.push_back(&mSquareObject);
			mStageRender.joinData();
		}

		// Gets desired texture as pointer on index
		Texture* getStageTexturePtr(std::size_t index) { return &mStageTextureData[index]; }
		// Gets stage render data as pointer
		RenderData* getStageRenderDataPtr() { return &mStageRender; }
		// Gets stage geometry buffer as pointer
		GeometryBuffer* getStageGeometryBufferPtr() { return &mStageGeometryBuffer; }

		// Bind custom order to post processing textures (Texture Ids are from 0 to 31!, NOT GL TEXTURE ID)
		void bindCustomTextureOrder(std::uint32_t postProcessTextureId, std::uint32_t geometryBufferTextureId) {
			// Recreate stage (not needed)
			//if(mStageTextureOrder[postProcessTextureId] != geometryBufferTextureId) mCreated = false;
			
			mStageTextureOrder[postProcessTextureId] = geometryBufferTextureId;
		}

		// Resets custom texture order to default stage (0, 1, 2, ..., 30, 31)
		void resetCustomOrder() {
			for (std::size_t i = 0; i < 32; i++) {
				// Recreate stage (not needed)
				//if (mStageTextureOrder[i] != i) mCreated = false;

				mStageTextureOrder[i] = i;
			}
		}

		// Recreate stage render data
		void recreateStage() {
			if (mCreated) {
				mCreated = false;
			}
		}

		// Make stage renderable (needs to be in render loop before renderStage())
		void makeStageRenderable() {
			if (!mCreated) {
				relinkShaderProgram();
				useMaxTexture();
				mStageRender.bindData();

				mCreated = true;
			}
		}

		// Wrapper of potent::GeometryBuffer::initGraphicsBuffer(width, height)
		void initStageBuffers(int width, int height) {
			mStageGeometryBuffer.initGraphicsBuffer(width, height);
		}

		// Wrapper of potent::GeometryBuffer::beginGeometryBuffer(), which is basicly binding framebuffer
		void beginStage() {
			mStageGeometryBuffer.beginGeometryBuffer();
		}

		// Wrapper of potent::GeometryBuffer::endGeometryBuffer(), which is basicly unbinding framebuffer
		void endStage() {
			mStageGeometryBuffer.endGeometryBuffer();
		}

		// Render post process stage
		void renderStage(RMat projectionMatrix = RMat::Identity(), RMat viewMatrix = RMat::Identity(), RMat transformMatrix = RMat::Identity()) {
			if (mCreated) {
				mStageTextureData[0].name = "GBUFFER_POSITION_TEXTURE";
				mStageTextureData[1].name = "GBUFFER_NORMAL_TEXTURE";
				mStageTextureData[2].name = "GBUFFER_DIFFUSE_SPECULAR_TEXTURE";

				for (std::uint32_t i = 0; i < mStageGeometryBuffer.USED_TEXTURES; i++) {
					mStageTextureData[i].textureBuffer = mStageGeometryBuffer.textures[i];
					mStageRender.texturesPtr[i] = &mStageTextureData[i];
				}

				render(&mStageRender, projectionMatrix, viewMatrix, transformMatrix);
			}
		}
	};

	void debugDrawPoint(RVec point, RMat projection, RMat view, RMat transform = RMat::Identity(), RVec color = RVec(0.0f, 1.0f, 0.0f, 1.0f)) {
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
		glUniform4f(glGetUniformLocation(sp.id, "uColor"), color.x, color.y, color.z, color.w);

		glDrawArrays(GL_POINTS, 0, 1);

		vao.unbind();
		sp.unuse();

		vs.~Shader();
		fs.~Shader();
		sp.~ShaderProgram();
		vao.~VArray();
		vbo.~VBuffer();
	}

	void debugDrawModel(RenderObjectData* model, RMat projection, RMat view, int type = GL_LINES, bool transposeTransform = 1, RVec color = RVec(0.0f, 1.0f, 0.0f, 1.0f)) {
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
		glUniform4f(glGetUniformLocation(sp.id, "uColor"), color.x, color.y, color.z, color.w);

		glDrawArrays(type, 0, model->meshData.vertices.size() / 3);

		vao.unbind();
		sp.unuse();

		vs.~Shader();
		fs.~Shader();
		sp.~ShaderProgram();
		vao.~VArray();
		vbo.~VBuffer();
	}

	void debugDrawLine(RVec start, RVec end, RMat projection, RMat view, RMat transform = RMat::Identity(), RVec color = RVec(0.0f, 1.0f, 0.0f, 1.0f)) {
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
		glUniform4f(glGetUniformLocation(sp.id, "uColor"), color.x, color.y, color.z, color.w);

		glDrawArrays(GL_LINES, 0, 2);

		vao.unbind();
		sp.unuse();

		vs.~Shader();
		fs.~Shader();
		sp.~ShaderProgram();
		vao.~VArray();
		vbo.~VBuffer();
	}

	void debugDrawTriangle(RVec t1, RVec t2, RVec t3, RMat projection, RMat view, RMat transform = RMat::Identity(), RVec color = RVec(0.0f, 1.0f, 0.0f, 1.0f)) {
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
		glUniform4f(glGetUniformLocation(sp.id, "uColor"), color.x, color.y, color.z, color.w);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		vao.unbind();
		sp.unuse();

		vs.~Shader();
		fs.~Shader();
		sp.~ShaderProgram();
		vao.~VArray();
		vbo.~VBuffer();
	}

	void debugDrawLinesBox(RMat projection, RMat view, RMat transform = RMat::Identity(), RVec color = RVec(0.0f, 1.0f, 0.0f, 1.0f)) {
		std::vector<float> p = {
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,

			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,

			-1.0f, 1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,

			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,

			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,

			1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,

			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,

			-1.0f, 1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,

			-1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,

			1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, -1.0f
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
		glUniformMatrix4fv(glGetUniformLocation(sp.id, "uTransform"), 1, 1, transform.m);
		glUniform4f(glGetUniformLocation(sp.id, "uColor"), color.x, color.y, color.z, color.w);

		glDrawArrays(GL_LINES, 0, 36);

		vao.unbind();
		sp.unuse();

		vs.~Shader();
		fs.~Shader();
		sp.~ShaderProgram();
		vao.~VArray();
		vbo.~VBuffer();
	}

	void debugDrawCollisionMesh(CollisionMesh* pCollisionMesh, RMat projection, RMat view, int type = GL_LINES, RVec color = RVec(0.0f, 1.0f, 0.0f, 1.0f)) {
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
		vbo.bindData(pCollisionMesh->vertices);

		glUniformMatrix4fv(glGetUniformLocation(sp.id, "uProjection"), 1, 0, projection.m);
		glUniformMatrix4fv(glGetUniformLocation(sp.id, "uView"), 1, 0, view.m);
		glUniformMatrix4fv(glGetUniformLocation(sp.id, "uTransform"), 1, 1, pCollisionMesh->pTransform == nullptr ? RMat::Identity().m : pCollisionMesh->pTransform->GetTransform().m);
		glUniform4f(glGetUniformLocation(sp.id, "uColor"), color.x, color.y, color.z, color.w);

		glDrawArrays(type, 0, pCollisionMesh->vertices.size() / 3);

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