#ifndef _2DE_GAME_UTILS_H_
#define _2DE_GAME_UTILS_H_

#include "2de_Core.h"
//#include "2de_resource.h"
#include "2de_GraphicsLow.h"

class CUnitBase : public CRenderObject, public CUpdateObject
{
	
};

class CTileset : public CResource
{
private:
	CTexture	*Texture;
public:
	byte		TileWidth;
	byte		TileHeight;
	int			HorNumTiles;
	int			VerNumTiles;
	CAABB		*BBox;

	CTileset ();
	static CObject* NewTileset()
	{
		return new CTileset;
	}
	

	Vector2Array<4> GetCellTC(int CellIndex);
	void SetSettings(byte _TileWidth, byte _TileHeight, int _HorNumTiles, int _VerNumTiles);
	bool LoadFromFile();
	bool SaveToFile();
	CTexture * GetTexture() const
	{
		assert(Texture != NULL);
		return Texture;
	}
	void SetTexture(const string &TextureName)
	{
		Texture = CTextureManager::Instance()->GetTextureByName(TextureName);
	}

	void RenderTileSet(); //FOR DEBUGGING

};

class CTileSetManager : public CList, public CTSingleton<CTileSetManager>
{
public:
	CTileset* GetTileset(const string &ATilesetName)
	{
		CTileset *Tileset = NULL;
		Tileset = dynamic_cast<CTileset*>(GetObject(&ATilesetName));
		if (Tileset)
			Tileset->CheckLoad();
		return Tileset;
	}
protected:
	CTileSetManager();
	friend class CTSingleton<CTileSetManager>;
};

struct CMapCellInfo
{
	int index;
	int interaction;
	float z; // if 0 then - check for collision; else z.0f = 1/256.0f;
	Vector2Array<4> tc; // Его размер тем не менее должен быть 4
	Vector2 pos[4];
};

/**
* Cells: ^>
*/

class CLevelMap : public CResource, public CRenderObject, public CUpdateObject, public CList
{
private:
	int				numCellsHor, numCellsVer;
public:
	CMapCellInfo	*Cells;
	CTileset		*TileSet;

	CLevelMap(){};
	CLevelMap(int AnumCellsHor, int AnumCellsVer, const string &ATilesetName,
		const string &AName);
	~CLevelMap(){}
	
	bool Update(float dt)
	{
		return true;
	}
	bool GenCells();
	bool LoadFromFile();
	bool SaveToFile();
	bool Render();
//private:
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

#endif // _2DE_GAME_UTILS_H
