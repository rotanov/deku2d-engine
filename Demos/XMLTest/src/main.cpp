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
	logical_root->SetAttribute("attributewithentities", "\"test\"");

	logical_root->Children.AddLast(new CXMLCommentNode("Some kind of comment1"));
	logical_root->Children.AddLast(new CXMLNormalNode("subelement2"));
	logical_root->Children.AddLast(new CXMLCommentNode("Some kind of comment2"));

/*

<subsub>
   test&quot;tetete&lt;   
</subsub>
<subsub>
	test white space 	handling shit			   
</subsub>
<subsub>
test&quot; this &lt;shit &gt; out	&lt;
</subsub>

*/
	CXMLNormalNode *subelement3 = new CXMLNormalNode("subelement3");
	logical_root->Children.AddLast(subelement3);

	CXMLNormalNode *subsub = new CXMLNormalNode("subsub");
	subsub->Children.AddLast(new CXMLTextNode("  test\"tetete<   "));
	subelement3->Children.AddLast(subsub);

	subsub = new CXMLNormalNode("subsub");
	subsub->Children.AddLast(new CXMLTextNode("	test white space 	handling shit			   "));
	subelement3->Children.AddLast(subsub);

	subsub = new CXMLNormalNode("subsub");
	subsub->Children.AddLast(new CXMLTextNode("test\" this <shit > out	<"));
	subelement3->Children.AddLast(subsub);
 
	CXMLCommentNode *commentnode = new CXMLCommentNode;
 	commentnode->SetValue("I'm comment");
 
 	logical_root->Children.AddLast(commentnode);
 
	// ну попрробуй.. сохрани меня!..
	xml.SaveToFile("test.xml");

	CXML parsetest;
	parsetest.LoadFromFile("test.xml");
	parsetest.SaveToFile("test1.xml");

	CXML somenewxml(parsetest);
	somenewxml.SaveToFile("test2.xml");
	CXML somenewxml2;
	somenewxml2 = somenewxml;
	somenewxml2.SaveToFile("test3.xml");

	cout << somenewxml.Root.Last()->Children[0]->NextSibling()->NextSibling("subelement3")->Children.First("subsub")->NextSimilarSibling()->Children.First()->GetValue() << endl;

	cout << somenewxml.Root.Last()->Children.Last()->Children.Last()->Children.Last()->GetParent()->GetParent()->GetParent()->GetValue() << endl;

	return 0;
}
