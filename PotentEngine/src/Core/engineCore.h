#pragma once
#ifndef _POTENT_ENGINE_CORE_
#define _POTENT_ENGINE_CORE_

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <vector>
#include <string>

#define ENGINE_ERROR(msg) std::cout << "[ERROR]: " << msg << " in " << __FILE__ << ":" << __LINE__ << " @ " << __FUNCTION__ << std::endl;
#define ENGINE_WARN(msg) std::cout << "[WARNING]: " << msg << " in " << __FILE__ << ":" << __LINE__ << " @ " << __FUNCTION__ << std::endl;
#define ENGINE_INFO(msg) std::cout << "[INFO]: " << msg << std::endl;
#define ENGINE_GL_ERROR(msg) std::cout << "[GL ERROR]: " << msg << std::endl;
#define ENGINE_GL_WARN(msg) std::cout << "[GL WARNING]: " << msg << std::endl;

namespace potent {
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
		"void main() {\noCol = vec4(0.0, 1.0, 0.0, 1.0);\ngl_FragDepth = 0.0;\n}\n";

	std::vector<std::uint8_t> loadImageData(std::string path, std::uint32_t* pWidth, std::uint32_t* pHeight, std::uint32_t* pColorChannels = nullptr) {
		int width, height, nrChann;
		
		stbi_set_flip_vertically_on_load(1);
		std::uint8_t* pixels = stbi_load(path.c_str(), &width, &height, &nrChann, 0);

		std::vector<std::uint8_t> result;
		result.assign(&pixels[0], &pixels[width * height * nrChann]);

		*pWidth = width;
		*pHeight = height;

		if (pColorChannels != nullptr) {
			*pColorChannels = nrChann;
		}

		ENGINE_INFO("Returning image data \"" << path << "\" of " << width << "x" << height << " pixels with " << nrChann << " byte color space");

		stbi_image_free(pixels);

		return result;
	}
}

#endif