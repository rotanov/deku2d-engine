#include "2de_Components.h"

#include "2de_Engine.h"

//////////////////////////////////////////////////////////////////////////
// CGameObject::traverse_iterator

bool CGameObject::traverse_iterator::IsEnd() const
{
	return true;
}

bool CGameObject::traverse_iterator::IsValid() const
{
	return true;
}

CGameObject::traverse_iterator::traverse_iterator(CGameObject &AGameObject) : GameObject(&AGameObject)
{

}

CGameObject& CGameObject::traverse_iterator::operator *()
{
	return *GameObject;
}

const CGameObject& CGameObject::traverse_iterator::operator *() const 
{
	return *GameObject;
}

//////////////////////////////////////////////////////////////////////////
// CGameObject::traverse_iterator_bfs

CGameObject::traverse_iterator_bfs::traverse_iterator_bfs(CGameObject &AGameObject) : traverse_iterator(AGameObject), Index(0)
{
	Queue.push(GameObject);
}

bool CGameObject::traverse_iterator_bfs::Ok()
{
	return GameObject != NULL;
}

CGameObject::traverse_iterator_bfs& CGameObject::traverse_iterator_bfs::operator++()
{
	if (GameObject->Parent != NULL && Index + 1 < GameObject->Parent->Children.size())
	{
		Index++;
		GameObject = GameObject->Parent->Children[Index];
		Queue.push(GameObject);
	}
	else if (!Queue.empty())
	{
		GameObject = Queue.front();
		Queue.pop();
		if (GameObject != NULL && GameObject->Children.size() > 0)
		{
			GameObject = GameObject->Children[0];
			if (GameObject->Children.size() > 0)
				Queue.push(GameObject);
		}
		Index = 0;
	}
	else
	{
		GameObject = NULL;
		Index = 0;
	}
	return *this;
}

CGameObject::traverse_iterator_bfs& CGameObject::traverse_iterator_bfs::operator--()
{
	return *this;
}

bool CGameObject::traverse_iterator_bfs::operator ==(const CGameObject::traverse_iterator_bfs &rhs) const
{
	return true;			
}

bool CGameObject::traverse_iterator_bfs::operator !=(const CGameObject::traverse_iterator_bfs &rhs) const
{
	return !(*this == rhs);
}

//////////////////////////////////////////////////////////////////////////
// CGameObject

CGameObject::CGameObject() : Parent(NULL), Scene(NULL)
{
	PutIntoScene(CSceneManager::Instance()->GetCurrentScene());
	CEventManager::Instance()->Subscribe("Create", this);
}

CGameObject::~CGameObject()
{
	Scene->Remove(this);
	while (Children.size() > 0)
		// Note, that here we adding children to parent in reverse order, i think it's not that important for now.
		Children.back()->SetParent(Parent);
	SetParent(NULL);
}

void CGameObject::Attach(CGameObject* AGameObject)
{
	if (!AGameObject)
	{
		Log("ERROR", "NULL pointer passed to CGameObject::Attach");
		return;
	}

	if (AGameObject == this)
	{
		Log("ERROR", "Can't attach object '%s' to itself", GetName().c_str());
		return;
	}

	Children.push_back(AGameObject);
	AGameObject->SetParent(this);

	CEvent *AttachedEvent = new CEvent("Attached", this);	// I think we also need some other "Attached" event. For object that has been attached
	AttachedEvent->SetData("Name", AGameObject->GetName());
	CEventManager::Instance()->TriggerEvent(AttachedEvent);
}

void CGameObject::Detach(CGameObject* AGameObject)
{
	if (!AGameObject)
	{
		Log("ERROR", "NULL pointer passed to CGameObject::Detach");
		return;
	}

	vector<CGameObject *>::iterator it = find(Children.begin(), Children.end(), AGameObject);
	if (it == Children.end())
	{
		Log("ERROR", "Can't detach: object '%s' is not attached to object '%s'", AGameObject->GetName().c_str(), GetName().c_str());
		return;
	}

	Children.erase(it);
	AGameObject->Parent = NULL;

	CEvent *DetachedEvent = new CEvent("Detached", this);	// Same as for attached
	DetachedEvent->SetData("Name", AGameObject->GetName());
	CEventManager::Instance()->TriggerEvent(DetachedEvent);
}

CGameObject* CGameObject::GetParent() const
{
	return Parent;
}

void CGameObject::SetParent(CGameObject* AGameObject)
{
	if (AGameObject == this)
	{
		Log("ERROR", "Object '%s' can't be parent of itself", GetName().c_str());
		return;
	}

	if (Parent != NULL)
	{
		Parent->Detach(this);
	}

	Parent = AGameObject;

	if (Parent != NULL)
	{
		if (find(Parent->Children.begin(), Parent->Children.end(), this) == Parent->Children.end())
			Parent->Children.push_back(this);
	}
}

void CGameObject::PutIntoScene(CAbstractScene *AScene)
{
	assert(AScene != NULL);
	if (Scene != NULL)
		Scene->Remove(this);
	Scene = AScene;
	Scene->Add(this);
}

CAbstractScene* CGameObject::GetScene() const
{
	assert(Scene != NULL);
	return Scene;
}

void CGameObject::JustDoIt()
{

}

void CGameObject::Deserialize(CXMLNode *AXML)
{
	if (AXML->HasAttribute("ClassName"))
	{
		ClassName = AXML->GetAttribute("ClassName");
	}

	if (AXML->HasAttribute("Script"))
	{
		SetScript(CFactory::Instance()->Get<CScript>(AXML->GetAttribute("Script")));
	}
}

void CGameObject::SetScript(CScript *AScript)
{
	CLuaVirtualMachine::Instance()->RunScript(AScript);

	CEvent *CreateEvent = new CEvent("Create", this);
	CreateEvent->SetData("Name", GetName());
	CEventManager::Instance()->TriggerEvent(CreateEvent);
}

void CGameObject::ProcessEvent(const CEvent &AEvent)
{
	if (AEvent.GetName() == "Create")
	{
		if (AEvent.GetData<string>("Name") == Name)
		{
			CLuaVirtualMachine::Instance()->CreateLuaObject(ClassName.empty() ? Name : ClassName, Name, this);
			CLuaVirtualMachine::Instance()->CallMethodFunction(Name, "OnCreate");
		}
	}
	else
	{
		CLuaFunctionCall fc(Name, "On" + AEvent.GetName());
		fc.PushArgument(const_cast<CEvent *>(&AEvent));
		fc.Call();
	}
}

//////////////////////////////////////////////////////////////////////////
// CPlaceableComponent

CPlaceableComponent::CPlaceableComponent() : doIgnoreCamera(false), doMirrorHorizontal(false), doMirrorVertical(false), Transformation()
{
}

void CPlaceableComponent::SetAngle(float AAngle)
{
	float Angle = AAngle;
	if (Abs(AAngle) > 360.0f)
		Angle = AAngle - Sign(AAngle)*((static_cast<int>(AAngle) / 360) * 360.0f); 
	if (Angle < 0.0f)
		Angle = 360.0f + Angle;
	Transformation.SetAngle(Angle);
}

float CPlaceableComponent::GetAngle() const
{
	return Transformation.GetAngle();
}

void CPlaceableComponent::SetLayer(int Layer)
{
	Transformation.SetDepth(Layer == 0 ? 0.0f : Layer / 100.0f);	// @todo: get rid of magic number "100.0f"
}

float CPlaceableComponent::GetDepth() const
{
	return Transformation.GetDepth();
}

float CPlaceableComponent::GetScaling() const
{
	return Transformation.GetScaling();
}

void CPlaceableComponent::SetScaling(float AScaling)
{
	Transformation.SetScaling(AScaling);
}

int CPlaceableComponent::GetLayer() const
{
	return Transformation.GetDepth() * 100.0f;
}

const Vector2& CPlaceableComponent::GetPosition() const
{
	return Transformation.GetTranslation();
}

Vector2& CPlaceableComponent::GetPosition()
{
	return Transformation.GetTranslation();
}

void CPlaceableComponent::SetPosition(const Vector2 &APosition)
{
	Transformation.SetTranslation(APosition);
}

void CPlaceableComponent::SetTransformation(const CTransformation &ATransformation)
{
	Transformation = ATransformation;
}

CTransformation& CPlaceableComponent::GetTransformation()
{
	return Transformation;
}

const CTransformation& CPlaceableComponent::GetTransformation() const
{
	return Transformation;
}

bool CPlaceableComponent::isMirrorVertical() const
{
	return doMirrorVertical;
}

void CPlaceableComponent::SetMirrorVertical(bool MirrorOrNot)
{
	doMirrorHorizontal = MirrorOrNot;
}

bool CPlaceableComponent::isMirrorHorizontal() const
{
	return doMirrorHorizontal;
}

void CPlaceableComponent::SetMirrorHorizontal(bool MirrorOrNot)
{
	doMirrorHorizontal = MirrorOrNot;
}

bool CPlaceableComponent::isIgnoringParentTransform() const
{
	return doIgnoreCamera;
}

void CPlaceableComponent::SetIgnoreParentTransform(bool doIgnore)
{
	doIgnoreCamera = doIgnore;
}

void CPlaceableComponent::Deserialize(CXMLNode *AXML)
{
	CGameObject::Deserialize(AXML);

	if (AXML->HasAttribute("Angle"))
		SetAngle(from_string<float>(AXML->GetAttribute("Angle")));

	if (AXML->HasAttribute("Position"))
	{
		Vector2 APosition = V2_ZERO;
		istringstream iss(AXML->GetAttribute("Position"));
		vector<string> tokens;
		copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(tokens));
		if (tokens.size() == 2)
		{
			APosition.x = from_string<float>(tokens[0]);
			APosition.y = from_string<float>(tokens[1]);
			SetPosition(APosition);
		}
		else
			Log("WARNING", "Incorrect position in prototype");
	}

	if (AXML->HasAttribute("Scaling"))
		SetScaling(from_string<float>(AXML->GetAttribute("Scaling")));

	if (AXML->HasAttribute("Layer"))
		SetLayer(from_string<int>(AXML->GetAttribute("Layer")));

	// TODO: add more..	// Later
}

//////////////////////////////////////////////////////////////////////////
// CRenderableComponent

CRenderableComponent::CRenderableComponent(CModel *AModel /*= NULL*/) : Model(AModel), Visible(true)
{
	SetName("CRenderableComponent");	
}

const RGBAf& CRenderableComponent::GetColor() const
{
	return Configuration.Color;
}

RGBAf& CRenderableComponent::GetColor()
{
	return Configuration.Color;
}

void CRenderableComponent::SetColor(const RGBAf &AColor)
{
	Configuration.Color = AColor;
}

EBlendingMode CRenderableComponent::GetBlendingMode() const
{
	return Configuration.GetBlendingMode();
}

void CRenderableComponent::SetBlendingMode(EBlendingMode ABlendingMode)
{
	Configuration.SetBlendingMode(ABlendingMode);
}

CModel* CRenderableComponent::GetModel() const
{
	if (Model != NULL)
		Model->CheckLoad();
	return Model;
}

void CRenderableComponent::SetModel(CModel *AModel)
{
	if (Model == NULL)
		delete Model;
	Model = AModel;
}

void CRenderableComponent::SetConfiguration(const CRenderConfig &AConfiguraton)
{
	Configuration = AConfiguraton;
}

const CRenderConfig& CRenderableComponent::GetConfiguration() const
{
	return Configuration;
}

CRenderConfig& CRenderableComponent::GetConfiguration()
{
	return Configuration;
}

CRenderableComponent::~CRenderableComponent()
{
	CRenderManager::Instance()->Remove(this);
}

bool CRenderableComponent::GetVisibility() const
{
	return Visible;
}

void CRenderableComponent::SetVisibility(bool AVisible)
{
	Visible = AVisible;
}

void CRenderableComponent::Deserialize(CXMLNode *AXML)
{
	CGameObject::Deserialize(AXML);

	if (AXML->HasAttribute("Model"))
	{
		string ModelName = AXML->GetAttribute("Model");
		if (ModelName == "Circle")
		{
			float Radius = 64.0f; // DEFAULT_CIRCLE_RADIUS
			if (AXML->HasAttribute("Radius"))
				Radius = from_string<float>(AXML->GetAttribute("Radius"));
			unsigned Precision = 16; // DEF
			if (AXML->HasAttribute("Precision"))
				Precision = from_string<unsigned>(AXML->GetAttribute("Precision"));
			EModelType ModelType = MODEL_TYPE_LINES;
			if (AXML->HasAttribute("ModelType"))
				ModelType = CRenderManager::SelectModelTypeByStringIdentifier(AXML->GetAttribute("ModelType"));
			SetModel(CRenderManager::Instance()->CreateModelCircle(Radius, ModelType, Precision));
		}
		else if (ModelName == "Box")
		{
			float Width = 32.0f, Height = 32.0f;
			if (AXML->HasAttribute("Width"))
				Width = from_string<float>(AXML->GetAttribute("Width"));
			if (AXML->HasAttribute("Height"))
				Height = from_string<float>(AXML->GetAttribute("Height"));
			CTexture *Texture = NULL;
			if (AXML->HasAttribute("Texture"))
				Texture = CFactory::Instance()->Get<CTexture>(AXML->GetAttribute("Texture"));
			Vector2Array<4> TexCoords;
			if (Texture != NULL && AXML->HasAttribute("TexCoords"))
			{
				istringstream iss(AXML->GetAttribute("TexCoords"));
				vector<string> tokens;
				copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(tokens));
				if (tokens.size() == 4)
					for(unsigned i = 0; i < 4; i++)
						TexCoords[i] = from_string<float>(tokens[i]);
				else
					Log("WARNING", "Attribute 'TexCoords' has improper format");
			}
			if (Texture != NULL && !AXML->HasAttribute("TexCoords"))
				TexCoords = V2_QUAD_BIN;

			EModelType ModelType = MODEL_TYPE_LINES;
			if (AXML->HasAttribute("ModelType"))
				ModelType = CRenderManager::SelectModelTypeByStringIdentifier(AXML->GetAttribute("ModelType"));

			SetModel(CRenderManager::Instance()->CreateModelBox(Width, Height, ModelType, Texture, TexCoords));
			
		}
		else if (ModelName == "Line")
		{
			Vector2 p0 = V2_ZERO, p1 = V2_ZERO;
			if (AXML->HasAttribute("Points"))
			{
				istringstream iss(AXML->GetAttribute("Points"));
				vector<string> tokens;
				copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(tokens));
				if (tokens.size() == 4)
				{
					p0.x = from_string<float>(tokens[0]);
					p0.y = from_string<float>(tokens[1]);
					p1.x = from_string<float>(tokens[2]);
					p1.y = from_string<float>(tokens[3]);
				}
				else
					Log("WARNING", "Attribute 'Points' has improper format");
			}
			SetModel(CRenderManager::Instance()->CreateModelLine(p0, p1));
		}
		else
			SetModel(CFactory::Instance()->Get<CModel>(ModelName));
	}

	if (AXML->HasAttribute("Visible"))
		SetVisibility(from_string<bool>(AXML->GetAttribute("Visible")));

	if (AXML->HasAttribute("Color"))
	{
		RGBAf AColor = COLOR_WHITE;
		istringstream iss(AXML->GetAttribute("Color"));
		vector<string> tokens;
		copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(tokens));
		if (tokens.size() == 4)
		{
			AColor.r = from_string<float>(tokens[0]);
			AColor.g = from_string<float>(tokens[1]);
			AColor.b = from_string<float>(tokens[2]);
			AColor.a = from_string<float>(tokens[3]);
			SetColor(AColor);
		}
		else
			Log("WARNING", "Attribute 'Color' has improper format");
	}

	if (AXML->HasAttribute("BlendingMode"))
		SetBlendingMode(CRenderManager::SelectBlendingModeByIdentifier(AXML->GetAttribute("BlendingMode")));
}


//////////////////////////////////////////////////////////////////////////
// CText

CText::CText() : Font(CFontManager::Instance()->GetDefaultFont())
{	
	assert(Font != NULL);
	SetText("");
}

CText::CText(const string &AText) : Font(CFontManager::Instance()->GetDefaultFont())
{
	assert(Font != NULL);
	SetText(Characters);
}

CFont* CText::GetFont() const
{
	assert(Font != NULL);
	return Font;
}

string& CText::GetText()
{
	return Characters;
}

const string& CText::GetText() const
{
	return Characters;
}

void CText::SetFont(CFont *AFont)
{
	if (!AFont)
	{
		Log("ERROR", "NULL pointer passed to CText::SetFont");
		return;
	}

	Font = AFont;
	SetText(Characters);	// Пересчитать BOX, шрифт же меняется.  ///<<<< OLOLOLO --------
	_UpdateSelfModel();						///			|
}									///			|
									///			|
void CText::SetText(const string &AText)				///			|
{									///			|
	if (Characters == AText)	///<<<<	OLOLOLO	----------------------------------------|
		return;
	Characters = AText;
	//SetBox(CBox(GetPosition(), GetPosition() + Vector2(Font->GetStringWidth(Characters), Font->GetStringHeight(Characters))));
	_UpdateSelfModel();
}

CText& CText::operator =(const string &AText)
{
	SetText(AText);
	return *this;
}

/*
float CText::StringCoordToCursorPos(int x, int y) const
{
	if (Characters.length() == 0)
		return -1;
	Vector2 Local = Vector2(x, y) - GetPosition();
	if (Local.x < 0)
		return -1;

	for (int i = 0; i < Characters.length(); i++)
	{
		float SubstrWidth = Font->GetStringWidthEx(0, i, Characters);
		float SymbolCenterCoord = SubstrWidth - Font->GetDistance() - (Font->SymbolWidth((byte)Characters[i] - 32) / 2.0f);
		if (Local.x < SymbolCenterCoord)
			return (i - 1);
	}
	return (Characters.length() - 1.0f);
}
*/

CText::~CText()
{
	
}

unsigned char CText::operator[](unsigned int index) const
{
	return Characters[index];
}

unsigned int CText::Length() const
{
	return Characters.length();
}

void CText::Deserialize(CXMLNode *AXML)
{
	CRenderableComponent::Deserialize(AXML);

	if (AXML->HasAttribute("Text"))
	{
		SetText(AXML->GetAttribute("Text"));
	}
	if (AXML->HasAttribute("Font"))
	{
		SetFont(CFactory::Instance()->Get<CFont>(AXML->GetAttribute("Font")));
	}
}

void CText::_UpdateSelfModel()
{
	if (Model != NULL)
	{
		Model->SetPersistent(true);	// to prevent auto-unloading of destroyed object..
		Model->SetDestroyed();
		Model = NULL;
	}

	SetModel(CRenderManager::CreateModelText(this));
}


//////////////////////////////////////////////////////////////////////////
// CTimerComponent

CTimerComponent::CTimerComponent() : Enabled(false), Interval(0.0f), Accumulated(0.0f)
{
}

CTimerComponent::CTimerComponent(float AInterval) : Enabled(false), Interval(AInterval), Accumulated(0.0f)
{
}

void CTimerComponent::ProcessEvent(const CEvent &AEvent)
{
	if (AEvent.GetName() == "EveryFrame")
	{
		Accumulated += CEngine::Instance()->GetDeltaTime();
		if (Accumulated >= Interval)
		{
			CEvent *TimerTickEvent = new CEvent("TimerTick", this);
			TimerTickEvent->SetData("Name", GetName());
			CEventManager::Instance()->TriggerEvent(TimerTickEvent);
			Accumulated = 0.0f;
		}
	}
}

bool CTimerComponent::isEnabled() const
{
	return Enabled;
}

void CTimerComponent::SetEnabled(bool AEnabled)
{
	Enabled = AEnabled;
	if (Enabled)
		CEventManager::Instance()->Subscribe("EveryFrame", this);
	else
		CEventManager::Instance()->Unsubscribe("EveryFrame", this);

	Accumulated = 0.0f;
}

float CTimerComponent::GetInterval() const
{
	return Interval;
}

void CTimerComponent::SetInterval(float AInterval)
{
	Interval = AInterval;
	Accumulated = 0.0f;	// is it needed?.. or make separate Reset(), SetElapsed() or whatever?..
}

void CTimerComponent::Deserialize(CXMLNode *AXML)
{
	CGameObject::Deserialize(AXML);

	if (AXML->HasAttribute("Interval"))
	{
		SetInterval(from_string<float>(AXML->GetAttribute("Interval")));
	}

	if (AXML->HasAttribute("Enabled"))
	{
		SetEnabled(from_string<bool>(AXML->GetAttribute("Enabled")));
	}
}

//////////////////////////////////////////////////////////////////////////
// CPrototype

CPrototype::CPrototype()
{
	Persistent = true; // persistent by default, at least now..
}

CPrototype::~CPrototype()
{
	Unload();
}

bool CPrototype::Load()
{
	if (Loaded)
		return true;

	if (Source == LOAD_SOURCE_FILE)
	{
		XML.LoadFromFile(Filename);
	}
	else if (Source == LOAD_SOURCE_MEMORY)
	{
		XML.LoadFromMemory(MemoryLoadData, MemoryLoadLength);
	}
	else
	{
		Log("ERROR", "Can't load prototype: no load source specified");
		return false;
	}

	if (XML.Root.First("Prototype")->IsErroneous() || !XML.Root.First("Prototype")->HasAttribute("Name"))
	{
		Log("ERROR", "Prototype named '%s' has invalid format", Name.c_str());
		return false;
	}

	CResource::BasicLoad();

	return true;
}

void CPrototype::Unload()
{
	if (!Loaded)
		return;

	CResource::BasicUnload();
}

CXMLNode* CPrototype::GetRootNode()
{
	CheckLoad();
	return XML.Root.First("Prototype");
}

