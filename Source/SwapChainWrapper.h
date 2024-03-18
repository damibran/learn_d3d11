#pragma once

#include "DXGIWindowWrapper.h"
#include <assert.h>
#include <d3d11.h>
#include <dxgi1_2.h>

#include "DeviceWrapper.h"
#include "RenderData/DepthBuffer.h"

namespace dmbrn {
	class SwapChainWrapper {
	public:
		SwapChainWrapper(DXGIWindowWrapper& window, DeviceWrapper& device) :
			depth_buffer_(device.getDevice())
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
			// swapChainDesc.Flags = CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

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
			CleanupRenderTarget();
			if (pSwapChain) {
				pSwapChain->Release();
				pSwapChain = nullptr;
			}
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

			depth_buffer_.resize(device.getDevice(), resizeWidth, resizeHeight);
		}

		void setRenderTargets(ID3D11DeviceContext* cntx, const DirectX::SimpleMath::Vector4& cc)
		{
			float temp[4] = { cc.x,cc.y,cc.z,cc.w };
			cntx->OMSetRenderTargets(1, &mainRenderTargetView, depth_buffer_.getView());
			cntx->ClearRenderTargetView(mainRenderTargetView, temp);

			cntx->ClearDepthStencilView(depth_buffer_.getView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
			cntx->OMSetDepthStencilState(depth_buffer_.getState(), 0);
		}


	private:
		IDXGISwapChain1* pSwapChain = nullptr;
		ID3D11RenderTargetView* mainRenderTargetView = nullptr;
		DepthBuffer depth_buffer_;

		void CreateRenderTarget(DeviceWrapper& device)
		{
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
			device.getDevice()->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
			pBackBuffer->Release();
		}

		void CleanupRenderTarget()
		{
			if (mainRenderTargetView) {
				mainRenderTargetView->Release();
				mainRenderTargetView = nullptr;
			}
		}
	};
}