#include "2de_RenderableComponent.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// CRenderableComponent

	CRenderableComponent::CRenderableComponent(CModel *AModel /*= NULL*/) : Model(AModel), Visible(true)
	{
		SetName("CRenderableComponent");	
	}

	CRenderableComponent::~CRenderableComponent()
	{
	}

	CRenderableComponent* CRenderableComponent::Clone(const string &ACloneName /*= ""*/) const
	{
		return CloneHelper(new CRenderableComponent(*this), ACloneName);
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
				SetModel(RenderManager->CreateModelCircle(Radius, ModelType, Precision));
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
					Texture = Factory->Get<CTexture>(AXML->GetAttribute("Texture"));
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
					TexCoords = Const::Graphics::V2_QUAD_BIN;

				EModelType ModelType = MODEL_TYPE_LINES;
				if (AXML->HasAttribute("ModelType"))
					ModelType = CRenderManager::SelectModelTypeByStringIdentifier(AXML->GetAttribute("ModelType"));

				SetModel(RenderManager->CreateModelBox(Width, Height, ModelType, Texture, TexCoords));
				
			}
			else if (ModelName == "Line")
			{
				Vector2 p0 = Const::Math::V2_ZERO, p1 = Const::Math::V2_ZERO;
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
				SetModel(RenderManager->CreateModelLine(p0, p1));
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
				Factory->Add(LinesModel);
				LinesModel->DisableLoading();
				SetModel(LinesModel);
			}
			else
				SetModel(Factory->Get<CModel>(ModelName));
		}

		if (AXML->HasAttribute("Visible"))
			SetVisibility(from_string<bool>(AXML->GetAttribute("Visible")));

		if (AXML->HasAttribute("Color"))
		{
			RGBAf AColor = Const::Color::WHITE;
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

		CDebugBoxComponent *DebugBox = dynamic_cast<CDebugBoxComponent*>(GetChild(0));
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

}	// namespace Deku2D
