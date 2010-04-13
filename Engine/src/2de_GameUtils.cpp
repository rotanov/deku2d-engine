#include "2de_GameUtils.h"
#include "2de_Engine.h"

//-------------------------------------------//
//			CTileSet functions				 //
//-------------------------------------------//

bool CTileset::LoadFromFile()
{
	CFactory *Factory = CFactory::Instance();
	CFile file;
	if (!file.Open(filename, CFile::OPEN_MODE_READ))
	{
		Log.Log("ERROR", "Can't open TileSet file %s", filename.c_str());
		return false;
	}

	char * TextureName = NULL;
	file.ReadLine(TextureName);
	//Texture = dynamic_cast<CTexture*>((dynamic_cast<CTextureManager*>(Factory->GetManager(MANAGER_TYPE_TEX)))->GetObject(TextureName));
	Texture = CTextureManager::Instance()->GetTextureByName(TextureName);

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
		p.grSegment(Vector2(i*TileWidth, 0.0f),
			Vector2(i*TileWidth, Texture->height));
	for (int i = 0; i <= VerNumTiles; i++)
		p.grSegment(Vector2(0.0f, i*TileHeight),
			Vector2(Texture->width, i*TileHeight));
}

bool CTileset::SaveToFile()
{
	CFile file;
	if (!file.Open(filename, CFile::OPEN_MODE_WRITE))
	{
		Log.Log("ERROR", "Can't open TileSet file %s", filename.c_str());
		return false;
	}

	file.Write(Texture->GetName(), strlen(Texture->GetName())+1);
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
	CTileSetManager::Instance()->AddObject(this);
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
	assert(Texture->width != 0 && Texture->height != 0);

	Vector2 t;
	Vector2Array<4> tc;
	t.x = CellIndex % HorNumTiles;
	t.y = CellIndex / HorNumTiles;
	for (int i = 0; i < 4; i++)
	{
		tc[i] = (t + V2_QuadBin[i]);
		tc[i].x *= (float)TileWidth/Texture->width;
		tc[i].y *= (float)TileHeight/Texture->height;
	}
	return tc;
}

//-------------------------------------------//
//				CMap functions				 //
//-------------------------------------------//
	

bool CLevelMap::Render()
{
// 	if (!loaded) And what if we can't load it? Write it to log every frame?
// 		LoadFromFile();
	
	CMapCellInfo *t;
	
	glPushAttrib(GL_TEXTURE_BIT | GL_BLEND);
	glEnable(GL_TEXTURE);
	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	TileSet->GetTexture()->Bind();
	glBegin(GL_QUADS);
	for(int i=0;i<numCellsVer;i++)
		for(int j=0;j<numCellsHor;j++)
		{
			t = Cells + _Cell(j, i);

			for(int k=0;k<4;k++)
			{
				t->tc[k].glTexCoord();
				glVertex3f(t->pos[k].x,	t->pos[k].y, t->z);
			}
		}
	glEnd();
	glPopAttrib();
	return true;
}

bool CLevelMap::LoadFromFile()
{
	CFile File;
	CFactory *Factory = CFactory::Instance();
	if (!File.Open(filename, CFile::OPEN_MODE_READ))
	{
		Log.Log("ERROR", "Can't load level %s", filename.c_str());
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
		Log.Log("Error", "Tileset %s for map %s not found in resources", GetName(), TileSetName);
		return false;
	}
	//TileSet->Texture->Load();

	File.Read(&numCellsHor, sizeof(numCellsHor));
	File.Read(&numCellsVer, sizeof(numCellsVer));
	if (Cells != NULL)
		delete [] Cells;
	Cells = new CMapCellInfo [numCellsVer*numCellsHor];
	File.Read(Cells, numCellsVer*numCellsHor*sizeof(CMapCellInfo));

	GenCells();

	File.Close();
	loaded = true;
	return true;
}


bool CLevelMap::SaveToFile()
{
	CFile file;
	if (!file.Open(filename, CFile::OPEN_MODE_WRITE))
	{
		Log.Log("ERROR", "Can't open file %s to save the map", filename.c_str());
		return false;
	}
	
	file.Write(TileSet->GetName(), strlen(TileSet->GetName())+1);
	file.Write(&numCellsHor, sizeof(numCellsHor));
	file.Write(&numCellsVer, sizeof(numCellsVer));
	file.Write(Cells, sizeof(CMapCellInfo)*numCellsHor*numCellsVer);
	file.Close();
	
	return true;
}

bool CLevelMap::GenCells()
{
	for(int i=0;i<numCellsVer;i++)
		for(int j=0;j<numCellsHor;j++)
		{
			CMapCellInfo *t = &(Cells[_Cell(j, i)]);
			Vector2 ji = Vector2(j, i);
			scalar w = TileSet->TileWidth;
			scalar h = TileSet->TileHeight;
			for(int k = 0; k < 4; k++)
			{
				t->pos[k] = (ji + V2_QuadBin[k]);
				t->pos[k].x *= w;
				t->pos[k].y *= h;
			}
			t->tc = TileSet->GetCellTC(t->index);
		}
	return true;
}

CLevelMap::CLevelMap(int AnumCellsHor, int AnumCellsVer, const string &ATilesetName, const string &AName) : numCellsHor(AnumCellsHor), numCellsVer(AnumCellsVer)
{
	TileSet = 	CFactory::Instance()->Get<CTileset>(ATilesetName);
	if (TileSet != NULL)
		TileSet->CheckLoad();
	Cells = new CMapCellInfo [numCellsVer * numCellsHor];
	SetName(AName);
}

bool CCompas::Render()
{
		Vector2 v1, n;
		CEngine *Ninja = CEngine::Instance();
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
		pr.grCircleL(Vector2(100,100), depth);;
		pr.grSegment(Vector2(100, 100), (Vector2(100, 100) + n*depth));

//  		pr.grCircleL(Vector2(100,100), depth, RGBAf(depth/ (90.0f * 2), 0.0f, 0.0f, 0.9f));
//  		pr.grSegment(Vector2(100, 100), (Vector2(100, 100) + n*depth), 1.0f, RGBAf(depth/( 90.0f * 2), 0.0f, 0.0f, 0.9f));

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		return true;
}

CCompas::CCompas()
{
	SetName("CCompas");
	CEngine *Ninja = CEngine::Instance();
	CRenderManager::Instance()->AddObject(this);
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
