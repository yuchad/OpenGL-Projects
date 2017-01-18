#pragma once
struct MeshVertex
{
	VECTOR3D	position;
	VECTOR3D    normal;
};



struct MeshQuad
{
	// pointers to vertices of each quad
	MeshVertex *vertices[4];
};

class QuadMesh
{
private:

	int maxMeshSize;
	int minMeshSize;
	float meshDim;

	int numVertices;
	MeshVertex *vertices;

	int numQuads;
	MeshQuad *quads;

	int numFacesDrawn;

	GLfloat mat_ambient[4];
	GLfloat mat_specular[4];
	GLfloat mat_diffuse[4];
	GLfloat mat_shininess[1];


private:
	bool CreateMemory();
	void FreeMemory();

public:

	typedef std::pair<int, int> MaxMeshDim;

	QuadMesh(int maxMeshSize = 40, float meshDim = 1.0f);

	~QuadMesh()
	{
		FreeMemory();
	}

	MaxMeshDim GetMaxMeshDimentions()
	{
		return MaxMeshDim(minMeshSize, maxMeshSize);
	}

	bool InitMesh(int meshSize, VECTOR3D origin, double meshLength, double meshWidth, VECTOR3D dir1, VECTOR3D dir2);
	void DrawMesh(int meshSize);
	void UpdateMesh();
	void QuadMesh::SetMaterial(VECTOR3D ambient, VECTOR3D diffuse, VECTOR3D specular, double shininess);
	void ComputeNormals();


};

