#include "2de_Xml.h"

int main(int argc, char* argv[])
{
 	CXML xml;
 
 	CXMLNormalNode *logical_root = new CXMLNormalNode("root"); // only logical root, not really
	xml.Root.AddLast(new CXMLPrologNode);
 	xml.Root.AddLast(logical_root);

	logical_root->Children.AddFirst(new CXMLNormalNode("subelement1"));
	logical_root->SetAttribute("attr0", "val0");
	logical_root->SetAttribute("attr1", "val1");
	logical_root->SetAttribute("attr2", "val2");
	logical_root->SetAttribute("attr3", "val3");
	logical_root->SetAttribute("attr4", "val4");
	logical_root->SetAttribute("a0", "val4");

	logical_root->Children.AddLast(new CXMLCommentNode("Some kind of comment1"));
	logical_root->Children.AddLast(new CXMLNormalNode("subelement2"));
	logical_root->Children.AddLast(new CXMLCommentNode("Some kind of comment2"));

	CXMLNormalNode *subsub = new CXMLNormalNode("subsub");
	CXMLNormalNode *subelement3 = new CXMLNormalNode("subelement3");

	subelement3->Children.AddLast(subsub);
	logical_root->Children.AddLast(subelement3);

	subsub->Children.AddLast(new CXMLTextNode("   test   "));
 
 	CXMLCommentNode *commentnode = new CXMLCommentNode;
 	commentnode->SetValue("I'm comment");
 
 	logical_root->Children.AddLast(commentnode);
 
	// ну попрробуй.. сохрани меня!..
	xml.SaveToFile("test.xml");

	CXML parsetest;
	parsetest.LoadFromFile("test.xml");
	parsetest.SaveToFile("test1.xml");

	return 0;
}
