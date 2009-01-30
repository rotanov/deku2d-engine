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

	Texture = (CTexture*)((CTextureManager*)Factory->GetManager(MANAGER_TYPE_TEX))->GetObject(TextureName);


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

	file.Write(Texture->name.c_str(), strlen(Texture->name.c_str())+1);
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

	char *tmp = NULL;
	File.ReadLine(tmp);
	name = tmp;
	File.ReadLine(TileSetName);

	TileSet = NULL;
	TileSet = (CTileSet*)Factory->GetObject(TileSetName);
	if (TileSet == NULL)
	{
		TileSet = (CTileSet*)Factory->Create(OBJ_USER_DEFINED, CTileSet::NewTileSet);		
		//TileSet->LoadFromFile(tilesetname);
	}

	File.Read(&numCellsHor, sizeof(numCellsHor));
	File.Read(&numCellsVer, sizeof(numCellsVer));
	if (Cells != NULL)
		delete [] Cells;
	Cells = new CMapCellInfo [numCellsVer*numCellsVer];
	File.Read(Cells, numCellsVer*numCellsVer*sizeof(CMapCellInfo));
	File.Close();
	Factory->FreeInst();
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
	file.Write((void*)name.data(), strlen(name.data())+1);
	file.Write(TileSetName, strlen(TileSetName)+1);
	file.Write(&numCellsHor, sizeof(numCellsHor));
	file.Write(&numCellsVer, sizeof(numCellsVer));
	file.Write(Cells, sizeof(CMapCellInfo)*numCellsHor*numCellsVer);
	file.Close();
}
