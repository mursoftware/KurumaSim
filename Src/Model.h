#pragma once

#include "RenderManager.h"






struct SUBSET_MATERIAL
{
	char						Name[256];
	MATERIAL					Material;

	std::unique_ptr<TEXTURE>	TextureBaseColor;
	std::unique_ptr<TEXTURE>	TextureSubColor;
};

struct SUBSET
{
	char			Name[256];
	unsigned int	StartIndex;
	unsigned int	IndexNum;
	SUBSET_MATERIAL	Material;
	std::unique_ptr<CONSTANT_BUFFER>	ConstantBuffer;

};




struct MODEL_SUBSET_MATERIAL
{
	char						Name[256];
	MATERIAL					Material;

	char						TextureNameBaseColor[256];
	char						TextureNameSubColor[256];
};

struct MODEL_SUBSET
{
	char					Name[256];
	unsigned int			StartIndex;
	unsigned int			IndexNum;
	MODEL_SUBSET_MATERIAL	Material;
};

struct MODEL
{
	VERTEX_3D		*VertexArray;
	unsigned int	VertexNum;

	unsigned int	*IndexArray;
	unsigned int	IndexNum;

	MODEL_SUBSET	*SubsetArray;
	unsigned int	SubsetNum;
};





struct COLLISION_FACE
{
	Vector3			Position[3];
	Vector3			Normal;
};





class Model
{
private:

	std::string		m_FileName;
	bool			m_Loaded = false;
	bool			m_Collision{};
	bool			m_AO{};



	ComPtr<ID3D12Resource>	m_VertexBuffer;
	ComPtr<ID3D12Resource>	m_IndexBuffer;
	unsigned int	m_VertexNum{};
	unsigned int	m_IndexNum{};


	std::vector<SUBSET>			m_SubsetArray;


	float m_Size{};

	float m_BBMinX{};
	float m_BBMaxX{};
	float m_BBMinZ{};
	float m_BBMaxZ{};

	int m_CollisionDevideX{};
	int m_CollisionDevideZ{};

	std::vector<COLLISION_FACE>		*m_CollisionFace{};

	std::vector<Vector3>			m_PositionList;



	void LoadObj( const char *FileName, MODEL *Model );
	void LoadMaterial( const char *FileName, MODEL_SUBSET_MATERIAL **MaterialArray, unsigned int *MaterialNum );

	void LoadObjBin(const char* FileName, MODEL* Model);
	void SaveObjBin(const char* FileName, MODEL* Model);

public:

	~Model();


	void Draw(bool OverridTexture =false, std::unordered_map<std::string, MATERIAL>* OverridMaterial =nullptr);
	void DrawNonIndex(bool OverrideTexture, std::unordered_map<std::string, MATERIAL>* OverridMaterial = nullptr);

	void Load(const char *FileName, int Priority= THREAD_PRIORITY_NORMAL, bool Collision=false, bool AO = false);
	void LoadThread();


	bool IsLoaded() { return m_Loaded; }


	bool RayHitCheck(Vector3 position, Vector3 direction, Vector3* hitPosition, Vector3* hitNormal);


	std::vector<Vector3>* GetPositionList() { return &m_PositionList; }


	float GetSize() { return m_Size; }
};