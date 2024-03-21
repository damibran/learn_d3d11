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
			directional.transform = { {0,2,0},{DirectX::XMConvertToRadians(-45),0,0} };
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

			data->amb.color = DirectX::SimpleMath::Vector3(0.04, 0.14, 0.72);
			data->amb.intensity = 0.3;

			data->dir.dir = directional.transform.getRotationMatrix().Forward();
			data->dir.color = DirectX::SimpleMath::Vector3(0.9, 0.78, 0.26);
			data->dir.intensity = 1;

			data->pont.pos = point.transform.position;
			data->pont.color = DirectX::SimpleMath::Vector3(0.9, 0.13, 0.26);
			data->pont.radius = 20;
			data->pont.max_intensity = 4;
			data->pont.falloff = 1;
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
			alignas(16)
				struct Ambient
			{
				alignas(16) DirectX::SimpleMath::Vector3 color;
				float intensity;
			} amb;
			alignas(16)
				struct Directional
			{
				alignas(16) DirectX::SimpleMath::Vector3 dir;
				alignas(16) DirectX::SimpleMath::Vector3 color;
				float intensity;
			} dir;
			alignas(16)
				struct Point
			{
				alignas(16) DirectX::SimpleMath::Vector3 pos;
				alignas(16) DirectX::SimpleMath::Vector3 color;
				float radius;
				float max_intensity;
				float falloff;
			} pont;
		} lights_cb_cpu;

		ConstantBuffer<decltype(lights_cb_cpu)> constBuf;
	};
}
