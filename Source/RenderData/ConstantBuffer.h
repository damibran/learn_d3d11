#pragma once

#include <d3d11.h>

namespace dmbrn {
	template <typename T>
	class ConstantBuffer {
	public:
		ConstantBuffer(ID3D11Device* device, T& bufData)
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
				&constantBuffer);
		}
		~ConstantBuffer()
		{
			constantBuffer->Release();
		}

		void bind(ID3D11DeviceContext* cntx, UINT slot)
		{
			cntx->VSSetConstantBuffers(slot, 1, &constantBuffer);
		}

		// TODO: better RAII mapping helper structure
		struct MapGuard
		{
			MapGuard(ID3D11DeviceContext* cntx, ConstantBuffer<T>& buf) :context(cntx), bufffer(buf)
			{
				D3D11_MAPPED_SUBRESOURCE res = {};
				cntx->Map(bufffer.constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

				data = reinterpret_cast<T*>(res.pData);
			}
			~MapGuard()
			{
				context->Unmap(bufffer.constantBuffer, 0);
			}
			T* operator->()
			{
				return data;
			}
		private:
			T* data;
			ID3D11DeviceContext* context;
			ConstantBuffer<T>& bufffer;
		};

		MapGuard map(ID3D11DeviceContext* cntx)
		{
			return MapGuard{ cntx,*this };
		}

	private:
		ID3D11Buffer* constantBuffer;
	};
}