#ifndef LIGHT_TYPES
#define LIGHT_TYPES

#include <d3d10.h>
#include <d3dx10.h>

struct PointLight
{
    D3DXVECTOR3 position;
    D3DXVECTOR4 color;
};

struct DirectionalLight
{
    D3DXVECTOR3 position;
    D3DXVECTOR3 direction;
    D3DXVECTOR4 color;
};

#endif