#pragma once

#include <vector>

#include "../IGameComponent.h"
#include "LineComponent.h"

namespace dmbrn
{
	//TODO: temp bad implementation better populate big vertex buffer
	class GridComponent : public IGameComponent
	{
	public:
		GridComponent(GameToComponentBridge bridge, RastState& rs, const std::wstring& shaderPath, int zr, int xr)
			:IGameComponent(bridge)
		{
			zlines.reserve(zr);
			for (int i = -zr / 2; i < zr / 2; ++i)
			{
				TransformComponent trans;
				trans.scale.x = zr;
				trans.position.y = -0.001;
				trans.position.z = i;
				trans.position.x = -trans.scale.x/2;
				zlines.emplace_back(bridge, rs, shaderPath, trans, DirectX::SimpleMath::Vector3{ 1,1,1 });
			}

			xlines.reserve(xr);
			for (int i = -xr / 2; i < xr / 2; ++i)
			{
				TransformComponent trans;
				trans.setRad({ 0,-DirectX::XMConvertToRadians(90),0 });
				trans.position.y = -0.001;
				trans.scale.x = zr;
				trans.position.x = i;
				trans.position.z = -trans.scale.x / 2;
				xlines.emplace_back(bridge, rs, shaderPath, trans, DirectX::SimpleMath::Vector3{ 1,1,1 });
			}
		}


		void Update(float) override
		{
		}


		void RenderDataUpdate() override
		{
			for (auto&& line : zlines)
			{
				line.RenderDataUpdate();
			}

			for (auto&& line : xlines)
			{
				line.RenderDataUpdate();
			}
		}


		void Draw() override
		{
			for (auto&& line : zlines)
			{
				line.Draw();
			}

			for (auto&& line : xlines)
			{
				line.Draw();
			}
		}

	private:

		std::vector<LineComponent> zlines;
		std::vector<LineComponent> xlines;

	};
}