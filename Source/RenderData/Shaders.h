#pragma once

#include <string>
#include <d3d11.h>
#include <d3dcompiler.h>

namespace dmbrn
{
	class Shaders
	{
	public:

		Shaders(Game& game, std::wstring shaderPath)
		{
			ID3DBlob* errorVertexCode = nullptr;
			HRESULT res = D3DCompileFromFile(shaderPath.c_str(),
				nullptr /*macros*/,
				nullptr /*include*/,
				"VSMain",
				"vs_5_0",
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
				0,
				&vertexShaderByteCode,
				&errorVertexCode);

			if (FAILED(res)) {
				// If the shader failed to compile it should have written something to the error message.
				if (errorVertexCode) {
					char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

					std::cout << compileErrors << std::endl;

					assert(false);
				}
				// If there was  nothing in the error message then it simply could not find the shader file itself.
			}

			//D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

			D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0",
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelShaderByteCode, nullptr);

			gDevice.getDevice()->CreateVertexShader(
				vertexShaderByteCode->GetBufferPointer(),
				vertexShaderByteCode->GetBufferSize(),
				nullptr, &vertexShader);

			gDevice.getDevice()->CreatePixelShader(
				pixelShaderByteCode->GetBufferPointer(),
				pixelShaderByteCode->GetBufferSize(),
				nullptr, &pixelShader);
		}

		void bindShaders(ID3D11DeviceContext* context)
		{
			context->VSSetShader(vertexShader, nullptr, 0);
			context->PSSetShader(pixelShader, nullptr, 0);
		}

		ID3DBlob* getVertexBC()
		{
			return vertexShaderByteCode;
		}

		~Shaders() 
		{
			pixelShader->Release();
			vertexShader->Release();
			pixelShaderByteCode->Release();
			vertexShaderByteCode->Release();
		}

	private:
		std::wstring shaderPath;
		ID3DBlob* vertexShaderByteCode;
		ID3DBlob* pixelShaderByteCode;
		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
	};
}