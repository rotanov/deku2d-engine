#include "2de_GameUtils.h"

#include "2de_GraphicsHigh.h"

//////////////////////////////////////////////////////////////////////////
// CTileset
CTileset::CTileset() : TileWidth(0), TileHeight(0), HorNumTiles(0), VerNumTiles(0), BBox(NULL), Texture(NULL)
{
	CTileSetManager::Instance()->Add(this);
}

CTileset::~CTileset()
{
	Unload();
	CTileSetManager::Instance()->Remove(this);
}

bool CTileset::Load()
{
	if (Loaded)
		return true;

	CStorage *storage;

	if (Source == LOAD_SOURCE_FILE)
	{
		storage = new CFile(Filename, CStorage::OPEN_MODE_READ);
		if (!storage->Good())
		{
			Log("ERROR", "Can't open TileSet file '%s'", Filename.c_str());
			return false;
		}
	}
	else if (Source == LOAD_SOURCE_MEMORY)
	{
		storage = new CMemory(MemoryLoadData, MemoryLoadLength, CStorage::OPEN_MODE_READ);
		if (!storage->Good())
		{
			Log("ERROR", "Can't open TileSet from memory storage");
			return false;
		}
	}
	else
	{
		Log("ERROR", "Can't load TileSet: no load source specified");
		return false;
	}

	string TextureName;
	storage->ReadString(TextureName);

	Texture = CTextureManager::Instance()->Get(TextureName);
	Texture->CheckLoad();

	storage->Read(&TileWidth, sizeof(TileWidth));
	storage->Read(&TileHeight, sizeof(TileHeight));
	storage->Read(&HorNumTiles, sizeof(HorNumTiles));
	storage->Read(&VerNumTiles, sizeof(VerNumTiles));

	if (BBox != NULL)
		delete [] BBox;

	BBox = new CBox[HorNumTiles * VerNumTiles];
	storage->Read(BBox, sizeof(CBox) * HorNumTiles * VerNumTiles);

	storage->Close();
	delete storage;

	CResource::Load();

	return true;
}

void CTileset::Unload()
{
	if (!Loaded)
		return;

	if (BBox != NULL)
	{
		delete[] BBox;
		BBox = NULL;
	}

	CResource::Unload();
}

bool CTileset::SaveToFile(const string &AFilename)
{
	CFile file;
	if (!file.Open(AFilename, CFile::OPEN_MODE_WRITE))
	{
		Log("ERROR", "Can't open TileSet file %s", Filename.c_str());
		return false;
	}

	file.Write(Texture->GetName().c_str(), Texture->GetName().length() + 1);
	file.Write(&TileWidth, sizeof(TileWidth));
	file.Write(&TileHeight, sizeof(TileHeight));
	file.Write(&HorNumTiles, sizeof(HorNumTiles));
	file.Write(&VerNumTiles, sizeof(VerNumTiles));
	file.Write(BBox, sizeof(CBox) * HorNumTiles * VerNumTiles);
	file.Close();

	return true;
}

void CTileset::SetSettings(byte ATileWidth, byte ATileHeight, int AHorNumTiles, int AVerNumTiles)
{
	HorNumTiles = AHorNumTiles;
	VerNumTiles = AVerNumTiles;
	TileWidth = ATileWidth;
	TileHeight = ATileHeight;
	if (BBox != NULL)
		delete [] BBox;
	BBox = new CBox [HorNumTiles * VerNumTiles];
	for (int i = 0; i < HorNumTiles * VerNumTiles; i++)
		BBox[i] = CBox(0, 0, TileHeight, TileWidth);
}

Vector2Array<4> CTileset::GetCellTC(int CellIndex)
{
	assert(Texture->Width != 0 && Texture->Height != 0);

	Vector2 t;
	Vector2Array<4> tc;
	t.x = CellIndex % HorNumTiles;
	t.y = CellIndex / HorNumTiles;
	for (int i = 0; i < 4; i++)
	{
		tc[i] = (t + V2_QuadBin[i]);
		tc[i].x *= (float)TileWidth/Texture->Width;
		tc[i].y *= (float)TileHeight/Texture->Height;
	}
	return tc;
}

CTexture* CTileset::GetTexture() const
{
	assert(Texture != NULL);
	return Texture;
}

void CTileset::SetTexture(const string &TextureName)
{
	Texture = CTextureManager::Instance()->Get(TextureName);
}

//////////////////////////////////////////////////////////////////////////
// CTileSetManager
CTileSetManager::CTileSetManager()
{
	SetName("Tileset manager");
}

CTileset* CTileSetManager::GetTileset(const string &ATilesetName)
{
	CTileset *Tileset = NULL;
	Tileset = dynamic_cast<CTileset*>(Get(ATilesetName));
	if (Tileset)
		Tileset->CheckLoad();
	return Tileset;
}

//////////////////////////////////////////////////////////////////////////
// Default title screen
CDefaultTitleScreen::CDefaultTitleScreen()
{
	int ScrWidth = CGLWindow::Instance()->GetWidth();
	int ScrHeight = CGLWindow::Instance()->GetHeight();
	SetPosition(Vector2(ScrWidth * 0.5f, ScrHeight * 0.5f));
	CUpdateManager::Instance()->RootGameObject->Attach(this);
	this->Attach(&TextTeam);
	this->Attach(&TextDeku);
	TextDeku.SetPosition(Vector2(40.0f, -47.0f));
	TextTeam.SetPosition(Vector2(40.0f, -74.0f));
	TextDeku.SetScaling(2);
	TextTeam.SetScaling(2);
	TextDeku.SetText("Deku");
	TextTeam.SetText("Team");
}

void CDefaultTitleScreen::SetTexture(CTexture* ATexture)
{
	assert(ATexture != NULL);
	SetModel(CRenderManager::CreateModelBox(256, 256, MODEL_TYPE_TRIANGLES, ATexture));
}
