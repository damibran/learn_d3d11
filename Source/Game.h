#pragma once

#include <chrono>

using duration = std::chrono::duration<double>;
using sys_clock = std::chrono::system_clock;
using time_point = std::chrono::time_point<sys_clock, duration>;

#include "DeviceWrapper.h"
#include "DXGIWindowWrapper.h"
#include "SwapChainWrapper.h"
#include "ImGuiWrapper.h"

namespace dmbrn
{
	class Game
	{
	public:
		Game()
		{

		}

		void run()
		{
			window.show();

			while (!window.windowShouldClose())
			{
				tp2_ = sys_clock::now();
				const duration elapsed_time = tp2_ - tp1_;
				tp1_ = tp2_;
				const double delta_time = elapsed_time.count();

				imGui.drawImGuiUI();

				drawFrame(delta_time);
			}
		}

	private:
		//Scene scene_;
		DeviceWrapper device;
		DXGIWindowWrapper window;
		SwapChainWrapper swapChain{ window,device };
		ImGuiWrapper imGui{ device,window };

		time_point tp1_ = std::chrono::time_point_cast<duration>(sys_clock::now());
		time_point tp2_ = std::chrono::time_point_cast<duration>(sys_clock::now());

		void drawFrame(double delta_time)
		{

			// Handle window resize (we don't resize directly in the WM_SIZE handler)
			if (window.pendingResize)
			{
				swapChain.resize(device, window.resizeWidth, window.resizeHeight);
				window.pendingResize = false;
			}

			// Rendering
			ImGui::Render();
			const float clear_color_with_alpha[4] = { imGui.clear_color.x * imGui.clear_color.w, imGui.clear_color.y * imGui.clear_color.w, imGui.clear_color.z * imGui.clear_color.w, imGui.clear_color.w };
			device.getContext()->OMSetRenderTargets(1, &swapChain.getRenderTarget(), nullptr);
			device.getContext()->ClearRenderTargetView(swapChain.getRenderTarget(), clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			swapChain->Present(1, 0); // Present with vsync
		}
	};
}