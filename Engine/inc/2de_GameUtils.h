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
	~CTileset();
	

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
		Texture = CTextureManager::Instance()->GetObject(TextureName);
	}

	void RenderTileSet(); //FOR DEBUGGING

};

class CTileSetManager : public CCommonManager <list <CTileset*> >/*public CList*/, public CTSingleton<CTileSetManager>
{
public:
	CTileset* GetTileset(const string &ATilesetName)
	{
		CTileset *Tileset = NULL;
		Tileset = dynamic_cast<CTileset*>(GetObject(ATilesetName));
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
* Cells: >^
*/

class CLevelMap : /*public CResource,*/ public CRenderObject
{
private:
	size_t HorizontalCellsCount;
	size_t VerticalCellsCount;
	CMapCellInfo *Cells;
	CTileset *TileSet;

	int GetCellIndex(int h, int v);
public:
	CLevelMap();
	CLevelMap(size_t AHorizontalCellsCount, size_t AVerticalCellsCount,
		const string &ATilesetName, const string &AName);
	virtual ~CLevelMap();
	
	bool GenCells();
// 	bool LoadFromFile();
// 	bool SaveToFile();
	void Render();
	CMapCellInfo* GetMapCell(size_t HorizontalIndex, size_t VerticalIndex);
	CMapCellInfo* GetMapCell(Vector2 APosition) // Position inside map with scaling in mind
	{
		return GetMapCell((int)APosition.x / (Scaling * TileSet->TileWidth), (int)APosition.y / (Scaling * TileSet->TileHeight));
	}
	CAABB GetCellAABB(const Vector2 &V);

};

class CCompas : public CRenderObject
{
public:
	CCompas();
	~CCompas();
	void Render();	
};

#endif // _2DE_GAME_UTILS_H
