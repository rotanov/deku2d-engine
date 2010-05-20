#ifndef	_FONT_EDITOR_H_
#define	_FONT_EDITOR_H_

#include "2de_Engine.h"
#include "2de_Gui.h"

// @todo: Extract some defines to config

// Константы, определяющие расположение некоторых элементов интерфейса.
const int				INTERFACE_OFFSET_X	= 128;
const int				LEFT_MARGIN			= 8;
const int				EDIT_WIDTH			= INTERFACE_OFFSET_X - LEFT_MARGIN*3;
const int				EDIT_HEIGHT			= 32;
const int				BUTTON_WIDTH		= INTERFACE_OFFSET_X - LEFT_MARGIN*3;
const int				BUTTON_HEIGHT		= 32;
// Константы, определяющие цвет элементов интерфейса.
const RGBAf				COLOR_FIRST(.4f, .4f, .4f, 1.0f);
const RGBAf				COLOR_SECOND(.5f, .5f, .6f, 1.0f);
const RGBAf				COLOR_THIRD(0.6f, 0.7f, 0.8f, 0.5f);
const RGBAf				COLOR_FOURTH(0.9f, 0.8f, 0.2f, 1.0f);
// Ограничения:
const float ZOOM_MAX = 16.0f;
const float ZOOM_MIN = 0.5f;
const float ZOOM_STEP = 0.5f;

// прототипы, чёрт подери... не работает без них у меня.. то что они там где-то френды ещё не значит, что они видны при присваивании их адресов...
bool LoadFont(CObject *Caller);
bool SaveFont(CObject *Caller);
bool LoadTexture(CObject *Caller);
bool ExitFontEditor(CObject *Caller);
bool ShowTestPhrase(CObject *Caller);
bool GoToPrevChar(CObject *Caller);
bool GoToNextChar(CObject *Caller);
bool ExposeRect(CObject *Caller);

class CFontEditor : public CRenderObject, public CUpdateObject
{
	enum EEditorState
	{
		ES_GRIP_TOOL,
		ES_SELECT_TOOL,
		ES_MOVE_TOOL,
		ES_NONE,
	};
	enum ESelectionCornerKind
	{
		SCK_LEFT_BOTTOM,
		SCK_RIGHT_BOTTOM,
		SCK_RIGHT_TOP,
		SCK_LEFT_TOP,
		SCK_CENTER,
		SCK_NONE,
	};
public:
	Vector2					MousePosition;
	Vector2					MouseDelta;
	int						WindowWidth;
	int						WindowHeight;
	Vector2					Offset;
	float					Zoom;
	CTexture				*FontTexture;
	CFont					*Font;
	int						CurrentSymbol;
	CEdit					*edFontTextureName;
	CEdit					*edFontname;
	CEdit					*edCurrentCymbol;
	CLabel					*lblSampleText;
	CLabel					*lblCharachterSelectedASCIIIndex;
	CPrimitiveRender		PRender;
	Vector2					SelectionBoxes[SCK_NONE * 4];
	ESelectionCornerKind	CornerKind;
	EEditorState			State;
	EEditorState			PreviousState;

	CFontEditor();
	~CFontEditor();
	void SetZoom(float AZoom);
	void SetSelectedBoxTo(int Index);
	bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);	
	void Render();
	void Update(float dt);
	
	friend bool LoadFont(CObject *Caller);
	friend bool SaveFont(CObject *Caller);
	friend bool LoadTexture(CObject *Caller);
	friend bool ExitFontEditor(CObject *Caller);
	friend bool ShowTestPhrase(CObject *Caller);
	friend bool GoToPrevChar(CObject *Caller);
	friend bool GoToNextChar(CObject *Caller);
	friend bool ExposeRect(CObject *Caller);
};


#endif	// _FONT_EDITOR_H_
