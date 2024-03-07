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
#include "SimpleMath.h"
#include <math.h>
#include <random>

namespace dmbrn {
	class CubeComponent : public IGameComponent {
	public:
		CubeComponent(GameToComponentBridge bridge, RastState& rs, const std::wstring& shaderPath, const TransformComponent& trans)
			: IGameComponent(bridge)
			, transform(trans)
			, rastState(rs)
			, shaders(bridge.device.getDevice(), shaderPath)
			, vertexBuffer(bridge.device.getDevice(), shaders.getVertexBC(), vertexBufferData)
			, indexBuffer(bridge.device.getDevice(), indexBufferData)
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

			constBuf.upmap(bridge.device.getContext());
		}

		void Draw() override
		{
			rastState.bind(bridge.device.getContext());

			bridge.device.getContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			vertexBuffer.bindVertexBuffer(bridge.device.getContext());
			indexBuffer.bind(bridge.device.getContext());
			shaders.bindShaders(bridge.device.getContext());

			constBuf.bind(bridge.device.getContext(), 1);

			bridge.device.getContext()->DrawIndexed(sizeof(indexBufferData.data), 0, 0);
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

		struct VertexBufferData {
			struct alignas(16) Vertex
			{
				DirectX::SimpleMath::Vector3 pos;
				DirectX::SimpleMath::Vector4 color;
			};

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

			//https://www.3dgep.com/learning-directx-12-2/
			Vertex data[8] = {
				Vertex { DirectX::SimpleMath::Vector3(-1.0f, -1.0f, -1.0f), DirectX::SimpleMath::Vector4(1, 0, 0, 1.0f) },
				Vertex { DirectX::SimpleMath::Vector3(-1.0f,  1.0f, -1.0f), DirectX::SimpleMath::Vector4(0, 1, 0, 1.0f) },
				Vertex { DirectX::SimpleMath::Vector3(1.0f,  1.0f, -1.0f), DirectX::SimpleMath::Vector4(0, 0, 1, 1.0f) },
				Vertex { DirectX::SimpleMath::Vector3(1.0f, -1.0f, -1.0f), DirectX::SimpleMath::Vector4(0, 0, 0, 1) },
				Vertex { DirectX::SimpleMath::Vector3(-1.0f, -1.0f,  1.0f), DirectX::SimpleMath::Vector4(1, 0, 0, 1.0f) },
				Vertex { DirectX::SimpleMath::Vector3(-1.0f,  1.0f,  1.0f), DirectX::SimpleMath::Vector4(0, 1, 0, 1.0f) },
				Vertex { DirectX::SimpleMath::Vector3(1.0f,  1.0f,  1.0f), DirectX::SimpleMath::Vector4(0, 0, 1, 1.0f) },
				Vertex { DirectX::SimpleMath::Vector3(1.0f, -1.0f,  1.0f), DirectX::SimpleMath::Vector4(0, 0, 0, 1) }
			};
		} vertexBufferData;

		VertexBuffer<decltype(vertexBufferData)> vertexBuffer;

		struct IndexBufferData {
			static inline DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;

			UINT data[36] = {
				0, 1, 2, 0, 2, 3,
				4, 6, 5, 4, 7, 6,
				4, 5, 1, 4, 1, 0,
				3, 2, 6, 3, 6, 7,
				1, 5, 6, 1, 6, 2,
				4, 0, 3, 4, 3, 7
			};
		} indexBufferData;

		IndexBuffer<decltype(indexBufferData)> indexBuffer;

		struct SModelMat {
			DirectX::SimpleMath::Matrix model;
		} modelMat;

		ConstantBuffer<decltype(modelMat)> constBuf;

		static inline std::random_device rd;  // Will be used to obtain a seed for the random number engine
		static inline std::mt19937 gen{ rd() }; // Standard mersenne_twister_engine seeded with rd()
		static inline std::uniform_real_distribution<> dis{ -1.0, 1.0 };
	};
}