#pragma once

#include "GLFWwindowWrapper.h"

namespace dmbrn
{
	/**
	 * \brief container for singletons
	 */
	struct Singletons
	{
		Singletons() = delete;

		static inline GLFWwindowWrapper window;
	};
}
