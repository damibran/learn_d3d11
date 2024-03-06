#pragma once

namespace dmbrn {
	template <typename T>
	class VertexBuffer {
	public:
		VertexBuffer(ID3D11Device* device, ID3DBlob* vertexShaderByteCode, const T& vertBufData)
		{
			// actually buffers can and should if so have same layout same as vertexShaders
			device->CreateInputLayout(
				vertBufData.desc,
				std::size(vertBufData.desc),
				vertexShaderByteCode->GetBufferPointer(),
				vertexShaderByteCode->GetBufferSize(),
				&layout);


			D3D11_BUFFER_DESC vertexBufDesc = {};
			vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufDesc.CPUAccessFlags = 0;
			vertexBufDesc.MiscFlags = 0;
			vertexBufDesc.StructureByteStride = 0;
			vertexBufDesc.ByteWidth = sizeof(T::Vertex) * std::size(vertBufData.data);

			D3D11_SUBRESOURCE_DATA vertexData = {};
			vertexData.pSysMem = vertBufData.data;
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;

			device->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);
		}

		void bindVertexBuffer(ID3D11DeviceContext* context)
		{
			context->IASetInputLayout(layout);
			// TODO: strides and offsets should be defined in VertexBufData
			UINT strides[] = { sizeof(T::Vertex) };
			UINT offsets[] = { 0 };
			context->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
		}

		~VertexBuffer()
		{
			vertexBuffer->Release();
			layout->Release();
		}

	private:
		ID3D11InputLayout* layout;
		ID3D11Buffer* vertexBuffer;
	};

	template <typename T>
	class VertexBufferVec {
	public:
		VertexBufferVec(ID3D11Device* device, ID3DBlob* vertexShaderByteCode, const T& vertBufData)
		{
			// actually buffers can and should if so have same layout same as vertexShaders
			device->CreateInputLayout(
				vertBufData.desc,
				std::size(vertBufData.desc),
				vertexShaderByteCode->GetBufferPointer(),
				vertexShaderByteCode->GetBufferSize(),
				&layout);


			D3D11_BUFFER_DESC vertexBufDesc = {};
			vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufDesc.CPUAccessFlags = 0;
			vertexBufDesc.MiscFlags = 0;
			vertexBufDesc.StructureByteStride = 0;
			vertexBufDesc.ByteWidth = sizeof(T::Vertex) * std::size(vertBufData.data);

			D3D11_SUBRESOURCE_DATA vertexData = {};
			vertexData.pSysMem = vertBufData.data.data();
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;

			device->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);
		}

		void bindVertexBuffer(ID3D11DeviceContext* context)
		{
			context->IASetInputLayout(layout);
			// TODO: strides and offsets should be defined in VertexBufData
			UINT strides[] = { sizeof(T::Vertex) };
			UINT offsets[] = { 0 };
			context->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
		}

		~VertexBufferVec()
		{
			vertexBuffer->Release();
			layout->Release();
		}

	private:
		ID3D11InputLayout* layout;
		ID3D11Buffer* vertexBuffer;
	};
}