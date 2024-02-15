#pragma once

#include <assert.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include "DXGIWindowWrapper.h"
#include "Singletons.h"

namespace dmbrn
{
	class SwapChainWrapper
	{
	public:
		SwapChainWrapper(DXGIWindowWrapper& window,DeviceWrapper& device)
		{
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
			swapChainDesc.Width = 0;
			swapChainDesc.Height = 0;
			swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.Stereo = FALSE;
			swapChainDesc.SampleDesc = { 1, 0 };
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 2;
			swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			// It is recommended to always allow tearing if tearing support is available.
			//swapChainDesc.Flags = CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

			HRESULT hr;
			IDXGIDevice2* pDXGIDevice;
			hr = device.getDevice()->QueryInterface(__uuidof(IDXGIDevice2), (void**)&pDXGIDevice);

			IDXGIAdapter* pDXGIAdapter;
			hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pDXGIAdapter);

			IDXGIFactory2* pIDXGIFactory;
			pDXGIAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&pIDXGIFactory);

			pIDXGIFactory->CreateSwapChainForHwnd(device.getDevice(), *window, &swapChainDesc, nullptr, nullptr, &pSwapChain);

			pIDXGIFactory->Release();
			pDXGIAdapter->Release();
			pDXGIDevice->Release();

			CreateRenderTarget(device);
		}

		~SwapChainWrapper()
		{
			if (pSwapChain) { pSwapChain->Release(); pSwapChain = nullptr; }
		}

		IDXGISwapChain1* operator->()
		{
			return pSwapChain;
		}

		void resize(DeviceWrapper& device, UINT resizeWidth, UINT resizeHeight)
		{
			CleanupRenderTarget();
			pSwapChain->ResizeBuffers(0, resizeWidth, resizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget(device);
		}

		ID3D11RenderTargetView*& getRenderTarget()
		{
			return mainRenderTargetView;
		}
		
	private:
		IDXGISwapChain1* pSwapChain = nullptr;
		ID3D11RenderTargetView* mainRenderTargetView = nullptr;

		void CreateRenderTarget(DeviceWrapper&device)
		{
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
			device.getDevice()->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
			pBackBuffer->Release();
		}

		void CleanupRenderTarget()
		{
			if (mainRenderTargetView) { mainRenderTargetView->Release(); mainRenderTargetView = nullptr; }
		}
	};
}