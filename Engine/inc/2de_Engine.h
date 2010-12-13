#ifndef _2DE_ENGINE_H_
#define _2DE_ENGINE_H_

#include "2de_Core.h"
#include "2de_Components.h"
#include "2de_Config.h"
#include "2de_Event.h"
#include "2de_Factory.h"
#include "2de_GameUtils.h"
#include "2de_GraphicsLow.h"
#include "2de_GraphicsHigh.h"
#include "2de_Gui.h"
#include "2de_ImageUtils.h"
#include "2de_LuaUtils.h"
#include "2de_Math.h"
#include "2de_Resource.h"
#include "2de_Sound.h"
#include "2de_SpatialManager.h"
#include "2de_Update.h"
#include "2de_Xml.h"

const int ENGINE_VERSION = 0x001;	// it's a pity that it's not used.. i DO WANT some build counter, that increments automatically every build :)
const float FIXED_DELTA_TIME = 0.02f;

class CAbstractStateHandler
{
public:
	virtual void OnBeforeInitialize() { }
	virtual bool OnInitialize() { return true; }
	virtual void OnBeforeFinalize() { }
	virtual void OnFinalize() { }

	virtual void OnFocusGain() { }
	virtual void OnFocusLost() { }

	virtual bool OnArgumentsProcessing() { return true; }
};

class CEngine
{
public:
// IMMA BREAKING THE INCAPSULATION FOR DEBUGGING AND TESTING
	CAbstractSpatialManager *SpatialManager;
// SORRY
	static CEngine* Instance();

	bool Run(int argc, char *argv[]);
	void Pause();
	void ShutDown();

	bool isFinalizing() const;

	template<typename T>
	void SetStateHandler();

	bool IsExitOnEscapeEnabled() const;
	bool IsLimitFPSEnabled() const;
	bool IsCalcFPSEnabled() const;
	bool IsKeyRepeatEnabled() const;
	bool IsShowFPSEnabled(bool AdoShowFPS) const;
	void SetExitOnEscape(bool AdoExitOnEscape);
	void SetDoLimitFPS(bool AdoLimitFPS);
	void SetDoCalcFPS(bool AdoCalcFPS);

	// temporarily here.. basically, we need key repeat in GUI only, so in 
	// future it'll be handled by focusable GUI widget-groups (forms, panels, whatever..)
	void SetDoKeyRepeat(bool AdoKeyRepeat); 
	void SetDoShowFPS(bool AdoShowFPS);

	float GetDeltaTime() const;
	string GetProgramName() const;
	void SetProgramName(const string &AProgramName);
	unsigned long GetFPSLimit() const;
	void SetFPSLimit(unsigned long AFPSLimit);
	bool IsKeyDown(const SDLKey& AKey) const;
	const Vector2& GetMousePosition() const;
	unsigned long GetFPS() const;

protected:
	CEngine();
	~CEngine();

private:
	CMouseCursor *Cursor;	// @todo: rename and rearrange these variables according to common sense.
	bool Keys[SDLK_LAST];
	Vector2 MousePosition;
	float dt;

	bool Initialized;
	bool Finalizing;
	
	bool isHaveFocus;
	bool userReInit;

	bool doExitOnEscape;
	bool doLoadDefaultResourceList;
	bool doLimitFPS;
	bool doCalcFPS;
	unsigned long FPSCount;
	unsigned long FPSLimit;

	string ProgramName;
	CText *FPSText;

	CAbstractStateHandler *StateHandler;
	static CEngine EngineInstance;
	
	bool Initialize();
	void Finalize();
	bool ProcessEvents();
	bool ProcessArguments(int argc, char *argv[]);

	// possibly incapsulate into something.. too many things in this class are dedicated to fps...
	void CalcFPS();
	bool LimitFPS();
};

template<typename T>
void CEngine::SetStateHandler()
{
	if (StateHandler)
		delete StateHandler;
	
	StateHandler = new T;
}

#endif // _2DE_ENGINE_H_
