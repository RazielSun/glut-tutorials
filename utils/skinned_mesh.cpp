
#include <assert.h>
#include "skinned_mesh.h"

#define POSITION_LOCATION       0
#define UV_LOCATION             1
#define NORMAL_LOCATION         2
#define BONE_ID_LOCATION        3
#define BONE_WEIGHT_LOCATION    4

void SkinnedMesh::VertexBoneData::AddBoneData(uint BoneID, float Weight)
{
    for (uint i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(IDs) ; i++) {
        if (Weights[i] == 0.0) {
            IDs[i]     = BoneID;
            Weights[i] = Weight;
            return;
        }        
    }
    
    // should never get here - more bones than we have space for
    assert(0);
}

SkinnedMesh::~SkinnedMesh()
{
	Clear();
}

void SkinnedMesh::Clear()
{
	for (unsigned int i = 0; i < m_Textures.size(); i++)
	{
		SAFE_DELETE(m_Textures[i]);
	}
}

bool SkinnedMesh::LoadMesh(const std::string& fileName)
{
	Clear();

    glGenVertexArrays(1, &m_VAO);   
    glBindVertexArray(m_VAO);
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_buffers), m_buffers);

	bool success = false;

    m_scene = m_Importer.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
    
    if (m_scene)
    {
        success = InitFromScene(m_scene, fileName);
    }
    else
    {
        printf("Error parsing '%s': '%s'\n", fileName.c_str(), m_Importer.GetErrorString());
    }

    glBindVertexArray(0);

	return success;
}

bool SkinnedMesh::InitFromScene(const aiScene* scene, const std::string& fileName)
{
	m_Entries.resize(scene->mNumMeshes);
	m_Textures.resize(scene->mNumMaterials);

    std::vector<uint> Indices;
    std::vector<Vector3f> Positions;
    std::vector<Vector2f> UVS;
    std::vector<Vector3f> Normals;
    std::vector<VertexBoneData> Bones;

    uint NumVertices = 0;
    uint NumIndices = 0;

	for (uint i = 0; i < m_Entries.size(); i++)
	{
        m_Entries[i].MaterialIndex = scene->mMeshes[i]->mMaterialIndex;
        m_Entries[i].NumIndices = scene->mMeshes[i]->mNumFaces * 3;
        m_Entries[i].BaseVertex = NumVertices;
        m_Entries[i].BaseIndex = NumIndices;

        NumVertices += scene->mMeshes[i]->mNumVertices;
        NumIndices += m_Entries[i].NumIndices;
	}

    Indices.reserve(NumIndices);
    Positions.reserve(NumVertices);
    UVS.reserve(NumVertices);
    Normals.reserve(NumVertices);
    Bones.reserve(NumVertices);

    for (uint i = 0; i < m_Entries.size(); i++)
    {
        const aiMesh* mesh = scene->mMeshes[i];
        InitMesh(i, mesh, Indices, Positions, UVS, Normals, Bones);
    }
        
	if (!InitMaterials(scene, fileName)) {
        return false;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[IB]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[UV_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(UVS[0]) * UVS.size(), &UVS[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(UV_LOCATION);
    glVertexAttribPointer(UV_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[BONE_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BONE_ID_LOCATION);
    glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);

    glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
    glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

    return true;
}

void SkinnedMesh::InitMesh(unsigned int Index,
                        const aiMesh* mesh,
                        std::vector<uint>& Indices,
                        std::vector<Vector3f>& Positions,
                        std::vector<Vector2f>& UVS,
                        std::vector<Vector3f>& Normals,
                        std::vector<VertexBoneData>& Bones)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		const aiVector3D* pos = &(mesh->mVertices[i]);
		const aiVector3D* normal = &(mesh->mNormals[i]);
		const aiVector3D* uv = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &Zero3D;

        Positions.push_back(Vector3f(pos->x, pos->y, pos->z));
        UVS.push_back(Vector2f(uv->x, uv->y));
        Normals.push_back(Vector3f(normal->x, normal->y, normal->z));
	}

    LoadBones(Index, mesh, Bones);

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);
		Indices.push_back(face.mIndices[0]);
		Indices.push_back(face.mIndices[1]);
		Indices.push_back(face.mIndices[2]);
	}
}

void SkinnedMesh::LoadBones(uint Index, const aiMesh* mesh, std::vector<VertexBoneData>& Bones)
{
    for (uint i = 0; i < mesh->mNumBones; i++)
    {
        uint BoneIndex = 0;
        std::string BoneName(mesh->mBones[i]->mName.data);

        if (m_BoneMapping.find(BoneName) == m_BoneMapping.end())
        {
            BoneIndex = m_NumBones;
            m_NumBones++;            
            BoneInfo bi;            
            m_BoneInfo.push_back(bi);
            m_BoneInfo[BoneIndex].BoneOffset.UpdateFromAssimpMatrix4x4(mesh->mBones[i]->mOffsetMatrix);            
            m_BoneMapping[BoneName] = BoneIndex;
        }
        else
        {
            BoneIndex = m_BoneMapping[BoneName];
        }                      
        
        for (uint j = 0 ; j < mesh->mBones[i]->mNumWeights ; j++)
        {
            uint VertexID = m_Entries[Index].BaseVertex + mesh->mBones[i]->mWeights[j].mVertexId;
            float Weight  = mesh->mBones[i]->mWeights[j].mWeight;                   
            Bones[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }
}

void SkinnedMesh::BoneTransform(float TimeInSeconds, std::vector<Matrix4f>& Transforms)
{
    Matrix4f Identity;
    Identity.InitIdentity();
    
    float TicksPerSecond = (float)(m_scene->mAnimations[0]->mTicksPerSecond != 0 ? m_scene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = std::fmod(TimeInTicks, (float)m_scene->mAnimations[0]->mDuration);

    ReadNodeHeirarchy(AnimationTime, m_scene->mRootNode, Identity);

    Transforms.resize(m_NumBones);

    for (uint i = 0 ; i < m_NumBones ; i++)
    {
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }
}

void SkinnedMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform)
{
    //
}

bool SkinnedMesh::InitMaterials(const aiScene* scene, const std::string& fileName)
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
    }

    return success;
}

void SkinnedMesh::Render()
{
    glBindVertexArray(m_VAO);

    for (unsigned int i = 0; i < m_Entries.size(); i++)
    {
        const uint MaterialIndex = m_Entries[i].MaterialIndex;

        if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex])
        {
            m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
        }

        glDrawElementsBaseVertex(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(uint) * m_Entries[i].BaseIndex), m_Entries[i].BaseVertex);
    }

    glBindVertexArray(0);
}
