#include "Terrain.h"


const D3DXVECTOR4 WHITE(0.95f, 0.95f, 0.95f, 1.0f);
const D3DXVECTOR4 SAND(1.0f, 0.96f, 0.62f, 1.0f);
const D3DXVECTOR4 LIGHT_YELLOW_GREEN(0.48f, 0.77f, 0.46f, 1.0f);
const D3DXVECTOR4 DARK_YELLOW_GREEN(0.1f, 0.48f, 0.19f, 1.0f);
const D3DXVECTOR4 DRK_BROWN(0.45f, 0.39f, 0.34f, 1.0f);
const D3DXVECTOR4 DRK_BLUE(0.25f, 0.39f, 0.80f, 1.0f);


float Terrain::GetHeight(float x, float z) const
{
    return 0.0f;
    //return 0.3f * ( z * sinf(0.1f * x) + x * cosf(0.1f * z) );
}

void Terrain::GenerateMesh()
{
    /*
    float halfWidth = (fieldWidth - 1) * cellSpacing * 0.5f;
    float halfDepth = (fieldDepth - 1) * cellSpacing * 0.5f;

    for(int i = 0; i < fieldWidth; ++i)
    {
        float z = halfDepth - i * cellSpacing;
        for(int j = 0; j < fieldDepth; ++j)
        {
            float x = -halfWidth + j * cellSpacing;
            float y = GetHeight(float(i), float(j));
            //float y = 0;

            D3DXVECTOR4 transformed;
            D3DXVec3Transform(&transformed, &D3DXVECTOR3(x, y, z), &positionMatrix);
            vertices[int(i * fieldWidth + j)].position = D3DXVECTOR3(transformed.x, transformed.y, transformed.z);
            vertices[int(i * fieldWidth + j)].normal = D3DXVECTOR3(0, 1, 0);
            vertices[int(i * fieldWidth + j)].texCoord = D3DXVECTOR2(0,1);

            
            if(y < -15.0f) {
                vertices[int(i * fieldDepth + j)].color = DRK_BLUE;
            } else if( y < -5.0f) {
                vertices[int(i * fieldDepth + j)].color = SAND;
            } else if (y < 5.0f) {
                vertices[int(i * fieldDepth + j)].color = LIGHT_YELLOW_GREEN;
            } else if (y < 12.0f) {
                vertices[int(i * fieldDepth + j)].color = DARK_YELLOW_GREEN;
            } else if (y < 20.0f) {
                vertices[int(i * fieldDepth + j)].color = DRK_BROWN;
            } else {
                vertices[int(i * fieldDepth + j)].color = WHITE;
            }
            
            
        }
    }

    int k = 0;
    for(int i = 0; i < fieldWidth - 1; ++i)
    {
        for(int j = 0; j < fieldDepth - 1; ++j)
        {
            indices[k] = i * fieldDepth + j;
            indices[k + 1] = i * fieldDepth + j + 1;
            indices[k + 2] = (i + 1) * fieldDepth + j;

            indices[k + 3] = (i + 1) * fieldDepth + j;
            indices[k + 4] = i * fieldDepth + j + 1;
            indices[k + 5] = (i + 1) * fieldDepth + j + 1;
            k += 6;
        }
    }
    */    
    
    std::vector<vertex> vertices(4);
    std::vector<unsigned int> indices(6);

    vertices[0].position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f );
    vertices[0].texCoord = D3DXVECTOR2(0.0f, 0.0f);
    vertices[0].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

    vertices[1].position = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
    vertices[1].texCoord = D3DXVECTOR2(1.0f, 0.0f);
    vertices[1].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

    vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
    vertices[2].texCoord = D3DXVECTOR2(1.0f, 1.0f);
    vertices[2].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

    vertices[3].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
    vertices[3].texCoord = D3DXVECTOR2(0.0f, 1.0f);
    vertices[3].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 2;
    indices[4] = 3;
    indices[5] = 0;


    D3DX10CreateMesh(pDevice, layout, sizeof(layout)/sizeof(layout[0]), layout[0].SemanticName, vertices.size(), 2, D3DX10_MESH_32_BIT, &pMesh);

    pMesh->SetVertexData(0, &vertices[0]);
    pMesh->SetIndexData(&indices[0], 6);
    pMesh->CommitToDevice();
}

