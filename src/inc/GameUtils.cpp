#include "GameUtils.h"

//-------------------------------------------//
//			CTileSet functions				 //
//-------------------------------------------//

bool CTileSet::LoadFromFile()
{
	CFactory *Factory = CFactory::Instance();
	CFile file;
	if (!file.Open(filename.c_str(), CFILE_READ))
	{
		Log("AHTUNG", "Can't open TileSet file %s", filename);
		return false;
	}

	file.ReadLine(TextureName);

	Texture = dynamic_cast<CTexture*>((dynamic_cast<CTextureManager*>(Factory->GetManager(MANAGER_TYPE_TEX)))->GetObject(TextureName));


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
	glLoadIdentity();
	Texture->Bind();
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(0, 0);
		glTexCoord2f(1, 0); glVertex2f(Info.TileWidth*Info.HorNumTiles, 0);
		glTexCoord2f(1, 1); glVertex2f(Info.TileWidth*Info.HorNumTiles, Info.TileHeight*Info.VerNumTiles);
		glTexCoord2f(0, 1); glVertex2f(0, Info.TileHeight*Info.VerNumTiles);
	glEnd();
}

bool CTileSet::SaveToFile()
{
	CFile file;
	if (!file.Open(filename.c_str(), CFILE_WRITE))
	{
		Log("AHTUNG", "Can't open TileSet file %s", filename);
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
	glLoadIdentity();
	CMapCellInfo *t;
	glEnable(GL_TEXTURE);
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
	return true;
}

bool CLevelMap::LoadFromFile()
{
	CFile File;
	CFactory *Factory = CFactory::Instance();
	if (!File.Open(filename.c_str(), CFILE_READ))
	{
		Log("AHTUNG", "Can't load level %s", filename.c_str());
		return false;
	}

	File.ReadLine(TileSetName);

	TileSet = NULL;
	TileSet = dynamic_cast<CTileSet*>(Factory->GetObject(TileSetName));
	if (TileSet == NULL)
	{
		TileSet = dynamic_cast<CTileSet*>(Factory->Create(OBJ_USER_DEFINED, CTileSet::NewTileSet));		
		//TileSet->LoadFromFile(tilesetname);
	}
	TileSet->Texture->Load();

	File.Read(&numCellsHor, sizeof(numCellsHor));
	File.Read(&numCellsVer, sizeof(numCellsVer));
	if (Cells != NULL)
		delete [] Cells;
	Cells = new CMapCellInfo [numCellsVer*numCellsHor];
	File.Read(Cells, numCellsVer*numCellsHor*sizeof(CMapCellInfo));

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
		Log("AHTUNG", "Can't open file %s to save the map", filename);
		return false;
	}
	
	file.Write(TileSetName, strlen(TileSetName)+1);
	file.Write(&numCellsHor, sizeof(numCellsHor));
	file.Write(&numCellsVer, sizeof(numCellsVer));
	file.Write(Cells, sizeof(CMapCellInfo)*numCellsHor*numCellsVer);
	file.Close();
}
