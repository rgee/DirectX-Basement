#include "Mesh.h"

void Mesh::LoadFromFile(const std::string& filename)
{
    vertex* vertices;
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

                vertices[j].color = D3DXVECTOR4(meshes[meshID[0]]->mColors[j]->r,
                                                meshes[meshID[0]]->mColors[j]->g,
                                                meshes[meshID[0]]->mColors[j]->b,
                                                meshes[meshID[0]]->mColors[j]->a);
            }
        }
    }
}