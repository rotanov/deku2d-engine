#include "2de_Serialization.h"
#include "2de_Xml.h"
#include "2de_RTTI.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

namespace Deku2D
{
	void SerializeObjectToJSON(CObject* o, const std::string& name)
	{
		class CStateInfo
		{
		public:
			int depth;
			string s;
			std::map<void*, int> addresTable;
			int ptrCount;

			CStateInfo() : depth(0), s(""), ptrCount(0) {}
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
			static string RepeatString(char* str, unsigned count)
			{
				return RepeatString(string(str), count);
			}


			static void Helper(CStateInfo& state, void* next, const string &nextName)
			{
				const static char* ident = " "; // "\t"
				state.s += RepeatString(ident, state.depth);
				state.s += "{\n";
				state.depth++;

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

							state.s += RepeatString(ident, state.depth) +
									   "\"" +
									   i->second->Name() +
									   "\" : \"" +
									   stringValue.c_str() +
									   "\"" +
									   RepeatString(",", typeInfo->BaseInfo() != 0 || nextIter != props.end()) +
										"\n";
						}
						else if (i->second->IsArray())
						{
							state.s += RepeatString(ident, state.depth) +
									   "\"" +
									   i->second->Name() +
									   "\" : \n" +
									   RepeatString(ident, state.depth) +
									   "[\n";
							state.depth++;
							TypeInfo* elementInfo = TypeInfo::GetTypeInfo(i->second->TypeName());
							if(elementInfo->IsIntegral())
								for (int j = 0; j < i->second->GetArraySize(next); j++)
								{
									state.s += RepeatString(ident, state.depth) +
											   "\"" +
											   elementInfo->GetString(i->second->GetValue(next, j)) +
											   "\"" +
											   RepeatString(",", j != i->second->GetArraySize(next) - 1 ) +
											   "\n";
								}
							else
								for (int j = 0; j < i->second->GetArraySize(next); j++)
								{
									Helper(state, i->second->GetValue(next, j), elementInfo->Name());
								}
							state.depth--;
							state.s += RepeatString(ident, state.depth) +
									   "]" +
									   RepeatString(",", nextIter != props.end() || typeInfo->BaseInfo()) +
									   "\n";
						}
						else
						{
							state.s += RepeatString(ident, state.depth) +
									   "\"" +
									   i->second->Name() +
									   "\" : \n";
							bool alreadySerialized = false;
							bool nullPtr = false;
							if ( i->second->IsPointer() )
							{
								if (i->second->GetValue(next) == 0)
									nullPtr = true;
								if (nullPtr)
								{
									state.s += "0";
								}
								else
								{
									alreadySerialized = state.addresTable.count( i->second->GetValue(next) ) == 1;
									if (alreadySerialized)
									{
										state.s += "\"@ptr" +
												   Convert<int>::ToString( state.addresTable[ i->second->GetValue(next) ] ) +
												   "\",\n";
									}
									else
									{
										state.ptrCount++;
										state.addresTable[ i->second->GetValue(next) ] = state.ptrCount;
										state.s += RepeatString(ident, state.depth) +
												   "{\n" +
												   RepeatString(ident, state.depth + 1) +
												   "\"@ptr\" : " +
												   Convert<int>::ToString(state.ptrCount) +
												   ",\n" +
												   RepeatString(ident, state.depth + 1) +
												   "\"@value\" :\n";
										state.depth++;
									}
								}
							}
							if (!alreadySerialized && !nullPtr)
							{
								void *value = i->second->GetValue(next);
								Helper(state, value, i->second->TypeName());
								if ( i->second->IsPointer() )
								{
									state.depth--;
									state.s += RepeatString(ident, state.depth) +
											   "}\n";
								}
								else
									delete value;
							}
						}
					}
					typeInfo = typeInfo->BaseInfo();
				}
				state.depth--;
				state.s += RepeatString( ident, state.depth );
				if (state.depth == 0)
					state.s += "}\n";
				else
					state.s += "},\n";
			}
		};

		CStateInfo state;
		//state.s += (string)"\"" + name + "\" : \n";
		T::Helper(state, o, "CObject");
		state.SaveToFile(name);
	}

};	// namespace Deku2D
