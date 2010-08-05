#include "Model.h"
using namespace Assimp;
void Model::LoadMeshData(ID3D10Device* pD3DDevice) 
{
    Assimp::Importer importer;
    const aiScene* input_scene = importer.ReadFile(fileName,
        aiProcess_MakeLeftHanded |
        aiProcess_OptimizeMeshes |
        aiProcess_SortByPType    |
        aiProcess_GenNormals);

    unsigned int* meshID;
    unsigned int numVertices;
    unsigned int numTriangles;
    unsigned int* indices;
    vertex* vertices;
    aiMesh** meshes = input_scene->mMeshes;
    aiNode* rootNode = input_scene->mRootNode;
    aiNode** children = rootNode->mChildren;
    aiNode* child = rootNode;

    unsigned int i = 0;
    while( i < rootNode->mNumChildren)
    {
        child = children[i];
        i++;

        if(child->mNumMeshes > 0){
            meshID = child->mMeshes;
            numVertices = meshes[meshID[0]]->mNumVertices;
            numTriangles = meshes[meshID[0]]->mNumFaces;

            vertices = new vertex[numVertices];

            for(unsigned int v = 0; v < meshes[meshID[0]]->mNumVertices; ++v)
            {
                vertices[v] = vertex(meshes[meshID[0]]->mVertices[v], 
                                     D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1.0f),
                                     meshes[meshID[0]]->mNormals[v]);
            }

            D3DX10CreateMesh(pD3DDevice, position_color_normal_Desc, 3, "POSITION", numVertices, numTriangles, D3DX10_MESH_32_BIT, &pMesh);

            pMesh->SetVertexData(0, vertices);

            indices = new unsigned int[numTriangles * 3];

            for(unsigned int i = 0; i < meshes[meshID[0]]->mNumFaces; ++i)
            {
                indices[i * 3 + 0] = meshes[meshID[0]]->mFaces[i].mIndices[0];
                indices[i * 3 + 1] = meshes[meshID[0]]->mFaces[i].mIndices[1];
                indices[i * 3 + 2] = meshes[meshID[0]]->mFaces[i].mIndices[2];
            }

            pMesh->SetIndexData(indices, 0);
            pMesh->GenerateAdjacencyAndPointReps(0.001f);

            /* Reorder faces to reduce cache misses */
            pMesh->Optimize(D3DX10_MESHOPT_VERTEX_CACHE, 0, 0);
            pMesh->CommitToDevice();
        }

    }


    delete[] vertices;
    delete[] indices;
}