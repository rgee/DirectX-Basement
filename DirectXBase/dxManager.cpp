#include "dxManager.h"




DXManager::DXManager()
    : pD3DDevice(NULL),
      pSwapChain(NULL),
      pRenderTargetView(NULL),
      textureIndex(0)
{
    D3DXMatrixIdentity(&worldMatrix);
	D3DXMatrixIdentity(&projectionMatrix);
	D3DXMatrixIdentity(&viewMatrix);
    
}

DXManager::~DXManager()
{
    if(pD3DDevice) {
        pD3DDevice->Release();
    }
    if(pSwapChain) {
        pSwapChain->Release();
    }
    if(pRenderTargetView) {
        pRenderTargetView->Release();
    }
}

bool DXManager::Initialize(HWND* hW)
{
    // Keyboard
    rid[0].usUsagePage = 1;
    rid[0].usUsage = 6;
    rid[0].dwFlags = 0;
    rid[0].hwndTarget = NULL;

    // Mouse
    rid[1].usUsagePage = 1;
    rid[1].usUsage = 2;
    rid[1].dwFlags = 0;
    rid[1].hwndTarget = NULL;


    RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE));

    hWnd = hW;

    RECT windowDimensions;
    GetClientRect(*hWnd, &windowDimensions);
    UINT height = windowDimensions.bottom - windowDimensions.top;
    UINT width = windowDimensions.right - windowDimensions.left;

    /* Set up the swap chain */
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 4;
    swapChainDesc.OutputWindow = *hWnd;
    swapChainDesc.Windowed = true;

    /* Actually create the D3D device */
    if(FAILED(D3D10CreateDeviceAndSwapChain(NULL,
                                            D3D10_DRIVER_TYPE_HARDWARE,
                                              NULL,
                                            0,
                                            D3D10_SDK_VERSION,
                                            &swapChainDesc,
                                            &pSwapChain,
                                            &pD3DDevice))) {
        return FatalError("D3D Device creation failed!");
    }


    /* Load Shaders */
    if(FAILED(D3DX10CreateEffectFromFile("simple_shader.fx",
                                               NULL,
                                               NULL,
                                               "fx_4_0",
                                               D3D10_SHADER_ENABLE_STRICTNESS,
                                               0,
                                               pD3DDevice,
                                               NULL,
                                               NULL,
                                               &pBasicEffect,
                                               NULL,
                                               NULL))) {
        return FatalError("Could not load shader.");
    }

    pBasicTechnique = pBasicEffect->GetTechniqueByName("full");

    /* Get the effect variables */
    pViewMatrixEffectVar = pBasicEffect->GetVariableByName("View")->AsMatrix();
    pProjMatrixEffectVar = pBasicEffect->GetVariableByName("Projection")->AsMatrix();
    pWorldMatrixEffectVar = pBasicEffect->GetVariableByName("World")->AsMatrix();
    pTextureSR = pBasicEffect->GetVariableByName("tex2D")->AsShaderResource();


    

    D3D10_PASS_DESC passDesc;
    pBasicTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
    /* Create and set the input layout */

    UINT numElements = sizeof(layout) / sizeof(layout[0]);
    if(FAILED(pD3DDevice->CreateInputLayout(layout,
                                            numElements,
                                            passDesc.pIAInputSignature,
                                            passDesc.IAInputSignatureSize,
                                            &pVertexLayout))) {
        return FatalError("Could not create input layout");
    }

    pD3DDevice->IASetInputLayout(pVertexLayout);

    pBasicTechnique->GetDesc(&techDesc);

    /* ------------------------------------
     * Set up the rasterizer stage 
     * -------------------------------------*/

    /* Create the viewport */
    viewport.Height = height;
    viewport.Width = width;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;

    /* Bind viewport to the render pipeline */
    pD3DDevice->RSSetViewports(1, &viewport);


    D3D10_RASTERIZER_DESC rasterizerState;
    rasterizerState.CullMode = D3D10_CULL_NONE;
    rasterizerState.FillMode = D3D10_FILL_SOLID;
    rasterizerState.FrontCounterClockwise = true;
    rasterizerState.DepthBias = false;
    rasterizerState.DepthBiasClamp = 0;
    rasterizerState.SlopeScaledDepthBias = 0;
    rasterizerState.DepthClipEnable = true;
    rasterizerState.ScissorEnable = false;
    rasterizerState.MultisampleEnable = true;
    rasterizerState.AntialiasedLineEnable = true;

    ID3D10RasterizerState* pRasterState;
    pD3DDevice->CreateRasterizerState(&rasterizerState, &pRasterState);

    pD3DDevice->RSSetState(pRasterState);


    /* Create the render target view */

    /* A 2d texture resource to represent the back buffer */
    ID3D10Texture2D* pBackBuffer;
    if(FAILED(pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer))) {
        return FatalError("Could not get back buffer.");
    }

    if(FAILED(pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView))) {
        return FatalError("Could not create render target view");
    }

    /* Release this texture object. We don't need it anymore. */
    pBackBuffer->Release();
    

    	//create depth stencil texture
    D3D10_TEXTURE2D_DESC descDepth;
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D10_USAGE_DEFAULT;
    descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;   
    
	if( FAILED( pD3DDevice->CreateTexture2D( &descDepth, NULL, &pDepthStencil ) ) ) {
        return FatalError("Could not create depth stencil texture");
    }

        // Create the depth stencil view
    D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    
    if( FAILED( pD3DDevice->CreateDepthStencilView( pDepthStencil, &descDSV, &pDepthStencilView ) ) ) {
        return FatalError("Could not create depth stencil view");
    }

    /* Bind render target to the pipeline */
    pD3DDevice->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);


    /*
    D3DXMatrixLookAtLH(&viewMatrix, new D3DXVECTOR3(0.0f, 350.0f, 200.0f),
                                    new D3DXVECTOR3(0.0f, 200.0f, 0.0f),
                                    new D3DXVECTOR3(0.0f, 1.0f, 0.0f));
    D3DXMatrixPerspectiveFovLH(&projectionMatrix, (float)D3DX_PI * 0.5f, (float)width/(float)height, 0.1f, 3000.0f);
    */
    camera.SetPositionAndView(0,0,0,-20.0f, -20.0f);
    camera.SetPerspectiveProjection(45.0f, (float)width/(float)height, 0.1f, 3000.0f);

    pProjMatrixEffectVar->SetMatrix(*camera.GetProjectionMatrix());
    pViewMatrixEffectVar->SetMatrix(*camera.GetViewMatrix());

    InitializeScene();

    return true;
}

bool DXManager::InitializeScene()
{
    D3DXMATRIX terrainPos;
    D3DXMatrixTranslation(&terrainPos, 0.0f, 0.0f, 100.0f);

    pTerrain = new Terrain(200, 200, terrainPos, pD3DDevice);
    
	return true;
}

void DXManager::Update()
{
    camera.Update();
    pProjMatrixEffectVar->SetMatrix(*camera.GetProjectionMatrix());
    pViewMatrixEffectVar->SetMatrix(*camera.GetViewMatrix());
}

void DXManager::Render()
{
    pD3DDevice->ClearRenderTargetView( pRenderTargetView, D3DXCOLOR(0,0,0,0));
    pD3DDevice->ClearDepthStencilView( pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );

    pWorldMatrixEffectVar->SetMatrix(worldMatrix);
    for(UINT pass = 0; pass < techDesc.Passes; pass++)
    {
        pBasicTechnique->GetPassByIndex(pass)->Apply(0);
        pTerrain->GetMesh()->DrawSubset(0);
    }     
    pSwapChain->Present(0,0);

}

bool DXManager::FatalError(LPCSTR msg)
{
    MessageBox(*hWnd, msg, "Fatal Error", MB_ICONERROR);
    return false;
}

bool DXManager::LoadTextures()
{
    std::vector<std::string> filenames;
    filenames.push_back("textures/t1.bmp");
    filenames.push_back("textures/t2.bmp");
	filenames.push_back("textures/t3.bmp");

    for(int i=0; i < (int)filenames.size(); i++)
    {
        textureSRV.push_back(NULL);
        
        /* Create the texture and its view */
        if(FAILED(D3DX10CreateShaderResourceViewFromFile(pD3DDevice, filenames[i].c_str(), NULL, NULL, &textureSRV[i], NULL)))
        {
            char err[256];
            sprintf_s(err, "Could not load texture: %s!", filenames[i].c_str());
            return FatalError(err);
        }
    }
    return true;
}

void DXManager::ProcessMessage(UINT msg, LPARAM lparam)
{
    switch(msg)
    {
    case WM_INPUT:
        UINT bufferSize;
        GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));

        BYTE* buffer = new BYTE[bufferSize];

        GetRawInputData((HRAWINPUT)lparam, RID_INPUT, (LPVOID)buffer, &bufferSize, sizeof(RAWINPUTHEADER));

        RAWINPUT* raw = (RAWINPUT*)buffer;

        if(raw->header.dwType == RIM_TYPEMOUSE)
        {   
            long mx = raw->data.mouse.lLastX;
            long my = raw->data.mouse.lLastY;

            camera.AdjustYawPitch(mx*0.005f, my*0.005f );
        }
        if(raw->header.dwType == RIM_TYPEKEYBOARD)
        {
            USHORT keyCode = raw->data.keyboard.VKey;
            bool keyUp = raw->data.keyboard.Flags & RI_KEY_BREAK;

            if(keyUp) {
                switch(keyCode)
                {
                case 0x57:
                    camera.SetMovementToggles(0, 0.0f);
                    break;
                case 0x53:
                    camera.SetMovementToggles(1, 0.0f);
                    break;
                case 0x41:
                    camera.SetMovementToggles(2, 0.0f);
                    break;
                case 0x44:
                    camera.SetMovementToggles(3, 0.0f);
                }

            } else {
                switch(keyCode)
                {
                case 0x57:
                    camera.SetMovementToggles(0, 50.0f);
                    break;
                case 0x53:
                    camera.SetMovementToggles(1, -50.0f);
                    break;
                case 0x41:
                    camera.SetMovementToggles(2, -50.0f);
                    break;
                case 0x44:
                    camera.SetMovementToggles(3, 50.0f);
                }
            }
        }
        SetCursorPos(0.0f, 0.0f);
        break;
    }
}