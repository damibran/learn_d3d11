#pragma once

#include <random>

#include "Components/СoncreteComponent/ModelComponent.h"

namespace dmbrn
{
	class KatamaryCollectable :public ModelComponent
	{
	public:
		KatamaryCollectable(GameToComponentBridge bridge, RastState& rs, InputLayout<VertexType>* il, const std::wstring& shaderPath, const std::wstring& path, const TransformComponent& trans = {}) :
			ModelComponent(bridge, rs, il, shaderPath, path)
		{
			transform = trans;
			transform.position = DirectX::SimpleMath::Vector3{ pos_distr(e1),0,pos_distr(e1) };
			transform.setRad(DirectX::SimpleMath::Vector3{ rot_distr(e1),rot_distr(e1),rot_distr(e1) });
			localAABB.Transform(localAABB, transform.getMatrix());
			// than localAABB is world for this class
		}

	private:
		// Seed with a real random value, if available
		static inline std::random_device r;
		// Choose a random mean between 1 and 6
		static inline std::default_random_engine e1{ r() };
		static inline std::uniform_real<float> pos_distr{ -20, 20 };
		static inline std::uniform_real<float> rot_distr{ DirectX::XMConvertToRadians(-180), DirectX::XMConvertToRadians(180) };
	};
}
