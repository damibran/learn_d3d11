#pragma once

#include "Components/IGameComponent.h"
#include "Components/СoncreteComponent/CoordFrameComponent.h"
#include "RenderData/ConstantBuffer.h"

namespace dmbrn
{
	class LightComponent :public IGameComponent
	{
	public:

		LightComponent(GameToComponentBridge bridge, RastState& rs) :
			IGameComponent(bridge),
			constBuf(bridge.device.getDevice(), lights_cb_cpu),
			directional(bridge, rs),
			point(bridge, rs)
		{
			directional.transform = { {},{DirectX::XMConvertToRadians(-45),0,0} };
			point.transform = { {20,2,20},{} };
		}

		void Update(float) override
		{

		}
		void RenderDataUpdate() override
		{
			directional.RenderDataUpdate();
			point.RenderDataUpdate();

			auto data = constBuf.map(bridge.device.getContext());

			data->dir = directional.transform.getRotationMatrix().Forward();
			data->point_pos = point.transform.position;
		}
		void Draw() override
		{
			directional.Draw();
			point.Draw();

			constBuf.bindToFragment(bridge.device.getContext(), 3);
		}

	private:
		CoordFrameComponent directional;
		CoordFrameComponent point;

		alignas(16)
		struct CBSLights
		{
			alignas(16) DirectX::SimpleMath::Vector3 dir;
			alignas(16) DirectX::SimpleMath::Vector3 point_pos;
		} lights_cb_cpu;

		ConstantBuffer<decltype(lights_cb_cpu)> constBuf;
	};
}
