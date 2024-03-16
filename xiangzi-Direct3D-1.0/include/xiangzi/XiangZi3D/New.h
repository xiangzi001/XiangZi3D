

#define MessageBox(ch)	{	MessageBoxA(NULL, ch, "demo", MB_ICONEXCLAMATION | MB_OK);	}
#define Pi 3.1415926535897932384626433832795028841971693993751059320781640628620899

#define pRelease(p) { if(p != NULL) p->Release(); p = NULL;}

// 顶点结构体
struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT2 color;
};

/*
Vertex vertices[] =
{
{ { -0.5f, -0.5f, -0.5f }, XMFLOAT4(Colors::Red) },     // left-bottom-front
{ { -0.5f, 0.5f, -0.5f }, XMFLOAT4(Colors::Yellow) },   // left-up-front
{ { 0.5f, 0.5f, -0.5f }, XMFLOAT4(Colors::Green) },     // right-up-front
{ { 0.5f, -0.5f, -0.5f }, XMFLOAT4(Colors::Orange) },   // right-bottom-front
{ { -0.5f, -0.5f, 0.5f }, XMFLOAT4(Colors::Pink) },     // left-bottom-back
{ { -0.5f, 0.5f, 0.5f }, XMFLOAT4(Colors::Blue) },      // left-up-back
{ { 0.5f, 0.5f, 0.5f }, XMFLOAT4(Colors::Black) },      // right-up-back
{ { 0.5f, -0.5f, 0.5f }, XMFLOAT4(Colors::White) },     // right-bottom-back
};
*/
Vertex vertices[] =
{
{ { -0.5f, -0.5f, -0.5f }, XMFLOAT2(0,0) },     // left-bottom-front
{ { -0.5f, 0.5f, -0.5f }, XMFLOAT2(0,0) },   // left-up-front
{ { 0.5f, 0.5f, -0.5f }, XMFLOAT2(0,0) },     // right-up-front
{ { 0.5f, -0.5f, -0.5f }, XMFLOAT2(0,0) },   // right-bottom-front
{ { -0.5f, -0.5f, 0.5f }, XMFLOAT2(0,0) },     // left-bottom-back
{ { -0.5f, 0.5f, 0.5f }, XMFLOAT2(0,0) },      // left-up-back
{ { 0.5f, 0.5f, 0.5f }, XMFLOAT2(0,0) },      // right-up-back
{ { 0.5f, -0.5f, 0.5f }, XMFLOAT2(0,0) },     // right-bottom-back
};

int indices[] =
{
    // front face
    0, 1, 2,
    0, 2, 3,
    // back face
    4, 6, 5,
    4, 7, 6,
    // left face
    4, 5, 1,
    4, 1, 0,
    // right face
    3, 2, 6,
    3, 6, 7,
    // top face
    1, 5, 6,
    1, 6, 2,
    // bottom face
    4, 0, 3,
    4, 3, 7,
};


class DirectWindow
{
private:
    XMouse m_test;
    XMouse m_now;
    XMouse m_old;
    XKey k_test;
    XKey k_now;
    XKey k_old;

public:
    bool use;

    HINSTANCE	hInstance;
    HWND		hWnd;
    UINT		winWidth;
    UINT		winHeight;

    RECT ClientRect;
    RECT WindowRect;

    POINT mouse;
    char mouse_kind;

    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    IDXGISwapChain* swapChain;

    ID3D11DepthStencilView* depthStencilView;
    ID3D11RenderTargetView* renderTargetView;

    ID3D11InputLayout* inputLayout;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;

    ID3D11Buffer* pVertexBuffer = NULL;
    ID3D11Buffer* pIndexBuffer = NULL;

    ID3D11Buffer* pConstantBufferWorld = NULL;
    ID3D11Buffer* pConstantBufferView = NULL;
    ID3D11Buffer* pConstantBufferProjection = NULL;

    XMMATRIX mView;
    XMMATRIX mProjection;

    DirectWindow() :
        use(false),
        hInstance(NULL),
        hWnd(NULL),
        winWidth(640),
        winHeight(480),
        ClientRect{},
        WindowRect{},
        mouse{},
        mouse_kind(0),
        m_test{ false,false,false,false,false,false },
        m_now{ false,false,false,false,false,false },
        m_old{ false,false,false,false,false,false },

        device(NULL),
        deviceContext(NULL),
        swapChain(NULL),
        depthStencilView(NULL),
        renderTargetView(NULL),
        inputLayout(NULL),
        vertexShader(NULL),
        pixelShader(NULL)
    {
    }
    ~DirectWindow(void) {
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

    bool Init();
    bool InitGPU(LPCWSTR Vs, LPCWSTR Ps);
    bool InitGPU_2D();
    bool InitGPU_3D();

    float angle;
    void Draw() {
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

    void CreateVertexBuffer() {

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

    void join(void* func) {
        thread t_main(*(void(*)()) func);
        t_main.detach();
    }
    void close();

    void create(const char*, RECT, HINSTANCE);

    LRESULT DefWinMSG(HWND, UINT, WPARAM, LPARAM);

    bool if_mouse_press(UINT msg) {
        return m_now(msg);
    }
    bool if_mouse_pressonce(UINT msg) {
        return (m_now(msg) && (!m_old(msg)));
    }
    bool if_key_press(WPARAM wParam) {
        return k_now(wParam);
    }
    bool if_key_pressonce(WPARAM wParam) {
        return (k_now(wParam) && (!k_old(wParam)));
    }

};

inline bool DirectWindow::Init() {
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
        return FALSE;
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
        return FALSE;
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
        return FALSE;
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
        return FALSE;
    }
    hr = device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);
    if (FAILED(hr))
    {
        MessageBox(NULL, "Create depth stencil view failed!", "error", MB_OK);
        return FALSE;
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

    return TRUE;

}

inline bool DirectWindow::InitGPU(LPCWSTR Vs, LPCWSTR Ps) {
    HRESULT hr;

    ID3DBlob* vertexShaderBlob = nullptr;
    ID3DBlob* pixelShaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    hr = D3DCompileFromFile(Vs, nullptr, nullptr, "VS", "vs_5_0", 0, 0, &vertexShaderBlob, &errorBlob);
    hr = D3DCompileFromFile(Ps, nullptr, nullptr, "PS", "ps_5_0", 0, 0, &pixelShaderBlob, &errorBlob);

    //MessageBox(_char(_str(hr)));

    device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &vertexShader);
    device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShader);

    deviceContext->VSSetShader(vertexShader, nullptr, 0);
    deviceContext->PSSetShader(pixelShader, nullptr, 0);


    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT numElements = ARRAYSIZE(inputElementDesc);

    inputLayout = nullptr;
    device->CreateInputLayout(inputElementDesc, numElements, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout);
    deviceContext->IASetInputLayout(inputLayout);

    return true;
}

inline bool DirectWindow::InitGPU_2D() {

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

    return true;
}

inline bool DirectWindow::InitGPU_3D() {

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
            float2 tex : TEXCOORD;
        };
        struct PixelInputType {
            float4 position : SV_POSITION;
            float2 tex : TEXCOORD;
        };
        PixelInputType main(VertexInputType input) {
            PixelInputType output;
            output.position = mul(mul(mul(float4(input.position, 1.0f), worldMatrix), viewMatrix), projectionMatrix);
            output.tex = input.tex;
            return output;
        })";

    const char* pixelShaderSource = R"(
        Texture2D tex : register(t0);
        SamplerState samplerLinear : register(s0);

        struct PixelInputType {
            float4 position : SV_POSITION;
            float2 tex : TEXCOORD;
        };
        float4 main(PixelInputType input) : SV_TARGET {
            return tex.Sample(samplerLinear, input.tex);
        })";

    ID3DBlob* vertexShaderBlob;
    D3DCompile(vertexShaderSource, strlen(vertexShaderSource), NULL, NULL, NULL, "main", "vs_5_0", 0, 0, &vertexShaderBlob, NULL);
    device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &vertexShader);

    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0; // 红色边界色
    samplerDesc.BorderColor[1] = 0; // 绿色边界色
    samplerDesc.BorderColor[2] = 0; // 蓝色边界色
    samplerDesc.BorderColor[3] = 0; // alpha值
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    ID3D11SamplerState* samplerState = nullptr;
    device->CreateSamplerState(&samplerDesc, &samplerState);

    deviceContext->VSSetShader(vertexShader, NULL, 0);
    deviceContext->PSSetShader(pixelShader, NULL, 0);

    return true;
}