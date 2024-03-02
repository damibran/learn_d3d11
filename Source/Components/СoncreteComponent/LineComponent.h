#pragma once

#include "Components/IGameComponent.h"
#include "../../RenderData/ConstantBuffer.h"
#include "../../RenderData/IndexBuffer.h"
#include "../../RenderData/RasterState.h"
#include "../../RenderData/Shaders.h"
#include "../../RenderData/VertexBuffer.h"
#include "../Transform.h"
#include "SimpleMath.h"
#include <math.h>

namespace dmbrn
{
	class LineComponent : public IGameComponent
	{

	public:

		LineComponent(GameToComponentBridge bridge, RastState& rs, const std::wstring& shaderPath, const TransformComponent& trans,
			DirectX::SimpleMath::Vector3 color) :
			IGameComponent(bridge)
			, rastState(rs)
			, shaders(bridge.device.getDevice(), shaderPath)
			, vertexBufferData(trans.getMatrix(),color)
			, vertexBuffer(bridge.device.getDevice(), shaders.getVertexBC(), vertexBufferData)
			, transform(trans)
		{
		}

		void Update(float) override
		{

		}

		void RenderDataUpdate() override
		{

		}


		void Draw() override
		{
			rastState.bind(bridge.device.getContext());

			bridge.device.getContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
			vertexBuffer.bindVertexBuffer(bridge.device.getContext());
			shaders.bindShaders(bridge.device.getContext());

			bridge.device.getContext()->Draw(2, 0);
		}

	private:

		RastState& rastState;
		Shaders shaders;

		TransformComponent transform;

		struct VertexBufferData {

			VertexBufferData(const DirectX::SimpleMath::Matrix& trans,const DirectX::SimpleMath::Vector3& color)
			{
				data[0].pos = DirectX::SimpleMath::Vector3::Transform(data[0].pos, trans);
				data[1].pos = DirectX::SimpleMath::Vector3::Transform(data[1].pos, trans);

				data[0].color = DirectX::SimpleMath::Vector4(color.x, color.y, color.z, 1);
				data[1].color = DirectX::SimpleMath::Vector4(color.x, color.y, color.z, 1);
			}

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

			Vertex data[2] = {
				Vertex { DirectX::SimpleMath::Vector3(0, 0, 0), DirectX::SimpleMath::Vector4(1, 0, 0, 1.0f) },
				Vertex { DirectX::SimpleMath::Vector3(1, 0, 0), DirectX::SimpleMath::Vector4(0, 1, 0, 1.0f) },
			};
		} vertexBufferData;

		VertexBuffer<decltype(vertexBufferData)> vertexBuffer;
	};
}