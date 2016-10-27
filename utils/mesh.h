
#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "utils.h"
#include "texture.h"

struct Vertex {
	Vector3f pos;
	Vector2f uv;
	Vector3f normal;
	Vector3f tangent;

	Vertex() {}

	Vertex(Vector3f _pos, Vector2f _uv)
	{
		pos = _pos;
		uv = _uv;
		normal = Vector3f(0.0f, 0.0f, 0.0f);
		tangent = Vector3f(0.0f, 0.0f, 0.0f);
	}

	Vertex(Vector3f _pos, Vector2f _uv, Vector3f _normal)
	{
		pos = _pos;
		uv = _uv;
		normal = _normal;
	}

	Vertex(const Vector3f& _pos, const Vector2f& _uv, const Vector3f& _normal, const Vector3f& _tangent)
	{
		pos = _pos;
		uv = _uv;
		normal = _normal;
		tangent = _tangent;
	}
};

class Mesh {
public:
	Mesh();
	~Mesh();

	bool LoadMesh(const std::string& fileName);
	void Render();

private:
	bool InitFromScene(const aiScene* scene, const std::string& fileName);
	void InitMesh(unsigned int Index, const aiMesh* paiMesh);
	bool InitMaterials(const aiScene* scene, const std::string& fileName);
	void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

	struct MeshEntry {
		MeshEntry();
		~MeshEntry();

		void Init(const std::vector<Vertex>& verticies, const std::vector<unsigned int>& indices);

		GLuint VB;
		GLuint IB;

		unsigned int numIndices;
		unsigned int materialIndex;
	};

	std::vector<MeshEntry> m_Entries;
	std::vector<Texture*> m_Textures; 
};

#endif /* MESH_H */