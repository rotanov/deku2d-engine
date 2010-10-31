#ifndef _2DE_GAME_UTILS_H_
#define _2DE_GAME_UTILS_H_

#include "2de_Core.h"
#include "2de_GraphicsLow.h"
#include "2de_Update.h"

class CUnitBase : public CRenderable, public CUpdatable
{
	
};

class CTileset : public CResource
{
public:
	CTileset();
	~CTileset();

	bool Load();
	void Unload();

	bool SaveToFile(const string &AFilename);

	void SetSettings(byte ATileWidth, byte ATileHeight, int AHorNumTiles, int AVerNumTiles);
	Vector2Array<4> GetCellTC(int CellIndex);

	CTexture* GetTexture() const
	{
		assert(Texture != NULL);
		return Texture;
	}
	void SetTexture(const string &TextureName)
	{
		Texture = CTextureManager::Instance()->Get(TextureName);
	}

	void RenderTileSet(); //FOR DEBUGGING

	unsigned int TileWidth;
	unsigned int TileHeight;
	unsigned int HorNumTiles;
	unsigned int VerNumTiles;
	CBox* BBox;

private:
	CResourceRefCounter<CTexture> Texture;

};

class CTileSetManager : public CCommonManager <list <CTileset*> >/*public CList*/, public CTSingleton<CTileSetManager>
{
public:
	CTileset* GetTileset(const string &ATilesetName)
	{
		CTileset *Tileset = NULL;
		Tileset = dynamic_cast<CTileset*>(Get(ATilesetName));
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

class CLevelMap : /*public CResource,*/ public CRenderable
{
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
		return GetMapCell((int)APosition.x / (GetScaling() * TileSet->TileWidth), (int)APosition.y / (GetScaling() * TileSet->TileHeight));
	}
	CBox GetCellAABB(const Vector2 &V);

private:
	size_t HorizontalCellsCount;
	size_t VerticalCellsCount;
	CMapCellInfo *Cells;
	CResourceRefCounter<CTileset> TileSet;

	int GetCellIndex(int h, int v);
};

class CCompas : public CRenderable
{
public:
	CCompas();
	~CCompas();
	void Render();
};

class CDefaultTitleScreen : public CRenderable
{
public:
	CDefaultTitleScreen() : Texture(NULL), TextDeku("Deku"), TextTeam("Team")
	{
		int ScrWidth = CGLWindow::Instance()->GetWidth();
		int ScrHeight = CGLWindow::Instance()->GetHeight();
		SetPosition(Vector2(ScrWidth * 0.5f, ScrHeight * 0.5f));
	}
	void SetTexture(CTexture* ATexture)
	{
		assert(ATexture != NULL);
		Texture = ATexture;
	}
	void Render()
	{
		CRenderManager::Instance()->DrawTexturedBox(this, CBox(Vector2(-50.0f, -50.0f), Vector2(50.0f, 50.0f)), Texture, CBox(V2_ZERO, V2_DIR_UP + V2_DIR_RIGHT).GetVertices());
	}

private:
	CResourceRefCounter<CTexture> Texture;
	CText TextDeku;
	CText TextTeam;
};

#endif // _2DE_GAME_UTILS_H
