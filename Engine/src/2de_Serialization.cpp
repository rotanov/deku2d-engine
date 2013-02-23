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
			FileStream fileStream;
			PrettyWriter<FileStream> writer;

			CStateInfo(FILE *file)
				: ptrCount(0)
				, fileStream(file)
				, writer(fileStream)
			{
			}
		};

		static void Helper(CStateInfo& state, void* next, const string &nextName)
		{
			state.writer.StartObject();

			TypeInfo *typeInfo = TypeInfo::GetTypeInfo(nextName);
			while (typeInfo->HasDerived())
			{
				if( typeInfo == typeInfo->GetRunTimeTypeInfo( next ) )
					break;	// TODO: костыль убрать.
				typeInfo = typeInfo->GetRunTimeTypeInfo( next );
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
						if ( i->second->IsPointer() )
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
				typeInfo = typeInfo->BaseInfo();
			}
			state.writer.EndObject();
		}

		void ToJSON(void *o, const string &className, const std::string& filename)
		{
			FILE* file = fopen(filename.c_str(), "w");
			CStateInfo state(file);
			Helper(state, o, className);
			fclose(file);
		}

	};	// namespace Serialization

};	// namespace Deku2D
