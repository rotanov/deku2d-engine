#ifndef _2DE_VISITOR_H_
#define _2DE_VISITOR_H_

#include "2de_Object.h"

namespace Deku2D
{
	/**
	 * Here are classes implementing visitor pattern.
	 * Taken from http://www.everfall.com/paste/id.php?bqa1eibc559f
	 * http://www.gamedev.ru/community/oo_design/articles/?id=431
	 * In our case we have visitable objects organized in tree, so
	 * We want to visit them as we enter the node and as we leave it,
	 * While dfs/bfs -traversing the tree. Because of that we have:
	 *	VisitOnEnter()
	 *	VisitOnLeave()
	 */

	class IVisitorBase
	{
	public:	
		virtual ~IVisitorBase() {};
	};

	template <typename T, typename R = void>
	class IVisitor : virtual public IVisitorBase
	{
	public:
		typedef R CReturnType;
		virtual CReturnType VisitOnEnter(T&) = 0;
		virtual CReturnType VisitOnLeave(T&) = 0;
	};

	template <typename R = void>
	class IVisitableBase	// Actually never used anywhere in code for now.
	{
	public:
		typedef R CReturnType;
		virtual ~IVisitableBase() {}
		virtual CReturnType AcceptOnEnter(IVisitorBase&) = 0;
		virtual CReturnType AcceptOnLeave(IVisitorBase&) = 0;

	protected:
		template <typename T>
		static CReturnType ConcreteAcceptOnEnter(T& visited, IVisitorBase& visitor)
		{
			if (IVisitor<T, R>* ptr = dynamic_cast<IVisitor<T, R>*>(&visitor))
			{
				return ptr->VisitOnEnter(visited);
			}
			return CReturnType();
		}

		template <typename T>
		static CReturnType ConcreteAcceptOnLeave(T& visited, IVisitorBase& visitor)
		{
			if (IVisitor<T, R>* ptr = dynamic_cast<IVisitor<T, R>*>(&visitor))
			{
				return ptr->VisitOnLeave(visited);
			}
			return CReturnType();
		}
	};

	template <typename R = void>
	class IVisitableObject : public CObject	// Inherit from CObject to inject visitable interface into branch of the inheritance
	{
		D2D_DECLARE_RTTI(IVisitableObject, CObject)

	public:
		typedef R CReturnType;
		virtual ~IVisitableObject() {}
		virtual CReturnType AcceptOnEnter(IVisitorBase&) = 0;
		virtual CReturnType AcceptOnLeave(IVisitorBase&) = 0;

	protected:
		template <typename T> 
		static CReturnType ConcreteAcceptOnEnter(T& visited, IVisitorBase& visitor)
		{
			if (IVisitor<T, R>* ptr = dynamic_cast<IVisitor<T, R>*>(&visitor))
			{
				return ptr->VisitOnEnter(visited);
			}
			return CReturnType();
		}

		template <typename T> 
		static CReturnType ConcreteAcceptOnLeave(T& visited, IVisitorBase& visitor)
		{
			if (IVisitor<T, R>* ptr = dynamic_cast<IVisitor<T, R>*>(&visitor))
			{
				return ptr->VisitOnLeave(visited);
			}
			return CReturnType();
		}
	};

	// Inject this in visitable class
	#define D2D_DECLARE_VISITABLE()	\
		public:	\
			virtual CReturnType AcceptOnEnter(IVisitorBase& visitor)	\
			{ return ConcreteAcceptOnEnter(*this, visitor); }	\
			virtual CReturnType AcceptOnLeave(IVisitorBase& visitor)	\
			{ return ConcreteAcceptOnLeave(*this, visitor); }

};	//	namespace Deku2D

#endif	// _2DE_VISITOR_H_
