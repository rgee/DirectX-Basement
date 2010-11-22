#include "Mesh.h"

Mesh::Mesh(const std::string& filename, ID3D10Device* pDevice)
    : device(pDevice)
{
    LoadFromFile(filename);
    D3DXMatrixIdentity(&worldMatrix);
}

Mesh::~Mesh()
{
}

ID3DX10Mesh* Mesh::GetMesh()
{
    return mesh;
}

D3DXMATRIX* Mesh::WorldMatrix() 
{
    return &worldMatrix;
}

void Mesh::LoadFromFile(const std::string& filename)
{
     //vertex* vertices;
    //DWORD* indices;
    //UINT* attributes;
    UINT numTriangles = 0;
    UINT numVertices = 0;

    unsigned int numFaces = 0;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_FixInfacingNormals |
        aiProcess_MakeLeftHanded |
        aiProcess_GenNormals |
        aiProcess_FindInvalidData |
        aiProcess_GenUVCoords |
        aiProcess_OptimizeGraph |
        aiProcess_OptimizeMeshes);

    if(!scene) return;

     const aiMesh* _mesh;
    for(size_t a = 0; a < scene->mNumMeshes; ++a)
    {
        _mesh = scene->mMeshes[a];
        numVertices += _mesh->mNumVertices;
        numFaces += _mesh->mNumFaces;
    }
    
    D3DX10CreateMesh(device, layout, sizeof(layout)/sizeof(layout[0]), layout[0].SemanticName , numVertices, numFaces, D3DX10_MESH_32_BIT, &mesh);

    std::vector<vertex> vertices(numVertices);
    std::vector<unsigned int> indices(numFaces * 3);
    std::vector<unsigned int> attributes(numFaces);

    size_t nextVertex = 0;
    size_t nextIndex = 0;
    for(unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        size_t meshStartVertex = nextVertex;
        _mesh = scene->mMeshes[i];
        for(unsigned int x = 0; x < _mesh->mNumFaces; ++x) 
        {

            indices[nextIndex++] = _mesh->mFaces[x].mIndices[0] + meshStartVertex;
            indices[nextIndex++] = _mesh->mFaces[x].mIndices[1] + meshStartVertex;
            indices[nextIndex++] = _mesh->mFaces[x].mIndices[2] + meshStartVertex;
            attributes[x] = i;
        }


        for(unsigned int x = 0; x < _mesh->mNumVertices; ++x)
        {
            vertices[nextVertex].position = D3DXVECTOR3(_mesh->mVertices[x].x, _mesh->mVertices[x].y, _mesh->mVertices[x].z);


            vertices[nextVertex].normal = D3DXVECTOR3(_mesh->mNormals[x].x, _mesh->mNormals[x].y, _mesh->mNormals[x].z);


            if(_mesh->HasTextureCoords(i)) 
            {
                vertices[nextVertex].texCoord = D3DXVECTOR2(_mesh->mTextureCoords[0][x].x, _mesh->mTextureCoords[0][x].y);
            }

            nextVertex++;
        }
    }

    
    mesh->SetVertexData(0, &vertices[0]);
    mesh->SetIndexData(&indices[0], numFaces * 3);
    mesh->SetAttributeData(&attributes[0]);
    mesh->CommitToDevice();






    /*
    size_t numVertices = 0;
    size_t numFaces = 0;

    aiNode* node = scene->mRootNode->mChildren[0];
    
    const aiMesh* mesh;
    for(size_t a = 0; a < node->mNumMeshes; ++a)
    {
        mesh = scene->mMeshes[node->mMeshes[a]];
        numVertices += mesh->mNumVertices;
        numFaces += mesh->mNumFaces;
    }

    D3DX10CreateMesh(device, layout, sizeof(layout)/sizeof(layout[0]), layout[0].SemanticName , numVertices, numFaces, D3DX10_MESH_32_BIT, &modelMesh);

    std::vector<vertex> vertices(numVertices);
    std::vector<unsigned int> indices(numFaces * 3);
    std::vector<unsigned int> attributes(numFaces);

    size_t nextVertex = 0; 
    size_t nextIndex = 0;

    for(size_t a = 0; a < node->mNumMeshes; ++a)
    {
        mesh = scene->mMeshes[node->mMeshes[a]];
        size_t meshStartIndex = nextVertex;

        for(size_t b = 0; b < mesh->mNumVertices; ++b)
        {
            vertices[nextVertex].position = D3DXVECTOR3(mesh->mVertices[b].x,
                                                        mesh->mVertices[b].y,
                                                        mesh->mVertices[b].z);

            vertices[nextVertex].normal = D3DXVECTOR3(mesh->mNormals[b].x,
                                                      mesh->mNormals[b].y,
                                                      mesh->mNormals[b].z);
            
            vertices[nextVertex].texCoord = D3DXVECTOR2(mesh->mTextureCoords[0][b].x,
                                                        mesh->mTextureCoords[0][b].y);

            ++nextVertex;
        }

        for(size_t b = 0; b < mesh->mNumFaces; ++b)
        {
            indices[nextIndex++] = mesh->mFaces[b].mIndices[0] + meshStartIndex;
            indices[nextIndex++] = mesh->mFaces[b].mIndices[1] + meshStartIndex;
            indices[nextIndex++] = mesh->mFaces[b].mIndices[2] + meshStartIndex;
            attributes[b] = a;
        }
    }


    modelMesh->SetVertexData(0, &vertices[0]);
    modelMesh->SetIndexData(&indices[0], numFaces * 3);
    modelMesh->SetAttributeData(&attributes[0]);
    */


    /*
    aiMesh** meshes = scene->mMeshes;
    UINT* meshID;

    aiNode* sceneRoot = scene->mRootNode;
    aiNode** children = sceneRoot->mChildren;
    
    aiMaterial** materials = scene->mMaterials;

    aiNode* child;
    bool rootNode = true;
    UINT i = 0;
    
    while(i < sceneRoot->mNumChildren)
    {
        if(rootNode)
        {
            child = sceneRoot;
            rootNode = false;
        } else {
            child = children[i];
            i++;
        }

        if(child->mNumMeshes > 0)
        {
            meshID = child->mMeshes;

            for(UINT x = 0; x < child->mNumMeshes; ++x)
            {
                numVertices += meshes[meshID[x]]->mNumVertices;
                numTriangles += meshes[meshID[x]]->mNumFaces;
            }

            vertices = new vertex[numVertices];
            indices = new DWORD[numTriangles*3];
            attributes = new UINT[numTriangles];

            for(UINT num_Meshes = 0; num_Meshes < child->mNumMeshes; ++num_Meshes)
            {

                for(UINT n = 0; n < meshes[meshID[num_Meshes]]->mNumVertices; ++n)
                {
                    UINT j = 0;
                    if(num_Meshes == 0)
                    {
                        j = n;
                    } else {
                        j = n + meshes[meshID[num_Meshes - 1]]->mNumVertices;
                    }

                        vertices[j].position = D3DXVECTOR3(meshes[meshID[num_Meshes]]->mVertices[j].x,
                                                           meshes[meshID[num_Meshes]]->mVertices[j].y,
                                                           meshes[meshID[num_Meshes]]->mVertices[j].z);

                        if(meshes[meshID[num_Meshes]]->HasTextureCoords(meshID[num_Meshes])) {
                        vertices[j].texCoord = D3DXVECTOR2(meshes[meshID[num_Meshes]]->mTextureCoords[0][j].x,
                                                           meshes[meshID[num_Meshes]]->mTextureCoords[0][j].y);
                        }

                        vertices[j].normal = D3DXVECTOR3(meshes[meshID[num_Meshes]]->mNormals[j].x,
                                                         meshes[meshID[num_Meshes]]->mNormals[j].y,
                                                         meshes[meshID[num_Meshes]]->mNormals[j].z);

                }

                
                for(UINT n = 0; n < meshes[meshID[num_Meshes]]->mNumFaces; n++)
                {
                    UINT j = 0;
                    if(num_Meshes == 0)
                    {
                        j = n;
                    } else {
                        j = n + meshes[meshID[num_Meshes - 1]]->mNumVertices;
                    }

                    indices[j * 3 + 0] = meshes[meshID[0]]->mFaces[j].mIndices[0];
                    indices[j * 3 + 1] = meshes[meshID[0]]->mFaces[j].mIndices[1]; 
                    indices[j * 3 + 2] = meshes[meshID[0]]->mFaces[j].mIndices[2];
                }
            }

            D3DX10CreateMesh(device, layout, sizeof(layout)/sizeof(layout[0]), layout[0].SemanticName , numVertices, numTriangles, D3DX10_MESH_32_BIT, &mesh);

            mesh->SetVertexData(0, vertices);


            mesh->SetIndexData(indices, numTriangles*3);
            mesh->CommitToDevice();

            delete[] vertices;
            delete[] indices;
        }
    }*/
    
}