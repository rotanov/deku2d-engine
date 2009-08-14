#include"Engine.h"
#include "LuaUtils.h"

GlobalLuaState* globalLuaState = 0;
CEngine* Ninja = CEngine::Instance();

#define		NUM_INPUTS 5
#define		MY_SYSTEM_FONT "data\\Font.fif"

byte			ZOOM  = 6;				// Zoom. That's All.
bool			keys[322];				// Array of keyboard handling boolean variables.
CGLWindow		window;					// Window class. All SDL GL Routines here.
CFont*			font;					// That's temp font to draw numbers 0-9.
CGLImageData	pic;					// That's font image.
int				x, y;					// Integers for handling mouse coords.
CRecti			rect[256];				// Rects for each symbol. Finally the most important data.
int				offx = 129;				// Magic numbers here, initializing offsets
int				offy = 0;				// ... same as before
int				rndex = 0;				// index of current rect
byte			Size = 16;				// One cell size.
char*			name = NULL;
char			temp[3];
int				focus;					// Index of input, having the focus
CMiniInput		input[NUM_INPUTS];

void NextControl()
{
	input[focus].havefocus = false;
	focus++;
	if (focus >= NUM_INPUTS)
		focus = 0;
	input[focus].havefocus = true;
}

void IncIndex()
{
	rndex++;
	if (rndex < 256)
	{
		input[0].text = new char [3];
		itoa(rndex, input[0].text, 10);
		input[0].symbols	= strlen(input[0].text);
		input[0].cp			= strlen(input[0].text);
		input[0].width		= input[0].font->GetStringWidth(input[0].text);
	}
	else
	{
		rndex--;
	}
}

void ProcessInput0(Uint16 unicode)
{
	{
		char ch;
		if ((unicode & 0xFF00) == 0)
			ch = unicode & 0xFF;
		if (ch <= '9' && ch >= '0')
		input[focus].AddChar(ch);
		if (atoi(input[focus].text)>=256)
		{
			itoa(255, input[focus].text, 10);
			input[focus].cp--;
			input[focus].symbols--;
			input[focus].width = input[0].font->GetStringWidth(input[0].text);
		}
		if (input[focus].symbols > 3)
		{
			input[focus].symbols = 3;
			input[focus].width = input[0].font->GetStringWidth(input[0].text);
			input[focus].cp = 3;
		}
		rndex = atoi(input[focus].text);
	}

}

void SomeInit()
{
// 	font->filename = MY_SYSTEM_FONT;
// 	if (!font->LoadFromFile())
// 	{
// 		Log("AHTUNG", "Can't Load main font: %s", MY_SYSTEM_FONT);
// 		printf("Warning: main font: %s have not been loaded\n", MY_SYSTEM_FONT);
// 	}
	font = Ninja->FontManager->GetFont("Font");

	input[0] = CMiniInput(10, 240, MY_SYSTEM_FONT);
	input[1] = CMiniInput(10, 190, MY_SYSTEM_FONT);
	input[2] = CMiniInput(10, 140, MY_SYSTEM_FONT);
	input[3] = CMiniInput(10, 90, MY_SYSTEM_FONT);
	input[4] = CMiniInput(10, 340, MY_SYSTEM_FONT);
	input[focus].havefocus = true;
	
	input[4].text = new char[256];
	strcpy(input[4].text,"Data//Font.bmp");
	input[4].symbols = strlen(input[4].text);
	input[4].cp = strlen(input[4].text);
	input[4].width = input[4].font->GetStringWidth(input[4].text);
	
	SDL_EnableUNICODE(1);

	input[0].text = new char [3];
	itoa(rndex, input[0].text, 10);
	input[0].symbols  = strlen(input[0].text);
	input[0].cp  = strlen(input[0].text);
	input[0].width = input[0].font->GetStringWidth(input[0].text);

	input[1].text = new char [3];
	itoa(ZOOM, input[1].text, 10);
	input[1].symbols  = strlen(input[1].text);
	input[1].cp  = strlen(input[1].text);
	input[1].width = input[1].font->GetStringWidth(input[1].text);

	input[2].text = new char [3];
	itoa(Size, input[2].text, 10);
	input[2].symbols  = strlen(input[2].text);
	input[2].cp  = strlen(input[2].text);
	input[2].width = input[2].font->GetStringWidth(input[2].text);
}

void LoadFromFile(char * filename)
{
	CFile file;

	if (!file.Open(filename, CFILE_READ))
	{
		Log("AHTUNG", "Can't Open font file: %s for editing", filename);
		printf("Can't Open font file: %s for editing\n", filename);
		return;
	}

	file.ReadLine(input[4].text);
	input[4].width = input[4].font->GetStringWidth(input[4].text);
	if (!pic.LoadTexture(input[4].text))
	{
		Log("AHTUNG", "Can't Load image: %s, specified in fontfile %s", input[4].text, filename);
		printf("Can't Load image: %s, specified in fontfile %s\n", input[4].text, filename);
		return;
	}
	offy = window.height - pic.height*ZOOM;
	file.Read(rect, sizeof(rect[0])*256);

	for(int i=0;i<256;i++)
	{
		rect[i].x0 *= ZOOM; 
		rect[i].x1 *= ZOOM;
		rect[i].y0 *= ZOOM;
		rect[i].y1 *= ZOOM;
	}
	file.Close();
}

void PleaseSaveMyWork(char * filename)
{
	CFile file;
	if (!file.Open(filename, CFILE_WRITE))
	{
		Log("AHTUNG", "Can't Open font file: %s for saving", filename);
		printf("Can't Open font file: %s for saving\n", filename);
		return;
	}

	file.Write(input[4].text, strlen(input[4].text));
	file.WriteByte((byte)0x00);

	for(int i=0;i<256;i++)
	{
		if (rect[i].x1 < rect[i].x0)
			std::swap(rect[i].x0, rect[i].x1);
		if (rect[i].y1 < rect[i].y0)
			std::swap(rect[i].y0, rect[i].y1);
		rect[i].x0 /= ZOOM; 
		rect[i].x1 /= ZOOM;
		rect[i].y0 /= ZOOM;
		rect[i].y1 /= ZOOM;
	}
	file.Write(&rect, sizeof(rect[0])*256);
	for(int i=0;i<256;i++)
	{
		rect[i].x0 *= ZOOM; 
		rect[i].x1 *= ZOOM;
		rect[i].y0 *= ZOOM;
		rect[i].y1 *= ZOOM;
	}
	file.Close();
}


void Draw()
{
	SDL_GetMouseState(&x, &y);
	glDisable(GL_SCISSOR_TEST);

	glDisable(GL_TEXTURE_2D);
	glLoadIdentity();
	glColor3f(.4f, .4f, .4f);
	glBegin(GL_QUADS);
		glVertex2i(0,	0);
		glVertex2i(128,	0);
		glVertex2i(128, 800);
		glVertex2i(0,	800);
	glEnd();
	glColor3f(.5f, .5f, .6f);
	glBegin(GL_LINES);
		glVertex2i(128,	0);
		glVertex2i(128, 800);

		glVertex2i(0,	570);
		glVertex2i(128, 570);

		if (x >= 10 && x <= 118)
			if (window.height-y >= 10 && window.height-y <= 40)
				glColor3f(0.9f, 0.8f, 0.2f);
			else
				glColor3f(0.6f, 0.7f, 0.8f);
		else
			glColor3f(0.6f, 0.7f, 0.8f);

		glVertex2i(10,	10);
		glVertex2i(118,	10);
		
		glVertex2i(118,	10);
		glVertex2i(118,	40);

		glVertex2i(118,	40);
		glVertex2i(10,	40);

		glVertex2i(10,	40);
		glVertex2i(10,	10);

		if (x >= 10 && x <= 118)
			if (window.height-y >= 50 && window.height-y <= 80)
				glColor3f(0.9f, 0.8f, 0.2f);
			else
				glColor3f(0.6f, 0.7f, 0.8f);
		else
			glColor3f(0.6f, 0.7f, 0.8f);


		glVertex2i(10,	50);
		glVertex2i(118,	50);
		
		glVertex2i(118,	50);
		glVertex2i(118,	80);

		glVertex2i(118,	80);
		glVertex2i(10,	80);

		glVertex2i(10,	80);
		glVertex2i(10,	50);

		if (x >= 10 && x <= 118)
			if (window.height-y >= 300 && window.height-y <= 330)
				glColor3f(0.9f, 0.8f, 0.2f);
			else
				glColor3f(0.6f, 0.7f, 0.8f);
		else
			glColor3f(0.6f, 0.7f, 0.8f);


		glVertex2i(10,	300);
		glVertex2i(118,	300);
		
		glVertex2i(118,	300);
		glVertex2i(118,	330);

		glVertex2i(118,	330);
		glVertex2i(10,	330);

		glVertex2i(10,	330);
		glVertex2i(10,	300);
	glEnd();


	glEnable(GL_TEXTURE_2D);
	SDL_GetMouseState(&x, &y);

	for(int i =0;i<NUM_INPUTS;i++)
		input[i].Update(x, window.height-y, SDL_GetMouseState(&x, &y));

	glColor3f(0.9f, 0.4f, 0.1f);
	font->p = Vector2(20, 580);
	font->Print("Font tool.");
	glColor3f(0.6f, 0.7f, 0.8f);
	font->p = Vector2(10, 270);
	font->Print("Char index:");
	font->p = Vector2(10, 220);
	font->Print("Zoom:");
	font->p = Vector2(10, 170);
	font->Print("Size:");
	font->p = Vector2(10, 120);
	font->Print("Name:");
	font->p = Vector2(10, 380);
	font->Print("Image file:");

			if (x >= 10 && x <= 118)
				if (window.height-y >= 10 && window.height-y <= 40)
				glColor3f(0.9f, 0.8f, 0.2f);
			else
				glColor3f(0.6f, 0.7f, 0.8f);
		else
			glColor3f(0.6f, 0.7f, 0.8f);

	font->p = Vector2(45, 20);
	font->Print("Save");

			if (x >= 10 && x <= 118)
			if (window.height-y >= 50 && window.height-y <= 80)
				glColor3f(0.9f, 0.8f, 0.2f);
			else
				glColor3f(0.6f, 0.7f, 0.8f);
		else
			glColor3f(0.6f, 0.7f, 0.8f);
	font->p = Vector2(45, 60);
	font->Print("Load");

		if (x >= 10 && x <= 118)
			if (window.height-y >= 300 && window.height-y <= 330)
				glColor3f(0.9f, 0.8f, 0.2f);
			else
				glColor3f(0.6f, 0.7f, 0.8f);
		else
			glColor3f(0.6f, 0.7f, 0.8f);

		font->p = Vector2(20, 310);
		font->Print("Load Image");


	glEnable(GL_SCISSOR_TEST);
	glScissor(128, 0, window.width-128, window.height);
	
	if (x > 128)
		if (SDL_GetRelativeMouseState(&x, &y)&SDL_BUTTON(2))
		{
			offx += x;
			offy -= y;
		}

		glEnable(GL_TEXTURE_2D);
		glLoadIdentity();
		glColor3f(1.0f, 1.0f, 1.0f);
		glTranslatef(offx/ZOOM*ZOOM, offy/ZOOM*ZOOM, 0);
		glBindTexture(GL_TEXTURE_2D, pic.GetTexID());
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(0,				0				);
		glTexCoord2f(1, 0); glVertex2f(pic.width*ZOOM,	0				);
		glTexCoord2f(1, 1); glVertex2f(pic.width*ZOOM,	pic.height*ZOOM	);
		glTexCoord2f(0, 1); glVertex2f(0,				pic.height*ZOOM	);
		glEnd();
	
	glDisable(GL_TEXTURE_2D);

	if (SDL_GetMouseState(&x, &y)&SDL_BUTTON(1))
	{if (x > 128)
	{
		rect[rndex].x1 = x/ZOOM*ZOOM - offx/ZOOM*ZOOM;
		rect[rndex].y1 = (window.height - y)/ZOOM*ZOOM - offy/ZOOM*ZOOM;
	}
	}
	if (SDL_GetMouseState(&x, &y)&SDL_BUTTON(3))
	{if (x > 128)
	{
		rect[rndex].x0 = (x/ZOOM)*ZOOM - offx/ZOOM*ZOOM;
		rect[rndex].y0 = ((window.height - y)/ZOOM)*ZOOM - offy/ZOOM*ZOOM;
	}
	}

		glEnable(GL_LINE_WIDTH);
		glLineWidth(3.0f);
		glLoadIdentity();
		glTranslatef(offx/ZOOM*ZOOM, offy/ZOOM*ZOOM, 0);
		glColor3f(0.4f,0.5f,0.6f);
		glBegin(GL_LINES);
		for(int i =0;i<256;i++)
		{
			glVertex2i(rect[i].x0, rect[i].y0);
			glVertex2i(rect[i].x1, rect[i].y0);
			
			glVertex2i(rect[i].x1, rect[i].y0);
			glVertex2i(rect[i].x1, rect[i].y1);
	
			glVertex2i(rect[i].x1, rect[i].y1);
			glVertex2i(rect[i].x0, rect[i].y1);
			
			glVertex2i(rect[i].x0, rect[i].y1);
			glVertex2i(rect[i].x0, rect[i].y0);
		}
		glEnd();

		
		for(int i =0;i<256;i++)
		{
			
			itoa(i, temp, 10);
			font->p = Vector2( (rect[i].x0+rect[i].x1)/2  + offx/ZOOM*ZOOM, (rect[i].y0+rect[i].y1)/2 + offy/ZOOM*ZOOM);
			font->Print(temp);
		}

		glLoadIdentity();
		glTranslatef(offx/ZOOM*ZOOM, offy/ZOOM*ZOOM, 0);
		
		glLineWidth(1.0f);
		glBegin(GL_LINES);
		glColor4f(0.5,0.5,0.5,0.5);
		for(unsigned int i=0;i<=pic.width;i+=Size)
			{
				glVertex2i(i*ZOOM,		0		);
				glVertex2i(i*ZOOM,		pic.height*ZOOM);
			}
		for(unsigned int i=0;i<=pic.height;i+=Size)
			{
				glVertex2i(0,			i*ZOOM	);
				glVertex2i(pic.width*ZOOM,	i*ZOOM	);
			}
		glEnd();

		glDisable(GL_LINE_WIDTH);
		glLineWidth(1.0f);
}

bool ProcessEvents()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
			{
				SDLKey sym = event.key.keysym.sym;
				keys[sym] = 1;

				if (sym == SDLK_BACKSPACE)			//Deleting character in active input
					input[focus].DelChar();

				if (sym == SDLK_TAB)
					NextControl();

				if (sym == SDLK_LCTRL)				//Incrementing current rect index
					IncIndex();
				
				if (focus == 0)
					ProcessInput0(event.key.keysym.unicode);
				else
				{
					char ch;
					if ((event.key.keysym.unicode & 0xFF00) == 0 )
						ch = event.key.keysym.unicode & 0xFF;
					if (ch >= ' ')
					{
						input[focus].AddChar(ch);
						if (focus == 1 && atoi(input[focus].text)!=0)
						{
							for(int i=0;i<256;i++)
							{
								if (rect[i].x1 < rect[i].x0)
									std::swap(rect[i].x0, rect[i].x1);
								if (rect[i].y1 < rect[i].y0)
									std::swap(rect[i].y0, rect[i].y1);
								rect[i].x0 /= ZOOM; 
								rect[i].x1 /= ZOOM;
								rect[i].y0 /= ZOOM;
								rect[i].y1 /= ZOOM;
							}
							ZOOM = atoi(input[focus].text);

							for(int i=0;i<256;i++)
							{
								rect[i].x0 *= ZOOM; 
								rect[i].x1 *= ZOOM;
								rect[i].y0 *= ZOOM;
								rect[i].y1 *= ZOOM;
							}

							offy = window.height - pic.height*ZOOM;
						}
						if (focus == 2 && atoi(input[focus].text)!=0)
						{
							Size = atoi(input[focus].text);
						}

					}
				}

				break;
			}
			case SDL_KEYUP:
			{
				SDLKey sym = event.key.keysym.sym;
				keys[sym] = 0;
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				if(event.button.button == SDL_BUTTON_LEFT)
				{
					if (event.button.x >= 10 && event.button.x <= 118)
						if (window.height-event.button.y >= 10 && window.height-event.button.y <= 40)
							PleaseSaveMyWork(input[3].text);

					if (event.button.x >= 10 && event.button.x <= 118)
						if (window.height-event.button.y >= 50 && window.height-event.button.y <= 80)
							LoadFromFile(input[3].text);

					if (event.button.x >= 10 && event.button.x <= 118)
						if (window.height-event.button.y >= 300 && window.height-event.button.y <= 330)
						{
							if (!pic.LoadTexture(input[4].text))
							{
								Log("AHTUNG", "Can't Load image: %s you specifed", input[4].text);
								printf("Can't Load image: %s you specifed\n", input[4].text);
							}
							offy = window.height - pic.height*ZOOM;
						}
				}
				break;
			}
			case SDL_VIDEORESIZE:
				{
					window.glResize(event.resize.w, event.resize.h);
					break;
				}
			case SDL_QUIT:
				SDLGLExit(1);
			break;
			default:
			{
				break;
			}
		}
	}
	return true;
}



int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_USER_INIT, &SomeInit);
	Ninja->SetState(STATE_RENDER_FUNC, &Draw);	
	Ninja->Run();
	Ninja->FreeInst();
	return 0x01;
}