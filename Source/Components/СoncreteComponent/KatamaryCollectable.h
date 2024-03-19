#pragma once

#include <random>

#include "Components/СoncreteComponent/ModelComponent.h"

namespace dmbrn
{
	class KatamaryCollectable :public ModelComponent
	{
	public:
		KatamaryCollectable(GameToComponentBridge bridge, RastState& rs, InputLayout<VertexType>* il, const std::wstring& shaderPath, const std::wstring& path) :
			ModelComponent(bridge, rs, il, shaderPath, path)
		{
			transform.position = DirectX::SimpleMath::Vector3{ uniform_dist(e1),0,uniform_dist(e1) };
			localAABB.Transform(localAABB, transform.getMatrix());
			// than localAABB is world for this class
		}

	private:
		// Seed with a real random value, if available
		static inline std::random_device r;
		// Choose a random mean between 1 and 6
		static inline std::default_random_engine e1{ r() };
		static inline std::uniform_real<float> uniform_dist{ -20, 20 };
	};
}
