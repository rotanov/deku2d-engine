#ifndef _COLOR_LINESH_H_
#define _COLOR_LINESH_H_ 

#include "2de_Engine.h"
#include <queue>
using std::pair;
using std::queue;
using std::make_pair;
using namespace Deku2d;

const unsigned CELL_SIZE = 32;
const unsigned CELLS_COUNT = 9;
const unsigned COLORS_COUNT = 7;
const unsigned BALLS_PER_TURN = 3;
const unsigned BALLS_IN_COMPLETED_LINE = 5;

const RGBAf LINES_COLORS[COLORS_COUNT/* + 1*/] = 
{
	RGBAf(1.0f, 0.5f, 0.5f, 1.0f),	// Red
	RGBAf(0.5f, 1.0f, 0.5f, 1.0f),	// Green
	RGBAf(0.5f, 0.5f, 1.0f, 1.0f),	// Blue

	RGBAf(0.5f, 1.0f, 1.0f, 1.0f),	// Cyan
	RGBAf(1.0f, 0.5f, 1.0f, 1.0f),	// Magenta
	RGBAf(1.0f, 1.0f, 0.5f, 1.0f),	// Yellow

	RGBAf(1.0f, 1.0f, 1.0f, 1.0f),	// Other
};

class CLiveBox : public CGameObject
{
public:
	float TTL;
	float Accumulated;
	CPlaceableComponent *Placing;
	CRenderableComponent *Visual;
	CLiveBox() : TTL(0.0f), Accumulated(0.0f)
	{
		CEventManager::Instance()->Subscribe("EveryFrame", this);
		this->Attach(Placing = CFactory::Instance()->New<CPlaceableComponent>());
		Placing->Attach(Visual = CFactory::Instance()->New<CRenderableComponent>());
		Visual->SetModel(CRenderManager::Instance()->CreateModelBox(1.0f, 1.0f, MODEL_TYPE_TRIANGLES));
	}
	void ProcessEvent(const CEvent &AEvent)
	{
		if (AEvent.GetName() != "EveryFrame")
			return;
		Accumulated += CEngine::Instance()->GetDeltaTime();
		Visual->SetColor(Const::Color::WHITE * (TTL - Accumulated) / TTL);
		if (Accumulated >= TTL)
			this->SetDestroyedSubtree();
	}
};

class CGridGame : public CGameObject
{
private:
	CEventManager *EventManager;
	CFactory *Factory;
private:
	class CCellSelection
	{
	public:
		CCellSelection(int Ax, int Ay);
		CCellSelection();
		unsigned GetX() const;
		unsigned GetY() const;
		bool isValid() const;

	private:
		bool Valid;
		unsigned x;
		unsigned y;		
	};

	class CCell
	{
		friend class CGridGame;

	public:
		CCell(int AColorIdentifier = -1);
		bool isClear();
		void MakeVisualPart();
		
		void SetupAt(int x, int y, unsigned Color);
		unsigned GetColorIdentifier() const;
		const CCellSelection& GetCoords() const
		{
			return Coords;
		}

	private:
		unsigned ColorIdentifier;
		CGameObject *VisualPart;
		CCellSelection Coords;

		void Invalidate();
	};

public:
	CGridGame();
	void ProcessEvent(const CEvent &AEvent);

private:
	CCell Cells[CELLS_COUNT][CELLS_COUNT];
	CCellSelection TurnSourceCell;
	CCellSelection TurnTargetCell;
	std::map<std::pair<unsigned, unsigned>, CGameObject*> StuffForDrawing;
	CGameObject *Grid;
	Vector2 GridPos;
	CRenderableComponent *GridMouseHighlight;
	CPlaceableComponent	*GridMouseHighlightPlacing;
	vector<CCell*> FreeCellsPool;

	void InitializeGame();
	void InvalidateCell(CCell& ACell);
	CCellSelection SelectCellByMousePosition(const Vector2 &MousePosition);
	int CountBallsInLine(int HorOrVer, int x, int y);
	void RemoveBalls(int HorOrVer, unsigned x, unsigned y, unsigned color);
	bool ProcessFilledLines(unsigned x, unsigned y, unsigned color);
	void SpawnBall(const CCellSelection &DestCell, unsigned ColorIdentifier);
	bool ProcessBFS(int SourceX, int SourceY, int DestinationX, int DestinationY);
	void SpawnFloorHightlight(unsigned x, unsigned y, unsigned TTL);
};


#endif // _COLOR_LINESH_H_
