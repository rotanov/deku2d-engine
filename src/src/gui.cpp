#include "gui.h"
//GUI scheme or may be it is "skin"
CGUIScheme				*GUIScheme=NULL;
bool					szBind=true;
CObjectList				Objects, Forms;
int						_mx, _my;
bool					GUICreated=false;
bool					_Mbt[10];
CGraphObj				*MouseFocus = NULL, *Focus = NULL;
int						CTag = 0;
CFontManager			*FontManager = NULL;
char					_key = 0;

bool MouseIn(int l, int t, int w, int h)
{
	int r = l + w;
	int b = t + h;
	SDL_GetMouseState(&_mx, &_my);

	if (l>r)
	{
		int tm = l;
		l = r;
		r = tm;
	}
	if (t>b)
	{
		int tm = t;
		t = b;
		b = tm;
	}
	if (((_mx >= l) && (_mx <= r)) && ((_my >= t) && (_my <= b)))
		return 1;

	return 0;
}

bool MouseInObjRect(PWidget obj)
{
	int l, t;
	l = obj->GetLeft();
	t = obj->GetTop();
	PWidget tmpobj = obj;
	return MouseIn(l, t, obj->Width, obj->Height);
}
bool MbState(byte btn)
{
	return _Mbt[btn];
}


bool GUIKeyUp(char key, SDLKey sym)
{
	if (Focus!=NULL)
		Focus->KeyProcess(sym, GUI_KEYUP);
	return 1;
}
bool GUIKeyDown(char key, SDLKey sym)
{
	_key = key;
	if (Focus!=NULL)
		Focus->KeyProcess(sym, GUI_KEYDOWN);
	return 1;
}
bool GUIMbDown(int x, int y, byte btn)
{
	_mx = x;
	_my = y;
	_Mbt[btn] = 1;
	if (MouseFocus!=NULL)
		MouseFocus->MouseProcess(btn, GUI_MBDOWN);
	return 1;
}
bool GUIMbUp(int x, int y, byte btn)
{
	_mx = x;
	_my = y;
	if (_Mbt[btn]!=0)
		if (MouseFocus!=NULL)
			MouseFocus->MouseProcess(btn, GUI_MBCLICK);
	else
		if (MouseFocus!=NULL)
			MouseFocus->MouseProcess(btn, GUI_MBUP);
	_Mbt[btn] = 0;
	return 1;
}

// bool GUIMbMove(int x, int y, byte btn)
// {
// 	_mx = x;
// 	_my = y;
// 	return 1;
// }

void						clearWidget(PWidget res)
{
	res->Align = 0;
	res->Caption="";
	res->Enabled = 1;
	res->Height = 0;
	res->Left = 0;
	res->fnt = NULL;
	res->MinHeight = 0;
	res->MinWidth = 0;
	res->name="";
	ToggleLog(0);
	res->Parent = dynamic_cast<PWidget>(Objects.GetObject("ScreenForm"));
	ToggleLog(1);
	res->Top = 0;
	memset(res->type, 0, 100);
	res->Visible = 0;
	res->Width = 0;
}

PWidget		newWidget(char *name, unsigned int Style){
	//first wee need to select proper class to create else raise exception(but i think better
	//will be to create simple dummy
	PWidget res=NULL;
	switch (Style & STYLE_OBJMASK)
	{
	case STYLE_OBJFORM:
		res = new CForm(Style);
		clearWidget(res);
		res->name=name;
		if (!(Style & STYLE_ADDTOLISTMASK))
		{
			Objects.AddObject(res);
			if (strcmp(name, "!@#$%^&*()") != 0 )
				Forms.AddObject(res);
		}
		res->Tag = CTag;
		CTag++;
		return res;
		break;
	case STYLE_OBJBUTTON:
		res = new CButton(Style);
		clearWidget(res);
		res->name=name;
		if (!(Style & STYLE_ADDTOLISTMASK))
			Objects.AddObject(res);
		res->Tag = CTag;
		CTag++;
		return res;
		break;
	case STYLE_OBJEDIT:
		res = new CEdit(Style);
		clearWidget(res);
		res->name=name;
		if (!(Style & STYLE_ADDTOLISTMASK))
			Objects.AddObject(res);
		res->Tag = CTag;
		CTag++;
		return res;
		break;
	default:
/*		res = szWidgets.Items[szWidgets.ItemCount] = new Cdummy();
		clearWidget(res);
		szWidgets.ItemCount++;
		return res;*/
		return NULL;
		break;
	}
}
void		FormAddWidget(PWidget obj, PWidget form){
	if (obj==NULL)
		return;
	if (form == NULL)
	{
		CForm *__form = dynamic_cast<CForm*>(Objects.GetObject("ScreenForm"));
		if (__form != NULL)
		{
			__form->Items.AddObject(obj);
			obj->Parent = __form;
		}
		return;
	}
	if (!ISFORM(form))
		return;
	CForm *_form=(CForm*)form;
	_form->Items.AddObject(obj);
	obj->Parent=form;
}
void		showWidget(PWidget obj){
	if (obj)
		obj->Visible = true;
}
void		hideWidget(PWidget obj){
	if (obj)
		obj->Visible = false;
}
void		enableWidget(PWidget obj){
	if (obj)
		obj->Enabled = true;
}
void		disableWidget(PWidget obj){
	if (obj)
		obj->Enabled = false;
}
void		setWidgetStyle(PWidget obj, unsigned int Style){
	if (!obj) return;
	obj->setStyle(Style);
}
PWidget		getWidget(char *name){
	return dynamic_cast<PWidget>(Objects.GetObject(name));
}

void CGraphObj::setStyle(unsigned int _Style)
{
	Style=(Style & STYLE_OBJMASK) + (_Style & XSTYLE_MASK); 
}
void CGraphObj::SetStyle(unsigned int style)
{
	Style=style; 
}

void CGraphObj::ReadComponent(CFile f)
{
}

int CGraphObj::GetLeft()
{
	int l;
	l = 0;
	PWidget tmpobj = this;
	while (tmpobj != NULL)
	{
		l += tmpobj->Left;
		tmpobj = tmpobj->Parent;
	}
	return l;
}

int CGraphObj::GetTop()
{
	int t;
	t = 0;
	PWidget tmpobj = this;
	while (tmpobj != NULL)
	{
		t += tmpobj->Top;
		if (ISFORM(tmpobj))
		{
			if (((CForm*)tmpobj)->Properties[GUI_DRAWFORMHEADER])
				t+= ((CForm*)tmpobj)->HeaderHeight;
		}
		tmpobj = tmpobj->Parent;
	}
	return t;
}

bool CGraphObj::SetFont( string FontName )
{
	Font = FontName;
	fnt = FontManager->GetFontEx(FontName);
	return (fnt!=NULL);
}
/*void BeginUI()
{
	szBind=false;
}
void EndUI()
{
	szBind=true;
}*/
CGUIScheme::CGUIScheme(char *fname, char *tname)
{
	//creating new form( - main form)
	CForm *frm = (CForm*)newWidget("ScreenForm", STYLE_OBJFORM);
	frm->Width = ScreenW();
	frm->Height = ScreenH();
	frm->Left = 0;
	frm->Top = 0;
	frm->Properties[GUI_DRAWFORMHEADER] = 0;
	frm->Properties[GUI_DRAWFORMBODY] = 0;
	frm->Properties[GUI_FORMRESIZABLE] = 0;
	frm->Properties[GUI_FORMCANMOVE] = 0;
	frm->Visible = true;
	frm->Enabled = true;
	CFile f;
	GUIScheme = this;
	if (fname==NULL||tname==NULL)
	{
		Log("AHTUNG","Specified pointer has NULL value!",fname);
		return;
	}
	if (f.Open(fname,CFILE_READ))
	{
		Log("INFO","Loading GUI scheme \"%s\".",fname);
		byte b=0;
		f.Read(&b, 1);
		if (b!='D')
		{
			Log("AHTUNG","File %s has unknown format or damaged.",fname);
			f.Close();
			return;
		}
		f.Read(&b, 1);
		if (b!='G')
		{
			Log("AHTUNG","File %s has unknown format or damaged.",fname);
			f.Close();
			return;
		}
		f.Read(&b, 1);
		if (b!='K')
		{
			Log("AHTUNG","File %s has unknown format or damaged.",fname);
			f.Close();
			return;
		}
		name = "";
		f.ReadString(name);
		f.Read(&StyleCount, 4);
		for (int i = 0;i < StyleCount;i++)
		{
			f.Read(&Styles[i], 20);
		}
		int type=0, tmp=0;
		f.Read(&ObjCount, 4);
		for (int i = 0;i < ObjCount;i++)
		{
			f.Read(&type, 4);
			f.Read(&tmp, 4);
			switch (type)
			{
			case 0:
				{
//					color Color
//						style Style
//						int Height
//						string Font
//# -Body----------------------
//						-Body
//						style Style
//						int MinHeight
//						int MinWidth
//						int MaxHeight
//						int MaxWidth
//						int Cursor
//						end
					//This is form
					//now we need to do something xP =)
					CForm * obj = (CForm*)newWidget("!@#$%^&*()", STYLE_OBJFORM|STYLE_DONOTADD);
					objects[i] = obj;
					obj->Tag = i;
					f.ReadString(obj->name);
					f.Read(&obj->HeaderColor, 4);
					f.Read(&obj->HeaderStyle, 4);
					f.Read(&obj->HeaderHeight, 4);
					f.ReadString(obj->Font);
					f.Read(&obj->BodyStyle, 4);
					f.Read(&obj->MinHeight, 4);
					f.Read(&obj->MinWidth, 4);
					f.Read(&obj->MaxHeight, 4);
					f.Read(&obj->MaxWidth, 4);
					f.Read(&obj->Cursor, 4);
					break;
				}
			case 1:
				{
					//button
					CButton *obj = (CButton*)newWidget("noname", STYLE_OBJBUTTON|STYLE_DONOTADD);
					objects[i] = obj;
					obj->Tag = i;
			//				string Name
			//# -Style name
			//					style Normal
			//					style On
			//					style Clicked
			//					int MinHeight
			//					int MinWidth
			//					int MaxHeight
			//					int MaxWidth
			//					end
					f.ReadString(obj->name);
					f.Read(obj->Styles, 12);
					f.Read(&obj->MinHeight, 4);
					f.Read(&obj->MinWidth, 4);
					f.Read(&obj->MaxHeight, 4);
					f.Read(&obj->MaxWidth, 4);
					f.ReadString(obj->Font);
					//f.Read(&obj->Cursor, 4);
					break;
				}
			case 3:
				{
					//edit
					CEdit *obj = (CEdit*)newWidget("noname", STYLE_OBJEDIT|STYLE_DONOTADD);
					objects[i] = obj;
					obj->Tag = i;
					f.ReadString(obj->name);
					f.Read(&obj->ThisStyle, 4);
					f.Read(&obj->MinHeight, 4);
					f.Read(&obj->MinWidth, 4);
					f.Read(&obj->MaxHeight, 4);
					f.Read(&obj->MaxWidth, 4);
					f.Read(&obj->Cursor, 4);
					break;
				}
			}
		}
		
		f.Close();
		Log("INFO","Gui Scheme \"%s\" loaded.",name.data());
		ImgData.LoadTexture(tname);
	}
	else
	{
		Log("AHTUNG","Cannot load GUI Scheme from file %s .",fname);
	}
}
int	CGUIScheme::GetXOffset(int StInd)
{
	return Styles[StInd].Data[4];
}
int	CGUIScheme::GetYOffset(int StInd)
{
	return Styles[StInd].Data[5];
}
int	CGUIScheme::GetCWidth(int StInd, int w)
{
	return w - Styles[StInd].Data[4] - Styles[StInd].Data[6];
}
int	CGUIScheme::GetCHeight(int StInd, int h)
{
	return h - Styles[StInd].Data[5] - Styles[StInd].Data[7];
}

void CGUIScheme::BeginUI()
{
	GUIScheme=this;
//	szBind = true;
/*	else
	{
		Log("AHTUNG", "Cannot bind GUI scheme %s!", name);
		return;
	}*/
	Drawing = 1;
	ImgData.BeginDraw();
}
void CGUIScheme::EndUI()
{
	if (Drawing)
		ImgData.EndDraw();
	Drawing = 0;
}
void CGUIScheme::_Draw(int StInd, byte target, int x, int y, float z, int w, int h)
{
	//now drawing xP
	if (!Drawing)
		return;
	if (target == 0|| target > 8)
		ImgData.PushQuadEx(x,y,z,w,h,Styles[StInd].Data[0], Styles[StInd].Data[1], Styles[StInd].Data[4], Styles[StInd].Data[5], 0); //- lt
	if (target == 1)
		ImgData.PushQuadEx(x,y,z,w,h,
		Styles[StInd].Data[0] + Styles[StInd].Data[4],
		Styles[StInd].Data[1],
		Styles[StInd].Data[2] - Styles[StInd].Data[6] - Styles[StInd].Data[4],
		Styles[StInd].Data[5], Styles[StInd].Data[9]);//t
	if (target == 2)
		ImgData.PushQuadEx(x,y,z,w,h,
		Styles[StInd].Data[0] + Styles[StInd].Data[2] - Styles[StInd].Data[6],//mb - 4
		Styles[StInd].Data[1],
		Styles[StInd].Data[6],//4, 5
		Styles[StInd].Data[7], 0);// rt
	if (target == 3)
		ImgData.PushQuadEx(x,y,z,w,h,
		Styles[StInd].Data[0],
		Styles[StInd].Data[1] + Styles[StInd].Data[5],
		Styles[StInd].Data[4],
		Styles[StInd].Data[3] - Styles[StInd].Data[7] - Styles[StInd].Data[5], Styles[StInd].Data[9]);//l
	if (target == 4)
		ImgData.PushQuadEx(x,y,z,w,h,
		Styles[StInd].Data[0] + Styles[StInd].Data[4],
		Styles[StInd].Data[1] + Styles[StInd].Data[5],
		Styles[StInd].Data[2] - Styles[StInd].Data[4] - Styles[StInd].Data[6],
		Styles[StInd].Data[3] - Styles[StInd].Data[7] - Styles[StInd].Data[5], Styles[StInd].Data[8]);//c
	if (target == 5)
		ImgData.PushQuadEx(x,y,z,w,h,
		Styles[StInd].Data[0] + Styles[StInd].Data[2] - Styles[StInd].Data[4],
		Styles[StInd].Data[1] + Styles[StInd].Data[5],
		Styles[StInd].Data[6],
		Styles[StInd].Data[3] - Styles[StInd].Data[7] - Styles[StInd].Data[5], Styles[StInd].Data[9]);//r
	if (target == 6)
		ImgData.PushQuadEx(x,y,z,w,h,
		Styles[StInd].Data[0],
		Styles[StInd].Data[1] + Styles[StInd].Data[3] - Styles[StInd].Data[7],
		Styles[StInd].Data[4],
		Styles[StInd].Data[7], 0);//bl
	if (target == 7)
		ImgData.PushQuadEx(x,y,z,w,h,
		Styles[StInd].Data[0] + Styles[StInd].Data[4],
		Styles[StInd].Data[1] + Styles[StInd].Data[3] - Styles[StInd].Data[7],
		Styles[StInd].Data[2] - Styles[StInd].Data[6] - Styles[StInd].Data[4],
		Styles[StInd].Data[7], Styles[StInd].Data[9]);//b
	if (target == 8)
		ImgData.PushQuadEx(x,y,z,w,h,
		Styles[StInd].Data[0] + Styles[StInd].Data[2] - Styles[StInd].Data[6],
		Styles[StInd].Data[1] + Styles[StInd].Data[3] - Styles[StInd].Data[7],
		Styles[StInd].Data[6],
		Styles[StInd].Data[7], 0);//br
}
bool CGUIScheme::Draw(int StInd, int x, int y, float z, int w, int h)
{
	_Draw(StInd, 0, x, y, z, Styles[StInd].Data[4], Styles[StInd].Data[5]);
	_Draw(StInd, 1, x + Styles[StInd].Data[4], y, z, w - Styles[StInd].Data[4] - Styles[StInd].Data[6], Styles[StInd].Data[5]);
	_Draw(StInd, 2, x + w - Styles[StInd].Data[6], y, z, Styles[StInd].Data[6], Styles[StInd].Data[5]);
	_Draw(StInd, 3, x, y + Styles[StInd].Data[5], z, Styles[StInd].Data[4], h - Styles[StInd].Data[5] - Styles[StInd].Data[7]);
	_Draw(StInd, 4, x + Styles[StInd].Data[4], y + Styles[StInd].Data[5], z, w - Styles[StInd].Data[4] - Styles[StInd].Data[6], h - Styles[StInd].Data[5] - Styles[StInd].Data[7]);
	_Draw(StInd, 5, x + w - Styles[StInd].Data[6], y + Styles[StInd].Data[5], z, Styles[StInd].Data[6], h - Styles[StInd].Data[5] - Styles[StInd].Data[7]);
	_Draw(StInd, 6, x, y + h - Styles[StInd].Data[7], z, Styles[StInd].Data[4], Styles[StInd].Data[7]);
	_Draw(StInd, 7, x + Styles[StInd].Data[4], y + h - Styles[StInd].Data[7], z, w - Styles[StInd].Data[4] - Styles[StInd].Data[6], Styles[StInd].Data[7]);
	_Draw(StInd, 8, x + w - Styles[StInd].Data[6], y + h - Styles[StInd].Data[7], z, Styles[StInd].Data[6], Styles[StInd].Data[7]);
	return true;
}
void CGUIScheme::Release()
{
}

PWidget CGUIScheme::CreateWidget( int kind )
{
//	PWidget tmp = NULL;
//	for (int i = 0; i < ObjCount; i++)
//	/{
//		if (objects[i]->getStyle())
//	}
	PWidget res = NULL;
	string _tmp = typeid(*objects[kind]).name();
	if (_tmp == "class CButton")
	{
		//here is button
		res = newWidget("NoName", STYLE_OBJBUTTON);
		
		/*CButton *tmp = (CButton*)objects[kind];
		res->name = tmp->name + "_noname";
		memcpy(res->Styles, tmp->Styles, 12);
		res->MinHeight = tmp->MinHeight;
		res->MinWidth = tmp->MinWidth;
		res->MaxHeight = tmp->MaxHeight;
		res->MaxWidth = tmp->MaxWidth;*/
	}
	if (_tmp == "class CForm")
	{
		res = newWidget("!@#$%^&*()", STYLE_OBJFORM);
		res->name = "NoName";
	}
	CopyWidget(kind, res);
//	switch (typeid(objects[kind]).name))
//	{
//	case "":
//	}
	return res;
}

void CGUIScheme::CopyWidget( int kind, PWidget wdg )
{
	if (wdg == NULL)
		return;
	string _tmp = typeid(*objects[kind]).name();
	if (_tmp == "class CButton")
	{
		//here is button
		CButton *res = (CButton*)wdg;
		CButton *tmp = (CButton*)objects[kind];
//		res->name = tmp->name + "_noname";
		memcpy(res->Styles, tmp->Styles, 12);
		res->MinHeight = tmp->MinHeight;
		res->MinWidth = tmp->MinWidth;
		res->MaxHeight = tmp->MaxHeight;
		res->MaxWidth = tmp->MaxWidth;
		res->Font		   = tmp->Font;
	}
	if (_tmp == "class CForm")
	{
		CForm *res = (CForm*)wdg;
		CForm *tmp = (CForm*)objects[kind];
		res->HeaderColor   = tmp->HeaderColor;
		res->HeaderStyle   = tmp->HeaderStyle;
		res->HeaderHeight  = tmp->HeaderHeight;
		res->Font		   = tmp->Font;
		res->BodyStyle	   = tmp->BodyStyle;
		res->MinHeight	   = tmp->MinHeight;
		res->MinWidth	   = tmp->MinWidth;
		res->MaxHeight	   = tmp->MaxHeight;
		res->MaxWidth	   = tmp->MaxWidth;
		res->Cursor		   = tmp->Cursor;
		res->Font		   = tmp->Font;
	}
	if (_tmp == "class CEdit")
	{
		CEdit *res = (CEdit*)wdg;
		CEdit *tmp = (CEdit*)objects[kind];
		res->ThisStyle	   = tmp->ThisStyle;
		res->MinHeight	   = tmp->MinHeight;
		res->MinWidth	   = tmp->MinWidth;
		res->MaxHeight	   = tmp->MaxHeight;
		res->MaxWidth	   = tmp->MaxWidth;
		res->Cursor		   = tmp->Cursor;
		res->Font		   = tmp->Font;
	}
/*	color Color
		style Style
		int Height
		string Font
# -Body----------------------
		-Body
		style Style
		int MinHeight
		int MinWidth
		int MaxHeight
		int MaxWidth
		int Cursor*/
}

CGUIScheme::~CGUIScheme()
{
	for (int i = 0; i < ObjCount; i++)
		SAFE_DELETE(objects[i]);
}
CControl::CControl()
{
//	Style=_Style;
	name = "";
}

CForm::CForm(unsigned int _Style)
{
	Style=_Style;
	name = "";
}
bool FormDraw(PWidget obj)
{
	if (obj->Visible)
		obj->Draw();
	return 1;
}
bool FormStep(PWidget obj)
{
	obj->Step();
	return 1;
}

PWidget addForm( char *Fname )
{
	PWidget frm = newWidget("noname", STYLE_OBJFORM);
	((CForm*)frm)->LoadForm(Fname);
	return frm;
}
void CForm::Draw()
{
	if (!this->Visible)
		return;
	if (Properties[GUI_DRAWFORMHEADER] == 1)
	{
		glColor4f((float)_GetRValue(HeaderColor)/256,(float)_GetGValue(HeaderColor)/256,(float)_GetBValue(HeaderColor)/256,1);
		GUIScheme->Draw(HeaderStyle, Left, Top, ZDepth, Width, HeaderHeight);
		glColor4f(1,1,1,1);
		Top = Top + HeaderHeight;
	}
	if (Properties[GUI_DRAWFORMBODY] == 1)
		GUIScheme->Draw(BodyStyle, Left, Top, ZDepth, Width, Height);
	CObject *obj;
	Items.Reset();
	while (Items.Enum(obj))
	{
		dynamic_cast<PWidget>(obj)->ZDepth = (float)(Tag/CTag)*(GUI_TOP - GUI_BOTTOM) + GUI_BOTTOM;
		(dynamic_cast<PWidget>(obj))->Draw();
	}
	if (Properties[GUI_DRAWFORMHEADER] == 1)
		Top = Top - HeaderHeight;
//	Body.Items.Call(FormDraw);
	//first of all drawing form but after somethiung else
}


bool CForm::LoadForm(char *Fname)
{
	CFile *file = new CFile(Fname, CFILE_READ);
	byte b = 0;
	file->Read(&b, 1);
	if (b != 'D')
	{
		file->Close();
		Log("AHTUNG", "Failed to load form \"%s\". File is damaged or has unknown extension.", Fname);
		return 0;
	}
	file->Read(&b, 1);
	if (b != 'F')
	{
		file->Close();
		Log("AHTUNG", "Failed to load form \"%s\". File is damaged or has unknown extension.", Fname);
		return 0;
	}
	file->Read(&b, 1);
	if (b != 'M')
	{
		file->Close();
		Log("AHTUNG", "Failed to load form \"%s\". File is damaged or has unknown extension.", Fname);
		return 0;
	}
	file->ReadString(name);
	string tmp;
	file->ReadString(tmp);
	file->Read(&Width, 4);
	file->Read(&Height, 4);
	file->Read(&Left, 4);
	file->Read(&Top, 4);
	//now setting form style
	int fstyle;
	file->Read(&fstyle, 4);
	GUIScheme->CopyWidget(fstyle, this);
	file->Read(&Halign, 4);
	file->Read(&Valign, 4);
	file->Read(Properties, 6);
	file->ReadString(Caption);
	int tmpi = 0;
	file->Read(&tmpi, 4);
	for (int i = 0; i < tmpi; i++)
	{
		//here adding objects...
		//reading style
		int st;
		file->Read(&st, 4);
		PWidget wdg = GUIScheme->CreateWidget(st);
		file->ReadString(wdg->name);
		file->ReadString(wdg->Caption);
		file->Read(&wdg->Left, 4);
		file->Read(&wdg->Top, 4);
		file->Read(&wdg->Width, 4);
		file->Read(&wdg->Height, 4);

		file->Read(&wdg->Align, 4);
		file->Read(&wdg->Valign, 4);

		byte b = 0;
		file->Read(&b, 1);
		wdg->Visible = (b==1);
		file->Read(&b, 1);
		wdg->Enabled = (b==1);

		//here will be script
		//not implemented... yet...

		int cnt = 0;
		file->Read(&cnt , 4);

		for (int i = 0; i < cnt; i++)
		{
			string __tmp;
			file->ReadString(__tmp);
		}


		FormAddWidget(wdg, this);
	}
	
	file->Close();
	delete file;	
	return 1;
}

bool CForm::Update( float dt )
{
	CObject *tmp;
	Items.Reset();
	bool flag = 0;
	while (Items.Enum(tmp))
	{
		PWidget obj = dynamic_cast<CGraphObj*>(tmp);
		if (MouseInObjRect(obj))
		{
			if (((MouseFocus != NULL) && (!ISFORM(MouseFocus))) && (MouseFocus != obj))
			{
				MouseFocus->Update(dt);
				flag = 1;
			}
			MouseFocus = obj;
			flag = 1;
			obj->Update(dt);
			//return true;
			break;
		}
	}
	if (Focus)
		Focus->Update(dt);
	
	if (MouseFocus != NULL)
		if (!(ISFORM(MouseFocus)))
			MouseFocus->Update(dt);
	if (flag)
		return true;
	MouseFocus = this;
	return true;
}

void CForm::DrawText()
{
	CObject *tmp;
	Items.Reset();
	if (Font != "")
		fnt = FontManager->GetFontEx(Font);
	if (fnt!=NULL)
	{
		//fnt->PrintEx(
		glColor4f(0,0,1,1);
		fnt->PrintRectEx(GetLeft() + GUIScheme->GetXOffset(HeaderStyle)
			,ScreenH()-GetTop() + GUIScheme->GetYOffset(HeaderStyle)
			,ZDepth + (float)(GUI_TOP - GUI_BOTTOM)/(CTag+5)
			,GUIScheme->GetCWidth(HeaderStyle, Width)///_szGUIScheme->GetCWidth(Styles[StyleInd], Width)//GUIScheme->GetCWidth(HeaderStyle, Width)
			,GUIScheme->GetCWidth(HeaderStyle, HeaderHeight)//_szGUIScheme->GetCWidth(Styles[StyleInd], Height)//GUIScheme->GetCWidth(HeaderStyle, HeaderHeight)
			,0,CFONT_VALIGN_CENTER | CFONT_HALIGN_CENTER, (char*)Caption.data());
		glColor4f(1,1,1,1);
	}
	while (Items.Enum(tmp))
	{
		PWidget obj = dynamic_cast<CGraphObj*>(tmp);
		obj->DrawText();
	}
}
//Button functions
CButton::CButton(unsigned int _Style)
{
	//Zero(name,sizeof(name));
	Style = _Style;
	StyleInd = 0;
	onClick = NULL;
}
CButton::~CButton()
{
}
void CButton::Draw()
{
	if (!Visible)
		return;
	PWidget ptr = Parent;
	int _left = Left;
	int _top = Top;
	while (ptr != NULL)
	{
		_left += ptr->Left;
		_top += ptr->Top;
		ptr = ptr->Parent;
	}
	GUIScheme->Draw(Styles[StyleInd], _left, _top, ZDepth, Width, Height);
}
void CButton::DrawText()
{
//	return;
	if (!Visible)
		return;
	fnt = FontManager->GetFontEx(Font);
	if (fnt!=NULL)
	{
		//fnt->PrintEx(
		glColor4f(0,0,1,1);
		fnt->PrintRectEx(GetLeft() + GUIScheme->GetXOffset(Styles[StyleInd])
			,ScreenH()-GetTop() - Height + GUIScheme->GetYOffset(Styles[StyleInd])
			,ZDepth + GUI_BOTTOM + (float)(GUI_TOP - GUI_BOTTOM)/(CTag+5)
			,GUIScheme->GetCWidth(Styles[StyleInd], Width)//ClientWidth
			,GUIScheme->GetCWidth(Styles[StyleInd], Height)//ClientHeight_sz
			,0,CFONT_VALIGN_CENTER | CFONT_HALIGN_CENTER, (char*)Caption.data());
		glColor4f(1,1,1,1);
	}
}
bool CButton::Update(float dt)
{
	//checking enabled
	if (Width<MinWidth)
		Width = MinWidth;
	if (Height<MinHeight)
		Height = MinHeight;
	ClientWidth = GUIScheme->GetCWidth(Styles[StyleInd], Width);
	ClientHeight = GUIScheme->GetCHeight(Styles[StyleInd], Height);
	if (!Enabled)
	{
		StyleInd = 3;
		return true;
	}
	if (MouseInObjRect(this))//MouseFocus == this)//
	{
		StyleInd = 1;
/*		if (MouseFocus != this && MouseFocus != NULL)
		{
			if ((MouseFocus->getStyle() & STYLE_OBJMASK) == STYLE_OBJBUTTON)
				((CButton*)MouseFocus)->StyleInd = 0;
		}
//		MouseFocus = this;*/
		if (MouseFocus == this)

		if (MbState(SDL_BUTTON_LEFT))
		{
			StyleInd = 2;
		}
	}
	else{
/*		if (MouseFocus == this)
			MouseFocus = NULL;
		else
			if (MouseFocus != NULL)
				MouseFocus->Update(dt);*/
		StyleInd = 0;
	}
	return true;
}

void CButton::MouseProcess(byte btn, byte event)
{
	if (!Enabled)
		return;
	if (event==GUI_MBCLICK)
	{
		if (MouseIn(Left, Top, Width, Height))
		{
			//SAFECALL(onClick, this);
			Focus = this;
		}
	}
}

CEdit::CEdit(unsigned int _Style)
{
	Style = _Style;
	onKeyPress = NULL;
	onClick = NULL;
	SelStart = 0;
	SelLength = 0;
	CurrentKey = SDLK_HELP;
	KeyState = 0;
	KeyTime = 0;
	Shift = 0;
	offset = 0;
}
void CEdit::Step()
{
	if (Width<MinWidth)
		Width = MinWidth;
	if (Height<MinHeight)
		Height = MinHeight;
	if (MouseIn(Left, Top, Width, Height))
	{
		if (MouseFocus != this && MouseFocus != NULL)
		{
			if ((MouseFocus->getStyle() & STYLE_OBJMASK) == STYLE_OBJBUTTON)
				((CButton*)MouseFocus)->StyleInd = 0;
		}
		MouseFocus = this;
	}
}

void CEdit::Draw()
{
	//now drawing
	if (!Visible)
		return;
	GUIScheme->Draw(ThisStyle, GetLeft(), GetTop(), 0, Width, Height);
}

void CEdit::DrawText()
{
	fnt = FontManager->GetFontEx(Font);
	if (fnt!=NULL)
	{
		//fnt->PrintEx(
		glColor4f(0,0,1,1);
		int l = GetLeft() + GUIScheme->GetXOffset(ThisStyle);
		fnt->PrintSelRect(l
			,ScreenH()-GetTop() - Height + GUIScheme->GetYOffset(ThisStyle)
			,ZDepth + GUI_BOTTOM + (float)(GUI_TOP - GUI_BOTTOM)/(CTag+5)
			,GUIScheme->GetCWidth(ThisStyle, Width)//ClientWidth
			,GUIScheme->GetCWidth(ThisStyle, Height)//ClientHeight_sz
			,offset,CFONT_VALIGN_CENTER | CFONT_HALIGN_LEFT, (char*)Caption.data(), SelStart, SelStart + SelLength - 1);
		glColor4f(1,1,1,1);
	}
}

void CEdit::MouseProcess(byte btn, byte event)
{
	if (!Enabled)
		return;
	if (fnt == NULL)
		fnt = FontManager->GetFontEx(Font);
	if (fnt == NULL)
		return;

	if (event==GUI_MBCLICK)
	{
		if (MouseInObjRect(this))
		{
			//SAFECALL(onClick, this);
			KeyState = 0;
			if (Focus)
			{
				if (typeid(*Focus).name() == "class CEdit")
				{
					if (!dynamic_cast<CEdit*>(Focus)->mbpr)
						Focus = this;
				}
				else
					Focus = this;
			}
			else
				Focus = this;
		}
	}
	if (event==GUI_MBDOWN&&btn==1)
	{
		//getting position
		if (!Focus)
			return;
		if (MouseIn(GetLeft() + GUIScheme->GetXOffset(ThisStyle),
					GetTop() + GUIScheme->GetYOffset(ThisStyle),
					GUIScheme->GetCWidth(ThisStyle, Width),
					GUIScheme->GetCHeight(ThisStyle, Height)))
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			x -= GetLeft() + GUIScheme->GetXOffset(ThisStyle);
			//getting letter pos
			if (!fnt)
				return;
			int i = 0;
			mbpr = 1;
			for (i = 0; i < Caption.length() - offset; i++)
				if (fnt->GetStringWidthEx(offset, offset + i - (i > 0), (char*)Caption.data()) >= x)
					break;
			if (Shift)
				SelLength = i + offset - SelStart;
			else
			{
				SelStart = i + offset;
				SelLength = 0;
			}
		}
	}
	if (event==GUI_MBUP&&btn==1)
	{
		mbpr = 0;
	}

}
void CEdit::KeyProcess(SDLKey &btn, byte event)
{
	if (event == GUI_KEYDOWN)
	{
		switch (btn)
		{
		case SDLK_LSHIFT:
			{
				Shift = 1;
				KeyState = (KeyState > 0);//0;mb 
				break;
			}
		case SDLK_RSHIFT:
			{
				Shift = 1;
				KeyState = (KeyState > 0);//0;mb 
				break;
			}
		case SDLK_DELETE:
			{
				//removing selection and setting key to delete
				//Caption.erase()
				if (SelLength != 0)
				{
					//Caption.replace(SelStart, SelStart + SelLength, "");
					Caption.erase(Caption.begin() + min(SelStart, SelStart + SelLength), Caption.begin() + max(SelStart, SelStart + SelLength));
					SelStart = min(SelStart, SelStart + SelLength);
				}
				else
					if (Caption.length() != SelStart)
						Caption.erase(Caption.begin() + SelStart, Caption.begin() + SelStart + 1);
				SelLength = 0;
				CurrentKey = SDLK_DELETE;
				KeyState = 1;
				KeyTime = SDL_GetTicks();
				break;
			}
		case SDLK_BACKSPACE:
			{
				if (SelLength != 0)
				{
					//Caption.replace(SelStart, SelStart + SelLength, "");
					Caption.erase(Caption.begin() + min(SelStart, SelStart + SelLength), Caption.begin() + max(SelStart, SelStart + SelLength));
					SelStart = min(SelStart, SelStart + SelLength);
				}
				else
					if (0 != SelStart)
						Caption.erase(Caption.begin() + SelStart - 1, Caption.begin() + SelStart);
				SelLength = 0;
				if (SelStart)
					SelStart--;
				CurrentKey = SDLK_BACKSPACE;
				KeyState = 1;
				KeyTime = SDL_GetTicks();
				break;
			}
		case SDLK_LEFT:
			{
				if (Shift)
				{
					if (SelLength + SelStart> 0)
						SelLength --;
				}
				else
				{
					if (SelLength != 0)
						SelStart = min(SelStart, SelStart + SelLength);
					else
						SelStart = max(0, SelStart - 1);
					SelStart = max(0, SelStart);
					SelLength = 0;
				}
				CurrentKey = SDLK_LEFT;
				KeyState = 1;
				KeyTime = SDL_GetTicks();
				break;
			}
		case SDLK_RIGHT:
			{
				if (Shift)
				{
					if (!(SelStart + SelLength >= Caption.length()))
						SelLength ++;
				}
				else
				{
					if (SelLength == 0)
					{
						if (SelStart + 1 > Caption.length())
							SelStart = Caption.length();
						else
							SelStart ++;
					}
					else
						SelStart = max(SelStart + SelLength, SelStart);
					if (SelStart + 1 > Caption.length())
						SelStart = Caption.length();
					SelLength = 0;
				}
				CurrentKey = SDLK_RIGHT;
				KeyState = 1;
				KeyTime = SDL_GetTicks();
				break;
			}
		case SDLK_HOME:
			{
				if (Shift)
					SelLength = -SelStart;
				else
				{
					SelStart = 0;
					SelLength = 0;
				}
				break;
			}
		case SDLK_END:
			{
				if (Shift)
					SelLength = Caption.length() - SelStart;
				else
				{
					SelStart = Caption.length();
					SelLength = 0;
				}
				break;
			}
		default:
			{
				char _out = _key;
				/*if (btn <= SDLK_z && btn >= SDLK_a)
				{
					_out = btn - SDLK_a;
					if (!Shift)
						_out += 'a';
					else
						_out += 'A';
				}
				if (btn == SDLK_SPACE)
				{
					_out = ' ';
				}
				if (btn <= SDLK_9 && btn >= SDLK_0)
				{
					_out = btn - SDLK_0;
					_out += '0';
				}*/
				if (_out != 0)
				{
					_cout = _out;
					CurrentKey = btn;
					KeyState = 1;
					KeyTime = SDL_GetTicks();
					if (SelLength == 0)
						Caption.insert(Caption.begin() + SelStart, 1, _out);
					else
					{
						Caption.erase(Caption.begin() + min(SelStart, SelStart + SelLength), Caption.begin() + max(SelStart, SelStart + SelLength));
						SelStart = min(SelStart, SelStart + SelLength);
						Caption.insert(Caption.begin() + SelStart, 1, _out);
					}
					SelLength = 0;
					SelStart++;
				}
				break;
			}
		}

	}
	if (event == GUI_KEYUP)
	{
		switch (btn)
		{
		case SDLK_LSHIFT:
			{
				Shift = 0;
				KeyState = (KeyState > 0);//0;mb 
				break;
			}
		case SDLK_RSHIFT:
			{
				Shift = 0;
				KeyState = (KeyState > 0);//0;mb 
				break;
			}
		case SDLK_DELETE:
			{
				//removing selection and setting key to delete
				//Caption.erase()
				if (CurrentKey == SDLK_DELETE)
					KeyState = 0;
				break;
			}
		case SDLK_BACKSPACE:
			{
				//removing selection and setting key to delete
				//Caption.erase()
				if (CurrentKey == SDLK_BACKSPACE)
					KeyState = 0;
				break;
			}
		case SDLK_LEFT:
			{
				//removing selection and setting key to delete
				//Caption.erase()
				if (CurrentKey == SDLK_LEFT)
					KeyState = 0;
				break;
			}
		case SDLK_RIGHT:
			{
				//removing selection and setting key to delete
				//Caption.erase()
				if (CurrentKey == SDLK_RIGHT)
					KeyState = 0;
				break;
			}
		default:
			{
				if (btn == CurrentKey)
					KeyState = 0;
				break;
			}
		}

	}
	return;
}

bool CEdit::Update( float dt )
{
	if (fnt == NULL)
		fnt = FontManager->GetFontEx(Font);
	if (fnt == NULL)
		return 1;
	//int sw = ;
	//int cw = ;
	while (fnt->GetStringWidthEx(offset, SelStart, (char*)Caption.data()) > GUIScheme->GetCWidth(ThisStyle, Width) - 10 && offset < Caption.length() - 1)
		offset++;
	while (fnt->GetStringWidthEx(0, SelStart, (char*)Caption.data()) < fnt->GetStringWidthEx(0, offset, (char*)Caption.data()) + 20 && offset > 0)
		offset--;
	if (mbpr&&MbState(1))
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		x -= GetLeft() + GUIScheme->GetXOffset(ThisStyle);
		//getting letter pos
		if (x>0)
		{
			int i = 0;
			//mbpr = 1;
			for (i = 0; i < Caption.length() - offset; i++)
				if (fnt->GetStringWidthEx(offset, offset + i - (i > 0), (char*)Caption.data()) >= x)
					break;
			//if (Shift)
				SelLength = i + offset - SelStart;
			//else
			//{
			//	SelStart = i + offset;
			//	SelLength = 0;
			//}
		}
	}
	else
		mbpr = 0;
	if (KeyState > 0)
	{
		bool flag = 0;
		if (KeyState == 2)
		{
			if (SDL_GetTicks() - KeyTime >= GUI_DELAY1)
			{
				KeyTime = SDL_GetTicks() - GUI_DELAY1;//0;
				flag = 1;
			}
		}
		if (KeyState == 1)
		{
			if (SDL_GetTicks() - KeyTime >= GUI_DELAY0)
			{
				KeyTime = SDL_GetTicks() - GUI_DELAY0;//0;
				KeyState = 2;
				flag = 1;
			}
		}
		if (flag)
		{
			switch (CurrentKey)
			{
			case SDLK_DELETE:
				{
					if (SelLength != 0)
					{
						//Caption.replace(SelStart, SelStart + SelLength, "");
						Caption.erase(Caption.begin() + min(SelStart, SelStart + SelLength), Caption.begin() + max(SelStart, SelStart + SelLength));
						SelStart = min(SelStart, SelStart + SelLength);
					}
					else
						if (Caption.length() != SelStart)
							Caption.erase(Caption.begin() + SelStart, Caption.begin() + SelStart + 1);
					SelLength = 0;
					break;
				}
			case SDLK_BACKSPACE:
				{
					if (SelLength != 0)
					{
						//Caption.replace(SelStart, SelStart + SelLength, "");
						Caption.erase(Caption.begin() + min(SelStart, SelStart + SelLength), Caption.begin() + max(SelStart, SelStart + SelLength));
						SelStart = min(SelStart, SelStart + SelLength);
					}
					else
						if (0 != SelStart)
							Caption.erase(Caption.begin() + SelStart - 1, Caption.begin() + SelStart);
					SelLength = 0;
					if (SelStart)
						SelStart--;
					break;
				}
			case SDLK_LEFT:
				{
					if (Shift)
					{
						if (SelLength + SelStart> 0)
							SelLength --;
					}
					else
					{
						if (SelLength != 0)
							SelStart = min(SelStart, SelStart + SelLength);
						else
							SelStart = max(0, SelStart - 1);
						SelStart = max(0, SelStart);
						SelLength = 0;
					}
					break;
				}
			case SDLK_RIGHT:
				{
					if (Shift)
					{
						if (!(SelStart + SelLength >= Caption.length()))
							SelLength ++;
					}
					else
					{
						if (SelLength == 0)
						{
							if (SelStart + 1 > Caption.length())
								SelStart = Caption.length();
							else
								SelStart ++;
						}
						else
							SelStart = max(SelStart + SelLength, SelStart);
						if (SelStart + 1 > Caption.length())
							SelStart = Caption.length();
						SelLength = 0;
					}
					break;
				}
			default:
				{
					char _out = _cout;
					/*if (CurrentKey <= SDLK_z && CurrentKey >= SDLK_a)
					{
						_out = CurrentKey - SDLK_a;
						if (!Shift)
							_out += 'a';
						else
							_out += 'A';
					}
					if (CurrentKey == SDLK_SPACE)
					{
						_out = ' ';
					}
					if (CurrentKey <= SDLK_9 && CurrentKey >= SDLK_0)
					{
						_out = CurrentKey - SDLK_0;
						_out += '0';
					}*/
					if (_out != 0)
					{
						if (SelLength == 0)
							Caption.insert(Caption.begin() + SelStart, 1, _out);
						else
						{
							Caption.erase(Caption.begin() + min(SelStart, SelStart + SelLength), Caption.begin() + max(SelStart, SelStart + SelLength));
							SelStart = min(SelStart, SelStart + SelLength);
							Caption.insert(Caption.begin() + SelStart, 1, _out);
						}
						SelLength = 0;
						SelStart++;
					}
					break;
				}

			}
		}
	}
	return true;
}
bool FormSort(CObject *obj1, CObject *obj2)
{
	(dynamic_cast<PWidget>(obj1))->ZDepth = GUI_BOTTOM + (dynamic_cast<PWidget>(obj1))->Tag*(GUI_TOP - GUI_BOTTOM)/CTag;
	(dynamic_cast<PWidget>(obj2))->ZDepth = GUI_BOTTOM + (dynamic_cast<PWidget>(obj2))->Tag*(GUI_TOP - GUI_BOTTOM)/CTag;
	return (dynamic_cast<PWidget>(obj1))->Tag <= (dynamic_cast<PWidget>(obj2))->Tag;
}
bool InvFormSort(CObject *obj1, CObject *obj2)
{
	(dynamic_cast<PWidget>(obj1))->ZDepth = GUI_BOTTOM + (dynamic_cast<PWidget>(obj1))->Tag*(GUI_TOP - GUI_BOTTOM)/CTag;
	(dynamic_cast<PWidget>(obj2))->ZDepth = GUI_BOTTOM + (dynamic_cast<PWidget>(obj2))->Tag*(GUI_TOP - GUI_BOTTOM)/CTag;
	return (dynamic_cast<PWidget>(obj1))->Tag >= (dynamic_cast<PWidget>(obj2))->Tag;
}

bool CGUIRenderer::Render()
{
	//here drawing scheme
	//first of all sorting 
	Forms.Sort(FormSort);
	CObject *tmp;

	Forms.Reset();
	GUIScheme->BeginUI();
	while (Forms.Enum(tmp))
	{
		PWidget obj = dynamic_cast<CGraphObj*>(tmp);
		(dynamic_cast<PWidget>(obj))->ZDepth = GUI_BOTTOM + (dynamic_cast<PWidget>(obj))->Tag*(GUI_TOP - GUI_BOTTOM)/CTag;
		(dynamic_cast<CForm*>(obj))->Items.Sort(FormSort);
		obj->Draw();
	}
	GUIScheme->EndUI();

	Forms.Reset();
	while (Forms.Enum(tmp))
	{
		PWidget obj = dynamic_cast<CGraphObj*>(tmp);
		obj->DrawText();
	}
	
	return 1;
}

bool CGUIRenderer::Update( float dt )
{
	Forms.Sort(InvFormSort);
	Forms.Reset();
	CObject *tmp;
	while (Forms.Enum(tmp))
	{
		CForm *obj = dynamic_cast<CForm*>(tmp);

		if (MouseIn(obj->Left, obj->Top, obj->Width, obj->HeaderHeight*obj->Properties[GUI_DRAWFORMHEADER]))
		{
			//header
			if (MouseFocus != NULL && MouseFocus != obj)
				if (!(ISFORM(MouseFocus)))
					MouseFocus->Update(dt);
			MouseFocus = obj;
			return true;
		}
		if (MouseIn(obj->Left, obj->Top + obj->HeaderHeight*obj->Properties[GUI_DRAWFORMHEADER], obj->Width, obj->Height))
		{
			//header
			obj->Update(dt);
			return true;
		}
	}
	if (MouseFocus != NULL)
		if (!(ISFORM(MouseFocus)))
			MouseFocus->Update(dt);
	return true;
}

CGUIRenderer::CGUIRenderer()
{
	FontManager = CFontManager::Instance("gui.cpp");
}

CGUIRenderer::~CGUIRenderer()
{
	FontManager->FreeInst("gui.cpp");
}
