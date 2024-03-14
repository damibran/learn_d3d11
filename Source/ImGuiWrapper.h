#pragma once

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "DXGIWindowWrapper.h"
#include "DeviceWrapper.h"

#include "Components/СoncreteComponent/Camera/CameraFPSController.h"
#include "Components/СoncreteComponent/Camera/CameraOrbitController.h"
#include "Components/СoncreteComponent/OrbitComponent.h"
#include "Components/СoncreteComponent/CubeComponent.h"
#include "Components/СoncreteComponent/SphereComponent.h"


namespace dmbrn
{
	class ImGuiWrapper
	{
	public:
		ImGuiWrapper(GameToComponentBridge bridge, std::vector<std::unique_ptr<IGameComponent>>& comps)
			:bridge(bridge),
			components(comps)
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			(void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
			// ImGui::StyleColorsLight();

			// Setup Platform/Renderer back ends
			ImGui_ImplWin32_Init(*bridge.window);
			ImGui_ImplDX11_Init(bridge.device.getDevice(), bridge.device.getContext());
		}
		~ImGuiWrapper()
		{
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}

		void drawImGuiUI()
		{
			ImGuiIO& io = ImGui::GetIO();
			(void)io;

			// Start the Dear ImGui frame
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
			{
				ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

				ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

				ImGui::End();
			}

			drawObjectSettingsWnd();

			drawOrbitCameraSettings();

			ImGui::Render();
		}

		void drawOrbitCameraSettings()
		{
			ImGui::Begin("Camera settings");

			if (ImGui::Checkbox("Use orbit", &useOrbitCamera))
			{
				if (useOrbitCamera)
				{
					// it became true
					components[0] = std::make_unique<CameraOrbitController>(GameToComponentBridge{ bridge.device, bridge.window });
				}
				else
				{ // it became false
					components[0] = std::make_unique<CameraFPSControllerComponent>(GameToComponentBridge{ bridge.device, bridge.window });
				}
			}

			if (auto cam = dynamic_cast<CameraOrbitController*>(components[0].get()))
			{
				if (ImGui::InputInt("object index", &orbitObjectInd))
				{
					if (auto c = dynamic_cast<CubeComponent*>(components[orbitObjectInd].get()))
					{
						cam->setCenterTransform(&c->transform);
					}
				}

				ImGui::DragFloat("Radius", &cam->radius);
				drawCameraSettings(cam->camera);
			}
			else if (auto cam = dynamic_cast<CameraFPSControllerComponent*>(components[0].get()))
			{
				ImGui::DragFloat("Speed", &cam->velocityMultipler);
				drawCameraSettings(cam->camera);
			}

			ImGui::End();
		}

		void drawCameraSettings(Camera& cam)
		{
			bool wasEdit = false;

			wasEdit |= ImGui::Checkbox("Use Perspective", &cam.perspective);
			wasEdit |= ImGui::DragFloat("Fov", &cam.FovAngleY, 0.01);
			wasEdit |= ImGui::DragFloat("Aspect", &cam.AspectRatio, 0.01);
			wasEdit |= ImGui::DragFloat("NearZ", &cam.NearZ, 0.01, 0.00001, 1, "%.6f");
			wasEdit |= ImGui::DragFloat("FarZ", &cam.FarZ, 0.1);

			if (wasEdit)
			{
				cam.updateCamera();
			}
		}

		void drawObjectSettingsWnd()
		{
			ImGui::Begin("Object settings");

			ImGui::InputInt("object index", &editObjectInd);

			if (auto c = dynamic_cast<CubeComponent*>(components[editObjectInd].get()))
			{
				ImGui::Text("CubeComponent");
				ImGui::InputFloat3("Axis", reinterpret_cast<float*>(&c->axis));
				ImGui::DragFloat("Speed", &c->speed);
			}

			else if (auto c = dynamic_cast<SphereComponent*>(components[editObjectInd].get()))
			{
				ImGui::Text("SphereComponent");
				ImGui::InputFloat3("Axis", reinterpret_cast<float*>(&c->axis));
				ImGui::DragFloat("Speed", &c->speed);
			}

			else if (auto c = dynamic_cast<OrbitController*>(components[editObjectInd].get()))
			{
				ImGui::Text("OrbitController");
				ImGui::InputFloat3("Axis", reinterpret_cast<float*>(&c->axis));
				ImGui::DragFloat("Radius", &c->radius);
				ImGui::DragFloat("Speed", &c->speed);
			}

			ImGui::End();
		}

		int editObjectInd = 5;
		int orbitObjectInd = 5;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		bool useOrbitCamera = true;

		GameToComponentBridge bridge;
		std::vector<std::unique_ptr<IGameComponent>>& components;

	private:
	};
}
