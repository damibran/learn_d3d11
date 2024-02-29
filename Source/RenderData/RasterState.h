#pragma once

#include <d3d11.h>

namespace dmbrn
{
	struct RastState
	{
		RastState(ID3D11Device* device, CD3D11_RASTERIZER_DESC rastDesc)
		{
			device->CreateRasterizerState(&rastDesc, &rastState);
		}

		~RastState()
		{

			rastState->Release();
			rastState = nullptr;
		}

		void bind(ID3D11DeviceContext* cntx)
		{
			cntx->RSSetState(rastState);
		}

	private:
		ID3D11RasterizerState* rastState = nullptr;
	};
}