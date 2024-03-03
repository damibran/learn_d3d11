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
			auto v = DirectX::SimpleMath::Vector3(0, 1, 1);
			//auto v = DirectX::SimpleMath::Vector3::Forward;
			v.Normalize();
			axis = v;
		}


		void Update(float) override
		{
			// update controlled transform based on center transform data

			angle += 0.01;

			auto m = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(axis, angle);
			DirectX::SimpleMath::Vector3 center_radius;

			if (axis != DirectX::SimpleMath::Vector3::Right)
			{
				//DirectX::SimpleMath::Vector3 normal = axis.Cross(DirectX::SimpleMath::Vector3::Right);
				//normal.Normalize();
				//center_radius = DirectX::SimpleMath::Vector3::Transform(normal, m);
				center_radius = m.Right();
			}
			else
			{
				//DirectX::SimpleMath::Vector3 normal = axis.Cross(DirectX::SimpleMath::Vector3::Forward);
				//normal.Normalize();
				//center_radius = DirectX::SimpleMath::Vector3::Transform(normal, m);
				center_radius = m.Forward();
			}

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

		DirectX::SimpleMath::Vector3 axis = DirectX::SimpleMath::Vector3::Right;
		float radius = 5;
		float angle = 0;

	private:
		TransformComponent& controlled;
		TransformComponent& center;
	};
}