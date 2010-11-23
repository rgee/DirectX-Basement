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
    swapChainDesc.SampleDesc.Quality = 6;
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
                                               D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION,
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
    pWorldInverseTransposeEffectVar = pBasicEffect->GetVariableByName("WorldInverseTranspose")->AsMatrix();


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
    rasterizerState.CullMode = D3D10_CULL_BACK;
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
    

    /* create depth stencil texture */
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

    pTerrain = new Terrain(80, 80, terrainPos, pD3DDevice);

    sphere = new Mesh("../assets/sphere.nff", pD3DDevice);
    testMesh = new Mesh("../assets/floor.obj", pD3DDevice);


    light.position = D3DXVECTOR3(0, 30, 0);
    light.color = D3DXVECTOR4(0.9f, 0.9f, 0.9f, 1.0f);
       
    LightVar = pBasicEffect->GetVariableByName("light");
    

    LoadTextures();
    pTextureSR->SetResource(floorTexture);

	return true;
}

void DXManager::Update()
{
    sphereRot += 0.001f;

    light.position = D3DXVECTOR3(20, 30, 45) + D3DXVECTOR3(0,15*sin(sphereRot)*0.5,0);
    //light.position = camera.GetPosition();
    LightVar->SetRawValue(&light, 0, sizeof(PointLight));

    camera.Update();
    pProjMatrixEffectVar->SetMatrix(*camera.GetProjectionMatrix());
    pViewMatrixEffectVar->SetMatrix(*camera.GetViewMatrix());
}

void DXManager::Render()
{
    D3DXMatrixTransformation(sphere->WorldMatrix(), NULL, NULL, NULL, NULL, NULL, &light.position);
    D3DXMatrixTransformation(testMesh->WorldMatrix(), NULL, NULL, NULL, NULL, NULL, &D3DXVECTOR3(0,-20,0));
  

    pD3DDevice->ClearRenderTargetView( pRenderTargetView, D3DXCOLOR(0,0,0,0));
    pD3DDevice->ClearDepthStencilView( pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );

    UINT subsets = 0;

    D3DXMATRIX worldITX;

    for(UINT pass = 0; pass < techDesc.Passes; pass++)
    {
        
        pWorldMatrixEffectVar->SetMatrix((*sphere->WorldMatrix()));
        D3DXMatrixInverse(sphere->WorldMatrix(), NULL, &worldITX);
        D3DXMatrixTranspose(&worldITX, &worldITX);
        pWorldInverseTransposeEffectVar->SetMatrix(worldITX);
        
        pBasicTechnique->GetPassByIndex(0)->Apply(0);
        
        
        sphere->GetMesh()->GetAttributeTable(NULL, &subsets);

        for(UINT subset = 0; subset < subsets; subset++)
        {
            sphere->GetMesh()->DrawSubset(subset);
        }
        
        D3DXMatrixInverse(testMesh->WorldMatrix(), NULL, &worldITX);
        D3DXMatrixTranspose(&worldITX, &worldITX);
        pWorldInverseTransposeEffectVar->SetMatrix(worldITX);
        pWorldMatrixEffectVar->SetMatrix(worldMatrix);

        pBasicTechnique->GetPassByIndex(0)->Apply(0);

        testMesh->GetMesh()->GetAttributeTable(NULL, &subsets);

        for(UINT subset = 0; subset < subsets; subset++)
        {
            testMesh->GetMesh()->DrawSubset(subset);
        }  
    }

            //pTextureSR->SetResource(floorTexture);



    pSwapChain->Present(0,0);

}

bool DXManager::FatalError(LPCSTR msg)
{
    MessageBox(*hWnd, msg, "Fatal Error", MB_ICONERROR);
    return false;
}

bool DXManager::LoadTextures()
{
    if(FAILED(D3DX10CreateShaderResourceViewFromFile(pD3DDevice, "../assets/metal_color.bmp", NULL, NULL, &floorTexture, NULL)))
    {
        char err[256];
        sprintf_s(err, "Could not load texture: %s!","../assets/metal_color.bmp");
        return FatalError(err);
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