#ifndef MESH
#define MESH

#include <vector>
#include <string>
#include <d3d10.h>
#include <d3dx10.h>
#include "vertex.h"
#include "assimp.hpp"
#include "assimp.h"
#include "aiScene.h"
#include "aiPostProcess.h"

class Mesh 
{
public:
    Mesh(const std::string& filename, ID3D10Device* device);
    ~Mesh();
private:
    void LoadFromFile(const std::string& filename);

    ID3DX10Mesh* mesh;
    ID3D10Device* device;
};


#endif