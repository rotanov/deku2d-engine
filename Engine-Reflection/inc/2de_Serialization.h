#ifndef _2DE_SERIALIZATION_H_
#define _2DE_SERIALIZATION_H_

namespace Deku2D
{
/*
	class ISerializer
	{
	public:
		template<typename TProperty>
		static void Serialize(ostream &out, const TProperty* property, const typename TProperty::TOwner* owner)
		{

		}
	};

	class CStreamSerializer
	{
	public:
		template<typename T>
		static void Serialize(ostream &out, const string& name, const T& value)
		{
			out << name << " = '" << value << "';";
		}

		template<typename T>
		static void Deserialize(istream &out, const string& name, T& value )
		{
			name;
			out >> value;
		}
*/

	class CNullClass;
	void SerializeObjectToXml(CNullClass* object, const std::string& name);

}; // namespace Deku2D

#endif //	_2DE_SERIALIZATION_H_