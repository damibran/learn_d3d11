#include <chrono>
#include <iostream>
#include <thread>
#include <string>

using duration = std::chrono::duration<double>;
using sys_clock = std::chrono::system_clock;
using time_point = std::chrono::time_point<sys_clock, duration>;

#include <GLFW/glfw3.h>
#include "Singletons.h"

namespace dmbrn
{
	class HelloTriangleApplication
	{
	public:

		void run()
		{
			while (!Singletons::window.windowShouldClose())
			{
				tp2_ = sys_clock::now();
				const duration elapsed_time = tp2_ - tp1_;
				tp1_ = tp2_;
				const double delta_time = elapsed_time.count();

				glfwPollEvents();

				Singletons::window.setWindowTitle("Vulkan. FPS: " + std::to_string(1.0f / delta_time));
			}
			//Singletons::device->waitIdle();
		}

	private:

		time_point tp1_ = std::chrono::time_point_cast<duration>(sys_clock::now());
		time_point tp2_ = std::chrono::time_point_cast<duration>(sys_clock::now());
	};
}

int main()
{
	try
	{
		dmbrn::HelloTriangleApplication app;
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
