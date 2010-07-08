#include "2de_Xml.h"

int main(int argc, char* argv[])
{
 	CXML xml;
 
 	CXMLNormalNode *node = new CXMLNormalNode("element");
 	xml.Root.Children.AddLast(node);
	node->Children.AddFirst(new CXMLNormalNode("subelement1"));
	node->SetAttribute("attr0", "val0");
	node->SetAttribute("attr1", "val1");
	node->SetAttribute("attr2", "val2");
	node->SetAttribute("attr3", "val3");
	node->SetAttribute("attr4", "val4");
	node->Children.AddLast(new CXMLCommentNode("Some kind of comment1"));
	node->Children.AddLast(new CXMLNormalNode("subelement2"));
	node->Children.AddLast(new CXMLCommentNode("Some kind of comment2"));
	node->Children.AddLast(new CXMLNormalNode("subelement3"));
 
 	CXMLCommentNode *commentnode = new CXMLCommentNode;
 	commentnode->SetValue("I'm comment");
 
 	node->Children.AddLast(commentnode);
 
	// ну попрробуй.. сохрани меня!..
	xml.SaveToFile("test.xml");

	return 0;
}
