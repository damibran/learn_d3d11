#pragma once

#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

namespace dmbrn
{
	template <class T>
	class DeviceLocalBuffer
	{
	public:
		DeviceLocalBuffer(const DeviceLocalBuffer& other) = delete;
		DeviceLocalBuffer(DeviceLocalBuffer&& other) noexcept = default;
		DeviceLocalBuffer& operator=(const DeviceLocalBuffer& other) = delete;
		DeviceLocalBuffer& operator=(DeviceLocalBuffer&& other) noexcept = default;

		DeviceLocalBuffer(ID3D11Device* device, UINT BindFlags, const std::vector<typename T::Object>& host_data)
		{
			// Fill in a buffer description.
			D3D11_BUFFER_DESC bufferDesc;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.ByteWidth = sizeof(T::Object) * std::size(host_data);
			bufferDesc.BindFlags = BindFlags; // D3D11_BIND_INDEX_BUFFER D3D11_BIND_VERTEX_BUFFER
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;

			// Define the resource data.
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = host_data.data();
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;

			device->CreateBuffer(&bufferDesc, &InitData, buffer.GetAddressOf());
		}

		void bindAsVertex(ID3D11DeviceContext* context) const
		{
			// TODO: strides and offsets should be defined in VertexBufData
			UINT strides[] = { sizeof(T::Object) };
			UINT offsets[] = { 0 };
			context->IASetVertexBuffers(0, 1, buffer.GetAddressOf(), strides, offsets);
		}

		void bindAsIndex(ID3D11DeviceContext* cntx) const
		{
			cntx->IASetIndexBuffer(buffer.Get(), T::format, 0);
		}

	private:
		ComPtr<ID3D11Buffer> buffer;
	};
}