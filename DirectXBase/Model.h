#ifndef MODEL
#define MODEL

#include <d3d10.h>
#include <d3dx10.h>
#include <string>
#include "vertex.h"

#include "assimp.hpp"
#include "aiMesh.h"
#include "aiScene.h"
#include "aiPostProcess.h"
#include "aiDefines.h"


/*
 * Models wrap the logic of creating and loading meshes.
 */
class Model
{
public:
    Model(std::string fname, ID3D10Device* pD3DDevice)
        : fileName(fname)
    {
        LoadMeshData(pD3DDevice);
    }
private:
    void LoadMeshData(ID3D10Device* pD3DDevice);
    std::string fileName;

    ID3DX10Mesh* pMesh;

    aiScene scene;
};

#endif