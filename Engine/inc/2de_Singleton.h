#ifndef _D2D_SINGLETON_H_
#define _D2D_SINGLETON_H_

#include "2de_Object.h"

namespace Deku2D
{
	/**
	 * CSingletonManager - a class that is responsible for destroying singletones.
	 */

	class CSingletonManager
	{
	public:
		static CSingletonManager* Instance();
		static void Init();
		void Add(CObject *AObject);
		void Clear();
		static void Finalize();

	private:
		CSingletonManager()
		{
		}
		static CSingletonManager *_instance;
		stack<CObject*> Singletones;
	};


	/**
	 * CTSingleton - singleton template class with automatic deletion using CSingletonManager. Acts like a singleton holder.
	 *
	 * To use it just create a static instance of this class as follows:
	 *
	 * 	class CSomeClass
	 * 	{
	 * 	public:
	 * 		<...>
	 * 	private:
	 * 		<...>
	 *	protected:
	 *		friend class CTSingleton<CSomeClass>;	// <-- don't forget it
	 *		CSomeClass() { }
	 * 	};
	 *
	 * 	static CTSingleton<CSomeClass> SomeClass;
	 *
	 * Now you can acces your singletoned class with -> operator, like this:
	 * 	SomeClass->SomeMethod();
	 *
	 *
	 * @todo: более описательное имя, а то разных реализаций синглтона всё-таки бывает много.
	 *	@todo: переделать всё это, как_у_александреску.
	 */

	template <typename T>
	class CTSingleton
	{
	public:
		static T* Instance();

		T* operator->()
		{
			return Instance();
		}

	private:
		static T * _instance;

	#ifdef _DEBUG
		static set<const std::type_info *> UnderConstruction;
	#endif // _DEBUG
	};

	template <typename T>
	T* CTSingleton<T>::Instance()
	{
	#ifdef _DEBUG
		if (UnderConstruction.count(&typeid(T)) == 1)
		{
			// we can't use CLog here (because it's singleton too), but still able to log to stdout
			Environment::LogToStdOut("ERROR", "Recursive singleton constructor call has been discovered, typeid: '%s'", typeid(T).name());

			//throw std::logic_error(string("Recursive singleton constructor call has been discovered, typeid: ") + typeid(T).name());
			D2D_RUNTIME_ERROR(string("Recursive singleton constructor call has been discovered, typeid: ") + typeid(T).name())
		}
	#endif // _DEBUG

		if (!_instance)
		{
	#ifdef _DEBUG
			UnderConstruction.insert(&typeid(T));
	#endif // _DEBUG

			_instance = new T;
			CSingletonManager::Instance()->Add(_instance);

	#ifdef _DEBUG
			UnderConstruction.erase(&typeid(T));
	#endif // _DEBUG
		}
		return _instance;
	}

	template <typename T>
	T* CTSingleton<T>::_instance = 0;

	#ifdef _DEBUG
	template <typename T>
	set<const std::type_info *> CTSingleton<T>::UnderConstruction;
	#endif // _DEBUG

};	// namespace Deku2D

#endif // _D2D_SINGLETON_H_
