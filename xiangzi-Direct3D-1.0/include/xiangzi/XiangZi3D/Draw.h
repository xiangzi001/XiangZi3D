
class Object {

private:
    ID3D11Device* device = NULL;
    ID3D11DeviceContext* deviceContext = NULL;
    ID3D11Buffer* pWorldBuffer = NULL;

    ID3D11Buffer* pVertexBuffer = NULL;
    ID3D11Buffer* pIndexBuffer = NULL;
    int IndexLen = 0;

    D3D11_MAPPED_SUBRESOURCE mappedData;
    XMMATRIX World;

    XMMATRIX Scaling = XMMatrixIdentity();
    XMMATRIX Translation = XMMatrixIdentity();
    XMMATRIX Rotation = XMMatrixIdentity();

public:
    void Init(DirectScene* win) {
        device = win->device;
        deviceContext = win->deviceContext;
        pWorldBuffer = win->pConstantBufferWorld;
    }

    template<class T>
    void Load(Vertex* vertices, int vLen, T* indices, int iLen) {

        HRESULT hr;

        D3D11_BUFFER_DESC bd{};
        D3D11_SUBRESOURCE_DATA initData{};

        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth = sizeof(Vertex) * vLen;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        initData.pSysMem = vertices;

        hr = device->CreateBuffer(&bd, &initData, &pVertexBuffer);

        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth = sizeof(T) * iLen;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;

        initData.pSysMem = indices;

        hr = device->CreateBuffer(&bd, &initData, &pIndexBuffer);

        IndexLen = iLen;
    }

    void SetScaling(float x, float y, float z) {
        Scaling = XMMatrixScaling(x, y, z);
    }

    void SetRotation(float x, float y, float z ) {
        Rotation = XMMatrixRotationX(x) * XMMatrixRotationY(y) * XMMatrixRotationZ(z);
    }

    void SetTranslation(float x, float y, float z ) {
        Translation = XMMatrixTranslation(x, y, z);
    }

    void Draw() {

        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        deviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
        deviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

        World = XMMatrixTranspose(Scaling * Rotation * Translation);
        deviceContext->Map(pWorldBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
        memcpy_s(mappedData.pData, sizeof(XMMATRIX), &World, sizeof(XMMATRIX));
        deviceContext->Unmap(pWorldBuffer, 0);

        deviceContext->DrawIndexed(IndexLen, 0, 0);
    }
};
