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
#include "IGameComponent.h"
#include "TriangleComponent.h"
#include "SimpleMath.h"

namespace dmbrn
{
	class Game
	{
	public:
		Game()
		{
			components.push_back(std::make_unique<TriangleComponent>(*this, L"./Shaders/MyVeryFirstShader.hlsl"));
			components.push_back(std::make_unique<TriangleComponent>(*this, L"./Shaders/MyVeryFirstShader.hlsl", 45, DirectX::SimpleMath::Vector2(0.2, 0.2)));

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

				if (window.getInput().IsKeyDown(Keys::W))
				{
					std::cout << "W";
				}

				for (auto&& comp : components)
				{
					comp->Update();
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
			ImGui::Render();
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