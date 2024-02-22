#pragma once

#include <string>
#include <DirectXMath.h>
#include<d3d11.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Input/Keys.h"
#include "Components/IGameComponent.h"
#include "SimpleMath.h"

namespace dmbrn
{
	class MovingRectangleComponent :public IGameComponent
	{
	public:
		MovingRectangleComponent(Game& game, const std::wstring& shaderPath, DirectX::SimpleMath::Vector2 scale,
			DirectX::SimpleMath::Vector2 offset = DirectX::SimpleMath::Vector2(0, 0), Keys key_up = Keys::W, Keys key_down=Keys::S)
			: IGameComponent(game), shaderPath(shaderPath), scale(scale), translation(offset),keyUp(key_up),keyDown(key_down)
		{
			sModelMat.model =
				DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(offset.x, offset.y, 0))*
				DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ scale.x,scale.y,1 });
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
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		ID3D11RasterizerState* rastState;

		struct Vertex
		{
			DirectX::SimpleMath::Vector4 pos;
			DirectX::SimpleMath::Vector4 color;
		};

		Vertex points[4] =
		{
			Vertex{DirectX::SimpleMath::Vector4(0, 0, 0, 1.0f),DirectX::SimpleMath::Vector4(1,0,0, 1.0f)},
			Vertex{DirectX::SimpleMath::Vector4(0,1,0, 1.0f),DirectX::SimpleMath::Vector4(0,1,0, 1.0f)},
			Vertex{DirectX::SimpleMath::Vector4(1, 0,0, 1.0f),DirectX::SimpleMath::Vector4(0,0,1, 1.0f)},
			Vertex{DirectX::SimpleMath::Vector4(1,1,0,1),DirectX::SimpleMath::Vector4(0,0,0,1)}
		};

		UINT indices[6] =
		{
			0,1,2,
			2,1,3
		};
	private:
		DirectX::SimpleMath::Vector2 scale;
		DirectX::SimpleMath::Vector2 translation;
		Keys keyUp, keyDown;
		struct SModelMat
		{
			DirectX::SimpleMath::Matrix model;
		}sModelMat;
		ID3D11Buffer* constantBufferModel;
	};
}