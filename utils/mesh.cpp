
#include <assert.h>
#include "mesh.h"

Mesh::MeshEntry::MeshEntry()
{
	VB = INVALID_OGL_VALUE;
	IB = INVALID_OGL_VALUE;
	numIndices = 0;
	materialIndex = INVALID_MATERIAL;
}

Mesh::MeshEntry::~MeshEntry()
{
	if (VB != INVALID_OGL_VALUE)
    {
        glDeleteBuffers(1, &VB);
    }

    if (IB != INVALID_OGL_VALUE)
    {
        glDeleteBuffers(1, &IB);
    }
}

void Mesh::MeshEntry::Init(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    glGenBuffers(1, &VB);
  	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	numIndices = indices.size();

    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, &indices[0], GL_STATIC_DRAW);
}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	Clear();
}

void Mesh::Clear()
{
	for (unsigned int i = 0; i < m_Textures.size(); i++)
	{
		SAFE_DELETE(m_Textures[i]);
	}
}

bool Mesh::LoadMesh(const std::string& fileName)
{
	Clear();

	bool success = false;
	Assimp::Importer Importer;

    const aiScene* scene = Importer.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    
    if (scene)
    {
        success = InitFromScene(scene, fileName);
    }
    else
    {
        printf("Error parsing '%s': '%s'\n", fileName.c_str(), Importer.GetErrorString());
    }

	return success;
}

void Mesh::Render()
{
	glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    for (unsigned int i = 0; i < m_Entries.size(); i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VB);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].IB);

        const unsigned int MaterialIndex = m_Entries[i].materialIndex;

        if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex])
        {
            m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
        }

        glDrawElements(GL_TRIANGLES, m_Entries[i].numIndices, GL_UNSIGNED_INT, 0);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

bool Mesh::InitFromScene(const aiScene* scene, const std::string& fileName)
{
	m_Entries.resize(scene->mNumMeshes);
	m_Textures.resize(scene->mNumMaterials);

	for (unsigned int i = 0; i < m_Entries.size(); i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		InitMesh(i, mesh);
	}
	return InitMaterials(scene, fileName);
}

void Mesh::InitMesh(unsigned int Index, const aiMesh* mesh)
{
	m_Entries[Index].materialIndex = mesh->mMaterialIndex;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		const aiVector3D* pos = &(mesh->mVertices[i]);
		const aiVector3D* normal = &(mesh->mNormals[i]);
		const aiVector3D* uv = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &Zero3D;

		Vertex v(Vector3f(pos->x, pos->y, pos->z), Vector2f(uv->x, uv->y), Vector3f(normal->x, normal->y, normal->z));

		vertices.push_back(v);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	m_Entries[Index].Init(vertices, indices);
}

bool Mesh::InitMaterials(const aiScene* scene, const std::string& fileName)
{
	std::string::size_type slash = fileName.find_last_of("/");
    std::string dir;

    if (slash == std::string::npos) {
        dir = ".";
    }
    else if (slash == 0) {
        dir = "/";
    }
    else {
        dir = fileName.substr(0, slash);
    }

    bool success = true;

    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        const aiMaterial* pMaterial = scene->mMaterials[i];

        m_Textures[i] = NULL;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
            {
                std::string FullPath = dir + "/" + Path.data;
                m_Textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

                if (!m_Textures[i]->Load())
                {
                    printf("Error loading texture '%s'\n", FullPath.c_str());
                    delete m_Textures[i];
                    m_Textures[i] = NULL;
                    success = false;
                }
                else
                {
                    printf("Loaded texture '%s'\n", FullPath.c_str());
                }
            }
        }

        if (!m_Textures[i])
        {
            m_Textures[i] = new Texture(GL_TEXTURE_2D, "content/white.png");

            success = m_Textures[i]->Load();
        }
    }

    return success;
}
