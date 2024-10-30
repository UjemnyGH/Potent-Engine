#pragma once
#ifndef _POTENT_ENGINE_BUFFERS_
#define _POTENT_ENGINE_BUFFERS_

#include <glad/gl.h>
#include "engineCore.h"
#include <fstream>
#include <vector>

namespace potent {
	struct Shader {
		std::uint32_t id;
		bool created = false;

		void loadFromMemory(const char* source, std::int32_t type) {
			id = glCreateShader(type);
			glShaderSource(id, 1, &source, nullptr);
			glCompileShader(id);

			std::int32_t isCompiled = 0;

			glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);

			if (!isCompiled) {
				std::int32_t maxLength = 0;
				glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

				std::string error;
				error.resize(maxLength);
				glGetShaderInfoLog(id, maxLength, &maxLength, &error[0]);

				ENGINE_ERROR("Shader error: \n" << error)

				glDeleteShader(id);

				return;
			}

			created = true;
		}

		void loadFromFile(const std::string filePath, std::int32_t type) {
			std::ifstream file(filePath, std::ios::binary | std::ios::ate);

			std::size_t length = file.tellg();
			file.seekg(0, std::ios::beg);

			std::string sourceBuffer;
			sourceBuffer.resize(length);

			file.read(&sourceBuffer[0], sourceBuffer.size());

			file.close();

            ENGINE_INFO("Loading " << filePath << " shader!");

			loadFromMemory(sourceBuffer.c_str(), type);
		}

		~Shader() {
			if (created) {
				glDeleteShader(id);

				created = false;
			}
		}
	};

	struct ShaderProgram {
		std::uint32_t id;
		bool created = false;

		void init() {
			if (!created) {
				id = glCreateProgram();

				created = true;
			}
		}

		void attach(Shader* shader) {
			init();

			glAttachShader(id, shader->id);
		}

		void link() {
			init();

			glLinkProgram(id);

			std::int32_t isLinked = 0;
			glGetProgramiv(id, GL_LINK_STATUS, &isLinked);

			if (!isLinked) {
				std::int32_t maxLength = 0;
				glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);

				std::string error;
				error.resize(maxLength);
				glGetProgramInfoLog(id, maxLength, &maxLength, &error[0]);

				ENGINE_ERROR("Program error: \n" << error);

				glDeleteProgram(id);
			}
		}

		void use() {
			init();

			glUseProgram(id);
		}

		void unuse() {
			glUseProgram(0);
		}

		~ShaderProgram() {
			if (created) {
				glDeleteProgram(id);

				created = false;
			}
		}
	};

	struct VArray {
		std::uint32_t id;
		bool created = false;

		void init() {
			if (!created) {
				glGenVertexArrays(1, &id);

				created = true;
			}
		}

		void bind() {
			init();

			glBindVertexArray(id);
		}

		void unbind() {
			glBindVertexArray(0);
		}

		~VArray() {
			if (created) {
				glDeleteVertexArrays(1, &id);

				created = false;
			}
		}
	};

	struct VBuffer {
		std::uint32_t id;
		bool created = false;

		void init() {
			if (!created) {
				glGenBuffers(1, &id);

				created = true;
			}
		}

		void bind() {
			init();

			glBindBuffer(GL_ARRAY_BUFFER, id);
		}

		void bindPlace(std::int32_t dimmensions, std::int32_t index) {
			bind();

			glVertexAttribPointer(index, dimmensions, GL_FLOAT, 0, 0, (void*)0);
			glEnableVertexAttribArray(index);
		}

		void bindPlace(std::int32_t dimmensions, std::int32_t index, std::int32_t stride, std::uintptr_t offset) {
			bind();

			glVertexAttribPointer(index, dimmensions, GL_FLOAT, 0, stride, (void*)offset);
			glEnableVertexAttribArray(index);
		}

		void bindData(std::vector<float> data) {
			bind();

			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), data.data(), GL_DYNAMIC_DRAW);
		}

		~VBuffer() {
			if (created) {
				glDeleteBuffers(1, &id);

				created = false;
			}
		}
	};

    struct TextureBuffer {
        uint32_t id;
        bool created = false;

        void init() {
            if (!created) {
                glGenTextures(1, &id);

                created = true;
            }
        }

        void bind() {
            init();

            glBindTexture(GL_TEXTURE_2D, id);
        }

        void bindUnit(uint32_t unit_id) {
            init();

            glBindTextureUnit(unit_id, id);
        }

        void bindData(std::vector<uint8_t> data, uint32_t width, uint32_t height, bool pixels = false) {
            bind();

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            if (pixels) {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }
            else {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 16);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
            glGenerateMipmap(GL_TEXTURE_2D);

        }

        ~TextureBuffer() {
            if (created) {
                glDeleteTextures(1, &id);

                created = false;
            }
        }
    };

    struct TextureArrayBuffer {
        uint32_t id;
        bool created = false;

        void init() {
            if (!created) {
                glGenTextures(1, &id);

                created = true;
            }
        }

        void bind() {
            init();

            glBindTexture(GL_TEXTURE_2D_ARRAY, id);
        }

        void bindUnit(uint32_t unit_id) {
            init();

            glBindTextureUnit(unit_id, id);
        }

        void bindData(std::vector<uint8_t> data, uint32_t width, uint32_t height, bool pixels = false, uint32_t layerCount = 1) {
            bind();

            glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, layerCount);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, layerCount, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 16);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

            if (pixels) {
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }
            else {
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
        }

        ~TextureArrayBuffer() {
            if (created) {
                glDeleteTextures(1, &id);

                created = false;
            }
        }
    };

    struct Framebuffer {
        uint32_t id, textureId;
        bool created = false, texCreated = false;

        void init() {
            if (!created) {
                glGenFramebuffers(1, &id);

                created = true;
            }
        }

        void bind() {
            init();

            glBindFramebuffer(GL_FRAMEBUFFER, id);
        }

        void unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void getData(int width, int height, int type, int internalFormat, int format, int tid, int attachment, int filter = GL_NEAREST) {
            glBindTexture(GL_TEXTURE_2D, tid);

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tid, 0);
        }

        void getTextureFromFrameDepthStencil(int width, int height) {
            bind();

            if (!texCreated) {
                glGenTextures(1, &textureId);

                texCreated = true;
            }

            glBindTexture(GL_TEXTURE_2D, textureId);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureId, 0);

            unbind();
        }

        void getTextureFromFrameDepth(int width, int height) {
            bind();

            if (!texCreated) {
                glGenTextures(1, &textureId);

                texCreated = true;
            }

            glBindTexture(GL_TEXTURE_2D, textureId);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureId, 0);

            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            unbind();
        }

        void getTextureFromFrameColor(int width, int height) {
            bind();

            if (!texCreated) {
                glGenTextures(1, &textureId);

                texCreated = true;
            }

            glBindTexture(GL_TEXTURE_2D, textureId);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

            unbind();
        }

        ~Framebuffer() {
            if (created) {
                glDeleteFramebuffers(1, &id);

                created = false;
            }

            if (texCreated) {
                glDeleteTextures(1, &textureId);

                texCreated = false;
            }
        }
    };

    struct Renderbuffer {
        uint32_t id;
        bool created = false;

        void init() {
            if (!created) {
                glGenRenderbuffers(1, &id);

                created = true;
            }
        }

        void bind() {
            init();

            glBindRenderbuffer(GL_RENDERBUFFER, id);
        }

        void unbind() {
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }

        void createStorage(Framebuffer* pFb, int width, int height) {
            bind();

            pFb->bind();

            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, id);

            pFb->getTextureFromFrameColor(width, height);

            pFb->unbind();
            unbind();
        }
    };

    struct GeometryBuffer {
        Framebuffer graphicsBuffer;
        Renderbuffer renderBuffer;
        TextureBuffer textures[32];
        const std::uint32_t USED_TEXTURES = 3;

        void initGraphicsBuffer(int width, int height) {
            graphicsBuffer.bind();
            for (std::uint32_t i = 0; i < USED_TEXTURES; i++) {
                textures[i].init();
            }

            graphicsBuffer.getData(width, height, GL_FLOAT, GL_RGBA16F, GL_RGBA, textures[0].id, GL_COLOR_ATTACHMENT0);
            graphicsBuffer.getData(width, height, GL_FLOAT, GL_RGBA16F, GL_RGBA, textures[1].id, GL_COLOR_ATTACHMENT1);
            graphicsBuffer.getData(width, height, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA, textures[2].id, GL_COLOR_ATTACHMENT2);

            unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
            glDrawBuffers(USED_TEXTURES, attachments);

            renderBuffer.bind();

            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer.id);

            graphicsBuffer.unbind();
        }

        void startGraphicsBuffer() {
            graphicsBuffer.bind();
        }

        void endGraphicsBuffer() {
            graphicsBuffer.unbind();
        }
    };

    struct ShaderStorageBuffer {
        std::uint32_t id;
        bool created = false;

        void init() {
            if (!created) {
                glGenBuffers(1, &id);

                created = true;
            }
        }

        void bind() {
            init();

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
        }

        void unbind() {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        void bindData(void* data, std::size_t size, std::uint32_t bindingBase = 1) {
            bind();

            glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingBase, id);

            unbind();
        }

        ~ShaderStorageBuffer() {
            if (created) {
                glDeleteBuffers(1, &id);

                created = false;
            }
        }
    };
}

#endif