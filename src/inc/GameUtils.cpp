#include "GameUtils.h"
#include "Ninja.h"

//-------------------------------------------//
//			CTileSet functions				 //
//-------------------------------------------//

bool CTileSet::LoadFromFile()
{
	CFactory *Factory = CFactory::Instance();
	CFile file;
	if (!file.Open(filename.c_str(), CFILE_READ))
	{
		Log("WARNING", "Can't open TileSet file %s", filename);
		return false;
	}

	file.ReadLine(TextureName);

	Texture = dynamic_cast<CTexture*>((dynamic_cast<CTextureManager*>(Factory->GetManager(MANAGER_TYPE_TEX)))->GetObjectByName(TextureName));
	Texture->GetTexID();


	file.Read(&Info, sizeof(Info));
	if (BBox != NULL)
		delete [] BBox;
	file.Read(BBox, sizeof(CBBox)*Info.HorNumTiles*Info.VerNumTiles);

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
	p.doUseCurrentCoordSystem = false;
	p.BlendingOption = PRM_RNDR_OPT_BLEND_NOONE;
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
		Log("WARNING", "Can't open TileSet file %s", filename);
		return false;
	}

	file.Write(TextureName, strlen(TextureName)+1);
	file.Write(&Info, sizeof(Info));
	file.Write(BBox, sizeof(CBBox)*Info.HorNumTiles*Info.VerNumTiles);

	file.Close();
}

CTileSet::CTileSet()
{
	BBox = NULL;
	TextureName = NULL;
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
	BBox = new CBBox [Info.HorNumTiles*Info.VerNumTiles];
	for(int i=0; i<Info.HorNumTiles*Info.VerNumTiles; i++)
		BBox[i] = CBBox(0, 0, Info.TileHeight, Info.TileWidth);
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
		Log("WARNING", "Can't load level %s", filename.c_str());
		return false;
	}

	File.ReadLine(TileSetName);

	TileSet = NULL;
	TileSet = dynamic_cast<CTileSet*>(Factory->GetObjectByName(TileSetName));
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
		Log("WARNING", "Can't open file %s to save the map", filename);
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
			int th, tv;
			th = t->index % TileSet->Info.HorNumTiles;
			tv = (t->index) / TileSet->Info.HorNumTiles;

			t->tc[0].x = (float)(th + 0)*TileSet->Info.TileWidth/TileSet->Texture->width;
			t->tc[0].y = (float)(tv + 0)*TileSet->Info.TileHeight/TileSet->Texture->height;
			t->pos[0].x = (j + 0)*TileSet->Info.TileWidth;
			t->pos[0].y = (i + 0)*TileSet->Info.TileHeight;

			t->tc[1].x = (float)(th + 1)*TileSet->Info.TileWidth/TileSet->Texture->width;
			t->tc[1].y = (float)(tv + 0)*TileSet->Info.TileHeight/TileSet->Texture->height;
			t->pos[1].x = (j + 1)*TileSet->Info.TileWidth;
			t->pos[1].y = (i + 0)*TileSet->Info.TileHeight;

			t->tc[2].x = (float)(th + 1)*TileSet->Info.TileWidth/TileSet->Texture->width;
			t->tc[2].y = (float)(tv + 1)*TileSet->Info.TileHeight/TileSet->Texture->height;
			t->pos[2].x = (j + 1)*TileSet->Info.TileWidth;
			t->pos[2].y = (i + 1)*TileSet->Info.TileHeight;	

			t->tc[3].x = (float)(th + 0)*TileSet->Info.TileWidth/TileSet->Texture->width;
			t->tc[3].y = (float)(tv + 1)*TileSet->Info.TileHeight/TileSet->Texture->height;
			t->pos[3].x = (j + 0)*TileSet->Info.TileWidth;
			t->pos[3].y = (i + 1)*TileSet->Info.TileHeight;
		}
	return true;
}

bool CCollisionInfo::LoadFromFile()
{
	if (loaded)
	{
		Log("Warning", "Resource %s already loaded", name);
		return false;
	}

	CFile file;
	if (!file.Open(filename.c_str(), CFILE_READ))
		return false;

	file.ReadLine(TileSetName);
	CFactory *Factory = CFactory::Instance();
	TileSet = dynamic_cast<CTileSet*>(Factory->GetObjectByName(TileSetName));
	Factory->FreeInst();
	if (TileSet == NULL)
		return false;
	TileSet->Texture->Load();

	boxes = new CBBox [TileSet->Info.HorNumTiles*TileSet->Info.VerNumTiles];
	file.Read(boxes, TileSet->Info.HorNumTiles*TileSet->Info.VerNumTiles*sizeof(CBBox));

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
	file.Write(boxes, TileSet->Info.HorNumTiles*TileSet->Info.VerNumTiles*sizeof(CBBox));

	return true;
}

bool CCompas::Render()
{
		Vector2 v1, n;
		CNinja *Ninja = CNinja::Instance();
		v1 = Ninja->RenderManager.Camera.Point;
		n = (-v1).Normalized();
		float depth = v1.Length()/40.0f;
		depth = clampf(depth, 0.0f, 90.0f);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glEnable(GL_LINE_WIDTH);
		glLineWidth(3.0f);
		CPrimitiveRender pr;
		pr.plClr = &RGBAf(0.6f, 0.9f, 0.7f, 0.9f);
		pr.grCircleL(Vector2(100,100), depth);;
		pr.grSegment(Vector2(100, 100), (Vector2(100, 100) + n*depth));

// 		pr.grCircleL(Vector2(100,100), depth, RGBAf(depth/ (90.0f * 2), 0.0f, 0.0f, 0.9f));
// 		pr.grSegment(Vector2(100, 100), (Vector2(100, 100) + n*depth), 1.0f, RGBAf(depth/( 90.0f * 2), 0.0f, 0.0f, 0.9f));

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Ninja->FreeInst();
		return true;
}

CCompas::CCompas()
{
	name = "Visual compass";
	CNinja *Ninja = CNinja::Instance();
	Ninja->RenderManager.AddObject(this);
	Ninja->FreeInst();	
}