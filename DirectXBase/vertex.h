#ifndef VERTEX
#define VERTEX
#include <d3d10.h>
#include <d3dx10.h>

struct vertex 
{
    D3DXVECTOR3 position;
    D3DXVECTOR4 color;

    vertex()
    {
    }

    vertex(D3DXVECTOR3 p, D3DXVECTOR4 c)
    {
        position = p;
        color = c;
    }

};

const D3D10_INPUT_ELEMENT_DESC layout[] = 
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
};

#endif