
#include "mesh.h"

Mesh::Mesh()
{
	//
}

Mesh::~Mesh()
{
	Clear();
}

bool Mesh::LoadMesh(const std::string& fileName)
{
	Clear();

	bool success = false;
	Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    
    if (pScene)
    {
        success = InitFromScene(pScene, fileName);
    }
    else
    {
        printf("Error parsing '%s': '%s'\n", fileName.c_str(), Importer.GetErrorString());
    }

	return success;
}

void Mesh::Render()
{
	//
}

void Mesh::Clear()
{
	for (unsigned int i = 0; i < m_Textures.size(); i++)
	{
		SAFE_DELETE(m_Textures[i]);
	}
}