#pragma once

#include "Shaders.h"

namespace dmbrn
{
	template<typename T>
	class InputLayout
	{
	public:
		InputLayout(ID3D11Device* device, const std::wstring ilShaderPath)
		{
			Shaders shader(device, ilShaderPath);

			device->CreateInputLayout(
				T::desc,
				std::size(T::desc),
				shader.getVertexBC()->GetBufferPointer(),
				shader.getVertexBC()->GetBufferSize(),
				&layout);
		}

		void bind(ID3D11DeviceContext* context)const
		{
			context->IASetInputLayout(layout);
		}

	private:
		ID3D11InputLayout* layout;
	};
}