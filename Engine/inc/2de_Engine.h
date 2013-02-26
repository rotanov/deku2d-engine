#ifndef _2DE_ENGINE_H_
#define _2DE_ENGINE_H_

#include "2de_Core.h"
#include "2de_GameObject.h"
#include "2de_PlaceableComponent.h"
#include "2de_RenderableComponent.h"
#include "2de_Text.h"
#include "2de_TimerComponent.h"
#include "2de_Prototype.h"
#include "2de_Config.h"
#include "2de_Event.h"
#include "2de_Factory.h"
#include "2de_GameUtils.h"
#include "2de_GraphicsLow.h"
#include "2de_GraphicsHigh.h"
#include "2de_ImageUtils.h"
#include "2de_Input.h"
#include "2de_LuaUtils.h"
#include "2de_Math.h"
#include "2de_Resource.h"
#include "2de_Sound.h"
#include "2de_SpatialManager.h"
#include "2de_Xml.h"

namespace Deku2D
{
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

		bool isRunning() const;
		bool isFinalizing() const;

		template<typename T>
		void SetStateHandler();

		void Lock();
		void Unlock();

		bool IsExitOnEscapeEnabled() const;
		bool IsLimitFPSEnabled() const;
		bool IsCalcFPSEnabled() const;
		bool IsKeyRepeatEnabled() const;
		bool IsShowFPSEnabled() const;
		bool IsIdleWhenInBackground() const;

		void SetExitOnEscape(bool AdoExitOnEscape);
		void SetDoLimitFPS(bool AdoLimitFPS);
		void SetDoCalcFPS(bool AdoCalcFPS);
		// temporarily here.. basically, we need key repeat in GUI only, so in 
		// future it'll be handled by focusable GUI widget-groups (forms, panels, whatever..)
		void SetDoKeyRepeat(bool AdoKeyRepeat); 
		void SetDoShowFPS(bool AdoShowFPS);
		void SetIdleWhenInBackground(bool AIdleWhenInBackground);

		string GetProgramName() const;
		void SetProgramName(const string &AProgramName);

		float GetDeltaTime() const;
		unsigned long GetFPS() const;
		unsigned long GetFPSLimit() const;
		void SetFPSLimit(unsigned long AFPSLimit);

		bool IsKeyDown(const SDLKey& AKey) const;
		const Vector2& GetMousePosition() const;

		CGameObject *RootGameObject;

	protected:
		CEngine();
		~CEngine();

	private:
		bool Keys[SDLK_LAST];
		Vector2 MousePosition;
		float dt;

		bool Initialized;
		bool Running;
		bool Finalizing;
		
		bool isHaveFocus;
		bool IdleWhenInBackground;

		bool doExitOnEscape;
		bool doLoadDefaultResourceList;
		bool doLimitFPS;
		bool doCalcFPS;
		unsigned long FPSCount;
		unsigned long FPSLimit;

		string ProgramName;
		CText *FPSText;

		SDL_mutex *BigEngineLock;

		CAbstractStateHandler *StateHandler;
		static CEngine EngineInstance;
		
		bool Initialize();
		void Finalize();
		bool ProcessEvents();
		bool ProcessArguments(int argc, char *argv[]);

		// possibly incapsulate into something.. too many things in this class are dedicated to fps...
		void CalcFPS();
		bool LimitFPS();

		void SetInitialValues();
	};

	template<typename T>
	void CEngine::SetStateHandler()
	{
		if (StateHandler)
			delete StateHandler;
		
		StateHandler = new T;
	}

}	//	namespace Deku2D

#endif // _2DE_ENGINE_H_
