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

		}


		void Update(float) override
		{
			// update controlled transform based on center transform data

			angle += 0.01;

			DirectX::SimpleMath::Vector3 center_radius = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(axis, angle).Right();

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

		DirectX::SimpleMath::Vector3 axis = DirectX::SimpleMath::Vector3::Up;
		float radius = 1;
		float angle = 0;

	private:
		TransformComponent& controlled;
		TransformComponent& center;
	};
}