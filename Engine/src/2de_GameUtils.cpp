#include "2de_GameUtils.h"

//-------------------------------------------//
//			CTileSet functions				 //
//-------------------------------------------//

bool CTileset::LoadFromFile()
{
	CFactory *Factory = CFactory::Instance();
	CFile file;
	if (!file.Open(Filename, CFile::OPEN_MODE_READ))
	{
		Log("ERROR", "Can't open TileSet file %s", Filename.c_str());
		return false;
	}

	char * TextureName = NULL;
	file.ReadLine(TextureName);
	//Texture = dynamic_cast<CTexture*>((dynamic_cast<CTextureManager*>(Factory->GetManager(MANAGER_TYPE_TEX)))->GetObject(TextureName));
	Texture = CTextureManager::Instance()->Get(TextureName);
	Texture->CheckLoad();

	file.Read(&TileWidth, sizeof(TileWidth));
	file.Read(&TileHeight, sizeof(TileHeight));
	file.Read(&HorNumTiles, sizeof(HorNumTiles));
	file.Read(&VerNumTiles, sizeof(VerNumTiles));

	if (BBox != NULL)
		delete [] BBox;
	BBox = new CAABB[HorNumTiles*VerNumTiles];
	file.Read(BBox, sizeof(CAABB)*HorNumTiles*VerNumTiles);

	file.Close();

	return true;
}

void CTileset::RenderTileSet()
{
	// encapsulate Opengl calls
	glEnable(GL_TEXTURE_2D);
	Texture->Bind();
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(0, 0);
		glTexCoord2f(1, 0); glVertex2f(TileWidth*HorNumTiles, 0);
		glTexCoord2f(1, 1); glVertex2f(TileWidth*HorNumTiles, TileHeight*VerNumTiles);
		glTexCoord2f(0, 1); glVertex2f(0, TileHeight*VerNumTiles);
	glEnd();

	CPrimitiveRender p;
	p.doUseGlobalCoordSystem = false;
	p.BlendingOption = PRM_RNDR_OPT_BLEND_NO_ONE;
	p.lClr = RGBAf(0.0f, 0.0f, 0.0f, 1.0f);
	p.pClr = RGBAf(0.0f, 0.0f, 0.0f, 1.0f);
	p.lwidth = 0.1f;
	
	for (int i = 0; i <= HorNumTiles; i++)
		p.grSegment(Vector2(i*TileWidth, 0),
			Vector2(i*TileWidth, Texture->Height));
	for (int i = 0; i <= VerNumTiles; i++)
		p.grSegment(Vector2(0, i*TileHeight),
			Vector2(Texture->Width, i*TileHeight));
}

bool CTileset::SaveToFile()
{
	CFile file;
	if (!file.Open(Filename, CFile::OPEN_MODE_WRITE))
	{
		Log("ERROR", "Can't open TileSet file %s", Filename.c_str());
		return false;
	}

	file.Write(Texture->GetName().c_str(), Texture->GetName().length() + 1);
	file.Write(&TileWidth, sizeof(TileWidth));
	file.Write(&TileHeight, sizeof(TileHeight));
	file.Write(&HorNumTiles, sizeof(HorNumTiles));
	file.Write(&VerNumTiles, sizeof(VerNumTiles));

	file.Write(BBox, sizeof(CAABB)*HorNumTiles*VerNumTiles);

	file.Close();

	return true;
}

CTileset::CTileset()
{
	BBox		=	NULL;
	Texture		=	NULL;
	TileWidth	=	0;
	TileHeight	=	0;
	HorNumTiles	=	0;
	VerNumTiles	=	0;
	CTileSetManager::Instance()->Add(this);
}

void CTileset::SetSettings( byte _TileWidth, byte _TileHeight, int _HorNumTiles, int _VerNumTiles)
{
	HorNumTiles = _HorNumTiles;
	VerNumTiles = _VerNumTiles;
	TileWidth = _TileWidth;
	TileHeight = _TileHeight;
	if (BBox != NULL)
		delete [] BBox;
	BBox = new CAABB [HorNumTiles*VerNumTiles];
	for(int i=0; i<HorNumTiles*VerNumTiles; i++)
		BBox[i] = CAABB(0, 0, TileHeight, TileWidth);
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

CTileset::~CTileset()
{
	if (BBox != NULL)
		delete [] BBox;

	CTileSetManager::Instance()->Remove(GetID());
}
//-------------------------------------------//
//				CMap functions				 //
//-------------------------------------------//
	

void CLevelMap::Render()
{
	CMapCellInfo *t;
	COLOR_WHITE.glSet();
	glEnable(GL_TEXTURE_2D);
	TileSet->GetTexture()->Bind();
	glBegin(GL_QUADS);
	for(int j = 0; j < HorizontalCellsCount; j++)
		for(int i = 0; i < VerticalCellsCount; i++)		
		{
			t = Cells + GetCellIndex(j, i);

			for(int k = 0; k < 4; k++)
			{
				t->tc[k].glTexCoord();
				glVertex3f(t->pos[k].x,	t->pos[k].y, t->z);
			}
		}
	glEnd();
	glPopAttrib();
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

	char *TileSetName = NULL;
	File.ReadLine(TileSetName);
	if (TileSetName == NULL)
		return false;
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
	for(int i=0;i<VerticalCellsCount;i++)
		for(int j=0;j<HorizontalCellsCount;j++)
		{
			CMapCellInfo *t = &(Cells[GetCellIndex(j, i)]);
			Vector2 ji = Vector2(j, i);
			float w = TileSet->TileWidth;
			float h = TileSet->TileHeight;
			for(int k = 0; k < 4; k++)
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

CAABB CLevelMap::GetCellAABB(const Vector2 &V)
{
	size_t CellWidth = TileSet->TileWidth * GetScaling();
	size_t CellHeight = TileSet->TileWidth * GetScaling();
	if (GetMapCell((int)V.x / CellWidth, (int)V.y / CellHeight )->index != 0)
		return CAABB( (((int)V.x / CellWidth)) * CellWidth,
		(((int)V.y/CellHeight)) * CellHeight, 
		CellWidth, CellHeight);
	else 
		return CAABB(0.0f, 0.0f, 0.0f, 0.0f);
		//throw std::runtime_error("Outside of a map");
}

CMapCellInfo* CLevelMap::GetMapCell(size_t HorizontalIndex, size_t VerticalIndex)
{
	HorizontalIndex = Clamp<size_t>(HorizontalIndex, 0, HorizontalCellsCount-1);
	VerticalIndex = Clamp<size_t>(VerticalIndex, 0, VerticalCellsCount-1);
	return &Cells[GetCellIndex(HorizontalIndex, VerticalIndex)];
}
void CCompas::Render()
{
		Vector2 v1, n;
		//CEngine *Ninja = CEngine::Instance();
		v1 = CRenderManager::Instance()->Camera.Point;
		n = (-v1).Normalized();
		float depth = v1.Length()/40.0f;
		depth = Clamp(depth, 0.0f, 90.0f);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glEnable(GL_LINE_WIDTH);
		glLineWidth(3.0f);
		CPrimitiveRender pr;
		pr.doUseGlobalCoordSystem = true;
		pr.plClr = &RGBAf(0.6f, 0.9f, 0.7f, 0.9f);
		pr.grCircleL(Vector2(100,100), depth);
		pr.grSegment(Vector2(100, 100), (Vector2(100, 100) + n*depth));

//  		pr.grCircleL(Vector2(100,100), depth, RGBAf(depth/ (90.0f * 2), 0.0f, 0.0f, 0.9f));
//  		pr.grSegment(Vector2(100, 100), (Vector2(100, 100) + n*depth), 1.0f, RGBAf(depth/( 90.0f * 2), 0.0f, 0.0f, 0.9f));

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
