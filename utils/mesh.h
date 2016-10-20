
#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "utils.h"
#include "texture.h"

struct Vertex {
	Vector3f pos;
	Vector2f uv;
	Vector3f normal;

	Vertex() {}

	Vertex(Vector3f _pos, Vector2f _uv)
	{
		pos = _pos;
		uv = _uv;
		normal = Vector3f(0.0f, 0.0f, 0.0f);
	}

	Vertex(Vector3f _pos, Vector2f _uv, Vector3f _normal)
	{
		pos = _pos;
		uv = _uv;
		normal = _normal;
	}
};

class Mesh {
public:
	Mesh();
	~Mesh();
	bool LoadMesh(const std::string& fileName);
	void Render();
private:
	void Clear();

	std::vector<Texture*> m_Textures; 
};

#endif /* MESH_H */