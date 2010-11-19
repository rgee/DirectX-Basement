#include "Mesh.h"

Mesh::Mesh(const std::string& filename, ID3D10Device* pDevice)
    : device(pDevice)
{
    LoadFromFile(filename);
}

Mesh::~Mesh()
{
}

ID3DX10Mesh* Mesh::GetMesh()
{
    return mesh;
}

void Mesh::LoadFromFile(const std::string& filename)
{
    vertex* vertices;
    DWORD* indices;
    UINT* attributes;
    UINT numTriangles;
    UINT numVertices;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_MakeLeftHanded |
        aiProcess_GenSmoothNormals);

    if(!scene) return;

    aiMesh** meshes = scene->mMeshes;
    UINT* meshID;

    aiNode* sceneRoot = scene->mRootNode;
    aiNode** children = sceneRoot->mChildren;

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
            numVertices = meshes[meshID[0]]->mNumVertices;
            numTriangles = meshes[meshID[0]]->mNumFaces;

            vertices = new vertex[numVertices];

            for(UINT j = 0; j < numVertices; ++j)
            {
                vertices[j].position = D3DXVECTOR3(meshes[meshID[0]]->mVertices[j].x,
                                                   meshes[meshID[0]]->mVertices[j].y,
                                                   meshes[meshID[0]]->mVertices[j].z);

                //vertices[j].color = D3DXVECTOR4(meshes[meshID[0]]->mColors[j]->r,
                //                                meshes[meshID[0]]->mColors[j]->g,
                //                                meshes[meshID[0]]->mColors[j]->b,
                //                                meshes[meshID[0]]->mColors[j]->a);

                //vertices[j].texCoord = D3DXVECTOR2(meshes[meshID[0]]->mTextureCoords[0][j].x,
                //                                   meshes[meshID[0]]->mTextureCoords[0][j].y);

                vertices[j].normal = D3DXVECTOR3(meshes[meshID[0]]->mNormals[j].x,
                                                 meshes[meshID[0]]->mNormals[j].y,
                                                 meshes[meshID[0]]->mNormals[j].z);

                //vertices[j].color = D3DXVECTOR4(0.8f, 0.2f, 0.2f, 1.0f);
            }

            D3DX10CreateMesh(device, layout, sizeof(layout)/sizeof(layout[0]), layout[0].SemanticName , numVertices, numTriangles, D3DX10_MESH_32_BIT, &mesh);

            mesh->SetVertexData(0, vertices);

            indices = new DWORD[numTriangles*3];
            for(UINT j = 0; j < meshes[meshID[0]]->mNumFaces; j++)
            {
                indices[j * 3 + 0] = meshes[meshID[0]]->mFaces[j].mIndices[0];
                indices[j * 3 + 1] = meshes[meshID[0]]->mFaces[j].mIndices[1]; 
                indices[j * 3 + 2] = meshes[meshID[0]]->mFaces[j].mIndices[2];
            }
            mesh->SetIndexData(indices, numTriangles*3);
            mesh->CommitToDevice();

            delete[] vertices;
            delete[] indices;
        }
    }
}