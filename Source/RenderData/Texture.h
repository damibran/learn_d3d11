#pragma once

#include <stdexcept>
#include <string>

#include "Utils/strToWstr.h"

#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
#include <directxtk/WICTextureLoader.h>


namespace dmbrn
{
	/**
	 * \brief manages texture image data on GPU
	 */
	class Texture
	{
	public:
		Texture(const Texture& other) = delete;
		Texture(Texture&& other) noexcept = default;
		Texture& operator=(const Texture& other) = delete;
		Texture& operator=(Texture&& other) noexcept = default;

		Texture(ID3D11Device* device, ID3D11DeviceContext* cntx,
			aiTextureType type,
			int index,
			const std::wstring& directory,
			const aiScene* scene,
			const aiMaterial* ai_material)
		{
			aiString s;
			ai_material->Get(AI_MATKEY_TEXTURE(type, index), s);

			if (s.length == 0)
			{
				// create one pixel white texture
			}
			else if (auto dif_texture = scene->GetEmbeddedTexture(s.C_Str()))
			{
				// create texture from memory
			}
			else
			{
				std::wstring path = directory + L"\\" + strToWstr(s.C_Str());

				DirectX::CreateWICTextureFromFileEx(
					device,
					cntx,
					path.c_str(),
					0,
					D3D11_USAGE_DEFAULT,
					D3D11_BIND_SHADER_RESOURCE,
					0,
					D3D11_RESOURCE_MISC_GENERATE_MIPS,
					DirectX::WIC_LOADER_FORCE_RGBA32,
					texture.GetAddressOf(),
					textureSRV.GetAddressOf());
			}

			D3D11_SAMPLER_DESC sampDesc = {};
			sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			device->CreateSamplerState(&sampDesc, sampler.GetAddressOf());
		}

		void bind(ID3D11DeviceContext* cntx)const
		{
			cntx->PSSetShaderResources(0, 1, textureSRV.GetAddressOf());
			cntx->PSSetSamplers(0, 1, sampler.GetAddressOf());
		}

	private:
		// actually there should be unique ptr
		ComPtr<ID3D11Resource> texture;
		ComPtr<ID3D11SamplerState> sampler;
		ComPtr<ID3D11ShaderResourceView> textureSRV;
	};
}