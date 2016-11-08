
#include <assert.h>
#include <stdio.h>
#include "skinned_mesh.h"

#define POSITION_LOCATION       0
#define UV_LOCATION             1
#define NORMAL_LOCATION         2
#define BONE_ID_LOCATION        3
#define BONE_WEIGHT_LOCATION    4

void SkinnedMesh::VertexBoneData::AddBoneData(uint BoneID, float Weight)
{
    for (uint i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(IDs) ; i++) {
        // printf("     add: %d %f = %f \n", i, Weights[i], Weight);
        if (Weights[i] == 0.0f) {
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

    m_scene = m_Importer.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    
    if (m_scene)
    {
        m_GlobalInverseTransform = m_scene->mRootNode->mTransformation;
        m_GlobalInverseTransform.Inverse();

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
	m_Entries.resize(scene->mNumMeshes, MeshEntry());
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
    Bones.resize(NumVertices, VertexBoneData());

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
            m_BoneInfo[BoneIndex].BoneOffset = mesh->mBones[i]->mOffsetMatrix;
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
    std::string NodeName(pNode->mName.data);
    const aiAnimation* pAnimation = m_scene->mAnimations[0];
    Matrix4f NodeTransformation = pNode->mTransformation;
    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

    if (pNodeAnim)
    {
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        Matrix4f ScalingM;
        ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);        
        Matrix4f RotationM(RotationQ.GetMatrix());

        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        Matrix4f TranslationM;
        TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);
        
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }

    Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;

    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end())
    {
        uint BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
    }

    for (uint i = 0; i < pNode->mNumChildren; i++)
    {
        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i],GlobalTransformation);
    }
}

const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
    for (uint i = 0; i < pAnimation->mNumChannels; i++)
    {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

        if (std::string(pNodeAnim->mNodeName.data) == NodeName)
        {
            return pNodeAnim;
        }
    }

    return NULL;
}

void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    uint NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }
    
    uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;    
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}

void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }
            
    uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    uint NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

uint SkinnedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{    
    for (uint i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }
    
    assert(0);

    return 0;
}

uint SkinnedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (uint i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }
    
    assert(0);

    return 0;
}


uint SkinnedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);
    
    for (uint i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }
    
    assert(0);

    return 0;
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
