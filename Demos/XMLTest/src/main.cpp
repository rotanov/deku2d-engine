#include "2de_Xml.h"

int main()
{
	CXML xml;

	CXMLNormalNode *node = new CXMLNormalNode("element");
	xml.Root.AddLast(node);

	CXMLCommentNode *commentnode = new CXMLCommentNode;
	commentnode->SetValue("I'm comment");

	node->Children.AddLast(commentnode);

	// ну попрробуй.. сохрани меня!..
	// xml.SaveToFile("test.xml");

	return 0;
}
