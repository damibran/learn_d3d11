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
#include "RenderData/RasterState.h"
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
		Game() :
			viewCB(device.getDevice(), viewMat)
		{
			components.push_back(std::make_unique<RacketComponent>(*this, rastState, L"./Shaders/MovingRec.hlsl", DirectX::SimpleMath::Vector2(0.1, 0.5), DirectX::SimpleMath::Vector2{ -0.8,0 }, Keys::Up, Keys::Down));
			components.push_back(std::make_unique<RacketComponent>(*this, rastState, L"./Shaders/MovingRec.hlsl", DirectX::SimpleMath::Vector2(0.1, 0.5), DirectX::SimpleMath::Vector2{ 0.8,0 }));
			components.push_back(std::make_unique<BallComponent>(*this, rastState, L"./Shaders/MovingRec.hlsl", DirectX::SimpleMath::Vector2(0.1, 0.1),
				*(dynamic_cast<RacketComponent*>(components[0].get())),
				*(dynamic_cast<RacketComponent*>(components[1].get())),
				DirectX::SimpleMath::Vector2{ 0,0 }, 0.2));

			imGui.setBallSpeedPtr(&dynamic_cast<BallComponent*>(components[2].get())->speed);
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
				double delta_time = elapsed_time.count();

				// Handle window resize (we don't resize directly in the WM_SIZE handler)
				if (window.pendingResize)
				{
					swapChain.resize(device, window.resizeWidth, window.resizeHeight);
					window.pendingResize = false;
					delta_time = 0;
				}

				// game loop body

				for (auto&& comp : components)
				{
					comp->Update(delta_time);
				}

				//physics.update();

				for (auto&& comp : components)
				{
					comp->PhysicsUpdate(delta_time);
				}

				for (auto&& comp : components)
				{
					comp->CollisionUpdate(delta_time);
				}

				for (auto&& comp : components)
				{
					comp->RenderDataUpdate();
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
		int lRacketScore = 0, rRacketScore = 0;

		struct SViewMat
		{
			DirectX::SimpleMath::Matrix view;
		}viewMat;

		ConstantBuffer<decltype(viewMat)> viewCB;
	private:
		SwapChainWrapper swapChain{ window,device };
		ImGuiWrapper imGui{ device,window,lRacketScore,rRacketScore };

		std::vector<std::unique_ptr<IGameComponent>>  components;

		RastState rastState{ device.getDevice(),CD3D11_RASTERIZER_DESC(D3D11_DEFAULT) };

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

			auto mat = viewCB.map(device.getContext());
			mat->view = DirectX::XMMatrixOrthographicLH(2* viewport.Width / viewport.Height,  2, 0.001, 1);
			mat->view = mat->view.Transpose();
			viewCB.upmap(device.getContext());

			// draw componentsS
			for (auto&& comp : components)
			{
				comp->Draw();
			}

			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			swapChain->Present(1, 0); // Present with vsync
		}
	};
}