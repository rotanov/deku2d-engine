#include "2de_Components.h"
#include "2de_Engine.h"

//////////////////////////////////////////////////////////////////////////
// CGameObject

CGameObject::CGameObject() : Parent(NULL), Scene(NULL), Script(NULL), Prototype(false), Created(false), Active(true), Dead(false), Enabled(true)
{
	PutIntoScene(CSceneManager::Instance()->GetCurrentScene());
	CEventManager::Instance()->Subscribe("Create", this);
}

CGameObject::~CGameObject()
{
	Scene->Remove(this);
	//	Note, that here we adding children to parent in reverse order, i think it's not that important for now.
	//	That order is not important at all. Overall comment should be removed.
	while (!Children.empty())
		Children.back()->SetParent(Parent);

	SetParent(NULL);

	CLuaVirtualMachine::Instance()->DestroyLuaObject(*this);
}

CGameObject* CGameObject::Clone() const
{
	return CloneHelper(new CGameObject(*this));
}

void CGameObject::Attach(CGameObject *AGameObject)
{
	if (!AGameObject)
	{
		Log("ERROR", "NULL pointer passed to CGameObject::Attach");
		return;
	}

	AGameObject->SetParent(this);
}

void CGameObject::Detach(CGameObject *AGameObject)
{
	if (!AGameObject)
	{
		Log("ERROR", "NULL pointer passed to CGameObject::Detach");
		return;
	}

	AGameObject->SetParent(NULL);
}

void CGameObject::Detach(unsigned index)
{
	if (index >= Children.size())
	{
		Log("ERROR", "CGameObject::Detach: index out of bounds");
		return;
	}

	CGameObject *AGameObject = *(Children.begin() + index);
	AGameObject->SetParent(NULL);
}

void CGameObject::SetDestroyedSubtree()
{
	CGameObject::_DestroySubtree(this);
}

const string& CGameObject::GetClassName() const
{
	return ClassName;
}

void CGameObject::SetClassName(const string &AClassName)
{
	ClassName = AClassName;
}

CGameObject* CGameObject::GetParent() const
{
	return Parent;
}

void CGameObject::SetParent(CGameObject *AGameObject)
{
	if (AGameObject == this)
	{
		Log("ERROR", "Object '%s' can't be parent of itself", GetName().c_str());
		return;
	}

	if (Parent != NULL)
	{
		ChildrenIterator it = find(Parent->Children.begin(), Parent->Children.end(), this);

		if (it != Parent->Children.end())
		{
			Parent->Children.erase(it);

			CEvent *DetachedEvent = new CEvent("Detached", this);
			DetachedEvent->SetData("Name", GetName());
			CEventManager::Instance()->TriggerEvent(DetachedEvent);
		}
	}

	Parent = AGameObject;

	if (Parent != NULL)
	{
		if (find(Parent->Children.begin(), Parent->Children.end(), this) == Parent->Children.end()) // probably remove this find..
		{
			Parent->Children.push_back(this);

			CEvent *AttachedEvent = new CEvent("Attached", this);
			AttachedEvent->SetData("Name", GetName());
			CEventManager::Instance()->TriggerEvent(AttachedEvent);
		}
	}

	if (Created)
		UpdateParentAndProtoFields();
}

CAbstractScene* CGameObject::GetScene() const
{
	assert(Scene != NULL);
	return Scene;
}

void CGameObject::PutIntoScene(CAbstractScene *AScene)
{
	assert(AScene != NULL);
	if (Scene != NULL)
		Scene->Remove(this);
	Scene = AScene;
	Scene->Add(this);
}

CGameObject* CGameObject::GetChild(unsigned index)
{
	return Children[index];
}

CGameObject* CGameObject::GetObjectByLocalName(const string &AName)
{
	LNOMType::const_iterator it = LocalNameObjectMapping.find(AName);
	if (it == LocalNameObjectMapping.end()) {
		return NULL;
	}

	return it->second;
}

unsigned CGameObject::GetChildCount()
{
	return Children.size();
}

CGameObject* CGameObject::FindFirstOfClass(const string &AClassName, bool ExceedPrototype /*= false*/)
{
	queue<CGameObject *> SearchQueue;
	CGameObject *current = NULL;

	SearchQueue.push(this);

	while (!SearchQueue.empty())
	{
		current = SearchQueue.front();
		SearchQueue.pop();

		if (current->ClassName == AClassName && current != this)
			return current;

		for (CGameObject::ChildrenIterator it = current->Children.begin(); it != current->Children.end(); ++it)
		{
			if (ExceedPrototype || !(*it)->IsPrototype())
				SearchQueue.push(*it);
		}
	}

	return NULL;
}

bool CGameObject::IsActive() const
{
	return Active;
}

void CGameObject::SetActive(bool AActive)
{
	Active = AActive;
}

bool CGameObject::IsPrototype() const
{
	return Prototype;
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

void CGameObject::FinalizeCreation()
{
	CLuaVirtualMachine::Instance()->SetLocalNamesFields(this);
	UpdateParentAndProtoFields();
	Created = true;
}

void CGameObject::ProcessEvent(const CEvent &AEvent)
{
	CLuaFunctionCall fc(Name, "On" + AEvent.GetName());
	fc.PushArgument(const_cast<CEvent *>(&AEvent));
	fc.Call();
}

CScript* CGameObject::GetScript() const
{
	return Script;
}

void CGameObject::SetScript(CScript *AScript)
{
	Script = AScript;

	CLuaVirtualMachine::Instance()->DestroyLuaObject(*this);
	CLuaVirtualMachine::Instance()->RunScript(AScript);
	CreateLuaObject();

	if (Created)
	{
		CLuaVirtualMachine::Instance()->SetLocalNamesFields(this);
		UpdateParentAndProtoFields();
	}
}

void CGameObject::DFSIterate(CGameObject *Next, IVisitorBase *Visitor)
{
	if (!Next->IsActive())
		return;
	Next->AcceptOnEnter(*Visitor);
	for (unsigned i = 0; i < Next->GetChildCount(); i++)
		DFSIterate(Next->Children[i], Visitor);
	Next->AcceptOnLeave(*Visitor);
}

void CGameObject::JustDoIt()
{

}

CGameObject::CGameObject(const CGameObject &AGameObject)
{
	Parent = NULL;
	Scene = AGameObject.Scene;
	ClassName = AGameObject.ClassName;
	Prototype = AGameObject.Prototype;
	Created = false;
	Active = AGameObject.Active;

	Dead = false;
	Enabled = true;

	PutIntoScene(Scene);
	CEventManager::Instance()->Subscribe("Create", this);
	Script = AGameObject.Script;
}

void CGameObject::_DestroySubtree(CGameObject *NextObject)
{
	if (NextObject == NULL)
		return;
	for (vector<CGameObject *>::iterator i = NextObject->Children.begin(); i != NextObject->Children.end(); ++i)
		_DestroySubtree(*i);
	NextObject->SetDestroyed();
}

CGameObject* CGameObject::FindPrototype()
{
	CGameObject *result = Parent;

	while (result && !result->Prototype)
		result = result->Parent;

	return result;
}

void CGameObject::UpdateParentAndProtoFields()
{
	// this recursive call may greatly affect performance, but it's needed because of creation order to make parentProto work
	for (ChildrenIterator it = Children.begin(); it != Children.end(); ++it)
		(*it)->UpdateParentAndProtoFields();

	CLuaVirtualMachine::Instance()->SetReferenceField(this, "parent", Parent);

	CGameObject *proto = FindPrototype();
	CLuaVirtualMachine::Instance()->SetReferenceField(this, "proto", proto);

	CGameObject *ParentProto = NULL;
	if (proto)
		ParentProto = proto->FindPrototype();
	CLuaVirtualMachine::Instance()->SetReferenceField(this, "parentProto", ParentProto);
}

void CGameObject::CreateLuaObject()
{
	if (!CLuaVirtualMachine::Instance()->IsObjectExists(Name))
		CLuaVirtualMachine::Instance()->CreateLuaObject(ClassName.empty() ? Name : ClassName, Name, this);
}

//////////////////////////////////////////////////////////////////////////
// CPlaceableComponent

CPlaceableComponent::CPlaceableComponent() : Box(), doIgnoreCamera(false), doMirrorHorizontal(false), doMirrorVertical(false), Transformation()
{
	SetName("CPlaceableComponent");	
}

CPlaceableComponent* CPlaceableComponent::Clone() const
{
	return CloneHelper(new CPlaceableComponent(*this));
}

void CPlaceableComponent::SetAngle(float AAngle)
{
	float Angle = AAngle;
	if (std::fabs(AAngle) > 360.0f)
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

const CBox& CPlaceableComponent::GetBox() const
{
	return Box;
}

void CPlaceableComponent::SetBox(const CBox &ABox)
{
	Box = ABox;
}

void CPlaceableComponent::UpdateBox(const CBox& ABox)
{
	Box.Union(ABox);
	if (typeid(*this) == typeid(CDebugBoxComponent))
		return;
#if defined(_DEBUG) && !defined(DISABLE_DEBUG_BOXES)
	CDebugBoxComponent *DebugBox = dynamic_cast<CDebugBoxComponent*>(Children[0]);
	if (NULL != DebugBox)
	{
		if (DebugBox->GetModel() == NULL)
			DebugBox->SetModel(CRenderManager::CreateModelBox(Box.Width(), Box.Height(), MODEL_TYPE_LINES));
		{
			DebugBox->Model->Vertices[0] = Vector2(Box.Min.x, Box.Min.y);
			DebugBox->Model->Vertices[1] = Vector2(Box.Max.x, Box.Min.y);

			DebugBox->Model->Vertices[2] = Vector2(Box.Max.x, Box.Min.y);
			DebugBox->Model->Vertices[3] = Vector2(Box.Max.x, Box.Max.y);

			DebugBox->Model->Vertices[4] = Vector2(Box.Max.x, Box.Max.y);
			DebugBox->Model->Vertices[5] = Vector2(Box.Min.x, Box.Max.y);

			DebugBox->Model->Vertices[6] = Vector2(Box.Min.x, Box.Max.y);
			DebugBox->Model->Vertices[7] = Vector2(Box.Min.x, Box.Min.y);
		}
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
// CRenderableComponent

CRenderableComponent::CRenderableComponent(CModel *AModel /*= NULL*/) : Model(AModel), Visible(true)
{
	SetName("CRenderableComponent");	
}

CRenderableComponent::~CRenderableComponent()
{
	if (Model != NULL && !CEngine::Instance()->isFinalizing())
	{
		Model->SetPersistent(true);	// to prevent auto-unloading of destroyed object..
		Model->SetDestroyed();
		Model = NULL;
	}

	//CRenderManager::Instance()->Remove(this);
}

CRenderableComponent* CRenderableComponent::Clone() const
{
	return CloneHelper(new CRenderableComponent(*this));
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
	if (Model != NULL)
	{
		Model->SetPersistent(true);	// to prevent auto-unloading of destroyed object..
		Model->SetDestroyed();
		Model = NULL;
	}
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
		else if (ModelName == "Line loop" && AXML->HasAttribute("Points"))
		{
			Vector2 *Vertices = NULL;
			istringstream iss(AXML->GetAttribute("Points"));
			vector<string> tokens;
			copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(tokens));
			Vertices = new Vector2 [(tokens.size() / 2) * 2];
			for(unsigned i = 0; i < tokens.size(); i += 2)
			{
				Vertices[i] = Vector2(from_string<float>(tokens[i]), from_string<float>(tokens[i + 1]));
				Vertices[i + 1] = Vector2(from_string<float>(tokens[(i + 2) % tokens.size()]), from_string<float>(tokens[(i + 3) % tokens.size()]));
			}
			CModel *LinesModel = new CModel(MODEL_TYPE_LINES, 0, (tokens.size() / 2) * 2, Vertices);
			CFactory::Instance()->Add(LinesModel);
			LinesModel->DisableLoading();
			SetModel(LinesModel);
		}
		else
			SetModel(CFactory::Instance()->Get<CModel>(ModelName));
	}

	if (AXML->HasAttribute("Visible"))
		SetVisibility(from_string<bool>(AXML->GetAttribute("Visible")));

	if (AXML->HasAttribute("Color"))
	{
		RGBAf AColor = color::WHITE;
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

const CBox& CRenderableComponent::GetBox() const
{
	assert(Model != 0);
	return Model->GetBox();
}

void CRenderableComponent::SetBox(const CBox& ABox)
{
	Box = ABox;
}

void CRenderableComponent::UpdateBox(const CBox& ABox)
{
	Box.Union(ABox);
	if (typeid(*this) == typeid(CDebugBoxComponent))
		return;
#if defined(_DEBUG) && !defined(DISABLE_DEBUG_BOXES)

	CDebugBoxComponent *DebugBox = dynamic_cast<CDebugBoxComponent*>(Children[0]);
	if (NULL != DebugBox)
	{
		if (DebugBox->GetModel() == NULL)
			DebugBox->SetModel(CRenderManager::CreateModelBox(Box.Width(), Box.Height(), MODEL_TYPE_LINES));
		{
			DebugBox->Model->Vertices[0] = Vector2(Box.Min.x, Box.Min.y);
			DebugBox->Model->Vertices[1] = Vector2(Box.Max.x, Box.Min.y);

			DebugBox->Model->Vertices[2] = Vector2(Box.Max.x, Box.Min.y);
			DebugBox->Model->Vertices[3] = Vector2(Box.Max.x, Box.Max.y);

			DebugBox->Model->Vertices[4] = Vector2(Box.Max.x, Box.Max.y);
			DebugBox->Model->Vertices[5] = Vector2(Box.Min.x, Box.Max.y);

			DebugBox->Model->Vertices[6] = Vector2(Box.Min.x, Box.Max.y);
			DebugBox->Model->Vertices[7] = Vector2(Box.Min.x, Box.Min.y);
		}
	}
#endif
}

CRenderableComponent::CRenderableComponent(const CRenderableComponent &ARenderableComponent) : CGameObject(ARenderableComponent)
{
	WorldTransform = ARenderableComponent.WorldTransform;
	Box = ARenderableComponent.Box;
	Model = ARenderableComponent.Model;
	Configuration = ARenderableComponent.Configuration;
	Visible = ARenderableComponent.Visible;

	//*this = ARenderableComponent;
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

CText::~CText()
{
}

CText* CText::Clone() const
{
	return CloneHelper(new CText(*this));
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
	_UpdateSelfModel(); // Recalculate model and box with new font.
}

void CText::SetText(const string &AText)
{
	if (Characters == AText)
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

unsigned char CText::operator[](unsigned index) const
{
	return Characters[index];
}

unsigned CText::Length() const
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

CText::CText(const CText &AText) : CRenderableComponent(AText)
{
	Font = AText.Font;
	SetText(AText.GetText());
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

CTimerComponent::CTimerComponent(float AInterval /*= 0.0f*/) : Enabled(false), Interval(AInterval), Accumulated(0.0f)
{
}

CTimerComponent* CTimerComponent::Clone() const
{
	return CloneHelper(new CTimerComponent(*this));
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

