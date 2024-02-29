#pragma once

#include "GameToComponentBridge.h"

namespace dmbrn
{
	class IGameComponent
	{
	public:
		virtual ~IGameComponent() = default;
		IGameComponent(GameToComponentBridge bridge) :bridge(bridge) {}

		virtual void Update(float) = 0;
		virtual void RenderDataUpdate() = 0;
		virtual void Draw() = 0;

	protected:
		GameToComponentBridge bridge;
	};
}