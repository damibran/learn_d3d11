#pragma once

#include "Components/IGameComponent.h"
#include "Components/Transform.h"
#include "RenderData/ConstantBuffer.h"

namespace dmbrn
{
	class LightComponent :public IGameComponent
	{
	public:

		LightComponent(GameToComponentBridge bridge) :
			IGameComponent(bridge),
			constBuf(bridge.device.getDevice(), lights_cb_cpu)
		{

		}

		void Update(float) override
		{

		}
		void RenderDataUpdate() override
		{
			auto data = constBuf.map(bridge.device.getContext());

			data->dir = directional_transform.getRotationMatrix().Forward();
			data->point_pos = point_transform.position;
		}
		void Draw() override
		{
			constBuf.bindToFragment(bridge.device.getContext(), 3);
		}

	private:
		TransformComponent directional_transform = { {},{DirectX::XMConvertToRadians(-45),0,0},{} };
		TransformComponent point_transform = { {10,2,10},{},{} };

		struct alignas(16) CBSLights
		{
			DirectX::SimpleMath::Vector3 dir;
			DirectX::SimpleMath::Vector3 point_pos;
		} lights_cb_cpu;

		ConstantBuffer<decltype(lights_cb_cpu)> constBuf;
	};
}
