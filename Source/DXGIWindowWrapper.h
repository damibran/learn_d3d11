#pragma once

#include "imgui.h"
#include "imgui_impl_win32.h"
#include <d3d11.h>
#include <dxgi1_2.h>
// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace dmbrn
{
	class DXGIWindowWrapper
	{
	public:

		DXGIWindowWrapper()
		{
			wc = { sizeof(wc), CS_CLASSDC, DXGIWindowWrapper::s_WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
			::RegisterClassExW(&wc);
			hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX12 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, this);
		}

		const HWND& operator*()
		{
			return hwnd;
		}

		bool windowShouldClose()
		{
			bool res = false;
			MSG msg;
			while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					res = true;
			}
			return res;
		}

		void show()
		{
			::ShowWindow(hwnd, SW_SHOWDEFAULT);
			::UpdateWindow(hwnd);
		}

		// Win32 message handler
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
				return true;

			switch (msg)
			{
			case WM_SIZE:
				if (wParam != SIZE_MINIMIZED)
				{
					pendingResize = true;
					resizeWidth = (UINT)LOWORD(lParam); // Queue resize
					resizeHeight = (UINT)HIWORD(lParam);
					//WaitForLastSubmittedFrame();
					//swapChain.resize(device, lParam);
				}
				return 0;
			case WM_SYSCOMMAND:
				if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
					return 0;
				break;
			case WM_DESTROY:
				::PostQuitMessage(0);
				return 0;
			}
			return ::DefWindowProcW(hWnd, msg, wParam, lParam);
		}

		static LRESULT CALLBACK s_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			DXGIWindowWrapper* pThis; // our "this" pointer will go here
			if (uMsg == WM_NCCREATE) 
			{
				// Recover the "this" pointer which was passed as a parameter
				// to CreateWindow(Ex).
				LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
				pThis = static_cast<DXGIWindowWrapper*>(lpcs->lpCreateParams);
				// Put the value in a safe place for future use
				SetWindowLongPtr(hwnd, GWLP_USERDATA,reinterpret_cast<LONG_PTR>(pThis));
			}
			else 
			{
				// Recover the "this" pointer from where our WM_NCCREATE handler
				// stashed it.
				pThis = reinterpret_cast<DXGIWindowWrapper*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			}
			if (pThis) 
			{
				// Now that we have recovered our "this" pointer, let the
				// member function finish the job.
				return pThis->WndProc(hwnd, uMsg, wParam, lParam);
			}
			// We don't know what our "this" pointer is, so just do the default
			// thing. Hopefully, we didn't need to customize the behavior yet.
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		bool pendingResize = false;
		UINT resizeWidth = 0, resizeHeight = 0;
	private:
		HWND hwnd;
		WNDCLASSEXW wc;
	};
}