
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
	SkinnedMesh();
	~SkinnedMesh();

	bool LoadMesh(const std::string& fileName);
	void Render();

private:
	bool InitFromScene(const aiScene* scene, const std::string& fileName);
	void InitMesh(unsigned int Index,
                        const aiMesh* mesh,
                        std::vector<uint>& Indices,
                        std::vector<Vector3f>& Positions,
                        std::vector<Vector2f>& UVS,
                        std::vector<Vector3f>& Normals);
	bool InitMaterials(const aiScene* scene, const std::string& fileName);
	void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

enum VB_TYPES {
    IB,
    POS_VB,
    NORMAL_VB,
    UV_VB,
    TOTAL_VB            
};
	
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

	const aiScene* m_scene;
    Assimp::Importer m_Importer;
};

#endif /* SKINNED_MESH_H */