#ifndef _2DE_TEXT_H_
#define _2DE_TEXT_H_

#include "2de_RenderableComponent.h"

namespace Deku2D
{
	/**
	*	Text.
	*	Place for comment.
	*/

	class CText : public CRenderableComponent
	{
		D2D_INJECT_TYPE_INFO(CText);

	public:
		// Treat as CRenderableComponent for now, if not uncomment the following and support visitors with one more branch
		//D2D_DECLARE_VISITABLE()

		CText();
		~CText();

		CText* Clone(const string &ACloneName = "") const;

		CFont* GetFont() const;
		string& GetText();
		const string& GetText() const;
		void SetFont(CFont *AFont);
		void SetText(const string &AText);
		CText& operator =(const string &AText);
		unsigned char operator [] (unsigned index) const;
		unsigned Length() const;
		void Deserialize(CXMLNode *AXML);

	protected:
		CText(const CText &AText);

	private:
		string Characters;
		CResourceRefCounter<CFont> Font;

		void _UpdateSelfModel();
	};

}	//	namespace Deku2D

#endif	//	_2DE_TEXT_H_
