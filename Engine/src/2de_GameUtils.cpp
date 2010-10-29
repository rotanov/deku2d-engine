#include "2de_GameUtils.h"

#include "2de_GraphicsHigh.h"

//////////////////////////////////////////////////////////////////////////
// CTileset

CTileset::CTileset() : TileWidth(0), TileHeight(0), HorNumTiles(0), VerNumTiles(0), BBox(NULL), Texture(NULL)
{
	CTileSetManager::Instance()->Add(this);
}

CTileset::~CTileset()
{
	Unload();
	CTileSetManager::Instance()->Remove(this);
}

bool CTileset::Load()
{
	if (Loaded)
		return true;

	CStorage *storage;

	if (Source == LOAD_SOURCE_FILE)
	{
		storage = new CFile(Filename, CStorage::OPEN_MODE_READ);
		if (!storage->Good())
		{
			Log("ERROR", "Can't open TileSet file '%s'", Filename.c_str());
			return false;
		}
	}
	else if (Source == LOAD_SOURCE_MEMORY)
	{
		storage = new CMemory(MemoryLoadData, MemoryLoadLength, CStorage::OPEN_MODE_READ);
		if (!storage->Good())
		{
			Log("ERROR", "Can't open TileSet from memory storage");
			return false;
		}
	}

	string TextureName;
	storage->ReadString(TextureName);

	Texture = CTextureManager::Instance()->Get(TextureName);
	Texture->CheckLoad();

	storage->Read(&TileWidth, sizeof(TileWidth));
	storage->Read(&TileHeight, sizeof(TileHeight));
	storage->Read(&HorNumTiles, sizeof(HorNumTiles));
	storage->Read(&VerNumTiles, sizeof(VerNumTiles));

	if (BBox != NULL)
		delete [] BBox;

	BBox = new CBox[HorNumTiles * VerNumTiles];
	storage->Read(BBox, sizeof(CBox) * HorNumTiles * VerNumTiles);

	storage->Close();
	delete storage;

	CResource::Load();

	return true;
}

void CTileset::Unload()
{
	if (!Loaded)
		return;

	if (BBox != NULL)
	{
		delete[] BBox;
		BBox = NULL;
	}

	CResource::Unload();
}

bool CTileset::SaveToFile(const string &AFilename)
{
	CFile file;
	if (!file.Open(AFilename, CFile::OPEN_MODE_WRITE))
	{
		Log("ERROR", "Can't open TileSet file %s", Filename.c_str());
		return false;
	}

	file.Write(Texture->GetName().c_str(), Texture->GetName().length() + 1);
	file.Write(&TileWidth, sizeof(TileWidth));
	file.Write(&TileHeight, sizeof(TileHeight));
	file.Write(&HorNumTiles, sizeof(HorNumTiles));
	file.Write(&VerNumTiles, sizeof(VerNumTiles));
	file.Write(BBox, sizeof(CBox) * HorNumTiles * VerNumTiles);
	file.Close();

	return true;
}

void CTileset::SetSettings(byte ATileWidth, byte ATileHeight, int AHorNumTiles, int AVerNumTiles)
{
	HorNumTiles = AHorNumTiles;
	VerNumTiles = AVerNumTiles;
	TileWidth = ATileWidth;
	TileHeight = ATileHeight;
	if (BBox != NULL)
		delete [] BBox;
	BBox = new CBox [HorNumTiles * VerNumTiles];
	for (int i = 0; i < HorNumTiles * VerNumTiles; i++)
		BBox[i] = CBox(0, 0, TileHeight, TileWidth);
}

Vector2Array<4> CTileset::GetCellTC(int CellIndex)
{
	assert(Texture->Width != 0 && Texture->Height != 0);

	Vector2 t;
	Vector2Array<4> tc;
	t.x = CellIndex % HorNumTiles;
	t.y = CellIndex / HorNumTiles;
	for (int i = 0; i < 4; i++)
	{
		tc[i] = (t + V2_QuadBin[i]);
		tc[i].x *= (float)TileWidth/Texture->Width;
		tc[i].y *= (float)TileHeight/Texture->Height;
	}
	return tc;
}

void CTileset::RenderTileSet()
{
	CRenderConfig TempInfo;
	TempInfo.SetLayer(-1);
	TempInfo.doIgnoreCamera = true;
	TempInfo.Color = COLOR_WHITE;
	CRenderManager *RenderManager = CRenderManager::Instance();
	RenderManager->DrawTexturedBox(&TempInfo, CBox(V2_ZERO, Vector2(TileWidth*HorNumTiles, TileHeight*VerNumTiles)), Texture, V2_QuadBin);
	TempInfo.SetLayer(0);
	
	for (int i = 0; i <= HorNumTiles; i++)
		RenderManager->DrawLine(&TempInfo,	Vector2(i*TileWidth, 0),
											Vector2(i*TileWidth, Texture->Height));
	for (int i = 0; i <= VerNumTiles; i++)
		RenderManager->DrawLine(&TempInfo,	Vector2(0, i*TileHeight),
											Vector2(Texture->Width, i*TileHeight));
}

//////////////////////////////////////////////////////////////////////////
// CLevelMap

void CLevelMap::Render()
{
	CRenderManager *RenerManager = CRenderManager::Instance();
	CMapCellInfo *t;
	for (int j = 0; j < HorizontalCellsCount; j++)
		for (int i = 0; i < VerticalCellsCount; i++)		
		{
			t = Cells + GetCellIndex(j, i);
// 			for (int k = 0; k < 4; k++)
// 			{
// 				//t->tc[k].glTexCoord();
// 				glVertex3f(t->pos[k].x,	t->pos[k].y, t->z);
// 			}
			RenerManager->DrawTexturedBox(this, CBox(t->pos[0], t->pos[2]), TileSet->GetTexture(), t->tc);
		}
}

/*
bool CLevelMap::LoadFromFile()
{
	CFile File;
	CFactory *Factory = CFactory::Instance();
	if (!File.Open(Filename, CFile::OPEN_MODE_READ))
	{
		Log("ERROR", "Can't load level %s", Filename.c_str());
		return false;
	}

	string TileSetName;
	File.ReadString(TileSetName);
	TileSet = NULL;
	TileSet = Factory->Get<CTileset>(TileSetName);
	if (TileSet == NULL)
	{
		Log("Error", "Tileset %s for map %s not found in resources", GetName(), TileSetName);
		return false;
	}
	//TileSet->Texture->Load();

	File.Read(&HorizontalCellsCount, sizeof(HorizontalCellsCount));
	File.Read(&VerticalCellsCount, sizeof(VerticalCellsCount));
	if (Cells != NULL)
		delete [] Cells;
	Cells = new CMapCellInfo [VerticalCellsCount*HorizontalCellsCount];
	File.Read(Cells, VerticalCellsCount*HorizontalCellsCount*sizeof(CMapCellInfo));

	GenCells();

	File.Close();
	loaded = true;
	return true;
}


bool CLevelMap::SaveToFile()
{
	CFile file;
	if (!file.Open(Filename, CFile::OPEN_MODE_WRITE))
	{
		Log("ERROR", "Can't open file %s to save the map", Filename.c_str());
		return false;
	}
	
	file.Write(TileSet->GetName(), strlen(TileSet->GetName())+1);
	file.Write(&HorizontalCellsCount, sizeof(HorizontalCellsCount));
	file.Write(&VerticalCellsCount, sizeof(VerticalCellsCount));
	file.Write(Cells, sizeof(CMapCellInfo)*HorizontalCellsCount*VerticalCellsCount);
	file.Close();
	
	return true;
}*/


bool CLevelMap::GenCells()
{
	for (int i=0; i < VerticalCellsCount; i++)
		for (int j=0; j < HorizontalCellsCount; j++)
		{
			CMapCellInfo *t = &(Cells[GetCellIndex(j, i)]);
			Vector2 ji = Vector2(j, i);
			float w = TileSet->TileWidth;
			float h = TileSet->TileHeight;
			for (int k = 0; k < 4; k++)
			{
				t->pos[k] = (ji + V2_QuadBin[k]);
				t->pos[k].x *= w;
				t->pos[k].y *= h;
				t->z = GetDepth();
			}
			t->tc = TileSet->GetCellTC(t->index);
		}
	return true;
}

CLevelMap::CLevelMap(size_t AHorizontalCellsCount, size_t AVerticalCellsCount,
					 const string &ATilesetName, const string &AName) :
HorizontalCellsCount(AHorizontalCellsCount), VerticalCellsCount(AVerticalCellsCount)
{
	TileSet = CFactory::Instance()->Get<CTileset>(ATilesetName);
	if (TileSet != NULL)
		TileSet->CheckLoad();
	Cells = new CMapCellInfo [VerticalCellsCount * HorizontalCellsCount];
	SetName(AName);
}

CLevelMap::CLevelMap() : HorizontalCellsCount(0), VerticalCellsCount(0),
Cells(NULL), TileSet(NULL)
{

}

CLevelMap::~CLevelMap()
{
	SAFE_DELETE(Cells);
}

int CLevelMap::GetCellIndex(int h, int v)
{
	return v * HorizontalCellsCount + h;
}

CBox CLevelMap::GetCellAABB(const Vector2 &V)
{
	unsigned int CellWidth = TileSet->TileWidth * GetScaling();
	unsigned int CellHeight = TileSet->TileWidth * GetScaling();
	if (GetMapCell((int)V.x / CellWidth, (int)V.y / CellHeight)->index != 0)
		return CBox((((int)V.x / CellWidth)) * CellWidth,
		(((int)V.y/CellHeight)) * CellHeight, 
		CellWidth, CellHeight);
	else 
		return CBox(0.0f, 0.0f, 0.0f, 0.0f);
		//throw std::runtime_error("Outside of a map");
}

CMapCellInfo* CLevelMap::GetMapCell(size_t HorizontalIndex, size_t VerticalIndex)
{
	HorizontalIndex = Clamp<size_t>(HorizontalIndex, 0, HorizontalCellsCount-1);
	VerticalIndex = Clamp<size_t>(VerticalIndex, 0, VerticalCellsCount-1);
	return &Cells[GetCellIndex(HorizontalIndex, VerticalIndex)];
}

//////////////////////////////////////////////////////////////////////////
// CCompas

void CCompas::Render()
{
		Vector2 v1, n;
		//CEngine *Ninja = CEngine::Instance();
		v1 = CRenderManager::Instance()->Camera.Point;
		n = (-v1).Normalized();
		float depth = v1.Length()/40.0f;
		depth = Clamp(depth, 0.0f, 90.0f);
// 		pr.doUseGlobalCoordSystem = true;
// 		pr.plClr = &RGBAf(0.6f, 0.9f, 0.7f, 0.9f); // warning: taking address of temporary
// 		pr.lClr = RGBAf(0.6f, 0.9f, 0.7f, 0.9f); // <- replaced by this to fix it.. don't know, is it correct..
// 		pr.grCircleL(Vector2(100,100), depth);
		CRenderManager::Instance()->DrawLine(this, Vector2(100, 100), (Vector2(100, 100) + n*depth));
// 		pr.grCircleL(Vector2(100,100), depth, RGBAf(depth/ (90.0f * 2), 0.0f, 0.0f, 0.9f));
// 		pr.grSegment(Vector2(100, 100), (Vector2(100, 100) + n*depth), 1.0f, RGBAf(depth/(90.0f * 2), 0.0f, 0.0f, 0.9f));
}

CCompas::CCompas()
{
	SetName("CCompas");
	//CEngine *Ninja = CEngine::Instance();
}

CCompas::~CCompas()
{
	
}

//////////////////////////////////////////////////////////////////////////
// CTileSetManager

CTileSetManager::CTileSetManager()
{
	SetName("Tileset manager");
}
