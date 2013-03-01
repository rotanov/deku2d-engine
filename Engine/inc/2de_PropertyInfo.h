#pragma once

#include "2de_Core.h"
#include <string>

using std::string;

class PropertyInfo
{
public:
	virtual const char* Name() const = 0;
	virtual const char* TypeName() const = 0;
	virtual const char* OwnerName() const = 0;	
	virtual void SetValue(void *owner, void *property) = 0;
	virtual void* GetValue(const void *owner) = 0;
	virtual bool IsPointer() = 0;
	virtual bool Integral() = 0;
	virtual bool IsArray() = 0;
	virtual void PushValue(void *owner, void *value) = 0;
	virtual unsigned GetArraySize(const void *owner) = 0;
	virtual void* GetValue(const void *owner, unsigned index) = 0;
};

/////////////////////////////////////////////////////////////////
#define _D2D_BEGIN_DECLARE_PROPERTY(OWNER, NAME)				\
	class OwnedPropertyInfo##OWNER##NAME : public PropertyInfo	\
	{															\
	public:														\

/////////////////////////////////////////////////////////////////////////////////
#define _D2D_DECLAREE_PROPERTY_BASE(OWNER, TYPE, NAME)							\
		OwnedPropertyInfo##OWNER##NAME()										\
		{																		\
			TypeInfo::GetTypeInfo(#OWNER)->Properties()[#NAME] = &_instance;	\
		}																		\
																				\
		virtual const char* Name() const										\
		{																		\
			return #NAME;														\
		}																		\
																				\
		virtual const char* TypeName() const									\
		{																		\
			return #TYPE;														\
		}																		\
																				\
		virtual const char* OwnerName() const									\
		{																		\
			return #OWNER;														\
		}																		\

/////////////////////////////////////////////////////////////////////////////////
#define _D2D_END_DECLARE_PROPERTY(OWNER, NAME)									\
		static OwnedPropertyInfo##OWNER##NAME _instance;						\
	};																			\
																				\
	OwnedPropertyInfo##OWNER##NAME OwnedPropertyInfo##OWNER##NAME::_instance;	\

/////////////////////////////////////////////////////////////////////////
#define D2D_DECLARE_PROPERTY_INFO_EX(OWNER, TYPE, NAME, SETTER, GETTER)	\
	_D2D_BEGIN_DECLARE_PROPERTY(OWNER, NAME)							\
	_D2D_DECLAREE_PROPERTY_BASE(OWNER, TYPE, NAME)						\
																		\
		virtual void SetValue(void* owner, void *property)				\
		{																\
			OWNER *typedOwner = static_cast<OWNER*>(owner);				\
			TYPE *typedProperty = static_cast<TYPE*>(property);			\
			typedOwner->SETTER(*typedProperty);							\
		}																\
																		\
		void* GetValue(const void *owner)								\
		{																\
			TYPE *value = new TYPE;										\
			*value = static_cast<const OWNER*>(owner)->GETTER();		\
			return value;												\
		}																\
																		\
		bool Integral()													\
		{																\
			return Deku2D::IsIntegral<TYPE>::result;					\
		}																\
																		\
		bool IsPointer()												\
		{																\
			return false;												\
		}																\
																		\
		bool IsArray()													\
		{																\
			return false;												\
		}																\
																		\
		void PushValue(void *owner, void *value)						\
		{																\
			throw std::runtime_error("Not Implemented.");				\
		}																\
																		\
		unsigned GetArraySize(const void *owner)						\
		{																\
			throw std::runtime_error("Not Implemented.");				\
		}																\
																		\
		void* GetValue(const void *owner, unsigned index)				\
		{																\
			throw std::runtime_error("Not Implemented.");				\
		}																\
																		\
	_D2D_END_DECLARE_PROPERTY(OWNER, NAME)								\

/////////////////////////////////////////////////////////////////////////////////////////////
#define D2D_DECLARE_ARRAY_PROPERTY_INFO_EX(OWNER, TYPE, NAME, PUSHER, GETTER, SIZEGETTER)	\
	_D2D_BEGIN_DECLARE_PROPERTY(OWNER, NAME)												\
	_D2D_DECLAREE_PROPERTY_BASE(OWNER, TYPE, NAME)											\
																							\
		virtual void SetValue(void* owner, void *property)									\
		{																					\
			throw std::runtime_error("Not Implemented.");									\
		}																					\
																							\
		void* GetValue(const void *owner)													\
		{																					\
			throw std::runtime_error("Not Implemented.");									\
		}															\
																							\
		bool Integral()																		\
		{																					\
			return false;																	\
		}																					\
																							\
		bool IsPointer()																	\
		{																					\
			return false;																	\
		}																					\
																							\
		bool IsArray()																		\
		{																					\
			return true;																	\
		}																					\
																							\
		void PushValue(void *owner, void *value)											\
		{																					\
			static_cast<OWNER*>(owner)->PUSHER(*(static_cast<TYPE*>(value)));				\
		}																					\
																							\
		unsigned GetArraySize(const void *owner)											\
		{																					\
			return static_cast<const OWNER*>(owner)->SIZEGETTER();							\
		}																					\
																							\
		void* GetValue(const void *owner, unsigned index)									\
		{																					\
			TYPE *value = new TYPE;															\
			*value = static_cast<const OWNER*>(owner)->GETTER(index);						\
			return value;																	\
		}																					\
																							\
	_D2D_END_DECLARE_PROPERTY(OWNER, NAME)													\

/////////////////////////////////////////////////////////////////////////////////////////////////
#define D2D_DECLARE_PTR_ARRAY_PROPERTY_INFO_EX(OWNER, TYPE, NAME, PUSHER, GETTER, SIZEGETTER)	\
	_D2D_BEGIN_DECLARE_PROPERTY(OWNER, NAME)													\
	_D2D_DECLAREE_PROPERTY_BASE(OWNER, TYPE, NAME)												\
																								\
		virtual void SetValue(void* owner, void *property)										\
		{																						\
			throw std::runtime_error("Not Implemented.");										\
		}																						\
																								\
		void* GetValue(const void *owner)														\
		{																						\
			throw std::runtime_error("Not Implemented.");										\
		};																						\
																								\
		bool Integral()																			\
		{																						\
			return false;																		\
		}																						\
																								\
		bool IsPointer()																		\
		{																						\
			return true;																		\
		}																						\
																								\
		bool IsArray()																			\
		{																						\
			return true;																		\
		}																						\
																								\
		void PushValue(void *owner, void *value)												\
		{																						\
			static_cast<OWNER*>(owner)->PUSHER(static_cast<TYPE*>(value));						\
		}																						\
																								\
		unsigned GetArraySize(const void *owner)												\
		{																						\
			return static_cast<const OWNER*>(owner)->SIZEGETTER();								\
		}																						\
																								\
		void* GetValue(const void *owner, unsigned index)										\
		{																						\
			TYPE *value = NULL;																	\
			value = static_cast<const OWNER*>(owner)->GETTER(index);							\
			return value;																		\
		}																						\
																								\
	_D2D_END_DECLARE_PROPERTY(OWNER, NAME)														\

/////////////////////////////////////////////////////////////////////////////
#define D2D_DECLARE_PTR_PROPERTY_INFO_EX(OWNER, TYPE, NAME, SETTER, GETTER)	\
	_D2D_BEGIN_DECLARE_PROPERTY(OWNER, NAME)								\
	_D2D_DECLAREE_PROPERTY_BASE(OWNER, TYPE, NAME)							\
																			\
	virtual void SetValue(void* owner, void *property)						\
	{																		\
		OWNER *typedOwner = static_cast<OWNER*>(owner);						\
		TYPE *typedProperty = static_cast<TYPE*>(property);					\
		typedOwner->SETTER(typedProperty);									\
	}																		\
																			\
	void* GetValue(const void *owner)										\
	{																		\
		return static_cast<const OWNER*>(owner)->GETTER();					\
	}																		\
																			\
	bool Integral()															\
	{																		\
		return Deku2D::IsIntegral<TYPE>::result;							\
	}																		\
																			\
	bool IsPointer()														\
	{																		\
		return true;														\
	}																		\
																			\
	bool IsArray()															\
	{																		\
		return false;														\
	}																		\
																			\
	void PushValue(void *owner, void *value)								\
	{																		\
		throw std::runtime_error("Not implemented.");						\
	}																		\
																			\
	unsigned GetArraySize(const void *owner)								\
	{																		\
		throw std::runtime_error("Not implemented.");						\
	}																		\
																			\
	void* GetValue(const void *owner, unsigned index)						\
	{																		\
		throw std::runtime_error("Not implemented.");						\
	}																		\
																			\
	_D2D_END_DECLARE_PROPERTY(OWNER, NAME)									\

/////////////////////////////////////////////////////////////////////////////////
#define D2D_DECLARE_PTR_PROPERTY_INFO(OWNER, TYPE, NAME)						\
	D2D_DECLARE_PTR_PROPERTY_INFO_EX(OWNER, TYPE, NAME, Set##NAME, Get##NAME)	\

/////////////////////////////////////////////////////////////////////////////
#define D2D_DECLARE_PROPERTY_INFO(OWNER, TYPE, NAME)						\
	D2D_DECLARE_PROPERTY_INFO_EX(OWNER, TYPE, NAME, Set##NAME, Get##NAME)	\

