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

	template <typename T>
	class IVisitor : virtual public IVisitorBase
	{
	public:
		virtual bool ShouldVisit(T&) = 0;
		virtual void VisitOnEnter(T&) = 0;
		virtual void VisitOnLeave(T&) = 0;
	};

	class IVisitableBase	// Actually never used anywhere in code for now.
	{
	public:
		virtual ~IVisitableBase() {}
		virtual void AcceptOnEnter(IVisitorBase&) = 0;
		virtual void AcceptOnLeave(IVisitorBase&) = 0;

	protected:
		template <typename T>
		static void ConcreteAcceptOnEnter(T& visited, IVisitorBase& visitor)
		{
			if (IVisitor<T>* ptr = dynamic_cast<IVisitor<T>*>(&visitor))
			{
				return ptr->VisitOnEnter(visited);
			}
		}

		template <typename T>
		static void ConcreteAcceptOnLeave(T& visited, IVisitorBase& visitor)
		{
			if (IVisitor<T>* ptr = dynamic_cast<IVisitor<T>*>(&visitor))
			{
				return ptr->VisitOnLeave(visited);
			}
		}
	};

	class IVisitableObject : public CObject	// Inherit from CObject to inject visitable interface into branch of the inheritance
	{
		D2D_TYPE_INFO_INJECT(IVisitableObject)

	public:
		virtual ~IVisitableObject() {}
		virtual void AcceptOnEnter(IVisitorBase&) = 0;
		virtual void AcceptOnLeave(IVisitorBase&) = 0;

	protected:
		template <typename T> 
		static void ConcreteAcceptOnEnter(T& visited, IVisitorBase& visitor)
		{
			if (IVisitor<T>* ptr = dynamic_cast<IVisitor<T>*>(&visitor))
			{
				if (ptr->ShouldVisit(visited))
					ptr->VisitOnEnter(visited);
			}
		}

		template <typename T> 
		static void ConcreteAcceptOnLeave(T& visited, IVisitorBase& visitor)
		{
			if (IVisitor<T>* ptr = dynamic_cast<IVisitor<T>*>(&visitor))
			{
				if (ptr->ShouldVisit(visited))
					ptr->VisitOnLeave(visited);
			}
		}
	};

	// Inject this in visitable class
	#define D2D_DECLARE_VISITABLE()\
		public:\
			virtual void AcceptOnEnter(IVisitorBase& visitor)\
			{\
				return ConcreteAcceptOnEnter(*this, visitor);\
			}\
\
			virtual void AcceptOnLeave(IVisitorBase& visitor)\
			{\
				return ConcreteAcceptOnLeave(*this, visitor);\
			}\

}	//	namespace Deku2D

#endif	// _2DE_VISITOR_H_
