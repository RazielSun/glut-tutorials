
#ifndef SKINNED_MESH_H
#define SKINNED_MESH_H

#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "utils.h"
#include "texture.h"

class SkinnedMesh {
public:
	SkinnedMesh() {}
	~SkinnedMesh();

	bool LoadMesh(const std::string& fileName);
	void Render();

	void BoneTransform(float TimeInSeconds, std::vector<Matrix4f>& Transforms);

private:
	void Clear();
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	bool InitFromScene(const aiScene* scene, const std::string& fileName);
	bool InitMaterials(const aiScene* scene, const std::string& fileName);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);

	#define INVALID_MATERIAL 0xFFFFFFFF

	enum VB_TYPES {
	    IB,
	    POS_VB,
	    NORMAL_VB,
	    UV_VB,
	    BONE_VB,
	    TOTAL_VB            
	};

	#define NUM_BONES_PER_VERTEX 4

    struct BoneInfo
    {
        Matrix4f BoneOffset;
        Matrix4f FinalTransformation;        

        BoneInfo()
        {
            BoneOffset.SetZero();
            FinalTransformation.SetZero();            
        }
    };
    
    struct VertexBoneData
    {        
        uint IDs[NUM_BONES_PER_VERTEX];
        float Weights[NUM_BONES_PER_VERTEX];
        
        void AddBoneData(uint BoneID, float Weight);

        VertexBoneData()
        {
        	Reset();
        }

        void Reset()
        {
            ZERO_MEM(IDs);
            ZERO_MEM(Weights);        
        }
    };

    void InitMesh(unsigned int Index,
                        const aiMesh* mesh,
                        std::vector<uint>& Indices,
                        std::vector<Vector3f>& Positions,
                        std::vector<Vector2f>& UVS,
                        std::vector<Vector3f>& Normals,
                        std::vector<VertexBoneData>& Bones);
	void LoadBones(uint Index, const aiMesh* mesh, std::vector<VertexBoneData>& Bones);
	
	GLuint m_VAO;
    GLuint m_buffers[TOTAL_VB];

	struct MeshEntry {
		MeshEntry()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = INVALID_MATERIAL;
		}

		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;
	};

	std::vector<MeshEntry> m_Entries;
	std::vector<Texture*> m_Textures;

	Matrix4f m_GlobalInverseTransform;

	std::map<std::string, uint> m_BoneMapping;
	std::vector<BoneInfo> m_BoneInfo;
    uint m_NumBones;

	const aiScene* m_scene;
    Assimp::Importer m_Importer;
};

#endif /* SKINNED_MESH_H */