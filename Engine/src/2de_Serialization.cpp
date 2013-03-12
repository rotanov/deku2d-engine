#include "2de_Serialization.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/filestream.h>

#include "2de_Xml.h"
#include "2de_RTTI.h"
#include "2de_TypeInfo.h"

namespace Deku2D
{
	namespace Serialization
	{
		using namespace rapidjson;

		class CStateInfo
		{
		public:
			std::map<void*, int> addressTable;
			int ptrCount;
			unsigned depth;
			FileStream fileStream;
			PrettyWriter<FileStream> writer;

			CStateInfo(FILE *file)
				: ptrCount(0)
				, fileStream(file)
				, writer(fileStream)
				, depth(0)
			{
			}
		};

		static void Helper(CStateInfo& state, const void* next, const std::string &nextName)
		{
			state.depth++;
			state.writer.StartObject();

			TypeInfo *typeInfo = TypeInfo::GetTypeInfo(nextName);
			while (typeInfo->HasDerived())
			{
				if (typeInfo == typeInfo->GetRunTimeTypeInfo(next))
					break;	// TODO: костыль убрать.
				typeInfo = typeInfo->GetRunTimeTypeInfo(next);
			}

			if (state.depth == 1)
			{
				state.writer.String("@type");
				state.writer.String(typeInfo->Name());
			}

			while (typeInfo)
			{
				map<string, PropertyInfo*> &props = typeInfo->Properties();
				for (map<string, PropertyInfo*>::iterator i = props.begin(); i != props.end(); ++i)
				{
					map<string, PropertyInfo*>::iterator nextIter = ++i;
					i--;

					if (i->second->Integral())
					{
						if (i->second->IsPointer())
						{
							printf( "we are here ");
						}
						void *value = i->second->GetValue(next);
						std::string stringTypeName = i->second->TypeName();
						TypeInfo* _typeInfo = TypeInfo::GetTypeInfo(stringTypeName);
						std::string stringValue = _typeInfo->GetString(value);

						state.writer.String(i->second->Name());
						state.writer.String(stringValue.c_str());											
					}
					else if (i->second->IsArray())
					{
						state.writer.String(i->second->Name());
						state.writer.StartArray();

						TypeInfo* elementInfo = TypeInfo::GetTypeInfo(i->second->TypeName());
						if(elementInfo->IsIntegral())
							for (int j = 0; j < i->second->GetArraySize(next); j++)
							{
								state.writer.String(elementInfo->GetString(i->second->GetValue(next, j)).c_str());
							}
						else
							for (int j = 0; j < i->second->GetArraySize(next); j++)
							{
								Helper(state, i->second->GetValue(next, j), elementInfo->Name());
							}
						state.writer.EndArray();
					}
					else
					{
						state.writer.String(i->second->Name());
						bool alreadySerialized = false;
						bool nullPtr = false;
						if ( i->second->IsPointer() )
						{
							if (i->second->GetValue(next) == 0)
								nullPtr = true;
							if (nullPtr)
							{
								state.writer.String("0");
							}
							else
							{
								alreadySerialized = state.addressTable.count( i->second->GetValue(next) ) == 1;
								if (alreadySerialized)
								{
									state.writer.String(("@ptr" + Convert<int>::
										ToString(state.addressTable[i->second->GetValue(next)])).c_str());

								}
								else
								{
									state.ptrCount++;
									state.addressTable[ i->second->GetValue(next) ] = state.ptrCount;
									state.writer.StartObject();
									state.writer.String("@ptr");
									state.writer.String(Convert<int>::ToString(state.ptrCount).c_str());
									state.writer.String("@value");
								}
							}
						}
						if (!alreadySerialized && !nullPtr)
						{
							void *value = i->second->GetValue(next);
							Helper(state, value, i->second->TypeName());
							if ( i->second->IsPointer() )
							{
								state.writer.EndObject();
							}
							else
								delete value;
						}
					}
				}
				if (typeInfo == typeInfo->BaseInfo())
					break;
				typeInfo = typeInfo->BaseInfo();
			}
			state.writer.EndObject();
			state.depth--;
		}

		void ToJSON(const void* instance, const std::string& typeName, const std::string& filename)
		{
			FILE* file = fopen(filename.c_str(), "w");
			CStateInfo state(file);
			Helper(state, instance, typeName);
			fclose(file);
		}

		void FromJSON(void* instance, const std::string& typeName, const std::string& filename)
		{
			rapidjson::Document document;
			char* buffer;
			FILE* fi = fopen(filename.c_str(), "rb");
			struct stat fileStat;
			stat(filename.c_str(), &fileStat);
			buffer = new char [fileStat.st_size + 1];
			buffer[fileStat.st_size] = 0;
			fread(buffer, 1, fileStat.st_size, fi);
			fclose(fi);
			document.Parse<0>(buffer);

			class T
			{
			public:
				// Properly working breakpoints on windows with msvc10 and cdb inside static-function-inside-class-inside-function?
				// Qt-Creator: "No, not heard."
				static void* Helper(rapidjson::Document::ValueType* document, const std::string& nextName, void* next)
				{
					bool isObject = document->IsObject();
					TypeInfo *typeInfo = TypeInfo::GetTypeInfo(nextName);
					//void *next = typeInfo->New();
					for (rapidjson::Document::ValueType::MemberIterator i = document->MemberBegin(); i != document->MemberEnd(); ++i)
					{
						std::string propertyName = i->name.GetString();

						if (propertyName.length() > 0 && propertyName[0] == '@')
						{
							continue;
						}

						PropertyInfo *prop = typeInfo->FindProperty(propertyName);
						if (i->value.IsObject())
						{
							// won't work
							//prop->SetValue(next, Helper(&(i->value), prop->TypeName()));
						}
						else if( i->value.IsArray())
						{
							for (int j = 0; j < i->value.Size(); j++)
							{
								TypeInfo *tempTypeInfo = TypeInfo::GetTypeInfo(prop->TypeName());
								void *temp = tempTypeInfo->New();
								tempTypeInfo->SetString(temp, i->value[j].GetString());
								prop->PushValue(next, temp);
								delete temp;
							}
						}
						else
						{
							TypeInfo *tempTypeInfo = TypeInfo::GetTypeInfo(prop->TypeName());
							void *temp = tempTypeInfo->New();
							tempTypeInfo->SetString(temp, i->value.GetString());
							prop->SetValue(next, temp);
							delete temp;
						}
					}
					return next;
				}
			};

			T::Helper(&document, typeName, instance);
		}

	}	// namespace Serialization

}	// namespace Deku2D
