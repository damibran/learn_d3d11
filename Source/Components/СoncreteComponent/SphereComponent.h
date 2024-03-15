#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <string>
#define _USE_MATH_DEFINES
#include "../../RenderData/ConstantBuffer.h"
#include "../../RenderData/IndexBuffer.h"
#include "../../RenderData/RasterState.h"
#include "../../RenderData/Shaders.h"
#include "../../RenderData/VertexBuffer.h"
#include "../Transform.h"
#include "Components/IGameComponent.h"
#include <directxtk/SimpleMath.h>
#include <math.h>

namespace dmbrn {
	class SphereComponent : public IGameComponent
	{
	public:
		SphereComponent(GameToComponentBridge bridge, RastState& rs, const std::wstring& shaderPath, const TransformComponent& trans)
			: IGameComponent(bridge)
			, transform(trans)
			, rastState(rs)
			, shaders(bridge.device.getDevice(), shaderPath)
			, vertexBufferData(std::move(makeVertexData(10, 10)))
			, vertexBuffer(bridge.device.getDevice(), shaders.getVertexBC(), vertexBufferData)
			, indexBuffer(bridge.device.getDevice(), vertexBufferData.indexBufferData)
			, constBuf(bridge.device.getDevice(), modelMat)
		{
			axis = DirectX::SimpleMath::Vector3(dis(gen), dis(gen), dis(gen));
			axis.Normalize();
		}


		void Update(float) override
		{
			angle += speed * 0.05;
			DirectX::SimpleMath::Vector3 normal;
			axis.Normalize(normal);
			auto m = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(normal, angle);
			transform.setRad(m.ToEuler());
		}

		void RenderDataUpdate() override
		{
			auto mat = constBuf.map(bridge.device.getContext());

			mat->model = transform.getMatrix();

			mat->model = mat->model.Transpose();
		}

		void Draw() override
		{
			rastState.bind(bridge.device.getContext());

			bridge.device.getContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			vertexBuffer.bindVertexBuffer(bridge.device.getContext());
			indexBuffer.bind(bridge.device.getContext());
			shaders.bindShaders(bridge.device.getContext());

			constBuf.bindToVertex(bridge.device.getContext(), 1);

			bridge.device.getContext()->DrawIndexed(vertexBufferData.indexBufferData.data.size(), 0, 0);
		}

		// temp lab3 data
		float angle = 0.1;
		float speed = 1;
		DirectX::SimpleMath::Vector3 axis = DirectX::SimpleMath::Vector3(0, 0, 1);
		// temp lab3 data

		TransformComponent transform;

	private:
		RastState& rastState;
		Shaders shaders;

		struct VertexBufferData
		{
			struct alignas(16) Vertex
			{
				DirectX::SimpleMath::Vector3 pos;
				DirectX::SimpleMath::Vector4 color;
			};

			VertexBufferData(std::pair<std::vector<Vertex>, std::vector<UINT>>&& d)
				:data(std::move(d.first)), indexBufferData(std::move(d.second))
			{
			}

			D3D11_INPUT_ELEMENT_DESC desc[2] = {
				D3D11_INPUT_ELEMENT_DESC {
					"SV_POSITION",
					0,
					DXGI_FORMAT_R32G32B32_FLOAT,
					0,
					0,
					D3D11_INPUT_PER_VERTEX_DATA,
					0 },
				D3D11_INPUT_ELEMENT_DESC {
					"COLOR",
					0,
					DXGI_FORMAT_R32G32B32A32_FLOAT,
					0,
					offsetof(Vertex, color),
					D3D11_INPUT_PER_VERTEX_DATA,
					0 }
			};

			std::vector<Vertex> data;

			struct IndexBufferData
			{
				static inline DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;

				IndexBufferData(std::vector<UINT>&& d)
					:data(d)
				{}

				std::vector<UINT> data;
			} indexBufferData;

		} vertexBufferData;

		// https://gist.github.com/Pikachuxxxx/5c4c490a7d7679824e0e18af42918efc
		static std::pair<std::vector<VertexBufferData::Vertex>, std::vector<UINT>> makeVertexData(int latitudes, int longitudes)
		{
			std::vector<VertexBufferData::Vertex> vertices;
			std::vector<UINT> indices;
			// add top vertex
			int radius = 1;
			float deltaLatitude = M_PI / latitudes;
			float deltaLongitude = 2 * M_PI / longitudes;
			float latitudeAngle;
			float longitudeAngle;

			// Compute all vertices first except normals
			for (int i = 0; i <= latitudes; ++i)
			{
				latitudeAngle = M_PI / 2 - i * deltaLatitude; /* Starting -pi/2 to pi/2 */
				float xy = radius * cosf(latitudeAngle);    /* r * cos(phi) */
				float z = radius * sinf(latitudeAngle);     /* r * sin(phi )*/

				/*
				 * We add (latitudes + 1) vertices per longitude because of equator,
				 * the North pole and South pole are not counted here, as they overlap.
				 * The first and last vertices have same position and normal, but
				 * different tex coords.
				 */
				for (int j = 0; j <= longitudes; ++j)
				{
					longitudeAngle = j * deltaLongitude;

					VertexBufferData::Vertex vertex;
					vertex.pos.x = xy * cosf(longitudeAngle);       /* x = r * cos(phi) * cos(theta)  */
					vertex.pos.y = xy * sinf(longitudeAngle);       /* y = r * cos(phi) * sin(theta) */
					vertex.pos.z = z;                               /* z = r * sin(phi) */

					vertex.color = DirectX::SimpleMath::Vector4(cosf(longitudeAngle), sinf(longitudeAngle), 0.5, 1);

					vertices.push_back(vertex);
				}
			}

			/*
			 *  Indices
			 *  k1--k1+1
			 *  |  / |
			 *  | /  |
			 *  k2--k2+1
			 */
			unsigned int k1, k2;
			for (int i = 0; i < latitudes; ++i)
			{
				k1 = i * (longitudes + 1);
				k2 = k1 + longitudes + 1;
				// 2 Triangles per latitude block excluding the first and last longitudes blocks
				for (int j = 0; j < longitudes; ++j, ++k1, ++k2)
				{
					if (i != 0)
					{
						indices.push_back(k1);
						indices.push_back(k2);
						indices.push_back(k1 + 1);
					}

					if (i != (latitudes - 1))
					{
						indices.push_back(k1 + 1);
						indices.push_back(k2);
						indices.push_back(k2 + 1);
					}
				}
			}
			return { std::move(vertices),std::move(indices) };
		}

		VertexBuffer<decltype(vertexBufferData)> vertexBuffer;

		IndexBuffer<decltype(vertexBufferData.indexBufferData)> indexBuffer;

		struct SModelMat {
			DirectX::SimpleMath::Matrix model;
		} modelMat;

		ConstantBuffer<decltype(modelMat)> constBuf;

		static inline std::random_device rd;  // Will be used to obtain a seed for the random number engine
		static inline std::mt19937 gen{ rd() }; // Standard mersenne_twister_engine seeded with rd()
		static inline std::uniform_real_distribution<> dis{ -1.0, 1.0 };
	};
}