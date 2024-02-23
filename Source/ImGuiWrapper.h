#pragma once

#include "DeviceWrapper.h"
#include "DXGIWindowWrapper.h"
#include "imgui_impl_dx11.h"


namespace dmbrn
{
	class ImGuiWrapper
	{
	public:
		ImGuiWrapper(DeviceWrapper& device, DXGIWindowWrapper& window)
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsLight();

			// Setup Platform/Renderer back ends
			ImGui_ImplWin32_Init(*window);
			ImGui_ImplDX11_Init(device.getDevice(), device.getContext());
		}
		~ImGuiWrapper()
		{
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}

		void drawImGuiUI()
		{
			ImGuiIO& io = ImGui::GetIO(); (void)io;

			// Start the Dear ImGui frame
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
			{
				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

				ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
				ImGui::End();
			}

			ImGui::Render();
		}

		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	private:
	};
}