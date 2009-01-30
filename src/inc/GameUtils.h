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

class CTileSet : public CResource
{
public:
	char *TextureName;
	CTexture *Texture;
	CTileSetInfo Info;
	CBBox *BBox;

	CTileSet ();
	static CObject* NewTileSet()
	{
		return new CTileSet;
	}
	

	void SetSettings(byte _TileWidth, byte _TileHeight, int _HorNumTiles, int _VerNumTiles, char *_ImageData);
	bool LoadFromFile();
	bool SaveToFile();
	void RenderTileSet(); //FOR DEBUGGING

};

struct CMapCellInfo
{
	int index;
	int interaction;
	int z; // if 0 then - check for collision; else z.0f = 1/256.0f;
	Vector2 tc[4];
	Vector2 pos[4];
};

/**
* Cells: ^>
*/

class CLevelMap : public CRenderResource
{
public:
	int				numCellsHor, numCellsVer;
	CMapCellInfo	*Cells;
	CTileSet		*TileSet;
	char			*TileSetName;

	static CObject *NewLevelMap();
	CLevelMap()
	{
		// TODO : in cpp ->
		name = "CLevelMap";
		type = T_RENDERABLE | T_UPDATABLE;
		TileSetName = NULL;
		name = "New map";
		numCellsHor = numCellsVer = 0;
		TileSet = NULL;
		Cells = NULL;
	}
	~CLevelMap(){}
	
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



#endif