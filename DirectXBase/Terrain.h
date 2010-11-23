#ifndef TERRAIN
#define TERRAIN

#include <math.h>
#include <d3d10.h>
#include <d3dx10.h>
#include <stdio.h>
#include <vector>
#include "vertex.h"


class Terrain
{
public:
    Terrain(int width, int depth, D3DXMATRIX position, ID3D10Device* pDevice)
        : fieldWidth(width),
          fieldDepth(depth),
          positionMatrix(position),
          cellSpacing(10.0f),
          pDevice(pDevice)
    {
        // Allocate memory for vertices
        //vertices = new vertex[width*depth];
        //indices = new unsigned int[width * depth * 6];
        GenerateMesh();
    }

    ~Terrain()
    {
        if(pMesh)
        {
            pMesh->Release();
        }
    }

    int GetWidth() const { return fieldWidth; }
    int GetDepth() const { return fieldDepth; }

    ID3DX10Mesh* GetMesh() { return pMesh ? pMesh : NULL; }
private:
    void GenerateMesh();
    float GetHeight(float x, float z)  const ;

    ID3DX10Mesh* pMesh;
    int fieldWidth;
    int fieldDepth;
    float cellSpacing;

    D3DXMATRIX positionMatrix;
    ID3D10Device* pDevice;
};
#endif