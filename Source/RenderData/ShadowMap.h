#pragma once

#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include <RenderData/Shaders.h>
#include <RenderData/RasterState.h>
#include <Components/СoncreteComponent/ModelComponent.h>

namespace dmbrn
{
	class ShadowMap
	{
	public:
		static inline int Width = 2048;
		static inline int Height = 2048;

		ShadowMap(ID3D11Device* device, RastState& raster) :
			shaders(device, L"./Shaders/ShadowMap.hlsl"),
			rast_state(raster),
			constBuf(device, lightMat)
		{
			D3D11_TEXTURE2D_DESC textureDesc{};
			textureDesc.Width = Width;
			textureDesc.Height = Height;
			textureDesc.ArraySize = 1;
			textureDesc.MipLevels = 1;
			textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			textureDesc.SampleDesc.Count = 1;

			if (FAILED(device->CreateTexture2D(&textureDesc, NULL, &m_DSTexture)))
				throw std::exception("ShadowMap textureDesc");

			// создаем depth stencil view
			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
			ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
			depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;
			if (FAILED(device->CreateDepthStencilView(m_DSTexture, &depthStencilViewDesc, &m_DSV)))
				throw std::exception("ShadowMap depthStencilViewDesc");

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
			shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;
			if (FAILED(device->CreateShaderResourceView(m_DSTexture, &shaderResourceViewDesc, &m_SRV)))
				throw std::exception("ShadowMap shaderResourceViewDesc");

			D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc{};

			depthStencilStateDesc.DepthEnable = TRUE;
			depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
			depthStencilStateDesc.StencilEnable = FALSE;

			device->CreateDepthStencilState(&depthStencilStateDesc, &ds_state_);

			D3D11_SAMPLER_DESC sampDesc{};
			sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
			//sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
			//sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			device->CreateSamplerState(&sampDesc, &sampler);
		}

		~ShadowMap()
		{
			sampler->Release();
			ds_state_->Release();
			m_SRV->Release();
			m_DSV->Release();
			m_DSTexture->Release();
		}

		// real ugly shit
		void SetLightTransfrom(TransformComponent* lt)
		{
			lightTransform = lt;
		}

		void RenderDataUpdate(ID3D11DeviceContext* cntx)
		{
			auto mat = constBuf.map(cntx);

			mat->lightMat = lightTransform->getMatrix().Invert() * lightProj;

			mat->lightMat = mat->lightMat.Transpose();
		}

		void bind(ID3D11DeviceContext* cntx)
		{
			constBuf.bindToVertex(cntx, 4);
			cntx->PSSetShaderResources(1, 1, &m_SRV);
			cntx->PSSetSamplers(1, 1, &sampler);
		}

		void Draw(ID3D11DeviceContext* cntx, std::vector<ModelComponent*> models)
		{
			D3D11_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<FLOAT>(Width), static_cast<FLOAT>(Height), 0.0f, 1.0f };
			cntx->RSSetViewports(1, &viewport);

			ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
			cntx->PSSetShaderResources(1, 1, nullSRV);
			cntx->OMSetRenderTargets(0, nullptr, m_DSV);
			cntx->ClearDepthStencilView(m_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
			cntx->OMSetDepthStencilState(ds_state_, 0);

			rast_state.bind(cntx);
			cntx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			constBuf.bindToVertex(cntx, 0);

			for (auto& model : models)
			{
				model->constBuf.bindToVertex(cntx, 1);
				for (auto& mesh : model->meshes)
				{
					mesh.bindVI(cntx);
					shaders.bindShaders(cntx);
					mesh.drawIndexed(cntx);
				}
			}
		}

	private:
		DirectX::SimpleMath::Matrix lightProj = DirectX::SimpleMath::Matrix::CreateOrthographic(100, 100, 0.0001, 1000);

		RastState& rast_state;

		Shaders shaders;

		ID3D11Texture2D* m_DSTexture;
		ID3D11DepthStencilView* m_DSV;
		ID3D11ShaderResourceView* m_SRV;
		ID3D11DepthStencilState* ds_state_;

		ID3D11SamplerState* sampler;

		TransformComponent* lightTransform;

		struct SLightMat {
			DirectX::SimpleMath::Matrix lightMat;
		} lightMat;

		ConstantBuffer<decltype(lightMat)> constBuf;
	};
}
