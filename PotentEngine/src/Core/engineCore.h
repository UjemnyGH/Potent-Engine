#pragma once
#ifndef _POTENT_ENGINE_CORE_
#define _POTENT_ENGINE_CORE_

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <vector>

#define ENGINE_ERROR(msg) std::cout << "[ERROR]: " << msg << " in " << __FILE__ << ":" << __LINE__ << " @ " << __FUNCSIG__ << std::endl;
#define ENGINE_WARN(msg) std::cout << "[WARNING]: " << msg << " in " << __FILE__ << ":" << __LINE__ << " @ " << __FUNCSIG__ << std::endl;
#define ENGINE_INFO(msg) std::cout << "[INFO]: " << msg << " in " << __FILE__ << ":" << __LINE__ << " @ " << __FUNCSIG__ << std::endl;

namespace potent {
	std::vector<std::uint8_t> loadImageData(std::string path, std::uint32_t* pWidth, std::uint32_t* pHeight, std::uint32_t* pColorChannerls = nullptr) {
		int width, height, nrChann;
		
		stbi_set_flip_vertically_on_load(1);
		std::uint8_t* pixels = stbi_load(path.c_str(), &width, &height, &nrChann, 0);

		std::vector<std::uint8_t> result;
		result.assign(&pixels[0], &pixels[width * height * nrChann]);

		*pWidth = width;
		*pHeight = height;

		if (pColorChannerls != nullptr) {
			*pColorChannerls = nrChann;
		}

		return result;
	}
}

#endif