#pragma once

#include <array>
#include <algorithm>

#include <d3d11.h>
#include <directxtk/SimpleMath.h>

#include <iostream>
#include <assimp/scene.h>
#include <assimp/texture.h>

#include "Utils/StdUtils.h"

#include "Texture.h"
#include "ConstantBuffer.h"

namespace dmbrn
{
	/**
	 * \brief describes surface properties of object
	 */
	class DiffusionMaterial
	{
	public:
		DiffusionMaterial(const DiffusionMaterial& other) = delete;
		DiffusionMaterial(DiffusionMaterial&& other) noexcept = default;
		DiffusionMaterial& operator=(const DiffusionMaterial& other) = delete;
		DiffusionMaterial& operator=(DiffusionMaterial&& other) noexcept = default;

		void bindMaterialData(ID3D11DeviceContext* cntx) const
		{
			diffuse.bind(cntx);
			base_color.bind(cntx, 2);
			//command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
			//	layout, 2,
			//	*descriptor_sets_[frame], nullptr);
		}

		DiffusionMaterial(ID3D11Device* device, ID3D11DeviceContext* cntx, const std::wstring& directory, const aiScene* scene,
			const aiMaterial* ai_material) :
			diffuse(device, cntx,aiTextureType_DIFFUSE, 0, directory, scene, ai_material),
			base_color(device, getBaseColor(ai_material))
		{
		}
	private:

		struct alignas(16) DiffConstrantBuffer
		{
			DirectX::SimpleMath::Vector4 base_color;
		};

		static DiffConstrantBuffer getBaseColor(const aiMaterial* ai_material)
		{
			DiffConstrantBuffer res;
			aiColor4D c;
			ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, c); // or AI_MATKEY_BASE_COLOR on first glance are same
			res.base_color = DirectX::SimpleMath::Vector4{ c.r, c.g, c.b, c.a };
			return res;
		}

		Texture diffuse;
		//TODO not dynamic usage
		ConstantBuffer<DiffConstrantBuffer> base_color;
	};
}