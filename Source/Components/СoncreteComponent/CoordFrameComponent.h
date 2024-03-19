#pragma once

#include "Components/IGameComponent.h"
#include "Components/Transform.h"
#include "RenderData/ConstantBuffer.h"
#include "RenderData/RasterState.h"
#include "RenderData/Shaders.h"
#include "RenderData/VertexBuffer.h"

namespace dmbrn
{
	class CoordFrameComponent :public IGameComponent
	{
	public:
		CoordFrameComponent(GameToComponentBridge bridge, RastState& rs) :
			IGameComponent(bridge),
			rastState(rs),
			shaders(bridge.device.getDevice(), L"./Shaders/ModelMatVColor.hlsl"),
			vertexBuffer(bridge.device.getDevice(), shaders.getVertexBC(), vertexBufferData)
			, constBuf(bridge.device.getDevice(), modelMat)
		{

		}

		void Update(float) override
		{

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

			bridge.device.getContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
			vertexBuffer.bindVertexBuffer(bridge.device.getContext());
			shaders.bindShaders(bridge.device.getContext());

			constBuf.bindToVertex(bridge.device.getContext(), 1);

			bridge.device.getContext()->Draw(6, 0);
		}
	private:
		RastState& rastState;
		Shaders shaders;

		TransformComponent transform;

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

			Vertex data[6] = {
				Vertex { DirectX::SimpleMath::Vector3(0, 0, 0), DirectX::SimpleMath::Vector4(1, 0, 0, 1.0f) },
				Vertex { DirectX::SimpleMath::Vector3(1, 0, 0), DirectX::SimpleMath::Vector4(1, 0, 0, 1.0f) },

				Vertex { DirectX::SimpleMath::Vector3(0, 0, 0), DirectX::SimpleMath::Vector4(0, 1, 0, 1.0f) },
				Vertex { DirectX::SimpleMath::Vector3(0, 1, 0), DirectX::SimpleMath::Vector4(0, 1, 0, 1.0f) },

				Vertex { DirectX::SimpleMath::Vector3(0, 0, 0), DirectX::SimpleMath::Vector4(0, 0, 1, 1.0f) },
				Vertex { DirectX::SimpleMath::Vector3(0, 0, 1), DirectX::SimpleMath::Vector4(0, 0, 1, 1.0f) }

			};
		}vertexBufferData;

		VertexBuffer<decltype(vertexBufferData)> vertexBuffer;

		struct SModelMat {
			DirectX::SimpleMath::Matrix model;
		} modelMat;

		ConstantBuffer<decltype(modelMat)> constBuf;
	};
}
