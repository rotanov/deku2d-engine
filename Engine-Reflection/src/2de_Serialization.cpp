#include "2de_Serialization.h"
#include "2de_Xml.h"
#include "2de_RTTI.h"

namespace Deku2D
{
	void SerializeObjectToXml(CNullClass* object, const std::string& name)
	{
		struct T
		{
			static void Helper(Deku2D::CXMLNormalNode* node, const CNullClass* next)
			{
				for (CRTTI::PropertyIterator i(next->GetRTTI()); i.Ok(); i++)
				{
					if (i->IfRTTIzed())
					{
						Deku2D::CXMLNormalNode *newNode = new Deku2D::CXMLNormalNode(reinterpret_cast<const CNullClass*>(i->GetRaw(next))->GetRTTI()->GetName());
						node->Children.AddLast(newNode);					
						Helper(newNode, reinterpret_cast<const CNullClass*>(i->GetRaw(next)));
					}
					else
 						node->SetAttribute(i->GetName(), i->GetStringValue(next));
				}
			}
		};

		Deku2D::CXML xml;
		xml.Root.AddFirst(new Deku2D::CXMLPrologNode());

		Deku2D::CXMLNormalNode *node = new Deku2D::CXMLNormalNode(object->GetRTTI()->GetName());
		xml.Root.AddLast(node);

		T::Helper(node, object);
		xml.SaveToFile(name);
	}

};	// namespace Deku2D