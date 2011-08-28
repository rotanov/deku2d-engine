#ifndef _2DE_INPUT_H_
#define _2DE_INPUT_H_

#include "2de_Core.h"
#include "2de_Xml.h"

namespace Deku2D
{
	class CKeyBindingManager : public CObject
	{
	public:
		bool IsBound(SDLKey AKey, const string &ABindGroup, const string &ABindAction);
		void Bind(const string &ABindGroup, const string &ABindAction, SDLKey AKey);
		void Unbind(const string &ABindGroup, const string &ABindAction, SDLKey AKey);
		void UnbindAction(const string &ABindGroup, const string &ABindAction);

		void Load();
		void Save();

	private:
		typedef multimap<string, SDLKey> BindsContainer;
		typedef map<string, BindsContainer> BindGroupsContainer;
		typedef BindsContainer::iterator BindsIterator;
		typedef BindGroupsContainer::iterator BindGroupsIterator;

		const char *ROOT_NODE_NAME;

		CKeyBindingManager();

		BindGroupsContainer Bindings;

		CXML Data;
		string Filename;

		friend class CTSingleton<CKeyBindingManager>;
	};

	static CTSingleton<CKeyBindingManager> KeyBindingManager;

}	//	namespace Deku2D


#endif // _2DE_INPUT_H_
