#ifndef  _GAME_UTILS_H_
#define _GAME_UTILS_H_

#include "CoreUtils.h"
#include "resource.h"
#include "OpenglUtils.h"

struct CTileSetInfo 
{
	byte TileWidth;
	byte TileHeight;
	int HorNumTiles;
	int VerNumTiles;
};

class CTileset : public CResource
{
public:
	CTexture *Texture;
	CTileSetInfo Info;
	CAABB *BBox;

	CTileset ();
	static CObject* NewTileSet()
	{
		return new CTileset;
	}
	

	Vector2* GetCellTC(int CellIndex);
	void SetSettings(byte _TileWidth, byte _TileHeight, int _HorNumTiles, int _VerNumTiles, char *_ImageData);
	bool LoadFromFile();
	bool SaveToFile();
	void RenderTileSet(); //FOR DEBUGGING

};

class CTileSetManager : public CList
{
public:
	CTileset* GetTileset(const string* ATilesetName)
	{
		CTileset *Tileset = NULL;
		Tileset = dynamic_cast<CTileset*>(GetObject(ATilesetName));
		if (Tileset)
			Tileset->CheckLoad();
		return Tileset;
	}
};

struct CMapCellInfo
{
	int index;
	int interaction;
	float z; // if 0 then - check for collision; else z.0f = 1/256.0f;
	Vector2 *tc; // ≈го размер тем не менее должен быть 4
	Vector2 pos[4];
};

/**
* Cells: ^>
*/

class CLevelMap : public CResource, public CRenderObject, public CUpdateObject, public CList
{
public:
	int				numCellsHor, numCellsVer;
	CMapCellInfo	*Cells;
	CTileset		*TileSet;

	static CObject *NewLevelMap();
	CLevelMap()
	{
		numCellsHor = numCellsVer = 0;
		Cells = NULL;
		TileSet = NULL;
		name = "CLevelMap";		
	}
	~CLevelMap(){}
	
	bool Update(float dt)
	{
		return true;
	}
	bool GenCells();
	bool LoadFromFile();
	bool SaveToFile();
	bool Render();
private:
	// h от horizontal v от vertical
	int _Cell(int h, int v)
	{
		return v*numCellsHor + h;
	}

};

class CCompas : public CRenderObject
{
public:
	CCompas();
	~CCompas();
	bool Render();	
};

#endif