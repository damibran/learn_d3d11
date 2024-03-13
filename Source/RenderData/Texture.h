#pragma once

#include <stdexcept>

#include <d3d11.h>
#include "Utils/image_data.h"

namespace dmbrn
{
	/**
	 * \brief manages texture image data on GPU
	 * TODO need to cache current layout it makes transitioning clearer
	 */
	class Texture
	{
	public:
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		Texture(Texture&&) = default;
		Texture& operator=(Texture&&) = default;

		//Texture(vk::Extent2D extent) :
		//{
		//
		//}

		Texture(ID3D11Device* device, const image_data& image_data)
		{
			D3D11_TEXTURE2D_DESC desc;
			desc.Width = image_data.width;
			desc.Height = image_data.height;
			desc.MipLevels = desc.ArraySize = 3;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;

			if (image_data.comp_per_pix != 4)
			{
				throw std::runtime_error("Unsupported texture comp count");
			}

			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = image_data.data.data();
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;

			device->CreateTexture2D(&desc, &InitData, &texture);

			D3D11_SAMPLER_DESC sampDesc = {};
			sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			device->CreateSamplerState(&sampDesc, &sampler);

			device->CreateShaderResourceView(texture, nullptr, &textureSRV);
		}

		~Texture()
		{
			textureSRV->Release();
			sampler->Release();
			texture->Release();
		}

	private:
		ID3D11Texture2D* texture;
		ID3D11SamplerState* sampler;
		ID3D11ShaderResourceView* textureSRV;
	};
}