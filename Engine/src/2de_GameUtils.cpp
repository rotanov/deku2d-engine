#include "2de_GameUtils.h"

#include "2de_GraphicsHigh.h"
#include "2de_Engine.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// CTileset

	CTileset::CTileset() : TileWidth(0), TileHeight(0), HorNumTiles(0), VerNumTiles(0), BBox(NULL), Texture(NULL)
	{
		ClassName = "Tileset";
		TileSetManager->Add(this);
	}

	CTileset::~CTileset()
	{
		Unload();
		TileSetManager->Remove(this);
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

		Texture = TextureManager->Get(TextureName);
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

		CResource::BasicLoad();

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

		CResource::BasicUnload();
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
			tc[i] = (t + Const::Graphics::V2_QuadBin[i]);
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
		Texture = TextureManager->Get(TextureName);
	}

	//////////////////////////////////////////////////////////////////////////
	// CTileSetManager

	CTileSetManager::CTileSetManager()
	{
		SetName("TileSetManager");
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
	// CDefaultTitleScreen

	CDefaultTitleScreen::CDefaultTitleScreen()
	{
		SetName("DefaultTitleScreen");
	}

	void CDefaultTitleScreen::SetTexture(CTexture* ATexture)
	{
		assert(ATexture != NULL);
		SetModel(CRenderManager::CreateModelBox(256, 256, MODEL_TYPE_TRIANGLES, ATexture));
	}

	void CDefaultTitleScreen::ProcessEvent( const CEvent &AEvent )
	{
		if (AEvent.GetName() != "WindowResize")
			return;
		if (this->GetParent() != NULL)
			static_cast<CPlaceableComponent*>(this->GetParent())->SetPosition(GLWindow->GetSize() * 0.5f);
	}

	void CDefaultTitleScreen::FinalizeCreation()
	{
		CRenderableComponent::FinalizeCreation();

		TextDeku = Factory->CreateComponent<CText>();
		TextTeam = Factory->CreateComponent<CText>();

		EventManager->Subscribe("WindowResize", this);
		CPlaceableComponent *tempPlacing = Factory->CreateComponent<CPlaceableComponent>();
		int ScrWidth = GLWindow->GetWidth();
		int ScrHeight = GLWindow->GetHeight();
		tempPlacing->SetPosition(Vector2(ScrWidth * 0.5f, ScrHeight * 0.5f));
		CEngine::Instance()->RootGameObject->Attach(tempPlacing);
		tempPlacing->Attach(this);
		CPlaceableComponent *DekuTextPlacing = Factory->CreateComponent<CPlaceableComponent>();
		CPlaceableComponent *TeamTextPlacing = Factory->CreateComponent<CPlaceableComponent>();
		tempPlacing->Attach(DekuTextPlacing);
		tempPlacing->Attach(TeamTextPlacing);
		DekuTextPlacing->Attach(TextDeku);
		TeamTextPlacing->Attach(TextTeam);
		DekuTextPlacing->SetPosition(Vector2(40.0f, -47.0f));
		TeamTextPlacing->SetPosition(Vector2(40.0f, -74.0f));
		DekuTextPlacing->SetScaling(2);
		TeamTextPlacing->SetScaling(2);
		TextDeku->SetText("Deku");
		TextTeam->SetText("Team");
	}

}	//	namespace Deku2D
