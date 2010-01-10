#ifndef	_FONT_EDITOR_H_
#define	_FONT_EDITOR_H_

#include "2de_Engine.h"

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

class CFontEditor : public CRenderObject, public CUpdateObject
{
	enum 
	{
		GRIP_TOOL,
		SELECT_TOOL,
		MOVE_TOOL,
	};
public:
	Vector2		MousePosition;
	Vector2		MouseDelta;
	bool		isGripToolEnabled;
	bool		doGripping;
	int			wheight;
	int			wwidth;

	float		Zoom;
	CTexture	*FontTexture;
	CFont		*Font;
	char		CurrentSymbol;
	int			OffsetX;
	int			OffsetY;

	CEdit		*edFontTextureName;
	CEdit		*edFontname;
	CEdit		*edCurrentCymbol;
	CLabel		*lblSampleText;

	void SetZoom(float AZoom);
	bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);
	CFontEditor();
	bool Render();
	bool Update(float dt);
	friend bool LoadFont(CObject *Caller);
	friend bool SaveFont(CObject *Caller);
	friend bool LoadTexture(CObject *Caller);  // Опять же не Load() а Acquire().;
	friend bool ExitFontEditor(CObject *Caller);
	friend bool ShowTestPhrase(CObject *Caller);

};


#endif	// _FONT_EDITOR_H_