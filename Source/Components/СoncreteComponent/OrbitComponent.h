#pragma once

#include "../IGameComponent.h"
#include "../Transform.h"

namespace dmbrn
{
	class OrbitController : public IGameComponent
	{
	public:
		OrbitController(GameToComponentBridge bridge, TransformComponent& cntrld,
			TransformComponent& cntr)
			:IGameComponent(bridge),
			controlled(cntrld),
			center(cntr)
		{
			auto v = DirectX::SimpleMath::Vector3(0, -1, -1);
			//auto v = DirectX::SimpleMath::Vector3::Forward;
			v.Normalize();
			axis = v;
		}

		// https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
		void Update(float) override
		{
			// update controlled transform based on center transform data

			angle += speed * 0.01;

			DirectX::SimpleMath::Vector3 n2Axis;

			if (axis != DirectX::SimpleMath::Vector3::Right)
			{
				n2Axis = axis.Cross(DirectX::SimpleMath::Vector3::Right);
				n2Axis.Normalize();
			}
			else
			{
				n2Axis = axis.Cross(DirectX::SimpleMath::Vector3::Forward);
				n2Axis.Normalize();
			}

			auto sns = DirectX::SimpleMath::Vector3(std::sin(angle));
			auto csns = DirectX::SimpleMath::Vector3(std::cos(angle));

			// n2Axis and axis.Cross(n2Axis) give us a plane of circle
			DirectX::SimpleMath::Vector3 center_radius = n2Axis * csns +  axis.Cross(n2Axis) * sns;

			center_radius *= radius;

			controlled.position = center.position + center_radius;
		}


		void RenderDataUpdate() override
		{
			// no data to update
		}


		void Draw() override
		{
			// no data to draw
		}

		void setAxis(const DirectX::SimpleMath::Vector3& v)
		{
			axis = v;
		}

		void setRadius(float r)
		{
			radius = r;
		}

		void setSpeed(float s)
		{
			speed = s;
		}

		DirectX::SimpleMath::Vector3 axis = DirectX::SimpleMath::Vector3::Right;
		float radius = 7;
		float angle = 0;
		float speed = 1;

	private:
		TransformComponent& controlled;
		TransformComponent& center;
	};
}