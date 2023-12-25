
#include <stdio.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "Pch.h"
#include "GameManager.h"
#include "RenderManager.h"
#include "MatrixVector.h"
#include "Model.h"




Model::~Model()
{
	if(m_CollisionFace)
		delete[] m_CollisionFace;
}



void Model::Load(const char* FileName, int Priority, bool Collision)
{
	m_FileName = FileName;
	m_Collision = Collision;


	if (Priority == THREAD_PRIORITY_ABOVE_NORMAL)
	{
		LoadThread();
	}
	else
	{
		std::thread thLoad(&Model::LoadThread, this);
		SetThreadPriority(thLoad.native_handle(), Priority);
		thLoad.detach();
	}
}



void Model::LoadThread()
{


	std::string dir(m_FileName);
	dir = dir.substr(0, dir.find_last_of("\\"));
	dir += "\\";


	std::string path(m_FileName);
	path = path.substr(0, path.find_last_of("."));
	path += ".objbin";

	



	MODEL model{};

	{
		std::unique_lock<std::mutex> lock(*GameManager::GetInstance()->GetFileMutex());


		bool findBin = false;

		HANDLE handleBin = CreateFile(path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (handleBin != INVALID_HANDLE_VALUE)
		{
			HANDLE handleObj = CreateFile(m_FileName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (handleObj != INVALID_HANDLE_VALUE)
			{
				FILETIME timeBin, timeObj;
				GetFileTime(handleBin, NULL, NULL, &timeBin);
				GetFileTime(handleObj, NULL, NULL, &timeObj);

				if (CompareFileTime(&timeBin, &timeObj) > 0)
					findBin = true;

				CloseHandle(handleObj);
			}
			else
			{
				findBin = true;
			}

			CloseHandle(handleBin);
		}






		if (findBin)
		//if (false)
		{
			LoadObjBin(path.c_str(), &model);
		}
		else
		{
			LoadObj(m_FileName.c_str(), &model);
			SaveObjBin(path.c_str(), &model);
		}

	}




	//Data construction for collision detection
	if (m_Collision)
	{
		m_BBMinX = 0.0f;
		m_BBMaxX = 0.0f;
		m_BBMinZ = 0.0f;
		m_BBMaxZ = 0.0f;

		for (unsigned int s = 0; s < model.SubsetNum; s++)
		{
			if (model.SubsetArray[s].Name[0] == '#')
			{
				for (unsigned int i = model.SubsetArray[s].StartIndex; i < model.SubsetArray[s].StartIndex + model.SubsetArray[s].IndexNum; i++)
				{
					Vector3 position = model.VertexArray[model.IndexArray[i]].Position;

					m_BBMinX = min(m_BBMinX, position.X);
					m_BBMaxX = max(m_BBMaxX, position.X);
					m_BBMinZ = min(m_BBMinZ, position.Z);
					m_BBMaxZ = max(m_BBMaxZ, position.Z);
				}
			}
		}


		m_BBMinX -= 10.0f;
		m_BBMaxX += 10.0f;

		m_BBMinZ -= 10.0f;
		m_BBMaxZ += 10.0f;

		m_CollisionDevideX = (int)((m_BBMaxX - m_BBMinX) / 5.0f);
		m_CollisionDevideZ = (int)((m_BBMaxZ - m_BBMinZ) / 5.0f);


		m_CollisionFace = new std::vector<COLLISION_FACE>[m_CollisionDevideX * m_CollisionDevideZ];


		for (unsigned int s = 0; s < model.SubsetNum; s++)
		{
			if (model.SubsetArray[s].Name[0] == '#')
			{
				for (unsigned int i = model.SubsetArray[s].StartIndex; i < model.SubsetArray[s].StartIndex + model.SubsetArray[s].IndexNum; i += 3)
					//for (unsigned int i = 0; i < model.IndexNum; i += 3)
				{
					COLLISION_FACE collisionFace;

					collisionFace.Position[0] = model.VertexArray[model.IndexArray[i + 0]].Position;
					collisionFace.Position[1] = model.VertexArray[model.IndexArray[i + 1]].Position;
					collisionFace.Position[2] = model.VertexArray[model.IndexArray[i + 2]].Position;

					Vector3 v01, v12, c;
					v01 = collisionFace.Position[1] - collisionFace.Position[0];
					v12 = collisionFace.Position[2] - collisionFace.Position[1];
					c = Vector3::Cross(v01, v12);

					//c = Vector3(0.0f, 1.0f, 0.0f);

					float l = c.Length();
					if (l > 0.0f)
					{
						c /= l;
						collisionFace.Normal = c;

						Vector3 position = (collisionFace.Position[0] + collisionFace.Position[1] + collisionFace.Position[2]) / 3.0f;
						int x, z;
						x = (int)((position.X - m_BBMinX) / (m_BBMaxX - m_BBMinX) * m_CollisionDevideX);
						z = (int)((position.Z - m_BBMinZ) / (m_BBMaxZ - m_BBMinZ) * m_CollisionDevideZ);

						assert(0 <= x && 0 <= z && x < m_CollisionDevideX&& z < m_CollisionDevideZ);

						m_CollisionFace[z + x * m_CollisionDevideZ].push_back(collisionFace);
					}
				}
			}
			else if (model.SubsetArray[s].Name[0] == '@')
			{
				Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

				for (unsigned int i = model.SubsetArray[s].StartIndex; i < model.SubsetArray[s].StartIndex + model.SubsetArray[s].IndexNum; i++)
				{
					position += model.VertexArray[model.IndexArray[i]].Position;
				}

				position /= (float)model.SubsetArray[s].IndexNum;

				m_PositionList.push_back(position);
			}

		}
	}






	{

		ComPtr<ID3D12Device> device = RenderManager::GetInstance()->GetDevice();

		HRESULT hr;
		D3D12_HEAP_PROPERTIES heapProperties{};
		D3D12_RESOURCE_DESC   resourceDesc{};

		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 0;
		heapProperties.VisibleNodeMask = 0;

		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;





		m_VertexNum = model.VertexNum;
		resourceDesc.Width = sizeof(VERTEX_3D) * m_VertexNum;
		hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_VertexBuffer));
		assert(SUCCEEDED(hr));

		VERTEX_3D* vertex;
		hr = m_VertexBuffer->Map(0, nullptr, (void**)&vertex);
		assert(SUCCEEDED(hr));

		memcpy(vertex, model.VertexArray, sizeof(VERTEX_3D) * m_VertexNum);

		m_VertexBuffer->Unmap(0, nullptr);





		m_IndexNum = model.IndexNum;
		resourceDesc.Width = sizeof(unsigned int) * m_IndexNum;
		hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_IndexBuffer));
		assert(SUCCEEDED(hr));

		unsigned int* index;
		hr = m_IndexBuffer->Map(0, nullptr, (void**)&index);
		assert(SUCCEEDED(hr));

		memcpy(index, model.IndexArray, sizeof(unsigned int) * m_IndexNum);

		m_IndexBuffer->Unmap(0, nullptr);
	}







	{
		std::unique_lock<std::mutex> lock(*RenderManager::GetInstance()->GetResourceCommandMutex());
		//RenderManager::GetInstance()->WaitGPU();

		//GameManager::GetInstance()->WaitDraw();
		//RenderManager::GetInstance()->ResourceCommandBegin();


		{
			m_SubsetArray.resize(model.SubsetNum);

			for (unsigned int i = 0; i < model.SubsetNum; i++)
			{
				strcpy(m_SubsetArray[i].Name, model.SubsetArray[i].Name);

				m_SubsetArray[i].StartIndex = model.SubsetArray[i].StartIndex;
				m_SubsetArray[i].IndexNum = model.SubsetArray[i].IndexNum;
				m_SubsetArray[i].Material.Material = model.SubsetArray[i].Material.Material;

				strcpy(m_SubsetArray[i].Material.Name, model.SubsetArray[i].Material.Name);

				m_SubsetArray[i].Material.TextureBaseColor = RenderManager::GetInstance()->LoadTexture((dir + model.SubsetArray[i].Material.TextureNameBaseColor).c_str());
				m_SubsetArray[i].Material.TextureSubColor = RenderManager::GetInstance()->LoadTexture((dir + model.SubsetArray[i].Material.TextureNameSubColor).c_str());
				//m_SubsetArray[i].Material.TextureARM = RenderManager::GetInstance()->LoadTexture((dir + model.SubsetArray[i].Material.TextureNameARM).c_str());
				//m_SubsetArray[i].Material.TextureNormal = RenderManager::GetInstance()->LoadTexture((dir + model.SubsetArray[i].Material.TextureNameNormal).c_str());


				if (strncmp(m_SubsetArray[i].Material.Name, "Tree", 4) == 0)
					m_SubsetArray[i].Material.Material.NormalWeight = 1.0f;
				else if (strncmp(m_SubsetArray[i].Material.Name, "Bush", 4) == 0)
					m_SubsetArray[i].Material.Material.NormalWeight = 1.0f;
				else
					m_SubsetArray[i].Material.Material.NormalWeight = 0.0f;




				{
					m_SubsetArray[i].ConstantBuffer = RenderManager::GetInstance()->CreateConstantBuffer(256);

					SUBSET_CONSTANT* constant;
					HRESULT hr = m_SubsetArray[i].ConstantBuffer->Resource->Map(0, nullptr, (void**)&constant);
					assert(SUCCEEDED(hr));

					constant->Material = m_SubsetArray[i].Material.Material;

					m_SubsetArray[i].ConstantBuffer->Resource->Unmap(0, nullptr);
				}

			}
		}

		//RenderManager::GetInstance()->ResourceCommandEnd();
		//GameManager::GetInstance()->StartDraw();

	}






	assert(model.VertexArray);
	assert(model.IndexArray);
	assert(model.SubsetArray);

	delete[] model.VertexArray;
	delete[] model.IndexArray;
	delete[] model.SubsetArray;


	m_Loaded = true;
}





void Model::Draw(bool OverrideTexture, std::unordered_map<std::string, MATERIAL>* OverridMaterial)
{
	if (!m_Loaded)
		return;



	ID3D12GraphicsCommandList* CommandList = RenderManager::GetInstance()->GetGraphicsCommandList();


	D3D12_VERTEX_BUFFER_VIEW vertexView{};
	vertexView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	vertexView.StrideInBytes = sizeof(VERTEX_3D);
	vertexView.SizeInBytes = sizeof(VERTEX_3D) * m_VertexNum;
	CommandList->IASetVertexBuffers(0, 1, &vertexView);



	D3D12_INDEX_BUFFER_VIEW indexView{};
	indexView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
	indexView.SizeInBytes = sizeof(unsigned int) * m_IndexNum;
	indexView.Format = DXGI_FORMAT_R32_UINT;
	CommandList->IASetIndexBuffer(&indexView);



	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	for( unsigned int i = 0; i < m_SubsetArray.size(); i++ )
	{
		if (m_SubsetArray[i].Name[0] != '@')
		if (m_SubsetArray[i].Material.Material.BaseColor.W == 1.0f)
		{

			if (OverridMaterial && (*OverridMaterial).count(m_SubsetArray[i].Material.Name))
			{
				RenderManager::GetInstance()->SetConstant(3, &(*OverridMaterial)[m_SubsetArray[i].Material.Name], sizeof(MATERIAL));
			}
			else
			{
				RenderManager::GetInstance()->SetGraphicsRootDescriptorTable(3, m_SubsetArray[i].ConstantBuffer->ShaderResourceView.Index);
			}



			if (!OverrideTexture)
			{
				RenderManager::GetInstance()->SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 2, m_SubsetArray[i].Material.TextureBaseColor->ShaderResourceView.Index);
				RenderManager::GetInstance()->SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 4, m_SubsetArray[i].Material.TextureSubColor->ShaderResourceView.Index);
			}


			CommandList->DrawIndexedInstanced(m_SubsetArray[i].IndexNum, 1, m_SubsetArray[i].StartIndex, 0, 0);
			RenderManager::GetInstance()->AddIndexCount(m_SubsetArray[i].IndexNum);
		}

	}



	for (unsigned int i = 0; i < m_SubsetArray.size(); i++)
	{
		if (m_SubsetArray[i].Name[0] != '@')
		if (m_SubsetArray[i].Material.Material.BaseColor.W != 1.0f)
		{

			if (OverridMaterial && (*OverridMaterial).count(m_SubsetArray[i].Material.Name))
			{
				RenderManager::GetInstance()->SetConstant(3, &(*OverridMaterial)[m_SubsetArray[i].Material.Name], sizeof(MATERIAL));
			}
			else
			{
				RenderManager::GetInstance()->SetGraphicsRootDescriptorTable(3, m_SubsetArray[i].ConstantBuffer->ShaderResourceView.Index);
			}




			if (!OverrideTexture)
			{
				RenderManager::GetInstance()->SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 2, m_SubsetArray[i].Material.TextureBaseColor->ShaderResourceView.Index);
			}


			CommandList->DrawIndexedInstanced(m_SubsetArray[i].IndexNum, 1, m_SubsetArray[i].StartIndex, 0, 0);
			RenderManager::GetInstance()->AddIndexCount(m_SubsetArray[i].IndexNum);
		}

	}

}





void Model::LoadObjBin(const char* FileName, MODEL* Model)
{
	FILE* file = fopen(FileName, "rb");
	assert(file);



	fread(&Model->VertexNum, sizeof(Model->VertexNum), 1, file);

	Model->VertexArray = new VERTEX_3D[Model->VertexNum]{};

	fread(Model->VertexArray, sizeof(VERTEX_3D), Model->VertexNum, file);




	fread(&Model->IndexNum, sizeof(Model->IndexNum), 1, file);

	Model->IndexArray = new unsigned int[Model->IndexNum]{};

	fread(Model->IndexArray, sizeof(unsigned int), Model->IndexNum, file);




	fread(&Model->SubsetNum, sizeof(Model->SubsetNum), 1, file);

	Model->SubsetArray = new MODEL_SUBSET[Model->SubsetNum]{};

	fread(Model->SubsetArray, sizeof(MODEL_SUBSET), Model->SubsetNum, file);



	fclose(file);

}



void Model::SaveObjBin(const char* FileName, MODEL* Model)
{
	FILE* file = fopen(FileName, "wb");
	assert(file);



	fwrite(&Model->VertexNum, sizeof(Model->VertexNum), 1, file);
	fwrite(Model->VertexArray, sizeof(VERTEX_3D), Model->VertexNum, file);


	fwrite(&Model->IndexNum, sizeof(Model->IndexNum), 1, file);
	fwrite(Model->IndexArray, sizeof(unsigned int), Model->IndexNum, file);


	fwrite(&Model->SubsetNum, sizeof(Model->SubsetNum), 1, file);
	fwrite(Model->SubsetArray, sizeof(MODEL_SUBSET), Model->SubsetNum, file);



	fclose(file);

}



void Model::LoadObj( const char *FileName, MODEL *Model )
{

	std::string dir(FileName);
	dir = dir.substr(0, dir.find_last_of("\\"));


	//Vector3		*positionArray{};
	Vector3		*normalArray{};
	Vector2		*texcoordArray{};
	Vector4		*colorArray{};

	unsigned int	positionNum = 0;
	unsigned int	normalNum = 0;
	unsigned int	texcoordNum = 0;
	unsigned int	colorNum = 0;

	//unsigned int	vertexNum = 0;
	unsigned int	indexNum = 0;
	unsigned int	in = 0;
	unsigned int	subsetNum = 0;

	MODEL_SUBSET_MATERIAL	*materialArray{};
	unsigned int	materialNum{};

	char str[256]{};
	char *s{};
	char c{};


	FILE *file = fopen( FileName, "rt" );
	assert(file);




	while( true )
	{
		(void)fscanf( file, "%s", str );

		if( feof( file ) != 0 )
			break;

		if( strcmp( str, "v" ) == 0 )
		{
			positionNum++;
		}
		else if( strcmp( str, "vn" ) == 0 )
		{
			normalNum++;
		}
		else if (strcmp(str, "vc") == 0)
		{
			colorNum++;
		}
		else if( strcmp( str, "vt" ) == 0 )
		{
			texcoordNum++;
		}
		else if( strcmp( str, "usemtl" ) == 0 )
		{
			subsetNum++;
		}
		else if( strcmp( str, "f" ) == 0 )
		{
			in = 0;

			do
			{
				(void)fscanf( file, "%s", str );
				//vertexNum++;
				in++;
				c = fgetc( file );
			}
			while( c != '\n' && c!= '\r' );


			//if( in == 4 )
			//	in = 6;

			indexNum += in;
		}
	}



	//positionArray = new Vector3[ positionNum ];
	normalArray = new Vector3[ normalNum ];
	texcoordArray = new Vector2[ texcoordNum ];
	colorArray = new Vector4[ colorNum ];


	Model->VertexArray = new VERTEX_3D[ positionNum ]{};
	Model->VertexNum = positionNum;

	Model->IndexArray = new unsigned int[ indexNum ]{};
	Model->IndexNum = indexNum;

	Model->SubsetArray = new MODEL_SUBSET[ subsetNum ]{};
	Model->SubsetNum = subsetNum;





	//Vector3* position = positionArray;
	Vector3* normal = normalArray;
	Vector2* texcoord = texcoordArray;
	Vector4* color = colorArray;

	unsigned int vc = 0;
	unsigned int ic = 0;
	unsigned int sc = 0;

	char objectName[256]{};


	fseek( file, 0, SEEK_SET );

	while( true )
	{
		(void)fscanf( file, "%s", str );

		if( feof( file ) != 0 )
			break;

		if( strcmp( str, "mtllib" ) == 0 )
		{

			(void)fscanf( file, "%s", str );

			char path[256]{};
			strcpy( path, dir.c_str() );
			strcat( path, "\\" );
			strcat( path, str );

			LoadMaterial( path, &materialArray, &materialNum );
		}
		else if( strcmp( str, "o" ) == 0 )
		{
			(void)fscanf( file, "%s", objectName);
		}
		else if( strcmp( str, "v" ) == 0 )
		{
			Vector3 position{};

			(void)fscanf( file, "%f", &position.X );
			(void)fscanf( file, "%f", &position.Y );
			(void)fscanf( file, "%f", &position.Z );
			position.X *= -1.0f;
			//*position *= 100.0f;
			//position++;

			Model->VertexArray[vc].Position = position;
			vc++;
		}
		else if( strcmp( str, "vn" ) == 0 )
		{
			(void)fscanf( file, "%f", &normal->X );
			(void)fscanf( file, "%f", &normal->Y );
			(void)fscanf( file, "%f", &normal->Z );
			normal->X *= -1.0f;
			normal++;
		}
		else if (strcmp(str, "vc") == 0)
		{
			(void)fscanf(file, "%f", &color->X);
			(void)fscanf(file, "%f", &color->Y);
			(void)fscanf(file, "%f", &color->Z);
			(void)fscanf(file, "%f", &color->W);
			color++;
		}

		else if( strcmp( str, "vt" ) == 0 )
		{
			(void)fscanf( file, "%f", &texcoord->X );
			(void)fscanf( file, "%f", &texcoord->Y );
			texcoord->Y = 1.0f - texcoord->Y;
			texcoord++;
		}
		else if( strcmp( str, "usemtl" ) == 0 )
		{
			(void)fscanf( file, "%s", str );

			strcpy(Model->SubsetArray[sc].Name, objectName);

			if( sc != 0 )
				Model->SubsetArray[ sc - 1 ].IndexNum = ic - Model->SubsetArray[ sc - 1 ].StartIndex;

			Model->SubsetArray[ sc ].StartIndex = ic;


			for( unsigned int i = 0; i < materialNum; i++ )
			{
				if( strcmp( str, materialArray[i].Name ) == 0 )
				{
					Model->SubsetArray[ sc ].Material = materialArray[i];
					break;
				}
			}

			sc++;
			
		}
		else if( strcmp( str, "f" ) == 0 )
		{
			in = 0;

			do
			{
				(void)fscanf( file, "%s", str );

				s = strtok( str, "/" );	
				int idx = atoi( s ) - 1;

				if( s[ strlen( s ) + 1 ] != '/' )
				{
					s = strtok( NULL, "/" );
					Model->VertexArray[idx].TexCoord = texcoordArray[ atoi( s ) - 1 ];
				}

				s = strtok( NULL, "/" );	
				Model->VertexArray[idx].Normal = normalArray[ atoi( s ) - 1 ];

				s = strtok( NULL, "/" );
				if (s)
					Model->VertexArray[idx].Color = colorArray[atoi(s) - 1];
					//Model->VertexArray[vc].Occlusion = colorArray[atoi(s) - 1].X;
				else
					Model->VertexArray[idx].Color = { 1.0f, 1.0f, 1.0f, 1.0f };
					//Model->VertexArray[vc].Occlusion = 1.0f;


				Model->IndexArray[ic] = idx;
				ic++;
				//vc++;

				in++;
				c = fgetc( file );
			}
			while( c != '\n' && c != '\r' );

			std::swap(Model->IndexArray[ic - in], Model->IndexArray[ic - in + 1]);
			/*
			if( in == 4 )
			{
				Model->IndexArray[ic] = vc - 2;
				ic++;
				Model->IndexArray[ic] = vc - 4;
				ic++;
			}*/
		}
	}


	if( sc != 0 )
		Model->SubsetArray[ sc - 1 ].IndexNum = ic - Model->SubsetArray[ sc - 1 ].StartIndex;


	fclose(file);


	//assert(positionArray);
	assert(normalArray);
	assert(texcoordArray);
	assert(colorArray);

	//delete[] positionArray;
	delete[] normalArray;
	delete[] texcoordArray;
	delete[] colorArray;



	assert(materialArray);

	delete[] materialArray;
}




void Model::LoadMaterial( const char *FileName, MODEL_SUBSET_MATERIAL **MaterialArray, unsigned int *MaterialNum )
{


	char str[256]{};

	FILE *file  = fopen( FileName, "rt" );
	assert(file);

	MODEL_SUBSET_MATERIAL* materialArray{};
	unsigned int materialNum = 0;

	while( true )
	{
		(void)fscanf( file, "%s", str );

		if( feof( file ) != 0 )
			break;

		if( strcmp( str, "newmtl" ) == 0 )
		{
			materialNum++;
		}
	}


	materialArray = new MODEL_SUBSET_MATERIAL[ materialNum ]{};


	int mc = -1;

	fseek( file, 0, SEEK_SET );

	while( true )
	{
		(void)fscanf( file, "%s", str );

		if( feof( file ) != 0 )
			break;


		if( strcmp( str, "newmtl" ) == 0 )
		{
			mc++;
			(void)fscanf( file, "%s", materialArray[ mc ].Name );
			//strcpy( materialArray[ mc ].TextureName, "" );
			//materialArray[mc].TextureBaseColor.ViewIndex = 0;
			//materialArray[mc].TextureARM.ViewIndex = 0;
			//materialArray[mc].TextureNormal.ViewIndex = 0;

			strcpy(materialArray[mc].TextureNameBaseColor, "");
			strcpy(materialArray[mc].TextureNameSubColor, "");
/*
			strcpy(materialArray[mc].TextureNameARM, "");
			strcpy(materialArray[mc].TextureNameNormal, "");
*/

		}
		else if( strcmp( str, "Ka" ) == 0 )
		{
			float ambient;
			(void)fscanf(file, "%f", &ambient);
			(void)fscanf(file, "%f", &ambient);
			(void)fscanf(file, "%f", &ambient);
		}
		else if( strcmp( str, "Kd" ) == 0 )
		{
			(void)fscanf( file, "%f", &materialArray[ mc ].Material.BaseColor.X );
			(void)fscanf( file, "%f", &materialArray[ mc ].Material.BaseColor.Y );
			(void)fscanf( file, "%f", &materialArray[ mc ].Material.BaseColor.Z );
			materialArray[ mc ].Material.BaseColor.W = 1.0f;
		}
		else if (strcmp(str, "Ke") == 0)
		{
			(void)fscanf(file, "%f", &materialArray[mc].Material.EmissionColor.X);
			(void)fscanf(file, "%f", &materialArray[mc].Material.EmissionColor.Y);
			(void)fscanf(file, "%f", &materialArray[mc].Material.EmissionColor.Z);
			materialArray[mc].Material.EmissionColor.W = 1.0f;
		}
		else if( strcmp( str, "Ks" ) == 0 )
		{
			float specular;
			(void)fscanf( file, "%f", &specular );
			(void)fscanf( file, "%f", &specular );
			(void)fscanf( file, "%f", &specular );

			materialArray[mc].Material.Specular = specular;
		}
		else if( strcmp( str, "Ns" ) == 0 )
		{
			float shininess;
			(void)fscanf( file, "%f", &shininess);
		}
		else if( strcmp( str, "d" ) == 0 )
		{
			(void)fscanf( file, "%f", &materialArray[ mc ].Material.BaseColor.W );
		}
		else if (strcmp(str, "Metallic") == 0)
		{
			(void)fscanf(file, "%f", &materialArray[mc].Material.Metallic);
		}
		else if (strcmp(str, "Roughness") == 0)
		{
			(void)fscanf(file, "%f", &materialArray[mc].Material.Roughness);
		}
		else if( strcmp( str, "map_Kd" ) == 0 )
		{
			(void)fscanf( file, "%s", str );
			strcat( materialArray[ mc ].TextureNameBaseColor, str);
		}
		else if (strcmp(str, "map_Ns") == 0)
		{
			(void)fscanf(file, "%s", str);
			strcat(materialArray[mc].TextureNameSubColor, str);
		}/*
		else if (strcmp(str, "refl") == 0)
		{
			(void)fscanf(file, "%s", str);
			strcat( materialArray[ mc ].TextureNameARM, str);
		}
		else if (strcmp(str, "map_Bump") == 0)
		{
			(void)fscanf(file, "%s", str);
			strcat( materialArray[ mc ].TextureNameNormal, str);
		}*/
	}

	fclose(file);


	*MaterialArray = materialArray;
	*MaterialNum = materialNum;

}



bool Model::RayHitCheck(Vector3 position, Vector3 direction, Vector3* hitPosition, Vector3* hitNormal)
{
	if (!m_Loaded)
		return false;


	int px = (int)((position.X - m_BBMinX) / (m_BBMaxX - m_BBMinX) * m_CollisionDevideX);
	int pz = (int)((position.Z - m_BBMinZ) / (m_BBMaxZ - m_BBMinZ) * m_CollisionDevideZ);


	float mint = 1.0f;
	bool hit = false;

	for (int x = px - 1; x < px + 2; x++)
	{
		if (x < 0 || m_CollisionDevideX <= x)
			continue;

		for (int z = pz - 1; z < pz + 2; z++)
		{
			if (z < 0 || m_CollisionDevideZ <= z)
				continue;

			int size = (int)m_CollisionFace[z + x * m_CollisionDevideZ].size();

			for (int i = 0; i < size; i++)
			{
				COLLISION_FACE* face = &m_CollisionFace[z + x * m_CollisionDevideZ][i];

				Vector3 vp0;
				float dnp, dnd, t;

				vp0 = face->Position[0] - position;
				dnp = Vector3::Dot(face->Normal, vp0);
				dnd = Vector3::Dot(face->Normal, direction);

				if (dnd >= 0.0f)
					continue;

				t = dnp / dnd;

				if (t < 0.0f || 1.0f < t)
					continue;


				Vector3 hp;

				hp = position + direction * t;


				Vector3 vph, vpl, c0, c1, c2;

				vph = hp - face->Position[0];
				vpl = face->Position[1] - face->Position[0];
				c2 = Vector3::Cross(vpl, vph);

				if (Vector3::Dot(face->Normal, c2) < 0.0f)
					continue;


				vph = hp - face->Position[1];
				vpl = face->Position[2] - face->Position[1];
				c0 = Vector3::Cross(vpl, vph);

				if (Vector3::Dot(face->Normal, c0) < 0.0f)
					continue;


				vph = hp - face->Position[2];
				vpl = face->Position[0] - face->Position[2];
				c1 = Vector3::Cross(vpl, vph);

				if (Vector3::Dot(face->Normal, c1) < 0.0f)
					continue;


				*hitPosition = hp;
				*hitNormal = face->Normal;
				hit = true;

				return true;
			}
		}
	}


	return false;
}

