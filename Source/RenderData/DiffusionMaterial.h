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
		/**
		 * \brief key into material registry
		 */
		struct MaterialRegistryHandle
		{
			image_data diffuse_texture;
			DirectX::SimpleMath::Vector4 base_color = { 0.f,0.f,0.f,0.f };

			struct hash
			{
				size_t operator()(const MaterialRegistryHandle& handle) const
				{
					std::hash<image_data> img_hasher;
					std::hash<DirectX::SimpleMath::Vector4> vec_hasher;
					const size_t res = img_hasher(handle.diffuse_texture) ^ vec_hasher(handle.base_color);
					std::cout << "Hash: " << res << std::endl;
					return res;
				}
			};

			bool operator==(const MaterialRegistryHandle& other) const
			{
				return base_color == other.base_color && diffuse_texture == other.diffuse_texture;
			}
		};


	public:
		DiffusionMaterial() = delete;

		DiffusionMaterial(const DiffusionMaterial&) = delete;
		DiffusionMaterial& operator=(const DiffusionMaterial&) = delete;

		DiffusionMaterial(DiffusionMaterial&&) = default;
		DiffusionMaterial& operator=(DiffusionMaterial&&) = default;

		// TODO actually some where it should delete it self from registry but reference counting needed so shred pointer to the rescue
		~DiffusionMaterial() = default;

		void bindMaterialData(ID3D11DeviceContext* cntx) const
		{
			//command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
			//	layout, 2,
			//	*descriptor_sets_[frame], nullptr);
		}

		static DiffusionMaterial* GetMaterialPtr(ID3D11Device* device, const std::string& directory, const aiScene* scene,
			const aiMaterial* ai_material)
		{
			std::cout << "\nNew mat\n";
			MaterialRegistryHandle handle = getHandle(directory, scene, ai_material);

			auto it = material_registry.find(handle);
			if (it == material_registry.end())
			{
				std::cout << "didn't found\n";
				it = material_registry.emplace(std::move(handle), DiffusionMaterial{device, directory, scene, ai_material }).
					first;
			}
			else
			{
				std::cout << "found\n";
			}

			return &it->second;
		}

		static size_t getRegistrySize()
		{
			return material_registry.size();
		}

	private:
		DiffusionMaterial(ID3D11Device* device, const std::string& directory, const aiScene* scene,
			const aiMaterial* ai_material) :
			diffuse(device, getTexture(aiTextureType_DIFFUSE, 0, directory, scene, ai_material)),
			base_color(device, getBaseColor(ai_material))
		{
		}

		static MaterialRegistryHandle getHandle(const std::string& directory, const aiScene* scene,
			const aiMaterial* ai_material)
		{
			MaterialRegistryHandle h;

			h.diffuse_texture = getTexture(aiTextureType_DIFFUSE, 0, directory, scene, ai_material);
			h.base_color = getBaseColor(ai_material).base_color;

			return h;
		}
		
		struct alignas(16) DiffConstrantBuffer
		{
			DirectX::SimpleMath::Vector4 base_color;
		};

		static DiffConstrantBuffer getBaseColor(const aiMaterial* ai_material)
		{
			DiffConstrantBuffer res;
			aiColor4D c;
			//ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, c); // or AI_MATKEY_BASE_COLOR on first glance are same
			res.base_color = DirectX::SimpleMath::Vector4{ c.r, c.g, c.b, c.a };
			return res;
		}

		[[nodiscard]] static image_data getTexture(aiTextureType type, int index, const std::string directory,
			const aiScene* scene,
			const aiMaterial* ai_material)
		{
			image_data res;
			aiString s;
			//ai_material->Get(AI_MATKEY_TEXTURE(type, index), s);

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
					//throw std::runtime_error("failed to load texture image! " + std::string(s.C_Str()));
					abort();

				res.copyData(temp, res.getLength());

				stbi_image_free(temp);
			}


			return res;
		}

		Texture diffuse;
		//TODO not dynamic usage
		ConstantBuffer<DiffConstrantBuffer> base_color;
		static inline std::unordered_map<MaterialRegistryHandle, DiffusionMaterial, MaterialRegistryHandle::hash>
			material_registry;
	};
}