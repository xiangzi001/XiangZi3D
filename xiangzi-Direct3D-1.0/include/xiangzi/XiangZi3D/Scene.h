

// 顶点结构体
struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT4 color;
};

class DirectScene
{
public:
    UINT		winWidth;
    UINT		winHeight;

    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    IDXGISwapChain* swapChain;

    ID3D11DepthStencilView* depthStencilView;
    ID3D11RenderTargetView* renderTargetView;

    ID3D11InputLayout* inputLayout;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;

    ID3D11Buffer* pConstantBufferWorld = NULL;
    ID3D11Buffer* pConstantBufferView = NULL;
    ID3D11Buffer* pConstantBufferProjection = NULL;

    XMMATRIX mView;
    XMMATRIX mProjection;

    DirectScene();
    ~DirectScene();

    void Blind(HWND hWnd);
    void Blind(DirectWindow* win);

    void InitGPU(LPCWSTR Vs, LPCWSTR Ps);
    void InitGPU_2D();
    void InitGPU_3D();

    void InitBuffer();

    void BeginPaint();
    void EndPaint();
};

DirectScene::DirectScene() :
    winWidth(640),
    winHeight(480),
    device(NULL),
    deviceContext(NULL),
    swapChain(NULL),
    depthStencilView(NULL),
    renderTargetView(NULL),
    inputLayout(NULL),
    vertexShader(NULL),
    pixelShader(NULL),
    pConstantBufferWorld(NULL),
    pConstantBufferView(NULL),
    pConstantBufferProjection(NULL)
{
}
DirectScene::~DirectScene() {
    pRelease(device);
    pRelease(deviceContext);
    pRelease(swapChain);
    pRelease(depthStencilView);
    pRelease(renderTargetView);
    pRelease(vertexShader);
    pRelease(pixelShader);
    pRelease(inputLayout);
    //pVertexBuffer->Release();
    //pIndexBuffer->Release();
}

void DirectScene::Blind(HWND hWnd) {
    HRESULT hr;

    //hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0,
    //    D3D11_SDK_VERSION, &sd, &swapChain, &device, &myFeatureLevel, &deviceContext);

    // a.创建设备和上下文
    D3D_FEATURE_LEVEL myFeatureLevel;

    hr = D3D11CreateDevice(
        NULL,						// 默认显示适配器
        D3D_DRIVER_TYPE_HARDWARE,
        0,							// 不使用软件设备
        0,
        NULL, 0,					// 默认的特征等级数组
        D3D11_SDK_VERSION,
        &device,
        &myFeatureLevel,
        &deviceContext);

    if (FAILED(hr))
    {
        MessageBox(NULL, "Create d3d11 device failed!", "error", MB_OK);
    }

    // b.4X多重采样质量等级
    UINT m4xMsaaQuality(0);
    device->CheckMultisampleQualityLevels(
        DXGI_FORMAT_R8G8B8A8_UNORM,
        4,
        &m4xMsaaQuality);

    // c.准备交换链属性
    DXGI_SWAP_CHAIN_DESC sd = { 0 };
    sd.BufferDesc.Width = winWidth;
    sd.BufferDesc.Height = winHeight;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    sd.SampleDesc.Count = 4;
    sd.SampleDesc.Quality = m4xMsaaQuality - 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = hWnd;
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    // d.创建交换链
    IDXGIDevice* dxgiDevice(NULL);
    device->QueryInterface(__uuidof(IDXGIDevice), (void**)(&dxgiDevice));
    IDXGIAdapter* dxgiAdapter(NULL);
    dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)(&dxgiAdapter));
    IDXGIFactory* dxgiFactory(NULL);
    dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)(&dxgiFactory));
    hr = dxgiFactory->CreateSwapChain(device, &sd, &swapChain);
    if (FAILED(hr))
    {
        MessageBox(NULL, "Create swap chain failed!", "error", MB_OK);
    }
    dxgiFactory->Release();
    dxgiAdapter->Release();
    dxgiDevice->Release();

    // e.创建渲染目标视图
    ID3D11Texture2D* backBuffer(NULL);
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
    hr = device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
    if (FAILED(hr))
    {
        MessageBox(NULL, "Create render target view failed!", "error", MB_OK);
    }
    backBuffer->Release();

    // f.创建深度缓冲区和其视图
    D3D11_TEXTURE2D_DESC depthStencilDesc = { 0 };
    depthStencilDesc.Width = winWidth;
    depthStencilDesc.Height = winHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 4;
    depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    ID3D11Texture2D* depthStencilBuffer(NULL);
    hr = device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
    if (FAILED(hr))
    {
        MessageBox(NULL, "Create depth stencil buffer failed!", "error", MB_OK);
    }
    hr = device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);
    if (FAILED(hr))
    {
        MessageBox(NULL, "Create depth stencil view failed!", "error", MB_OK);
    }

    // g.将视图绑定到输出合并器阶段
    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
    depthStencilBuffer->Release();

    // h.设置视口
    D3D11_VIEWPORT vp = { 0 };
    vp.TopLeftX = 0.f;
    vp.TopLeftY = 0.f;
    vp.Width = static_cast<float>(winWidth);
    vp.Height = static_cast<float>(winHeight);
    vp.MinDepth = 0.f;
    vp.MaxDepth = 1.f;

    deviceContext->RSSetViewports(1, &vp);
}

void DirectScene::Blind(DirectWindow* win) {
    winWidth = win->ClientRect.right;
    winHeight = win->ClientRect.bottom;
    Blind(win->hWnd);
}


void DirectScene::InitGPU(LPCWSTR Vs, LPCWSTR Ps) {
    ID3DBlob* errorBlob = nullptr;

    ID3DBlob* vertexShaderBlob;
    D3DCompileFromFile(Vs, nullptr, nullptr, "VS", "vs_5_0", 0, 0, &vertexShaderBlob, &errorBlob);
    device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &vertexShader);

    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT numElements = ARRAYSIZE(inputElementDesc);

    inputLayout = nullptr;
    device->CreateInputLayout(inputElementDesc, numElements, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout);
    deviceContext->IASetInputLayout(inputLayout);

    ID3DBlob* pixelShaderBlob;
    D3DCompileFromFile(Ps, nullptr, nullptr, "PS", "ps_5_0", 0, 0, &pixelShaderBlob, &errorBlob);
    device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &pixelShader);

    vertexShaderBlob->Release();
    pixelShaderBlob->Release();

    deviceContext->VSSetShader(vertexShader, NULL, 0);
    deviceContext->PSSetShader(pixelShader, NULL, 0);
}

void DirectScene::InitGPU_2D() {

    const char* vertexShaderCode = R"(
        struct VertexInput {
            float3 position : POSITION;
            float4 color : COLOR;
        };

        struct PixelInput {
            float4 position : SV_POSITION;
            float4 color : COLOR;
        };

        PixelInput main(VertexInput input) {
            PixelInput output;
            output.position = float4(input.position, 1.0f);
            output.color = input.color;
            return output;
        }
    )";

    const char* pixelShaderCode = R"(
        struct PixelInput {
            float4 position : SV_POSITION;
            float4 color : COLOR;
        };

        float4 main(PixelInput input) : SV_TARGET {
            return input.color;
        }
    )";

    ID3DBlob* vertexShaderBlob;
    D3DCompile(vertexShaderCode, strlen(vertexShaderCode), NULL, NULL, NULL, "main", "vs_5_0", 0, 0, &vertexShaderBlob, NULL);
    device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &vertexShader);

    ID3DBlob* pixelShaderBlob;
    D3DCompile(pixelShaderCode, strlen(pixelShaderCode), NULL, NULL, NULL, "main", "ps_5_0", 0, 0, &pixelShaderBlob, NULL);
    device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &pixelShader);

    deviceContext->VSSetShader(vertexShader, NULL, 0);
    deviceContext->PSSetShader(pixelShader, NULL, 0);

    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT numElements = ARRAYSIZE(inputElementDesc);

    inputLayout = nullptr;
    device->CreateInputLayout(inputElementDesc, numElements, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout);
    deviceContext->IASetInputLayout(inputLayout);

    vertexShaderBlob->Release();
    pixelShaderBlob->Release();
}

void DirectScene::InitGPU_3D() {

    /*
            float4x4 mvp = float4x4(0.7795, 0.8776, 0.8269, 0.8261,
                    0.0000, 2.2097, -0.4031, -0.4027,
                    -1.6342, 0.4185, 0.3944, 0.3940,
                    0.0000, 0.0000, 4.0040, 5.0000);
            output.position = mul(float4(input.position, 1.0f), mvp);

    */
    const char* vertexShaderSource = R"(
        cbuffer ConstantBuffer : register(b0) {
            matrix worldMatrix;
        };
        cbuffer ConstantBuffer : register(b1) {
            matrix viewMatrix;
        };
        cbuffer ConstantBuffer : register(b2) {
            matrix projectionMatrix;
        };

        struct VertexInputType {
            float3 position : POSITION;
            float4 color : COLOR;
        };
        struct PixelInputType {
            float4 position : SV_POSITION;
            float4 color : COLOR;
        };
        PixelInputType main(VertexInputType input) {
            PixelInputType output;
            output.position = mul(mul(mul(float4(input.position, 1.0f), worldMatrix), viewMatrix), projectionMatrix);
            output.color = input.color;
            return output;
        })";

    const char* pixelShaderSource = R"(
        struct PixelInputType {
            float4 position : SV_POSITION;
            float4 color : COLOR;
        };
        float4 main(PixelInputType input) : SV_TARGET {
            return input.color;
        })";

    ID3DBlob* vertexShaderBlob;
    D3DCompile(vertexShaderSource, strlen(vertexShaderSource), NULL, NULL, NULL, "main", "vs_5_0", 0, 0, &vertexShaderBlob, NULL);
    device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &vertexShader);

    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT numElements = ARRAYSIZE(inputElementDesc);

    inputLayout = nullptr;
    device->CreateInputLayout(inputElementDesc, numElements, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout);
    deviceContext->IASetInputLayout(inputLayout);

    ID3DBlob* pixelShaderBlob;
    D3DCompile(pixelShaderSource, strlen(pixelShaderSource), NULL, NULL, NULL, "main", "ps_5_0", 0, 0, &pixelShaderBlob, NULL);
    device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &pixelShader);

    vertexShaderBlob->Release();
    pixelShaderBlob->Release();

    deviceContext->VSSetShader(vertexShader, NULL, 0);
    deviceContext->PSSetShader(pixelShader, NULL, 0);
}

void DirectScene::InitBuffer() {

    HRESULT hr;

    D3D11_BUFFER_DESC bd{};
    D3D11_SUBRESOURCE_DATA initData{};

    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(XMMATRIX);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = device->CreateBuffer(&bd, NULL, &pConstantBufferWorld);
    hr = device->CreateBuffer(&bd, NULL, &pConstantBufferView);
    hr = device->CreateBuffer(&bd, NULL, &pConstantBufferProjection);
    deviceContext->VSSetConstantBuffers(0, 1, &pConstantBufferWorld);
    deviceContext->VSSetConstantBuffers(1, 1, &pConstantBufferView);
    deviceContext->VSSetConstantBuffers(2, 1, &pConstantBufferProjection);

    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_MAPPED_SUBRESOURCE mappedData;
    mProjection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)winWidth / (float)winHeight, 0.1f, 100.0f));
    deviceContext->Map(pConstantBufferProjection, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    memcpy_s(mappedData.pData, sizeof(mProjection), &mProjection, sizeof(mProjection));
    deviceContext->Unmap(pConstantBufferProjection, 0);
}


void DirectScene::BeginPaint() {
    float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    //使用清除颜色清除后备缓冲区，准备渲染新的帧
    deviceContext->ClearRenderTargetView(renderTargetView, ClearColor);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    D3D11_MAPPED_SUBRESOURCE mappedData;

    mView = XMMatrixTranspose(
        XMMatrixLookAtLH(
            XMVectorSet(5.0f, 0.0f, 0.0f, 0.0f),
            XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
            XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
        ));
    deviceContext->Map(pConstantBufferView, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    memcpy_s(mappedData.pData, sizeof(mView), &mView, sizeof(mView));
    deviceContext->Unmap(pConstantBufferView, 0);
}
void DirectScene::EndPaint() {
    swapChain->Present(0, 0);
}