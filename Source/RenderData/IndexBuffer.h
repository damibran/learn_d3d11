#pragma once

#include <d3d11.h>

namespace dmbrn {
template <class T>
class IndexBuffer {
public:
    IndexBuffer(ID3D11Device* device, const T& indices)
    {
        // Fill in a buffer description.
        D3D11_BUFFER_DESC bufferDesc;
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = sizeof(indices.data[0]) * std::size(indices.data);
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;

        // Define the resource data.
        D3D11_SUBRESOURCE_DATA InitData;
        InitData.pSysMem = &indices;
        InitData.SysMemPitch = 0;
        InitData.SysMemSlicePitch = 0;

        device->CreateBuffer(&bufferDesc, &InitData, &indexBuffer);
    }

    void bind(ID3D11DeviceContext* cntx)
    {
        cntx->IASetIndexBuffer(indexBuffer, T::format, 0);
    }

    ~IndexBuffer()
    {
        indexBuffer->Release();
    }

private:
    ID3D11Buffer* indexBuffer;
};
}