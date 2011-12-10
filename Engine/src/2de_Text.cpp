#include "2de_Text.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// CText

	CText::CText() : Font(FontManager->GetDefaultFont())
	{
		ClassName = "Text";
		assert(Font != NULL);
	}

	CText::~CText()
	{
	}

	CText* CText::Clone(const string &ACloneName /*= ""*/) const
	{
		return CloneHelper(new CText(*this), ACloneName);
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
			SetFont(Factory->Get<CFont>(AXML->GetAttribute("Font")));
		}
	}

	CText::CText(const CText &AText) : CRenderableComponent(AText)
	{
		Font = AText.Font;
		SetText(AText.GetText());
	}

	void CText::_UpdateSelfModel()
	{
		/*if (Model != NULL)
		{
			Model->SetPersistent(true);	// to prevent auto-unloading of destroyed object..
			Model->SetDestroyed();
			Model = NULL;
		}*/

		SetModel(CRenderManager::CreateModelText(this));
	}

}	// namespace Deku2D
