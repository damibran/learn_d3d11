#pragma once

#include <chrono>
#include <vector>
#include <memory>

using duration = std::chrono::duration<double>;
using sys_clock = std::chrono::system_clock;
using time_point = std::chrono::time_point<sys_clock, duration>;

#include "DeviceWrapper.h"
#include "DXGIWindowWrapper.h"
#include "SwapChainWrapper.h"
#include "ImGuiWrapper.h"
#include "Components/IGameComponent.h"
#include "Components/СoncreteComponent/TriangleComponent.h"
#include "Components/СoncreteComponent/RectangleComponent.h"
#include "Components/СoncreteComponent/RacketComponent.h"
#include "Components/СoncreteComponent/BallComponent.h"
#include "SimpleMath.h"

namespace dmbrn
{
	class Game
	{
	public:
		Game()
		{
			components.push_back(std::make_unique<RacketComponent>(*this, L"./Shaders/MovingRec.hlsl", DirectX::SimpleMath::Vector2(0.2, 0.5), DirectX::SimpleMath::Vector2{ -1,0 }, Keys::Up, Keys::Down));
			components.push_back(std::make_unique<RacketComponent>(*this, L"./Shaders/MovingRec.hlsl", DirectX::SimpleMath::Vector2(0.2, 0.5), DirectX::SimpleMath::Vector2{ 0.8,0 }));
			components.push_back(std::make_unique<BallComponent>(*this, L"./Shaders/MovingRec.hlsl", DirectX::SimpleMath::Vector2(0.1, 0.1), DirectX::SimpleMath::Vector2{ 0,0 }, 0.2));
		}

		void run()
		{
			window.show();

			for (auto&& comp : components)
			{
				comp->Initialize();
			}

			while (!window.windowShouldClose())
			{
				tp2_ = sys_clock::now();
				const duration elapsed_time = tp2_ - tp1_;
				tp1_ = tp2_;
				const double delta_time = elapsed_time.count();

				// Handle window resize (we don't resize directly in the WM_SIZE handler)
				if (window.pendingResize)
				{
					swapChain.resize(device, window.resizeWidth, window.resizeHeight);
					window.pendingResize = false;
				}

				// game loop body

				for (auto&& comp : components)
				{
					comp->Update(delta_time);
				}

				imGui.drawImGuiUI();

				drawFrame(delta_time);
			}

			for (auto&& comp : components)
			{
				comp->DestroyResources();
			}
		}

		//TODO: temp
		DeviceWrapper device;
		DXGIWindowWrapper window;
	private:
		SwapChainWrapper swapChain{ window,device };
		ImGuiWrapper imGui{ device,window };

		std::vector<std::unique_ptr<IGameComponent>>  components;

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

			// draw components
			for (auto&& comp : components)
			{
				comp->Draw();
			}

			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			swapChain->Present(1, 0); // Present with vsync
		}
	};
}