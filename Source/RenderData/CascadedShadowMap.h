#pragma once

#include <d3d11.h>
#include <exception>

#include "ConstantBuffer.h"
#include "Shaders.h"
#include "RenderData/RasterState.h"
#include "Components/Transform.h"
#include "Components/СoncreteComponent/ModelComponent.h"

namespace dmbrn
{
	class CascadedShadowMap
	{
	public:
		static inline constexpr int CascadeCount = 4;
		static inline constexpr float CascadesFarRatios[4] = { 0.2,0.4,0.6,1.0 };
		static inline constexpr int Width = 2048;
		static inline constexpr int Height = 2048;

		CascadedShadowMap(ID3D11Device* device, RastState& raster) :
			shaders(device, L"./Shaders/CascadedShadowMap.hlsl", false, true),
			rast_state(raster),
			constBuf(device, cascadesData)
		{
			D3D11_TEXTURE2D_DESC textureDesc{};
			textureDesc.Width = Width;
			textureDesc.Height = Height;
			textureDesc.ArraySize = CascadeCount;
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
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			depthStencilViewDesc.Texture2DArray.MipSlice = 0;
			depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
			depthStencilViewDesc.Texture2DArray.ArraySize = 4;
			if (FAILED(device->CreateDepthStencilView(m_DSTexture, &depthStencilViewDesc, &m_DSV)))
				throw std::exception("ShadowMap depthStencilViewDesc");

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
			shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2DArray.MipLevels = 1;
			shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
			shaderResourceViewDesc.Texture2DArray.ArraySize = 4;
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

		~CascadedShadowMap()
		{
			sampler->Release();
			ds_state_->Release();
			m_SRV->Release();
			m_DSV->Release();
			m_DSTexture->Release();
		}

		// real ugly shit
		void setCamAndLightPtrs(TransformComponent* lt, Camera* cam)
		{
			lightTransform = lt;
			camera = cam;
		}

		void RenderDataUpdate(ID3D11DeviceContext* cntx)
		{
			auto data = constBuf.map(cntx);

			SCascadesData newData = calcLightMatrices(lightTransform->getRotationMatrix().Forward(), *camera);

			for (int i = 0; i < CascadeCount; ++i)
			{
				data->viewProjMats[i] = newData.viewProjMats[i].Transpose();
				data->distances[i] = newData.distances[i];
			}
		}

		void bind(ID3D11DeviceContext* cntx)
		{
			constBuf.bindToFragment(cntx, 4);
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
			cntx->ClearDepthStencilView(m_DSV, D3D11_CLEAR_DEPTH, 1, 0);
			cntx->OMSetDepthStencilState(ds_state_, 0);

			rast_state.bind(cntx);
			cntx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			constBuf.bindToGeometry(cntx, 0);

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
		Camera* camera;

		alignas(16)
			struct SCascadesData {
			DirectX::SimpleMath::Matrix viewProjMats[CascadeCount];
			float distances[CascadeCount];
		} cascadesData;

		ConstantBuffer<decltype(cascadesData)> constBuf;

	private:
		static SCascadesData calcLightMatrices(const DirectX::SimpleMath::Vector3& lightDir, const Camera& cam)
		{
			SCascadesData res;
			float curNear = cam.NearZ;
			for (int i = 0; i < CascadeCount; ++i)
			{
				float curFar = cam.FarZ * CascadesFarRatios[i];

				const auto subFrustProj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(cam.FovAngleY, cam.AspectRatio, curNear, curFar);

				curNear = curFar;

				auto corners = getFrustumCornersWorldSpace(subFrustProj, cam.getView());

				DirectX::SimpleMath::Vector3 center = DirectX::SimpleMath::Vector3::Zero;
				for (const auto& v : corners)
				{
					center += DirectX::SimpleMath::Vector3(v.x, v.y, v.z);
				}
				center /= corners.size();

				//todo corner case up and dir
				const auto lightView = DirectX::SimpleMath::Matrix::CreateLookAt(
					center,
					center + lightDir,
					DirectX::SimpleMath::Vector3::Up
				);

				//std::cout<<center.x<<" " << center.y << " " << center.z << "\n";

				float minX = std::numeric_limits<float>::max();
				float maxX = std::numeric_limits<float>::lowest();
				float minY = std::numeric_limits<float>::max();
				float maxY = std::numeric_limits<float>::lowest();
				float minZ = std::numeric_limits<float>::max();
				float maxZ = std::numeric_limits<float>::lowest();
				for (const auto& v : corners)
				{
					const auto trf = DirectX::SimpleMath::Vector4::Transform(v, lightView);
					minX = std::min(minX, trf.x);
					maxX = std::max(maxX, trf.x);
					minY = std::min(minY, trf.y);
					maxY = std::max(maxY, trf.y);
					minZ = std::min(minZ, trf.z);
					maxZ = std::max(maxZ, trf.z);
				}

				static constexpr float zMult = 10.0f;
				minZ = (minZ < 0) ? minZ * zMult : minZ / zMult;
				maxZ = (maxZ < 0) ? maxZ / zMult : maxZ * zMult;

				auto lightProj = DirectX::SimpleMath::Matrix::CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ);

				res.viewProjMats[i] = lightView * lightProj;
				res.distances[i] = curFar;
			}

			return res;
		}

		static std::vector<DirectX::SimpleMath::Vector4> getFrustumCornersWorldSpace(const DirectX::SimpleMath::Matrix& proj, const DirectX::SimpleMath::Matrix& view)
		{
			const auto inv = (view * proj).Invert();

			std::vector<DirectX::SimpleMath::Vector4> frustumCorners;
			frustumCorners.reserve(8);
			for (unsigned int x = 0; x < 2; ++x)
			{
				for (unsigned int y = 0; y < 2; ++y)
				{
					for (unsigned int z = 0; z < 2; ++z)
					{
						const DirectX::SimpleMath::Vector4 pt =
							DirectX::SimpleMath::Vector4::Transform(DirectX::SimpleMath::Vector4(
								2.0f * static_cast<float>(x) - 1.0f,
								2.0f * static_cast<float>(y) - 1.0f,
								static_cast<float>(z),
								1.0f
							), inv);
						frustumCorners.push_back(pt / pt.w);
					}
				}
			}

			return frustumCorners;
		}
	};
}
