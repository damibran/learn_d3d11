#pragma once

#include <string>
#include <DirectXMath.h>
#include<d3d11.h>
#include "IGameComponent.h"


namespace dmbrn
{
	class TriangleComponent :public IGameComponent
	{
	public:
		TriangleComponent(Game& game, const std::wstring& shaderPath)
			: IGameComponent(game), shaderPath(shaderPath) {}

		TriangleComponent(Game& game, const std::wstring& shaderPath,float offset)
			: IGameComponent(game), shaderPath(shaderPath) {
			points[0].x += offset;
			points[1].y -= offset;
		}
		// Inherited via IGameComponent
		void Initialize() override;
		void Update() override;
		void Draw() override;
		void DestroyResources() override;

	private:
		std::wstring shaderPath;
		ID3DBlob* vertexShaderByteCode;
		ID3DBlob* pixelShaderByteCode;
		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
		ID3D11InputLayout* layout;
		ID3D11Buffer* vb;
		ID3D11RasterizerState* rastState;

		DirectX::XMFLOAT4 points[3] = {
			DirectX::XMFLOAT4(0, 0.5f, 0, 1.0f),
			DirectX::XMFLOAT4(0.5,-0.5,0, 1.0f),
			DirectX::XMFLOAT4(-0.5f, -0.5f,0, 1.0f)
		};
	};
}