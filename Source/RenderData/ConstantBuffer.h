#pragma once

#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

namespace dmbrn
{
	template <typename T>
	class ConstantBuffer
	{
	public:
		ConstantBuffer(const ConstantBuffer& other) = delete;
		ConstantBuffer(ConstantBuffer&& other) noexcept = default;
		ConstantBuffer& operator=(const ConstantBuffer& other) = delete;
		ConstantBuffer& operator=(ConstantBuffer&& other) noexcept = default;

		ConstantBuffer(ID3D11Device* device, const T& bufData)
		{
			// Fill in a buffer description.
			D3D11_BUFFER_DESC cbDesc;
			cbDesc.ByteWidth = sizeof(T);
			cbDesc.Usage = D3D11_USAGE_DYNAMIC;
			cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbDesc.MiscFlags = 0;
			cbDesc.StructureByteStride = 0;

			// Fill in the subresource data.
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = &bufData;
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;

			// Create the buffer.
			device->CreateBuffer(&cbDesc, &InitData,
				constantBuffer.GetAddressOf());
		}

		void bindToVertex(ID3D11DeviceContext* cntx, UINT slot)const
		{
			cntx->VSSetConstantBuffers(slot, 1, constantBuffer.GetAddressOf());
		}

		void bindToFragment(ID3D11DeviceContext* cntx, UINT slot)const
		{
			cntx->PSSetConstantBuffers(slot, 1, constantBuffer.GetAddressOf());
		}

		// TODO: better RAII mapping helper structure
		struct MapGuard
		{
			MapGuard(ID3D11DeviceContext* cntx, ID3D11Buffer* buf) :context(cntx), buffer(buf)
			{
				D3D11_MAPPED_SUBRESOURCE res = {};
				cntx->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

				data = reinterpret_cast<T*>(res.pData);
			}
			~MapGuard()
			{
				context->Unmap(buffer, 0);
			}
			T* operator->()
			{
				return data;
			}
		private:
			T* data;
			ID3D11DeviceContext* context;
			ID3D11Buffer* buffer;
		};

		MapGuard map(ID3D11DeviceContext* cntx)
		{
			return MapGuard{ cntx, this->constantBuffer.Get() };
		}

	private:
		// actually there should be unique ptr
		ComPtr<ID3D11Buffer> constantBuffer;
	};
}