#include "Terrain.h"


const D3DXVECTOR4 WHITE(0.95f, 0.95f, 0.95f, 1.0f);
const D3DXVECTOR4 SAND(1.0f, 0.96f, 0.62f, 1.0f);
const D3DXVECTOR4 LIGHT_YELLOW_GREEN(0.48f, 0.77f, 0.46f, 1.0f);
const D3DXVECTOR4 DARK_YELLOW_GREEN(0.1f, 0.48f, 0.19f, 1.0f);
const D3DXVECTOR4 DRK_BROWN(0.45f, 0.39f, 0.34f, 1.0f);
const D3DXVECTOR4 DRK_BLUE(0.25f, 0.39f, 0.80f, 1.0f);


float Terrain::GetHeight(float x, float z) const
{
    return 0.3f * ( z * sinf(0.1f * x) + x * cosf(0.1f * z) );
}

void Terrain::GenerateMesh()
{
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

            /*
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
            */
            
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

    D3DX10CreateMesh(pDevice, layout, 2, "POSITION", (fieldWidth * fieldDepth), ((fieldWidth-1) * (fieldDepth-1)) * 2, D3DX10_MESH_32_BIT, &pMesh);

    pMesh->SetVertexData(0, vertices);
    pMesh->SetIndexData(indices, fieldWidth * fieldDepth * 6);
    pMesh->CommitToDevice();
}

