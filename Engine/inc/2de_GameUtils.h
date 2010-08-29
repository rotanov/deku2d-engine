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
private:
	CTexture	*Texture;

public:
	unsigned int TileWidth;
	unsigned int TileHeight;
	unsigned int HorNumTiles;
	unsigned int VerNumTiles;
	CBox* BBox;

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
		Texture = CTextureManager::Instance()->Get(TextureName);
	}

	void RenderTileSet(); //FOR DEBUGGING

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
		return GetMapCell((int)APosition.x / (GetScaling() * TileSet->TileWidth), (int)APosition.y / (GetScaling() * TileSet->TileHeight));
	}
	CBox GetCellAABB(const Vector2 &V);

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

	}
	void SetTexture(CTexture* ATexture)
	{
		assert(ATexture != NULL);
		Texture = ATexture;
	}
	void Render()
	{
		int ScrWidth = CGLWindow::Instance()->GetWidth();
		int ScrHeight = CGLWindow::Instance()->GetHeight();

		glEnable(GL_TEXTURE_2D);
		Texture->Bind();

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(ScrWidth * 0.5f - 50.0f, ScrHeight * 0.5f - 50.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(ScrWidth * 0.5f + 50.0f, ScrHeight * 0.5f - 50.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(ScrWidth * 0.5f + 50.0f, ScrHeight * 0.5f + 50.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(ScrWidth * 0.5f - 50.0f, ScrHeight * 0.5f + 50.0f);
		glEnd();
	}
private:
	CTexture *Texture;
	CText TextDeku;
	CText TextTeam;
};

#endif // _2DE_GAME_UTILS_H
