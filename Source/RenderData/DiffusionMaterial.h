#pragma once

#include <array>
#include <algorithm>

#include <d3d11.h>
#include "SimpleMath.h"

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
			//command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
			//	layout, 2,
			//	*descriptor_sets_[frame], nullptr);
		}

		DiffusionMaterial(ID3D11Device* device, const std::string& directory, const aiScene* scene,
			const aiMaterial* ai_material) :
			diffuse(device, getTexture(aiTextureType_DIFFUSE, 0, directory, scene, ai_material)),
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

		[[nodiscard]] static image_data getTexture(aiTextureType type, int index, const std::string directory,
			const aiScene* scene,
			const aiMaterial* ai_material)
		{
			image_data res;
			aiString s;
			ai_material->Get(AI_MATKEY_TEXTURE(type, index), s);

			if (s.length == 0)
			{
				std::array<unsigned char, 4> white = { 255, 255, 255, 255 };

				res.width = 1;
				res.height = 1;
				res.comp_per_pix = 4;

				res.copyData(white.data(), res.getLength());

				stbi_info_from_memory(white.data(), 3, &res.width, &res.height,
					&res.comp_per_pix);
			}
			else if (auto dif_texture = scene->GetEmbeddedTexture(s.C_Str()))
			{
				if (dif_texture->mHeight == 0)
				{
					stbi_uc* temp = stbi_load_from_memory(reinterpret_cast<unsigned char const* const>(dif_texture->pcData),
						dif_texture->mWidth, &res.width, &res.height,
						&res.comp_per_pix,
						STBI_rgb_alpha);
					res.comp_per_pix = 4; // because req_comp = STBI_rgb_alpha

					if (!temp)
						//throw std::runtime_error("failed to load texture image! " + std::string(s.C_Str()));
						abort();

					res.copyData(temp, res.getLength());

					stbi_image_free(temp);
				}
				else
				{
					stbi_uc* temp = stbi_load_from_memory(reinterpret_cast<unsigned char const*>(dif_texture->pcData),
						dif_texture->mWidth * dif_texture->mHeight, &res.width,
						&res.height,
						&res.comp_per_pix, STBI_rgb_alpha);

					res.comp_per_pix = 4; // because req_comp = STBI_rgb_alpha

					if (!temp)
						//throw std::runtime_error("failed to load texture image! " + std::string(s.C_Str()));
						abort();

					res.copyData(temp, res.getLength());

					stbi_image_free(temp);
				}
			}
			else
			{
				std::string path = directory + "\\" + std::string(s.C_Str());

				stbi_uc* temp = stbi_load(path.c_str(), &res.width, &res.height, &res.comp_per_pix, STBI_rgb_alpha);
				res.comp_per_pix = 4; // because req_comp = STBI_rgb_alpha

				if (!temp)
					throw std::runtime_error("failed to load texture image! " + std::string(s.C_Str()));

				res.copyData(temp, res.getLength());

				stbi_image_free(temp);
			}


			return res;
		}

		Texture diffuse;
		//TODO not dynamic usage
		ConstantBuffer<DiffConstrantBuffer> base_color;
	};
}