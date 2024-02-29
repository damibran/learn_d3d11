#pragma once

#include "GameToComponentBridge.h"

namespace dmbrn
{
	class IGameComponent
	{
	public:
		virtual ~IGameComponent() = default;
		IGameComponent(GameToComponentBridge bridge) :bridge(bridge) {}

		virtual void Initialize() = 0;
		virtual void Update(float) = 0;
		virtual void PhysicsUpdate(float) = 0;
		virtual void CollisionUpdate(float) = 0;
		virtual void RenderDataUpdate() = 0;
		virtual void Draw() = 0;

		virtual void DestroyResources() = 0;

	protected:
		GameToComponentBridge bridge;
	};
}