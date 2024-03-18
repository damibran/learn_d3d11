#pragma once

#include <d3d11.h>

namespace dmbrn
{
	class DepthBuffer
	{
	public:
		DepthBuffer(ID3D11Device* device)
		{
			initDepthBuffer(device, 1, 1);
		}

		~DepthBuffer()
		{
			releseOld();
		}

		void resize(ID3D11Device* device, int width, int height)
		{
			releseOld();
			initDepthBuffer(device, width, height);
		}

		ID3D11DepthStencilView* getView()
		{
			return ds_view_;
		}

		ID3D11DepthStencilState* getState()
		{
			return ds_state_;
		}

	private:

		// Idk how to properly handle buffer resize, so we will recreate buffer
		void initDepthBuffer(ID3D11Device* device, int width, int height)
		{
			D3D11_TEXTURE2D_DESC descDepth;

			descDepth.Width = width;
			descDepth.Height = height;
			descDepth.MipLevels = 1;
			descDepth.ArraySize = 1;
			descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			descDepth.SampleDesc.Count = 1;
			descDepth.SampleDesc.Quality = 0;
			descDepth.Usage = D3D11_USAGE_DEFAULT;
			descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			descDepth.CPUAccessFlags = 0;
			descDepth.MiscFlags = 0;

			device->CreateTexture2D(&descDepth, nullptr, &texture);

			device->CreateDepthStencilView(texture, nullptr, &ds_view_);

			D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;

			depthStencilStateDesc.DepthEnable = TRUE;
			depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
			depthStencilStateDesc.StencilEnable = FALSE;

			device->CreateDepthStencilState(&depthStencilStateDesc, &ds_state_);
		}

		void releseOld()
		{
			ds_state_->Release();
			ds_view_->Release();
			texture->Release();
		}

		ID3D11Texture2D* texture;
		ID3D11DepthStencilView* ds_view_;
		ID3D11DepthStencilState* ds_state_;
	};
}
