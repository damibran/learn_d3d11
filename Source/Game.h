#pragma once

#include <chrono>
#include <memory>
#include <vector>

using duration = std::chrono::duration<double>;
using sys_clock = std::chrono::system_clock;
using time_point = std::chrono::time_point<sys_clock, duration>;

#include "DXGIWindowWrapper.h"
#include "DeviceWrapper.h"
#include "ImGuiWrapper.h"
#include "SwapChainWrapper.h"
#include "RenderData/Object.h"
#include "RenderData/InputLayout.h"

#include "Components/GameToComponentBridge.h"
#include "Components/IGameComponent.h"
#include "Components/СoncreteComponent/Camera/CameraFPSController.h"
#include "Components/СoncreteComponent/Camera/CameraOrbitController.h"
#include "Components/СoncreteComponent/RectangleComponent.h"
#include "Components/СoncreteComponent/TriangleComponent.h"
#include "Components/СoncreteComponent/Model.h"

namespace dmbrn {
	class Game {
	public:
		// left-hand coordinate system, rotation is clock wise while look from above
		Game()
		{
			components.push_back(std::make_unique<CameraFPSControllerComponent>(GameToComponentBridge{ device, window }));

			Model m = Model(device.getDevice(), &inputLayout, "Models\\Barrel\\Barrel.dae");
		}

		void run()
		{
			window.show();

			while (!window.windowShouldClose()) {
				tp2_ = sys_clock::now();
				const duration elapsed_time = tp2_ - tp1_;
				tp1_ = tp2_;
				double delta_time = elapsed_time.count();

				// Handle window resize (we don't resize directly in the WM_SIZE handler)
				if (window.pendingResize) {
					swapChain.resize(device, window.resizeWidth, window.resizeHeight);
					updateCameraAspect();
					window.pendingResize = false;
					delta_time = 0;
				}

				// game loop body

				for (auto&& comp : components) {
					comp->Update(delta_time);
				}

				// physics.update();

				for (auto&& comp : components) {
					comp->RenderDataUpdate();
				}

				imGui.drawImGuiUI();

				drawFrame(delta_time);
			}
		}

		// TODO: temp
		DeviceWrapper device;
		DXGIWindowWrapper window;

	private:
		SwapChainWrapper swapChain{ window, device };

		// there could be some set of common RastState and layouts
		RastState rastState{ device.getDevice(), CD3D11_RASTERIZER_DESC(D3D11_DEFAULT) };
		InputLayout<VertexType> inputLayout{ device.getDevice(), L"./Shaders/LayoutShader.hlsl" };

		std::vector<std::unique_ptr<IGameComponent>> components;

		ImGuiWrapper imGui{ {device, window},components };

		time_point tp1_ = std::chrono::time_point_cast<duration>(sys_clock::now());
		time_point tp2_ = std::chrono::time_point_cast<duration>(sys_clock::now());

		void drawFrame(double delta_time)
		{
			// Rendering
			RECT winRect;
			GetClientRect(*window, &winRect);
			D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)(winRect.right - winRect.left), (FLOAT)(winRect.bottom - winRect.top), 0.0f, 1.0f };
			device.getContext()->RSSetViewports(1, &viewport);
			const float clear_color_with_alpha[4] = { imGui.clear_color.x * imGui.clear_color.w, imGui.clear_color.y * imGui.clear_color.w, imGui.clear_color.z * imGui.clear_color.w, imGui.clear_color.w };
			device.getContext()->OMSetRenderTargets(1, &swapChain.getRenderTarget(), nullptr);
			device.getContext()->ClearRenderTargetView(swapChain.getRenderTarget(), clear_color_with_alpha);

			// draw componentsS
			for (auto&& comp : components) {
				comp->Draw();
			}

			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			swapChain->Present(1, 0); // Present with vsync
		}

		void updateCameraAspect()
		{
			if (auto cam = dynamic_cast<CameraOrbitController*>(components[0].get()))
			{
				cam->camera.AspectRatio = static_cast<float>(window.resizeWidth) / window.resizeHeight;
				cam->camera.updateCamera();
			}
			else if (auto cam = dynamic_cast<CameraFPSControllerComponent*>(components[0].get()))
			{
				cam->camera.AspectRatio = static_cast<float>(window.resizeWidth) / window.resizeHeight;
				cam->camera.updateCamera();
			}
		}
	};
}