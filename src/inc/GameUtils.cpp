#include "GameUtils.h"
#include "Engine.h"

//-------------------------------------------//
//			CTileSet functions				 //
//-------------------------------------------//

bool CTileSet::LoadFromFile()
{
	CFactory *Factory = CFactory::Instance();
	CFile file;
	if (!file.Open(filename.c_str(), CFILE_READ))
	{
		Log("ERROR", "Can't open TileSet file %s", filename);
		return false;
	}

	char * TextureName = NULL;
	file.ReadLine(TextureName);
	//Texture = dynamic_cast<CTexture*>((dynamic_cast<CTextureManager*>(Factory->GetManager(MANAGER_TYPE_TEX)))->GetObject(TextureName));
	Texture = CEngine::Instance()->TextureManager->GetTextureByName(TextureName);

	file.Read(&Info, sizeof(Info));
	if (BBox != NULL)
		delete [] BBox;
	file.Read(BBox, sizeof(CAABB)*Info.HorNumTiles*Info.VerNumTiles);

	file.Close();
}

void CTileSet::RenderTileSet()
{
	// encapsulate Opengl calls
	glEnable(GL_TEXTURE_2D);
	Texture->Bind();
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(0, 0);
		glTexCoord2f(1, 0); glVertex2f(Info.TileWidth*Info.HorNumTiles, 0);
		glTexCoord2f(1, 1); glVertex2f(Info.TileWidth*Info.HorNumTiles, Info.TileHeight*Info.VerNumTiles);
		glTexCoord2f(0, 1); glVertex2f(0, Info.TileHeight*Info.VerNumTiles);
	glEnd();

	CPrimitiveRender p;
	p.doUseGlobalCoordSystem = false;
	p.BlendingOption = PRM_RNDR_OPT_BLEND_NO_ONE;
	p.lClr = RGBAf(0.0f, 0.0f, 0.0f, 1.0f);
	p.pClr = RGBAf(0.0f, 0.0f, 0.0f, 1.0f);
	p.lwidth = 0.1f;
	
	for (int i = 0; i <= Info.HorNumTiles; i++)
		p.grSegment(Vector2(i*Info.TileWidth, 0.0f),
			Vector2(i*Info.TileWidth, Texture->height));
	for (int i = 0; i <= Info.VerNumTiles; i++)
		p.grSegment(Vector2(0.0f, i*Info.TileHeight),
			Vector2(Texture->width, i*Info.TileHeight));
}

bool CTileSet::SaveToFile()
{
	CFile file;
	if (!file.Open(filename.c_str(), CFILE_WRITE))
	{
		Log("ERROR", "Can't open TileSet file %s", filename);
		return false;
	}

	file.Write(Texture->name.c_str(), Texture->name.length()+1);
	file.Write(&Info, sizeof(Info));
	file.Write(BBox, sizeof(CAABB)*Info.HorNumTiles*Info.VerNumTiles);

	file.Close();
}

CTileSet::CTileSet()
{
	BBox = NULL;
	Texture = NULL;
	memset(&Info, 0, sizeof(Info));
}

void CTileSet::SetSettings( byte _TileWidth, byte _TileHeight, int _HorNumTiles, int _VerNumTiles, char *_ImageData )
{
	Info.HorNumTiles = _HorNumTiles;
	Info.VerNumTiles = _VerNumTiles;
	Info.TileWidth = _TileWidth;
	Info.TileHeight = _TileHeight;
	if (BBox != NULL)
		delete [] BBox;
	BBox = new CAABB [Info.HorNumTiles*Info.VerNumTiles];
	for(int i=0; i<Info.HorNumTiles*Info.VerNumTiles; i++)
		BBox[i] = CAABB(0, 0, Info.TileHeight, Info.TileWidth);
}

Vector2* CTileSet::GetCellTC(int CellIndex)
{
	if (Texture->width == 0 || Texture->height == 0)
		if (!loaded || !Texture->Load())
			Log("ERROR", "Using unloaded texture in Tileset %s", name);

	Vector2 *tc, t;
	tc = new Vector2[4];
	t.x = CellIndex % Info.HorNumTiles;
	t.y = CellIndex / Info.HorNumTiles;
	for (int i = 0; i < 4; i++)
	{
		tc[i] = (t + V2QuadBin[i]);
		tc[i].x *= (float)Info.TileWidth/Texture->width;
		tc[i].y *= (float)Info.TileHeight/Texture->height;
	}
	return tc;
}

//-------------------------------------------//
//				CMap functions				 //
//-------------------------------------------//
	

CObject * CLevelMap::NewLevelMap()
{
	return new CLevelMap;
}

bool CLevelMap::Render()
{
	if (!loaded)
		LoadFromFile();
	
	CMapCellInfo *t;
	
	glPushAttrib(GL_TEXTURE_BIT | GL_BLEND);
	glEnable(GL_TEXTURE);
	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	TileSet->Texture->Bind();
	glBegin(GL_QUADS);
	for(int i=0;i<numCellsVer;i++)
		for(int j=0;j<numCellsHor;j++)
		{
			t = Cells + _Cell(j, i);

			glTexCoord2f(t->tc[0].x, t->tc[0].y);
			glVertex3f(t->pos[0].x,	t->pos[0].y, t->z);

			glTexCoord2f(t->tc[1].x, t->tc[1].y); 
			glVertex3f(t->pos[1].x,	t->pos[1].y, t->z);

			glTexCoord2f(t->tc[2].x, t->tc[2].y); 
			glVertex3f(t->pos[2].x,	t->pos[2].y, t->z);

			glTexCoord2f(t->tc[3].x, t->tc[3].y); 
			glVertex3f(t->pos[3].x,	t->pos[3].y, t->z);
		}
	glEnd();
	glPopAttrib();
	return true;
}

bool CLevelMap::LoadFromFile()
{
	CFile File;
	CFactory *Factory = CFactory::Instance();
	if (!File.Open(filename.c_str(), CFILE_READ))
	{
		Log("ERROR", "Can't load level %s", filename.c_str());
		return false;
	}

	File.ReadLine(TileSetName);

	TileSet = NULL;
	TileSet = dynamic_cast<CTileSet*>(Factory->GetObject(TileSetName));
	if (TileSet == NULL)
	{
		Log("Error", "Tileset %s for map %s not found in resources", this->name, TileSetName);
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
	Factory->FreeInst();
	loaded = true;
	return true;
}


bool CLevelMap::SaveToFile()
{
	CFile file;
	if (!file.Open(filename.c_str(), CFILE_WRITE))
	{
		Log("ERROR", "Can't open file %s to save the map", filename);
		return false;
	}
	
	file.Write(TileSetName, strlen(TileSetName)+1);
	file.Write(&numCellsHor, sizeof(numCellsHor));
	file.Write(&numCellsVer, sizeof(numCellsVer));
	file.Write(Cells, sizeof(CMapCellInfo)*numCellsHor*numCellsVer);
	file.Close();
}

bool CLevelMap::GenCells()
{
	for(int i=0;i<numCellsVer;i++)
		for(int j=0;j<numCellsHor;j++)
		{
			CMapCellInfo *t = &(Cells[_Cell(j, i)]);
			Vector2 ji = Vector2(j, i);
			scalar w = TileSet->Info.TileWidth;
			scalar h = TileSet->Info.TileHeight;
			for(int k = 0; k < 4; k++)
			{
				t->pos[k] = (ji + V2QuadBin[k]);
				t->pos[k].x *= w;
				t->pos[k].y *= h;
			}
			t->tc = TileSet->GetCellTC(t->index);
		}
	return true;
}

bool CCollisionInfo::LoadFromFile()
{
	if (loaded)
	{
		Log("ERROR", "Resource %s already loaded", name);
		return false;
	}

	CFile file;
	if (!file.Open(filename.c_str(), CFILE_READ))
		return false;

	file.ReadLine(TileSetName);
	CFactory *Factory = CFactory::Instance();
	TileSet = dynamic_cast<CTileSet*>(Factory->GetObject(TileSetName));
	Factory->FreeInst();
	if (TileSet == NULL)
		return false;
	TileSet->Texture->Load();

	boxes = new CAABB [TileSet->Info.HorNumTiles*TileSet->Info.VerNumTiles];
	file.Read(boxes, TileSet->Info.HorNumTiles*TileSet->Info.VerNumTiles*sizeof(CAABB));

	loaded = true;
	return true;
}

bool CCollisionInfo::SaveToFile()
{
	CFile file;
	if (!file.Open(filename.c_str(), CFILE_WRITE))
		return false;
	file.Write(TileSetName, strlen(TileSetName)+1);
	if (TileSet == NULL)
		return false;
	file.Write(boxes, TileSet->Info.HorNumTiles*TileSet->Info.VerNumTiles*sizeof(CAABB));

	return true;
}

bool CCompas::Render()
{
		Vector2 v1, n;
		CEngine *Ninja = CEngine::Instance();
		v1 = Ninja->RenderManager.Camera.Point;
		n = (-v1).Normalized();
		float depth = v1.Length()/40.0f;
		depth = clampf(depth, 0.0f, 90.0f);
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
		Ninja->FreeInst();
		return true;
}

CCompas::CCompas()
{
	name = "Visual compass";
	CEngine *Ninja = CEngine::Instance();
	Ninja->RenderManager.AddObject(this);
	Ninja->FreeInst();	
}

CCompas::~CCompas()
{
	
}