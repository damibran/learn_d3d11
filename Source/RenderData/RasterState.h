#pragma once

#include <d3d11.h>

namespace dmbrn
{
	// actually Raster state is property of shader or material and later should be placed there
	struct RastState
	{
		RastState(const RastState& other) = delete;
		RastState(RastState&& other) noexcept = delete;
		RastState& operator=(const RastState& other) = delete;
		RastState& operator=(RastState&& other) noexcept = delete;

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

		static inline CD3D11_RASTERIZER_DESC Default
		{
			D3D11_FILL_SOLID,
			D3D11_CULL_BACK,
			TRUE,
			D3D11_DEFAULT_DEPTH_BIAS,
			D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
			D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
			TRUE,
			FALSE,
			FALSE,
			FALSE
		};

		static inline INT depthBias = 100000;
		static inline FLOAT depthBiasClamp = 0;
		static inline FLOAT scaledDepthBias = 1;

		static inline CD3D11_RASTERIZER_DESC ShadowMap
		{
			D3D11_FILL_SOLID,
			D3D11_CULL_FRONT,
			TRUE,
			depthBias,
			depthBiasClamp,
			scaledDepthBias,
			TRUE,
			FALSE,
			FALSE,
			FALSE
		};

	private:
		ID3D11RasterizerState* rastState = nullptr;
	};
}