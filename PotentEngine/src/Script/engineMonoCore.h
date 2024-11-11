#pragma once
#ifndef _POTENT_ENGINE_MONO_CORE_
#define _POTENT_ENGINE_MONO_CORE_

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/profiler.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/sgen-bridge.h>
#include <mono/metadata/metadata.h>
#include "../Core/engineCore.h"

namespace potent {
	class MonoScript {
	private:
		MonoDomain* mDomain;
		MonoAssembly* mAssembly;

	public:
		void openDomain(const char* name) {
			mDomain = mono_jit_init_version(name, "v4.0.30319");
		}

		void loadAssembly(const char* filename) {
			mAssembly = mono_domain_assembly_open(mDomain, filename);

			if (!mAssembly) {
				ENGINE_ERROR("Cannot open \"" << filename << "\" Mono assembly");
			}
		}

		void addInternalCall() {
			
		}
	};
}

#endif