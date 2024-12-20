#pragma once
#ifndef _POTENT_ENGINE_RENDER_DATA_
#define _POTENT_ENGINE_RENDER_DATA_

#include "engineMesh.h"
#include "../Core/engineMath.h"
#include "../Core/engineBuffers.h"
#include "../Scene/engineComponent.h"
#include <array>

namespace potent {
	struct Texture {
		TextureBuffer textureBuffer;
		TextureArrayBuffer textureArrayBuffer;
		std::string name;
	};

	class RenderObjectData : public Component {
	public:
		// Unoptimized but effective way of searching models
		std::string name;
		RTransform transform;
		MeshRawData meshData;
		std::vector<float> colors;
		std::vector<float> textureId;

		RenderObjectData() : Component() {
			mComponentId = Component_Mesh;
			componentName = "default_mesh" + std::to_string(sComponentCounter);
		}

		RenderObjectData(MeshRawData mesh) {
			mComponentId = Component_Mesh;
			componentName = "default_mesh";
			makeModel(mesh);
		}

		void makeModel(MeshRawData mesh) {
			meshData = mesh;
			colors.resize((meshData.vertices.size() / 3) * 4);
			std::fill(colors.begin(), colors.end(), 1.0f);
			textureId.resize((meshData.vertices.size() / 3));
			std::fill(textureId.begin(), textureId.end(), 32.0f);
			transform.SetScale(RVec(1.0f));
		}

		void debugDraw(RMat projection, RMat view, int type = GL_LINES) {
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
			vbo.bindData(meshData.vertices);

			glUniformMatrix4fv(glGetUniformLocation(sp.id, "uProjection"), 1, 0, projection.m);
			glUniformMatrix4fv(glGetUniformLocation(sp.id, "uView"), 1, 0, view.m);
			glUniformMatrix4fv(glGetUniformLocation(sp.id, "uTransform"), 1, 1, transform.GetTransform().m);

			glDrawArrays(type, 0, meshData.vertices.size() / 3);

			vao.unbind();
			sp.unuse();

			vs.~Shader();
			fs.~Shader();
			sp.~ShaderProgram();
			vao.~VArray();
			vbo.~VBuffer();
		}
	};

	struct RenderData {
		std::string name;
		// Pointers are just better
		std::vector<RenderObjectData*> meshData;
		std::vector<float> joinedData;
		std::size_t verticesOffset = 0;
		std::size_t normalsOffset = 0;
		std::size_t textureCoordinatesOffset = 0;
		std::size_t colorsOffset = 0;
		std::size_t textureIdOffset = 0;

		VArray vertexArray;
		VBuffer vertexBuffer;

		std::array<Texture*, 32> texturesPtr;

		bool rejoinedDataBound = false;

		RenderData() {
			for (int i = 0; i < 32; i++) {
				texturesPtr[i] = nullptr;
			}
		}

		void bindData() {
			if (!rejoinedDataBound) {
				vertexArray.bind();

				// Vertices at location 0
				vertexBuffer.bindPlace(3, 0, 3 * sizeof(joinedData[0]), verticesOffset				* sizeof(joinedData[0]));
				// Normals at location 1
				vertexBuffer.bindPlace(3, 1, 3 * sizeof(joinedData[0]), normalsOffset				* sizeof(joinedData[0]));
				// Texture coordinates at location 2
				vertexBuffer.bindPlace(2, 2, 2 * sizeof(joinedData[0]), textureCoordinatesOffset	* sizeof(joinedData[0]));
				// Colors at location 3
				vertexBuffer.bindPlace(4, 3, 4 * sizeof(joinedData[0]), colorsOffset				* sizeof(joinedData[0]));
				// Texture ids at location 4
				vertexBuffer.bindPlace(1, 4, 1 * sizeof(joinedData[0]), textureIdOffset				* sizeof(joinedData[0]));

				vertexBuffer.bindData(joinedData);

				vertexArray.unbind();

				rejoinedDataBound = true;
			}
		}

		void joinData() {
			std::vector<float> verticesTemp, normalsTemp, texCoordsTemp, colorsTemp, textureIdTemp;

			joinedData.clear();

			for (RenderObjectData* data : meshData) {
				for (std::size_t i = 0; i < data->meshData.vertices.size() / 3; i++) {
					RVec vertex = data->transform.GetTransform() * RVec(data->meshData.vertices[i * 3 + 0], data->meshData.vertices[i * 3 + 1], data->meshData.vertices[i * 3 + 2], 1.0f);

					verticesTemp.push_back(vertex.x);
					verticesTemp.push_back(vertex.y);
					verticesTemp.push_back(vertex.z);

					if (data->meshData.vertices.size() == data->meshData.normals.size()) {
						RVec normal = (data->transform.GetTransform() * RVec(data->meshData.normals[i * 3 + 0], data->meshData.normals[i * 3 + 1], data->meshData.normals[i * 3 + 2], 0.0f)).Normalize();
						
						normalsTemp.push_back(normal.x);
						normalsTemp.push_back(normal.y);
						normalsTemp.push_back(normal.z);
					}
				}

				if (data->meshData.vertices.size() != data->meshData.normals.size())
					std::copy(data->meshData.normals.begin(), data->meshData.normals.end(), std::back_inserter(normalsTemp));
				std::copy(data->meshData.textureCoordinates.begin(), data->meshData.textureCoordinates.end(), std::back_inserter(texCoordsTemp));
				std::copy(data->colors.begin(), data->colors.end(), std::back_inserter(colorsTemp));
				std::copy(data->textureId.begin(), data->textureId.end(), std::back_inserter(textureIdTemp));
			}

			verticesOffset = 0;
			std::copy(verticesTemp.begin(), verticesTemp.end(), std::back_inserter(joinedData));

			normalsOffset = joinedData.size();
			std::copy(normalsTemp.begin(), normalsTemp.end(), std::back_inserter(joinedData));

			textureCoordinatesOffset = joinedData.size();
			std::copy(texCoordsTemp.begin(), texCoordsTemp.end(), std::back_inserter(joinedData));

			colorsOffset = joinedData.size();
			std::copy(colorsTemp.begin(), colorsTemp.end(), std::back_inserter(joinedData));

			textureIdOffset = joinedData.size();
			std::copy(textureIdTemp.begin(), textureIdTemp.end(), std::back_inserter(joinedData));
			
			rejoinedDataBound = false;
		}

		void joinAndBindData() {
			joinData();

			bindData();
		}
	};
}

#endif