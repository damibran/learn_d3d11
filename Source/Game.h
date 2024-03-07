#pragma once

#include <chrono>
#include <memory>
#include <vector>

using duration = std::chrono::duration<double>;
using sys_clock = std::chrono::system_clock;
using time_point = std::chrono::time_point<sys_clock, duration>;

#include "Components/GameToComponentBridge.h"
#include "Components/IGameComponent.h"
#include "Components/СoncreteComponent/Camera/CameraFPSController.h"
#include "Components/СoncreteComponent/Camera/CameraOrbitController.h"
#include "Components/СoncreteComponent/RectangleComponent.h"
#include "Components/СoncreteComponent/TriangleComponent.h"
#include "Components/СoncreteComponent/CubeComponent.h"
#include "Components/СoncreteComponent/SphereComponent.h"
#include "Components/СoncreteComponent/LineComponent.h"
#include "Components/СoncreteComponent/GridComponent.h"
#include "Components/СoncreteComponent/OrbitComponent.h"
#include "DXGIWindowWrapper.h"
#include "DeviceWrapper.h"
#include "ImGuiWrapper.h"
#include "SimpleMath.h"
#include "SwapChainWrapper.h"

namespace dmbrn {
	class Game {
	public:
		// left-hand coordinate system, rotation is clock wise while look from above
		Game()
		{
			components.push_back(std::make_unique<CameraOrbitController>(GameToComponentBridge{ device, window }));

			components.push_back(std::make_unique<LineComponent>(GameToComponentBridge{ device, window }, rastState, L"./Shaders/Line.hlsl",
				TransformComponent{}, DirectX::SimpleMath::Vector3{ 1,0,0 }));

			components.push_back(std::make_unique<LineComponent>(GameToComponentBridge{ device, window }, rastState, L"./Shaders/Line.hlsl",
				TransformComponent{ {},{0,0,DirectX::XMConvertToRadians(90)} }, DirectX::SimpleMath::Vector3{ 0,1,0 }));

			components.push_back(std::make_unique<LineComponent>(GameToComponentBridge{ device, window }, rastState, L"./Shaders/Line.hlsl",
				TransformComponent{ {},{0,-DirectX::XMConvertToRadians(90),0} }, DirectX::SimpleMath::Vector3{ 0,0,1 }));

			components.push_back(std::make_unique<GridComponent>(GameToComponentBridge{ device, window }, rastState, L"./Shaders/Line.hlsl", 40, 40));

			// Mesh Components

			components.push_back(std::make_unique<CubeComponent>(GameToComponentBridge{ device, window }, rastState, L"./Shaders/MovingRec.hlsl",
				TransformComponent{}));
			auto center = reinterpret_cast<CubeComponent*>((--components.end())->get());

			components.push_back(std::make_unique<CubeComponent>(GameToComponentBridge{ device, window }, rastState, L"./Shaders/MovingRec.hlsl",
				TransformComponent{ }));
			auto c2 = reinterpret_cast<CubeComponent*>((--components.end())->get());

			components.push_back(std::make_unique<CubeComponent>(GameToComponentBridge{ device, window }, rastState, L"./Shaders/MovingRec.hlsl",
				TransformComponent{ { },{},{0.2,0.2,0.2} }));
			auto c3 = reinterpret_cast<CubeComponent*>((--components.end())->get());

			components.push_back(std::make_unique<CubeComponent>(GameToComponentBridge{ device, window }, rastState, L"./Shaders/MovingRec.hlsl",
				TransformComponent{ { },{},{0.2,0.2,0.2} }));
			auto c4 = reinterpret_cast<CubeComponent*>((--components.end())->get());

			components.push_back(std::make_unique<CubeComponent>(GameToComponentBridge{ device, window }, rastState, L"./Shaders/MovingRec.hlsl",
				TransformComponent{ { },{},{0.2,0.2,0.2} }));
			auto c5 = reinterpret_cast<CubeComponent*>((--components.end())->get());

			components.push_back(std::make_unique<SphereComponent>(GameToComponentBridge{ device, window }, rastState, L"./Shaders/MovingRec.hlsl",
				TransformComponent{ { },{},{0.2,0.2,0.2} }));
			auto s1 = reinterpret_cast<SphereComponent*>((--components.end())->get());

			components.push_back(std::make_unique<SphereComponent>(GameToComponentBridge{ device, window }, rastState, L"./Shaders/MovingRec.hlsl",
				TransformComponent{ { },{},{0.2,0.2,0.2} }));
			auto s2 = reinterpret_cast<SphereComponent*>((--components.end())->get());

			components.push_back(std::make_unique<SphereComponent>(GameToComponentBridge{ device, window }, rastState, L"./Shaders/MovingRec.hlsl",
				TransformComponent{ { },{} }));
			auto s3 = reinterpret_cast<SphereComponent*>((--components.end())->get());

			components.push_back(std::make_unique<SphereComponent>(GameToComponentBridge{ device, window }, rastState, L"./Shaders/MovingRec.hlsl",
				TransformComponent{ { },{} }));
			auto s4 = reinterpret_cast<SphereComponent*>((--components.end())->get());

			components.push_back(std::make_unique<SphereComponent>(GameToComponentBridge{ device, window }, rastState, L"./Shaders/MovingRec.hlsl",
				TransformComponent{ { },{} }));
			auto s5 = reinterpret_cast<SphereComponent*>((--components.end())->get());

			// Orbit Components

			components.push_back(std::make_unique<OrbitController>(GameToComponentBridge{ device, window }, center->transform, c2->transform, 6, 1));

			components.push_back(std::make_unique<OrbitController>(GameToComponentBridge{ device, window }, c2->transform, c3->transform, 3, 2));

			components.push_back(std::make_unique<OrbitController>(GameToComponentBridge{ device, window }, center->transform, c4->transform, 9, 2));

			components.push_back(std::make_unique<OrbitController>(GameToComponentBridge{ device, window }, center->transform, c5->transform, 9, -2));

			components.push_back(std::make_unique<OrbitController>(GameToComponentBridge{ device, window }, c5->transform, s1->transform, 3, 1));

			components.push_back(std::make_unique<OrbitController>(GameToComponentBridge{ device, window }, s1->transform, s2->transform, 1.5, -2));

			components.push_back(std::make_unique<OrbitController>(GameToComponentBridge{ device, window }, center->transform, s3->transform, 15, 3));

			components.push_back(std::make_unique<OrbitController>(GameToComponentBridge{ device, window }, c3->transform, s4->transform, 18, -3));

			components.push_back(std::make_unique<OrbitController>(GameToComponentBridge{ device, window }, c4->transform, s5->transform, 6, -3));
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

		RastState rastState{ device.getDevice(), CD3D11_RASTERIZER_DESC(D3D11_DEFAULT) };

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