#include "2de_Serialization.h"
#include "2de_Xml.h"
#include "2de_RTTI.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

namespace Deku2D
{
	void SerializeObjectToXml(CNullClass* object, const std::string& name)
	{
		/*
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
		*/
	}

	void SerializeObjectToJSON(CNullClass* object, const std::string& name)
	{
		/*
		class CStateInfo
		{
		public:
			int depth;
			string s;

			CStateInfo() : depth(0), s("") {}
			void SaveToFile(const string& filename)
			{
				FILE* fo = fopen(filename.c_str(), "w");
				fprintf(fo, "%s", s.c_str());
				fclose(fo);
			}
		};

		struct T
		{
			static string RepeatString(string str, unsigned count)
			{
				string r = "";
				while (count)
					r += str, count--;
				return r;
			}

			static void Helper(CStateInfo& state, const CNullClass* next)
			{
				state.s += RepeatString( "\t", state.depth );
				state.s += "{\n";
				state.depth++;

				for (CRTTI::PropertyIterator i(next->GetRTTI()); i.Ok(); i++)
				{
					if (i->IfRTTIzed())
					{
						state.s += RepeatString("\t", state.depth) + "\"" + reinterpret_cast<const CNullClass*>(i->GetRaw(next))->GetRTTI()->GetName() + "\" : \n";
						Helper(state, reinterpret_cast<const CNullClass*>(i->GetRaw(next)));
					}
					else
					{
						state.s += RepeatString("\t", state.depth) + "\"" + i->GetName() + "\" : \"" +  i->GetStringValue(next) + "\",\n";
						state.s += RepeatString("\t", state.depth) + "\"" + "Type" + "\" : \"" +  i->GetTypeName() + "\",\n";
						state.s += RepeatString("\t", state.depth) + "\"" + "Size" + "\" : \"" +  to_string(i->GetSize()) + "\",\n";
					}
				}
				state.depth--;
				state.s += RepeatString( "\t", state.depth );
				state.s += "},\n";
			}

			
		};

		CStateInfo state;
		state.s += "\"" + object->GetRTTI()->GetName() + "\" : \n";
		T::Helper(state, object);
		state.SaveToFile(name);
		*/
	}


	void Innuendo(CNullClass* object, const std::string& name)
	{
		/*
		struct T
		{

			static void Helper(const CNullClass* next)
			{

				for (CRTTI::PropertyIterator i(next->GetRTTI()); i.Ok(); i++)
				{
					if (i->IfRTTIzed())
					{
						// reinterpret_cast<const CNullClass*>(i->GetRaw(next))->GetRTTI()->GetName()
						Helper(reinterpret_cast<const CNullClass*>(i->GetRaw(next)));
					}
					//else if (i->kind == Compound)
					//{

					//}
					else
						;// i->GetName() i->GetStringValue(next) + "\",\n";
				}

			}


			
		};

		T::Helper(object);
		//SaveToFile(name);
		*/
	}

	CGameObject* DeserializeObjectFromJSON(const std::string &filename)
	{
		return NULL;
		/*
		CGameObject *result = NULL;
		rapidjson::Document document;
		char* buffer;
		FILE* fi = fopen(filename.c_str(), "rb");
		struct stat FileStat;
		stat(filename.c_str(), &FileStat);
		buffer = new char [FileStat.st_size + 1];
		buffer[FileStat.st_size] = 0;
		fread(buffer, 1, FileStat.st_size, fi);
		fclose(fi);
		document.Parse<0>(buffer);
		bool isObject;
		for (rapidjson::Document::MemberIterator i = document.MemberBegin(); i != document.MemberEnd(); ++i)
		{
			isObject = i->value.IsObject();
			std::string name = i->name.GetString();
			CRTTI *rtti = CRTTI::GetRTTIByName(name);
			if (rtti)
			{
				result = static_cast<CGameObject*>(rtti->MakeNew());
			}
		}
		return result;
		*/
	}

};	// namespace Deku2D