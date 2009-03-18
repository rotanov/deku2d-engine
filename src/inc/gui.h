#ifndef GUI_H
#define GUI_H

#include "event.h"
#include "CoreUtils.h"
#include "OpenglUtils.h"

/*
class CGraphObj
child of class CObject inherits all of it's propiertes
*/

//DEFINES

#define MAX_ITEMS 100
#define CHECK_NAME true//if true each object on creation will be checked

//Styles 0xAABBSSSS
//AA : Object type (max 255)
//BB : Draw style
//SSSS : style number
//Draw styles

//#fixed defines
#define GUI_OBJFORM			0x0
#define GUI_OBJBUTTON		0x1
#define GUI_OBJEDIT			0x2
#define STYLE_OBJMASK		0x00ff
#define STYLE_OBJDUMMY		0x00ff
#define STYLE_OBJFORM		0x0000
#define STYLE_OBJBUTTON		0x0001
#define STYLE_OBJEDIT		0x0002
#define STYLE_ADDTOLISTMASK 0xff00
#define STYLE_DONOTADD		0x0100
//#end of fixed defines

#define STYLE_DRAWMASK		0x00ff0000
#define STYLE_DRAWNONE		0x00000000
#define STYLE_DRAWNORMAL	0x00010000
#define STYLE_DRAWSTYLE		0x00020000

//Objects

#define STYLE_NONE			0x00000000
#define STYLE_MASK			0x0000ffff
#define XSTYLE_MASK			0x00ffffff

//animations
#define TLEFT_CORNER				0x10
#define TRIGHT_CORNER				0x20
#define BRIGHT_CORNER				0x30
#define BLEFT_CORNER				0x40
#define LEFT_SIDE					0x50
#define TOP_SIDE					0x60
#define RIGHT_SIDE					0x70
#define BOTTOM_SIDE					0x80

#define ANIM_BUTTON_MOUSE_OVER		0x01
#define ANIM_BUTTON_MOUSE_ON		0x02
#define ANIM_BUTTON_MOUSE_CLICK		0x03

//Align

#define ALIGN_LEFT					0x01
#define ALIGN_CENTER				0x02
#define ALIGN_RIGHT					0x03
#define ALIGN_MASK					0x0f
#define ALIGN_TOP					0x10
#define ALIGN_MIDDLE				0x20
#define ALIGN_BOTTOM				0x30
#define VALIGN_MASK					0xf0

//input defines

#define GUI_MBDOWN					0x0
#define GUI_MBUP					0x1
#define GUI_KEYDOWN					0x2
#define GUI_KEYUP					0x3
#define GUI_MBCLICK					0xf

#define GUI_BOTTOM					0.0f
#define GUI_TOP						1.0f


#define GUI_DRAWFORMBODY			0x0
#define GUI_DRAWFORMHEADER			0x1
#define GUI_FORMRESIZABLE			0x2
#define GUI_FORMCANMOVE				0x3
#define _GUI_DRAWFORMBODY			0x4
#define __GUI_DRAWFORMBODY			0x5


#define GUI_DELAY0					500
#define GUI_DELAY1					250

bool GUIKeyUp(char key, SDLKey sym);
bool GUIKeyDown(char key, SDLKey sym);
bool GUIMbDown(int x, int y, byte btn);
bool GUIMbUp(int x, int y, byte btn);
bool GUIMbMove(int x, int y, byte btn);
bool MouseIn(int l, int t, int w, int h);
bool MbState(byte btn);


bool GUIInit(char *fname);
void GUIStep();
void GUIDraw();

class CGUIRenderer : public CRenderObject, public CUpdateObject
{
public:
	CGUIRenderer();
	~CGUIRenderer();
	bool Render(); 
	bool Update(float dt);
	static CObject*	NewRenderer()
	{
		CRenderObject *res = new CGUIRenderer;
//		res->type = T_RENDERABLE|T_UPDATABLE;
		return res;
	}
};


class CGraphObj : public CObject//Update
{
public:
	CGraphObj()
	{
		Align = Left = Top = Width = Height = MinWidth = MinHeight = ClientHeight = ClientWidth = 0;
		Parent = NULL;
		Visible= false;
		Enabled = true;
		Caption = "";
		fnt = NULL;
	}
/*	static CObject*	NewWidget()
	{
		CRenderObject *res = newWidget();
		//		res->type = T_RENDERABLE|T_UPDATABLE;
		return res;
	}*/
	virtual void			Draw(){}
	virtual void			Step(){}
	virtual void			DrawText(){}
	virtual void			MouseProcess(byte btn, byte event){}
	virtual void			KeyProcess(SDLKey &btn, byte event){}
	unsigned int			getStyle(){return Style;}
	void					setStyle(unsigned int _Style);
	bool					Visible;
	bool					Enabled;
	CGraphObj				*Parent;
	int						Left, Top, Width, Height, MinWidth, MinHeight, 
							MaxWidth, MaxHeight, ClientHeight, ClientWidth;
	byte					Align;
	char					type[100];
	string					Caption;
	void					ReadComponent(CFile f);
	void					SetStyle(unsigned int style);
	CFont					*fnt;
	int						Tag;
	int						Cursor;
	string					Font;
	int						Valign, Halign;
	int						Kind;
	void					ApplyStyle(CGraphObj *wdg);
	float					ZDepth;
	int						GetLeft();
	int						GetTop();
	bool					SetFont(string FontName);
	virtual bool			Update(float dt){return 1;};
	static CObject*	NewRenderer()
	{
		CRenderObject *res = new CGUIRenderer;
		//		res->type = T_RENDERABLE|T_UPDATABLE;
		return res;
	}
protected:
	unsigned int			Style;					
};
typedef CGraphObj CWidget;
typedef CGraphObj *PWidget;
//typedef bool (*ObjCompCall)(PWidget, PWidget);
//typedef bool (*ObjCall)(PWidget);

//CGObj List


struct Style	// Структура, это класс со всеми членами в паблик по дефолту. Где префикс С блять?!?!
{
	unsigned short int Data[10];
};

class CGUIScheme : public CObject
{
public:
	CGUIScheme(char* fname, char* tname);
	~CGUIScheme();
	void					Release();
	PWidget					objects[200]; // с маленькой буквы
	int						ObjCount;
	Style					Styles[200]; // а тут с большой, блять, Петя, что за хуйня. Хотя это мелочи, но ты же сам понимаешь. О какой тут архитектуре речь, если ты...
	int						StyleCount;
	void					BeginUI();
	void					EndUI();
	void					_Draw(int StInd, byte target, int x, int y, float z, int w, int h);
	bool					Draw(int StInd, int x, int y, float z, int w, int h);
	int						GetXOffset(int StInd);
	int						GetYOffset(int StInd);
	int						GetCWidth(int StInd, int w);
	int						GetCHeight(int StInd, int h);
	void					AssignStyle(PWidget obj, int ObjKind, int StyleInd);
	int						GetFormInd(int ind);
	CGLImageData			ImgData;
	void					CopyWidget(int kind, PWidget wdg);
	PWidget					CreateWidget(int kind);
protected:
	bool					Drawing;
};


bool						LoadGUI(char *fname);
//void BeginUI();
//void EndUI();

//////////////////////////////////////////////////////////////////////////
// !!!! BAD !!! __forceinline is Microsoft specific
// делай как в mathutils.h
//////////////////////////////////////////////////////////////////////////

__INLINE bool ISFORM(CGraphObj *obj){return ((obj->getStyle() & STYLE_OBJMASK) == STYLE_OBJFORM)?true:false;}


PWidget						newWidget(char *name, unsigned int Style);
void						clearWidget(PWidget res);
void						FormAddWidget(PWidget obj, PWidget form);
void						showWidget(PWidget obj);
void						hideWidget(PWidget obj);
void						enableWidget(PWidget obj);
void						disableWidget(PWidget obj);
void						setWidgetStyle(PWidget obj, unsigned int Style);//NOTE if you are trying to change 
																			//object type, you'll get nothing=)
PWidget						getWidget(char *name);
PWidget						addForm(char *Fname);

class Cdummy : public CGraphObj
{
public:
	Cdummy(){}
	void					Draw(){}
	void					Step(){}
};
class CControl : public CGraphObj
{
public:
	CControl();
	~CControl(){};
	CObjectList				Items;
	virtual void			Draw(){};
	virtual void			Step(){};
};
//may be i'll cancel custom head bar
//color Color
//style Style
//int Height
//string Font
//# -Body----------------------
//-Body
//style Style
//int MinHeight
//int MinWidth
//int MaxHeight
//int MaxWidth
//int Cursor
class CForm : public CControl
{
public:
	CForm(unsigned int _Style);
//	CControl				Body;
	bool					LoadForm(char *Fname);
	void					Draw();
	void					DrawText();
	bool					Update(float dt);
	int						HeaderStyle;
	int						BodyStyle;
	DWORD					HeaderColor;
	int						HeaderHeight;
	byte					Properties[6];
};

class CButton : public CGraphObj
{
public:
	CButton(unsigned int _Style);
	~CButton();
	virtual void			Draw();
	virtual bool			Update(float dt);
	virtual void			MouseProcess(byte btn, byte event);
	unsigned int			Styles[4];//0 - normal, 1 - mouseon, 2 - clicked, 3 - not enabled
	CEvent					onClick;
	void					DrawText();
	byte					StyleInd;
};
class CCustomEdit : public CGraphObj
{
public:
	CCustomEdit(unsigned int _Style);
};
class CEdit : public CGraphObj
{
public:
	CEdit(unsigned int _Style);
	~CEdit(){};
	virtual void			Draw();
	virtual bool			Update(float dt);
	// сорри, нехуй в аське не отвечать
	virtual void			Step();
	virtual void			MouseProcess(byte btn, byte event);
	virtual void			KeyProcess(SDLKey &btn, byte event);
	CKeyEvent				onKeyPress;
	CEvent					onClick;
	int						SelStart, SelLength;
	int						_Style;
	void					DrawText();
	DWORD					ThisStyle;
	SDLKey					CurrentKey;
	byte					KeyState;
	DWORD					KeyTime;
	byte					Shift;
	char					_cout;
	int						offset;
	bool					mbpr;
};

/*
GUI library
Deku Team
*/

/*
	
*/


#endif