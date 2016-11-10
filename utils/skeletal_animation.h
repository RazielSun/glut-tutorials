
#ifndef SKELETAL_ANIMATION_H
#define SKELETAL_ANIMATION_H

#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "utils.h"
#include "texture.h"
#include "skinned_only_mesh.h"

class SkeletalAnimation {
public:
	SkeletalAnimation() {}
	~SkeletalAnimation() {}

	bool Load(const std::string& fileName);

	void BoneTransform(float TimeInSeconds, SkinnedOnlyMesh* mesh, std::vector<Matrix4f>& Transforms);

private:
	void ReadNodeHeirarchy(SkinnedOnlyMesh* mesh, float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	bool InitFromScene(const aiScene* scene, const std::string& fileName);
	bool InitMaterials(const aiScene* scene, const std::string& fileName);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);

	const aiScene* m_scene;
    Assimp::Importer m_Importer;
};

#endif /* SKELETAL_ANIMATION_H */