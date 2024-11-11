#pragma once
#ifndef _POTENT_ENGINE_MESH_
#define _POTENT_ENGINE_MESH_

#include "../Core/engineCore.h"
#include "../Core/engineMath.h"
#include <vector>
#include <fstream>
#include <string>

namespace potent {
    // Private enum
    enum MeshRawDataSourceInfo {
        MeshRawDataSourceInfo_vertices = 0x1,
        MeshRawDataSourceInfo_normals = 0x2,
        MeshRawDataSourceInfo_textureCoordinates = 0x4,
    };

    struct MeshRawData {
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> textureCoordinates;
    };

    /**
     * @brief Load binary PLY mesh from memory (source)
     *
     * @param src source of loaded ply mesh
     * @param headerEnd end of ply header
     * @param verticesAmount amount of vertices in source
     * @param facesAmount amount of faces in source
     * @param meshSourceInfo what our ply mesh have
     * @return Mesh what you expect it to return else than "Mesh"
     */
    MeshRawData meshSourceBinary_PLY(std::string& src, std::size_t headerEnd, uint64_t verticesAmount, uint64_t facesAmount, uint8_t meshSourceInfo) {
        MeshRawData tmp;
        MeshRawData result;

        uint8_t multiplier = 0;

        // Check that we have in mesh and add it to multiplier
        if (meshSourceInfo & MeshRawDataSourceInfo_vertices) multiplier += 3;
        if (meshSourceInfo & MeshRawDataSourceInfo_normals) multiplier += 3;
        if (meshSourceInfo & MeshRawDataSourceInfo_textureCoordinates) multiplier += 2;

        // If we have nothing add 1 to crash program
        if (multiplier == 0) multiplier += 1;

        // Loop over all vertices and load them to temporary mesh
        for (std::size_t i = 0; i < verticesAmount; i++) {
            // We need offset to first vertex
            const uint32_t vertexOffset = headerEnd + 11;

            if (meshSourceInfo & MeshRawDataSourceInfo_vertices) {
                // As it should be little endian load 3 vertices
                uint32_t ix = (
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 0)]) |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 1)]) << 8 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 2)]) << 16 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 3)]) << 24
                    );

                uint32_t iy = (
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 4)]) |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 5)]) << 8 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 6)]) << 16 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 7)]) << 24
                    );

                uint32_t iz = (
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 8)]) |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 9)]) << 8 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 10)]) << 16 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 11)]) << 24
                    );

                // Recast our 3 vertices to floats (more usable than integers)
                tmp.vertices.push_back(*reinterpret_cast<float*>(&ix));
                tmp.vertices.push_back(*reinterpret_cast<float*>(&iy));
                tmp.vertices.push_back(*reinterpret_cast<float*>(&iz));
            }

            if (meshSourceInfo & (MeshRawDataSourceInfo_normals | MeshRawDataSourceInfo_textureCoordinates)) {
                // Same as vertices
                uint32_t inx = (
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 12)]) |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 13)]) << 8 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 14)]) << 16 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 15)]) << 24
                    );

                uint32_t iny = (
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 16)]) |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 17)]) << 8 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 18)]) << 16 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 19)]) << 24
                    );

                uint32_t inz = (
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 20)]) |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 21)]) << 8 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 22)]) << 16 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 23)]) << 24
                    );

                uint32_t is = (
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 24)]) |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 25)]) << 8 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 26)]) << 16 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 27)]) << 24
                    );

                uint32_t it = (
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 28)]) |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 29)]) << 8 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 30)]) << 16 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 31)]) << 24
                    );

                tmp.normals.push_back(*reinterpret_cast<float*>(&inx));
                tmp.normals.push_back(*reinterpret_cast<float*>(&iny));
                tmp.normals.push_back(*reinterpret_cast<float*>(&inz));
                tmp.textureCoordinates.push_back(*reinterpret_cast<float*>(&is));
                tmp.textureCoordinates.push_back(*reinterpret_cast<float*>(&it));
            }
            else if (meshSourceInfo & MeshRawDataSourceInfo_normals && !(meshSourceInfo & MeshRawDataSourceInfo_textureCoordinates)) {
                // Same as vertices
                uint32_t inx = (
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 12)]) |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 13)]) << 8 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 14)]) << 16 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 15)]) << 24
                    );

                uint32_t iny = (
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 16)]) |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 17)]) << 8 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 18)]) << 16 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 19)]) << 24
                    );

                uint32_t inz = (
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 20)]) |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 21)]) << 8 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 22)]) << 16 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 23)]) << 24
                    );

                tmp.normals.push_back(*reinterpret_cast<float*>(&inx));
                tmp.normals.push_back(*reinterpret_cast<float*>(&iny));
                tmp.normals.push_back(*reinterpret_cast<float*>(&inz));
            }
            else if (!(meshSourceInfo & MeshRawDataSourceInfo_normals) && meshSourceInfo & MeshRawDataSourceInfo_textureCoordinates) {
                // Same as vertices
                uint32_t is = (
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 12)]) |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 13)]) << 8 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 14)]) << 16 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 15)]) << 24
                    );

                uint32_t it = (
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 16)]) |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 17)]) << 8 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 18)]) << 16 |
                    (uint32_t)((uint8_t)src[vertexOffset + (i * multiplier * sizeof(float) + 19)]) << 24
                    );

                tmp.textureCoordinates.push_back(*reinterpret_cast<float*>(&is));
                tmp.textureCoordinates.push_back(*reinterpret_cast<float*>(&it));
            }
        }

        uint32_t index = 0;

        for (std::size_t i = 0; i < facesAmount; i++) {
            const uint32_t facesOffset = headerEnd + 11 + (sizeof(float) * multiplier * verticesAmount);

            // Get number of indices to join
            uint8_t indicesAmount = (uint8_t)src[facesOffset + index];

            if (indicesAmount == 3) {
                // Get indices
                uint32_t ix = (
                    (uint32_t)((uint8_t)src[facesOffset + (index + 1)]) |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 2)]) << 8 |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 3)]) << 16 |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 4)]) << 24
                    );

                uint32_t iy = (
                    (uint32_t)((uint8_t)src[facesOffset + (index + 5)]) |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 6)]) << 8 |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 7)]) << 16 |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 8)]) << 24
                    );

                uint32_t iz = (
                    (uint32_t)((uint8_t)src[facesOffset + (index + 9)]) |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 10)]) << 8 |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 11)]) << 16 |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 12)]) << 24
                    );

                index += 13;

                // Make vertices

                if (meshSourceInfo & MeshRawDataSourceInfo_vertices) {
                    result.vertices.push_back(tmp.vertices[ix * 3 + 0]);
                    result.vertices.push_back(tmp.vertices[ix * 3 + 1]);
                    result.vertices.push_back(tmp.vertices[ix * 3 + 2]);

                    result.vertices.push_back(tmp.vertices[iy * 3 + 0]);
                    result.vertices.push_back(tmp.vertices[iy * 3 + 1]);
                    result.vertices.push_back(tmp.vertices[iy * 3 + 2]);

                    result.vertices.push_back(tmp.vertices[iz * 3 + 0]);
                    result.vertices.push_back(tmp.vertices[iz * 3 + 1]);
                    result.vertices.push_back(tmp.vertices[iz * 3 + 2]);
                }

                if (meshSourceInfo & MeshRawDataSourceInfo_normals) {
                    result.normals.push_back(tmp.normals[ix * 3 + 0]);
                    result.normals.push_back(tmp.normals[ix * 3 + 1]);
                    result.normals.push_back(tmp.normals[ix * 3 + 2]);

                    result.normals.push_back(tmp.normals[iy * 3 + 0]);
                    result.normals.push_back(tmp.normals[iy * 3 + 1]);
                    result.normals.push_back(tmp.normals[iy * 3 + 2]);

                    result.normals.push_back(tmp.normals[iz * 3 + 0]);
                    result.normals.push_back(tmp.normals[iz * 3 + 1]);
                    result.normals.push_back(tmp.normals[iz * 3 + 2]);
                }

                if (meshSourceInfo & MeshRawDataSourceInfo_textureCoordinates) {
                    result.textureCoordinates.push_back(tmp.textureCoordinates[ix * 2 + 0]);
                    result.textureCoordinates.push_back(tmp.textureCoordinates[ix * 2 + 1]);

                    result.textureCoordinates.push_back(tmp.textureCoordinates[iy * 2 + 0]);
                    result.textureCoordinates.push_back(tmp.textureCoordinates[iy * 2 + 1]);

                    result.textureCoordinates.push_back(tmp.textureCoordinates[iz * 2 + 0]);
                    result.textureCoordinates.push_back(tmp.textureCoordinates[iz * 2 + 1]);
                }
            }
            else if (indicesAmount == 4) {
                uint32_t ix = (
                    (uint32_t)((uint8_t)src[facesOffset + (index + 1)]) |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 2)]) << 8 |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 3)]) << 16 |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 4)]) << 24
                    );

                uint32_t iy = (
                    (uint32_t)((uint8_t)src[facesOffset + (index + 5)]) |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 6)]) << 8 |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 7)]) << 16 |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 8)]) << 24
                    );

                uint32_t iz = (
                    (uint32_t)((uint8_t)src[facesOffset + (index + 9)]) |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 10)]) << 8 |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 11)]) << 16 |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 12)]) << 24
                    );

                uint32_t iw = (
                    (uint32_t)((uint8_t)src[facesOffset + (index + 13)]) |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 14)]) << 8 |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 15)]) << 16 |
                    (uint32_t)((uint8_t)src[facesOffset + (index + 16)]) << 24
                    );

                index += 17;

                if (meshSourceInfo & MeshRawDataSourceInfo_vertices) {
                    result.vertices.push_back(tmp.vertices[ix * 3 + 0]);
                    result.vertices.push_back(tmp.vertices[ix * 3 + 1]);
                    result.vertices.push_back(tmp.vertices[ix * 3 + 2]);

                    result.vertices.push_back(tmp.vertices[iy * 3 + 0]);
                    result.vertices.push_back(tmp.vertices[iy * 3 + 1]);
                    result.vertices.push_back(tmp.vertices[iy * 3 + 2]);

                    result.vertices.push_back(tmp.vertices[iz * 3 + 0]);
                    result.vertices.push_back(tmp.vertices[iz * 3 + 1]);
                    result.vertices.push_back(tmp.vertices[iz * 3 + 2]);

                    result.vertices.push_back(tmp.vertices[ix * 3 + 0]);
                    result.vertices.push_back(tmp.vertices[ix * 3 + 1]);
                    result.vertices.push_back(tmp.vertices[ix * 3 + 2]);

                    result.vertices.push_back(tmp.vertices[iz * 3 + 0]);
                    result.vertices.push_back(tmp.vertices[iz * 3 + 1]);
                    result.vertices.push_back(tmp.vertices[iz * 3 + 2]);

                    result.vertices.push_back(tmp.vertices[iw * 3 + 0]);
                    result.vertices.push_back(tmp.vertices[iw * 3 + 1]);
                    result.vertices.push_back(tmp.vertices[iw * 3 + 2]);
                }

                if (meshSourceInfo & MeshRawDataSourceInfo_normals) {
                    result.normals.push_back(tmp.normals[ix * 3 + 0]);
                    result.normals.push_back(tmp.normals[ix * 3 + 1]);
                    result.normals.push_back(tmp.normals[ix * 3 + 2]);

                    result.normals.push_back(tmp.normals[iy * 3 + 0]);
                    result.normals.push_back(tmp.normals[iy * 3 + 1]);
                    result.normals.push_back(tmp.normals[iy * 3 + 2]);

                    result.normals.push_back(tmp.normals[iz * 3 + 0]);
                    result.normals.push_back(tmp.normals[iz * 3 + 1]);
                    result.normals.push_back(tmp.normals[iz * 3 + 2]);

                    result.normals.push_back(tmp.normals[ix * 3 + 0]);
                    result.normals.push_back(tmp.normals[ix * 3 + 1]);
                    result.normals.push_back(tmp.normals[ix * 3 + 2]);

                    result.normals.push_back(tmp.normals[iz * 3 + 0]);
                    result.normals.push_back(tmp.normals[iz * 3 + 1]);
                    result.normals.push_back(tmp.normals[iz * 3 + 2]);

                    result.normals.push_back(tmp.normals[iw * 3 + 0]);
                    result.normals.push_back(tmp.normals[iw * 3 + 1]);
                    result.normals.push_back(tmp.normals[iw * 3 + 2]);
                }

                if (meshSourceInfo & MeshRawDataSourceInfo_textureCoordinates) {
                    result.textureCoordinates.push_back(tmp.textureCoordinates[ix * 2 + 0]);
                    result.textureCoordinates.push_back(tmp.textureCoordinates[ix * 2 + 1]);

                    result.textureCoordinates.push_back(tmp.textureCoordinates[iy * 2 + 0]);
                    result.textureCoordinates.push_back(tmp.textureCoordinates[iy * 2 + 1]);

                    result.textureCoordinates.push_back(tmp.textureCoordinates[iz * 2 + 0]);
                    result.textureCoordinates.push_back(tmp.textureCoordinates[iz * 2 + 1]);

                    result.textureCoordinates.push_back(tmp.textureCoordinates[ix * 2 + 0]);
                    result.textureCoordinates.push_back(tmp.textureCoordinates[ix * 2 + 1]);

                    result.textureCoordinates.push_back(tmp.textureCoordinates[iz * 2 + 0]);
                    result.textureCoordinates.push_back(tmp.textureCoordinates[iz * 2 + 1]);

                    result.textureCoordinates.push_back(tmp.textureCoordinates[iw * 2 + 0]);
                    result.textureCoordinates.push_back(tmp.textureCoordinates[iw * 2 + 1]);
                }
            }
        }

        return result;
    }

    /**
     * @brief Load PLY mesh from file
     *
     * @param path path to file
     * @return MeshRawData
     */
    MeshRawData loadPLYMesh(std::string path) {
        std::ifstream file;
        file.open(path, std::ios::binary | std::ios::ate);

        uint32_t len = file.tellg();
        file.seekg(0, std::ios::beg);

        std::string src;
        src.resize(len);

        file.read((char*)src.data(), src.size());

        file.close();

        std::size_t plyMV = src.find("ply");

        if (plyMV == std::string::npos) return MeshRawData();

        std::size_t formatOff = src.find("format binary_little_endian");
        std::size_t verticesAmountOff = src.find("element vertex");
        std::size_t verticesOff = src.find("property float x");
        std::size_t normalsOff = src.find("property float nx");
        std::size_t textureCoordsOff = src.find("property float s");
        std::size_t facesAmountOff = src.find("element face");
        std::size_t headerEnd = src.find("end_header");

        uint64_t verticesAmount = std::stoi(std::string(src.begin() + verticesAmountOff + 15, src.begin() + src.find_first_of('\n', verticesAmountOff)));
        uint64_t facesAmount = std::stoi(std::string(src.begin() + facesAmountOff + 13, src.begin() + src.find_first_of('\n', facesAmountOff)));

        if (formatOff != std::string::npos) {
            return meshSourceBinary_PLY(src, headerEnd, verticesAmount, facesAmount, (verticesOff != std::string::npos ? MeshRawDataSourceInfo_vertices : 0) | (normalsOff != std::string::npos ? MeshRawDataSourceInfo_normals : 0) | (textureCoordsOff != std::string::npos ? MeshRawDataSourceInfo_textureCoordinates : 0));
        }

        return MeshRawData();
    }

    void negateMeshNormals(MeshRawData* pMesh) {
        std::vector<float> tempNormals;

        for (std::size_t i = 0; i < pMesh->normals.size() / 3; i++) {
            RVec normal = RVec(pMesh->normals[i * 3 + 0], pMesh->normals[i * 3 + 1], pMesh->normals[i * 3 + 2]).Negate();

            tempNormals.push_back(normal.x);
            tempNormals.push_back(normal.y);
            tempNormals.push_back(normal.z);
        }

        pMesh->normals.clear();
        std::copy(tempNormals.begin(), tempNormals.end(), std::back_inserter(pMesh->normals));
    }

    void calculateNormalsBasedOnVertices(MeshRawData* pMesh) {
        std::vector<float> tempNormals;

        for (std::size_t i = 0; i < pMesh->vertices.size() / 9; i++) {
            RVec a = RVec(pMesh->vertices[i * 9 + 0], pMesh->vertices[i * 9 + 1], pMesh->vertices[i * 9 + 2]);
            RVec b = RVec(pMesh->vertices[i * 9 + 3], pMesh->vertices[i * 9 + 4], pMesh->vertices[i * 9 + 5]);
            RVec c = RVec(pMesh->vertices[i * 9 + 6], pMesh->vertices[i * 9 + 7], pMesh->vertices[i * 9 + 8]);

            RVec normal = RVec::PlaneNormal(a, b, c);

            tempNormals.push_back(normal.x);
            tempNormals.push_back(normal.y);
            tempNormals.push_back(normal.z);

            tempNormals.push_back(normal.x);
            tempNormals.push_back(normal.y);
            tempNormals.push_back(normal.z);

            tempNormals.push_back(normal.x);
            tempNormals.push_back(normal.y);
            tempNormals.push_back(normal.z);
        }

        pMesh->normals.clear();
        std::copy(tempNormals.begin(), tempNormals.end(), std::back_inserter(pMesh->normals));
    }
}

#endif