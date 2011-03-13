#ifndef _2DE_GAME_UTILS_H_
#define _2DE_GAME_UTILS_H_

#include "2de_Core.h"
#include "2de_Components.h"
#include "2de_GraphicsLow.h"
#include "2de_Update.h"

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
	CTexture* GetTexture() const;
	void SetTexture(const string &TextureName);

	unsigned TileWidth;
	unsigned TileHeight;
	unsigned HorNumTiles;
	unsigned VerNumTiles;
	CBox* BBox;

private:
	CResourceRefCounter<CTexture> Texture;

};

class CTileSetManager : public CCommonManager <list <CTileset*> >/*public CList*/, public CTSingleton<CTileSetManager>
{
public:
	CTileset* GetTileset(const string &ATilesetName);

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

//	class CLevelMap	- helper class representing some common level map. Had CResourceRefCounter<CTileset> TileSet;
//	class CCompas - should represent some graphics, always pointing to origin or particular target. Just like compas.

/**
*	Default title screen for any thing make with engine. Easily disablable and replaceable
*	Also in CRenerable era was rendered like 
*	CRenderManager::Instance()->DrawTexturedBox(this, 
*		CBox(Vector2(-50.0f, -50.0f), Vector2(50.0f, 50.0f)),
*		Texture, CBox(V2_ZERO, V2_DIR_UP + V2_DIR_RIGHT).GetVertices());
*/
class CDefaultTitleScreen : public CRenderableComponent
{
public:
	CDefaultTitleScreen();
	void SetTexture(CTexture* ATexture);
	void ProcessEvent(const CEvent &AEvent);

private:
	CText TextDeku;
	CText TextTeam;
};

#endif // _2DE_GAME_UTILS_H
