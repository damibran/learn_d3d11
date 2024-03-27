#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>
#include <string>

namespace dmbrn {
	class Shaders {
	public:
		Shaders(ID3D11Device* device, const std::wstring& shaderPath, bool bPixelShader = true, bool bGeometryShader = false)
		{
			ID3DBlob* errorBlob = nullptr;
			HRESULT res = D3DCompileFromFile(shaderPath.c_str(),
				nullptr /*macros*/,
				nullptr /*include*/,
				"VSMain",
				"vs_5_0",
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
				0,
				&vertexShaderByteCode,
				&errorBlob);

			if (FAILED(res)) {
				// If the shader failed to compile it should have written something to the error message.
				if (errorBlob) {
					char* compileErrors = (char*)(errorBlob->GetBufferPointer());

					std::cout << compileErrors << std::endl;

					assert(false);
				}
				// If there was  nothing in the error message then it simply could not find the shader file itself.
			}


			device->CreateVertexShader(
				vertexShaderByteCode->GetBufferPointer(),
				vertexShaderByteCode->GetBufferSize(),
				nullptr, &vertexShader);

			// D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };
			if (bPixelShader)
			{
				D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0",
					D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelShaderByteCode, nullptr);


				device->CreatePixelShader(
					pixelShaderByteCode->GetBufferPointer(),
					pixelShaderByteCode->GetBufferSize(),
					nullptr, &pixelShader);
			}

			if (bGeometryShader)
			{
				res = D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "GSMain", "gs_5_0",
					D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &geometryShaderByteCode, &errorBlob);

				if (FAILED(res)) {
					// If the shader failed to compile it should have written something to the error message.
					if (errorBlob) {
						char* compileErrors = (char*)(errorBlob->GetBufferPointer());

						std::cout << compileErrors << std::endl;

						assert(false);
					}
					// If there was  nothing in the error message then it simply could not find the shader file itself.
				}

				device->CreateGeometryShader(
					geometryShaderByteCode->GetBufferPointer(),
					geometryShaderByteCode->GetBufferSize(),
					nullptr, &geometryShader);
			}
		}

		void bindShaders(ID3D11DeviceContext* context)
		{
			context->VSSetShader(vertexShader, nullptr, 0);
			context->PSSetShader(pixelShader, nullptr, 0);
			context->GSSetShader(geometryShader, nullptr, 0);
		}

		ID3DBlob* getVertexBC()
		{
			return vertexShaderByteCode;
		}

		~Shaders()
		{
			if (geometryShader)
			{
				geometryShader->Release();
				geometryShaderByteCode->Release();
			}
			if (pixelShader)
			{
				pixelShader->Release();
				pixelShaderByteCode->Release();
			}
			vertexShader->Release();
			vertexShaderByteCode->Release();
		}

	private:
		std::wstring shaderPath;
		ID3DBlob* vertexShaderByteCode = nullptr;
		ID3D11VertexShader* vertexShader = nullptr;
		ID3DBlob* pixelShaderByteCode = nullptr;
		ID3D11PixelShader* pixelShader = nullptr;
		ID3DBlob* geometryShaderByteCode = nullptr;
		ID3D11GeometryShader* geometryShader = nullptr;
	};
}