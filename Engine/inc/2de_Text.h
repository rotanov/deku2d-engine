#ifndef _2DE_TEXT_H_
#define _2DE_TEXT_H_

namespace Deku2D
{
	/**
	*	Text.
	*	Place for comment.
	*/

	class CText : public CRenderableComponent
	{
	public:
		// Treat as CRenderableComponent for now, if not uncomment the following and support visitors with one more branch
		//D2D_DECLARE_VISITABLE()

		CText();
		CText(const string &AText);
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

	/**
	* CTimerComponent - component that represents a timer.
	*/

	class CTimerComponent : public CGameObject
	{
	public:
		D2D_DECLARE_VISITABLE()

		CTimerComponent(float AInterval = 0.0f);

		CTimerComponent* Clone(const string &ACloneName = "") const;

		void ProcessEvent(const CEvent &AEvent);

		bool isEnabled() const;
		void SetEnabled(bool AEnabled);

		float GetInterval() const;
		void SetInterval(float AInterval);

		void Deserialize(CXMLNode *AXML);

	private:
		bool Enabled;
		float Interval;
		float Accumulated;

	};

}	//	namespace Deku2D

#endif	//	_2DE_TEXT_H_
