#ifndef DXMANAGER
#define DXMANAGER

#include <Windows.h>
#include <vector>
#include <string>
#include <d3d10.h>
#include <d3dx10.h>
#include "vertex.h"
#include "Terrain.h"
#include "Camera.h"


/* Manager class for interactions with DirectX */
class DXManager
{
public:
    DXManager();
    ~DXManager();

    /* Initialize the DirectX Manager */
    bool Initialize(HWND* hWnd);

    /* Render the current scene. */
    void Render();

    /* Update the state of the scene */
    void Update();

    /* Process a windows message */
    void ProcessMessage(UINT message, LPARAM lparam);

private:

    HWND* hWnd;

    /* The actual D3D Device */
    ID3D10Device* pD3DDevice;

    /* The swap chain. - Stores data in buffers for double/triple
       buffering and such. */
    IDXGISwapChain* pSwapChain;

    /* View of the render target resource */
    ID3D10RenderTargetView* pRenderTargetView;

    D3D10_VIEWPORT viewport;

    /* Buffer for vertex storage */
    ID3D10Buffer* pVertexBuffer;
    ID3D10Buffer* pIndexBuffer;

    /* Depth stencil and corresponding resource view */
    ID3D10Texture2D*			pDepthStencil;
	ID3D10DepthStencilView*		pDepthStencilView;	

    /* Responsible for accessing data during IA stage
       to build primitives. */
    ID3D10InputLayout* pVertexLayout;

    
    D3D10_TECHNIQUE_DESC    techDesc;

    /* Stores shader-relevant state */
    ID3D10Effect* pBasicEffect;

    /* Collection of effect passes */
    ID3D10EffectTechnique* pBasicTechnique;

    /* Effect Variables */
    ID3D10EffectMatrixVariable* pViewMatrixEffectVar;
    ID3D10EffectMatrixVariable* pProjMatrixEffectVar;
    ID3D10EffectMatrixVariable* pWorldMatrixEffectVar;

    /* Essential matrices */
    D3DXMATRIX viewMatrix;
    D3DXMATRIX projectionMatrix;
    D3DXMATRIX worldMatrix;

    /* Mesh */
    ID3DX10Mesh* pMesh;

    /* Textures */
    std::vector<ID3D10ShaderResourceView*> textureSRV;
    ID3D10EffectShaderResourceVariable* pTextureSR;
    int textureIndex;

    /* Terrain object for testing */
    Terrain* pTerrain;

    RAWINPUTDEVICE rid[2];

    Camera camera;

    /************************
     * Private Methods
     ************************/
    bool FatalError(LPCSTR msg);

    bool LoadTextures();

    bool InitializeScene();

    long last_mouse_x;
    long last_mouse_y;
};

#endif