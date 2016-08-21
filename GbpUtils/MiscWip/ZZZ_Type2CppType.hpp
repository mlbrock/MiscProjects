//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	MLB Google Protocol Buffers Support Library
//	////////////////////////////////////////////////////////////////////////////
/*
	File Name			:	VVV_Type2CppType.hpp

	File Description	:	Definition of the VVV_Type2CppType class template.

	Revision History	:	2016-07-26 --- Creation.
									Michael L. Brock

		Copyright Michael L. Brock 2016 - 2016.
		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
//	////////////////////////////////////////////////////////////////////////////

#ifndef HH___MLB__ProtoBuf__VVV_Type2CppType_hpp___HH

#define HH___MLB__ProtoBuf__VVV_Type2CppType_hpp___HH	1

//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	Include necessary header files...
//	////////////////////////////////////////////////////////////////////////////

#include <ProtoBuf/ProtoBuf.hpp>

//	////////////////////////////////////////////////////////////////////////////

namespace MLB {

namespace ProtoBuf {

/*
//	////////////////////////////////////////////////////////////////////////////
template <typename GpbType>
	struct VVV_Type2CppType;
//	----------------------------------------------------------------------------
template <> struct VVV_Type2CppType<Native_SInt32> {
	typedef Native_SInt32                                     type;
	typedef ::google::protobuf::RepeatedField<type>           repeated_type;
	static const ::google::protobuf::FieldDescriptor::CppType value =
		::google::protobuf::FieldDescriptor::CPPTYPE_INT32;
};
//	----------------------------------------------------------------------------
template <> struct VVV_Type2CppType<Native_SInt64> {
	typedef Native_SInt64                                     type;
	typedef ::google::protobuf::RepeatedField<type>           repeated_type;
	static const ::google::protobuf::FieldDescriptor::CppType value =
		::google::protobuf::FieldDescriptor::CPPTYPE_INT64;
};
//	----------------------------------------------------------------------------
template <> struct VVV_Type2CppType<Native_UInt32> {
	typedef Native_UInt32                                     type;
	typedef ::google::protobuf::RepeatedField<type>           repeated_type;
	static const ::google::protobuf::FieldDescriptor::CppType value =
		::google::protobuf::FieldDescriptor::CPPTYPE_UINT32;
};
//	----------------------------------------------------------------------------
template <> struct VVV_Type2CppType<Native_UInt64> {
	typedef Native_UInt64                                     type;
	typedef ::google::protobuf::RepeatedField<type>           repeated_type;
	static const ::google::protobuf::FieldDescriptor::CppType value =
		::google::protobuf::FieldDescriptor::CPPTYPE_UINT64;
};
//	----------------------------------------------------------------------------
template <> struct VVV_Type2CppType<Native_Float64> {
	typedef Native_Float64                                    type;
	typedef ::google::protobuf::RepeatedField<type>           repeated_type;
	static const ::google::protobuf::FieldDescriptor::CppType value =
		::google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE;
};
//	----------------------------------------------------------------------------
template <> struct VVV_Type2CppType<Native_Float32> {
	typedef Native_Float32                                    type;
	typedef ::google::protobuf::RepeatedField<type>           repeated_type;
	static const ::google::protobuf::FieldDescriptor::CppType value =
		::google::protobuf::FieldDescriptor::CPPTYPE_FLOAT;
};
//	----------------------------------------------------------------------------
template <> struct VVV_Type2CppType<bool> {
	typedef bool                                              type;
	typedef ::google::protobuf::RepeatedField<type>           repeated_type;
	static const ::google::protobuf::FieldDescriptor::CppType value =
		::google::protobuf::FieldDescriptor::CPPTYPE_BOOL;
};
//	----------------------------------------------------------------------------
template <> struct VVV_Type2CppType< ::google::protobuf::EnumValueDescriptor> {
	typedef ::google::protobuf::EnumValueDescriptor           type; 
	typedef ::google::protobuf::RepeatedField<type>           repeated_type; 
	static const ::google::protobuf::FieldDescriptor::CppType value =
		::google::protobuf::FieldDescriptor::CPPTYPE_ENUM;
};
//	----------------------------------------------------------------------------
template <> struct VVV_Type2CppType<std::string> {
	typedef std::string                                       type;
	typedef ::google::protobuf::RepeatedPtrField<type>        repeated_type;
	static const ::google::protobuf::FieldDescriptor::CppType value =
		::google::protobuf::FieldDescriptor::CPPTYPE_STRING;
};
//	----------------------------------------------------------------------------
template <> struct VVV_Type2CppType< ::google::protobuf::Message> {
	typedef ::google::protobuf::Message                       type;
	typedef ::google::protobuf::RepeatedPtrField<type>        repeated_type;
	static const ::google::protobuf::FieldDescriptor::CppType value =
		::google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE;
};
//	////////////////////////////////////////////////////////////////////////////
*/

} // namespace ProtoBuf

} // namespace MLB

//	////////////////////////////////////////////////////////////////////////////
//	****************************************************************************
//	****************************************************************************
//	****************************************************************************
//	////////////////////////////////////////////////////////////////////////////

namespace MLB {

namespace ProtoBuf {

//	////////////////////////////////////////////////////////////////////////////
template <typename GpbType>
	struct ZZZ_Type2CppType;
//	----------------------------------------------------------------------------
#define TYPE2CPPTYPE_MACRO(data_type, cpp_tag, method_tag, repeat_tag)			\
	template <> struct ZZZ_Type2CppType< data_type > {									\
	typedef data_type                                         type;				\
	typedef ::google::protobuf::Repeated##repeat_tag < type > repeated_type;	\
	static const ::google::protobuf::FieldDescriptor::CppType value =				\
		::google::protobuf::FieldDescriptor::CPPTYPE_##cpp_tag;						\
	typedef type (::google::protobuf::Reflection::*GetMethodType)(					\
		const ::google::protobuf::Message &msg,											\
		const ::google::protobuf::FieldDescriptor *field_descriptor) const;		\
	typedef void (::google::protobuf::Reflection::*SetMethodType)(					\
		::google::protobuf::Message *msg,													\
		const ::google::protobuf::FieldDescriptor *field_descriptor,				\
		const type value) const;																\
																										\
	static const GetMethodType GetMethod()													\
	{																									\
		return(&::google::protobuf::Reflection::Get##method_tag);					\
	}																									\
	static const SetMethodType SetMethod()													\
	{																									\
		return(&::google::protobuf::Reflection::Set##method_tag);					\
	}																									\
																										\
	static type Get(const ::google::protobuf::Reflection &reflection,				\
		const ::google::protobuf::Message &msg,											\
		const ::google::protobuf::FieldDescriptor &field_descriptor)				\
	{																									\
		return((reflection.*GetMethod())(msg, &field_descriptor));					\
	}																									\
	static type Get(const ::google::protobuf::Message &msg,							\
		const ::google::protobuf::FieldDescriptor &field_descriptor)				\
	{																									\
		return(Get(*msg.GetReflection(), msg, field_descriptor));					\
	}																									\
	static void Set(const ::google::protobuf::Reflection &reflection,				\
		::google::protobuf::Message &msg,													\
		const ::google::protobuf::FieldDescriptor &field_descriptor,				\
		const type &value)																		\
	{																									\
		(reflection.*SetMethod())(&msg, &field_descriptor, value);					\
	}																									\
	static void Set(::google::protobuf::Message &msg,									\
		const ::google::protobuf::FieldDescriptor &field_descriptor,				\
		const type &value)																		\
	{																									\
		Set(*msg.GetReflection(), msg, field_descriptor, value);						\
	}																									\
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
//TYPE2CPPTYPE_MACRO(Native_SInt32, INT32 , Int32 , Field)
TYPE2CPPTYPE_MACRO(Native_SInt64, INT64 , Int64 , Field)
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <> struct ZZZ_Type2CppType< Native_SInt32 > {
//	typedef data_type                                         type;
	typedef Native_SInt32                                         type;
//	typedef ::google::protobuf::Repeated##repeat_tag < type > repeated_type;
	typedef ::google::protobuf::RepeatedField < type > repeated_type;
/*
	static const ::google::protobuf::FieldDescriptor::CppType value =
		::google::protobuf::FieldDescriptor::CPPTYPE_##cpp_tag;
*/
	static const ::google::protobuf::FieldDescriptor::CppType value =
		::google::protobuf::FieldDescriptor::CPPTYPE_INT32;
	typedef type (::google::protobuf::Reflection::*GetMethodType)(
		const ::google::protobuf::Message &msg,
		const ::google::protobuf::FieldDescriptor *field_descriptor) const;
	typedef void (::google::protobuf::Reflection::*SetMethodType)(
		::google::protobuf::Message *msg,
		const ::google::protobuf::FieldDescriptor *field_descriptor,
		const type value) const;

	static const GetMethodType GetMethod()
	{
		return(&::google::protobuf::Reflection::GetInt32);
	}
	static const SetMethodType SetMethod()
	{
		return(&::google::protobuf::Reflection::SetInt32);
	}

	static type Get(const ::google::protobuf::Reflection &reflection,
		const ::google::protobuf::Message &msg,
		const ::google::protobuf::FieldDescriptor &field_descriptor)
	{
		return((reflection.*GetMethod())(msg, &field_descriptor));
	}
	static type Get(const ::google::protobuf::Message &msg,
		const ::google::protobuf::FieldDescriptor &field_descriptor)
	{
		return(Get(*msg.GetReflection(), msg, field_descriptor));
	}
	static void Set(const ::google::protobuf::Reflection &reflection,
		::google::protobuf::Message &msg,
		const ::google::protobuf::FieldDescriptor &field_descriptor,
		const type &value)
	{
		(reflection.*SetMethod())(&msg, &field_descriptor, value);
	}
	static void Set(::google::protobuf::Message &msg,
		const ::google::protobuf::FieldDescriptor &field_descriptor,
		const type &value)
	{
		Set(*msg.GetReflection(), msg, field_descriptor, value);
	}
};
//	////////////////////////////////////////////////////////////////////////////

/*
	::google::protobuf::Reflection methods.
// ////////////////////////////////////////////////////////////////////////////
int32                      GetInt32(const Message & message, const FieldDescriptor * field);
int64                      GetInt64(const Message & message, const FieldDescriptor * field);
uint32                     GetUInt32(const Message & message, const FieldDescriptor * field);
uint64                     GetUInt64(const Message & message, const FieldDescriptor * field);
float                      GetFloat(const Message & message, const FieldDescriptor * field);
double                     GetDouble(const Message & message, const FieldDescriptor * field);
bool                       GetBool(const Message & message, const FieldDescriptor * field);
string                     GetString(const Message & message, const FieldDescriptor * field);
const EnumValueDescriptor *GetEnum(const Message & message, const FieldDescriptor * field);
int                        GetEnumValue(const Message & message, const FieldDescriptor * field);
const Message             &GetMessage(const Message & message, const FieldDescriptor * field, MessageFactory * factory = NULL);
const string              &GetStringReference(const Message & message, const FieldDescriptor * field, string * scratch);
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
void     SetInt32(Message * message, const FieldDescriptor * field, int32 value);
void     SetInt64(Message * message, const FieldDescriptor * field, int64 value);
void     SetUInt32(Message * message, const FieldDescriptor * field, uint32 value);
void     SetUInt64(Message * message, const FieldDescriptor * field, uint64 value);
void     SetFloat(Message * message, const FieldDescriptor * field, float value);
void     SetDouble(Message * message, const FieldDescriptor * field, double value);
void     SetBool(Message * message, const FieldDescriptor * field, bool value);
void     SetString(Message * message, const FieldDescriptor * field, const string & value);
void     SetEnum(Message * message, const FieldDescriptor * field, const EnumValueDescriptor * value);
void     SetEnumValue(Message * message, const FieldDescriptor * field, int value);
Message *MutableMessage(Message * message, const FieldDescriptor * field, MessageFactory * factory = NULL);
void     SetAllocatedMessage(Message * message, Message * sub_message, const FieldDescriptor * field);
Message *ReleaseMessage(Message * message, const FieldDescriptor * field, MessageFactory * factory = NULL);
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
int32                      GetRepeatedInt32(const Message & message, const FieldDescriptor * field, int index);
int64                      GetRepeatedInt64(const Message & message, const FieldDescriptor * field, int index);
uint32                     GetRepeatedUInt32(const Message & message, const FieldDescriptor * field, int index);
uint64                     GetRepeatedUInt64(const Message & message, const FieldDescriptor * field, int index);
float                      GetRepeatedFloat(const Message & message, const FieldDescriptor * field, int index);
double                     GetRepeatedDouble(const Message & message, const FieldDescriptor * field, int index);
bool                       GetRepeatedBool(const Message & message, const FieldDescriptor * field, int index);
string                     GetRepeatedString(const Message & message, const FieldDescriptor * field, int index);
const EnumValueDescriptor *GetRepeatedEnum(const Message & message, const FieldDescriptor * field, int index);
int                        GetRepeatedEnumValue(const Message & message, const FieldDescriptor * field, int index);
const Message             &GetRepeatedMessage(const Message & message, const FieldDescriptor * field, int index);
const string              &GetRepeatedStringReference(const Message & message, const FieldDescriptor * field, int index, string * scratch);
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
void     SetRepeatedInt32(Message * message, const FieldDescriptor * field, int index, int32 value);
void     SetRepeatedInt64(Message * message, const FieldDescriptor * field, int index, int64 value);
void     SetRepeatedUInt32(Message * message, const FieldDescriptor * field, int index, uint32 value);
void     SetRepeatedUInt64(Message * message, const FieldDescriptor * field, int index, uint64 value);
void     SetRepeatedFloat(Message * message, const FieldDescriptor * field, int index, float value);
void     SetRepeatedDouble(Message * message, const FieldDescriptor * field, int index, double value);
void     SetRepeatedBool(Message * message, const FieldDescriptor * field, int index, bool value);
void     SetRepeatedString(Message * message, const FieldDescriptor * field, int index, const string & value);
void     SetRepeatedEnum(Message * message, const FieldDescriptor * field, int index, const EnumValueDescriptor * value);
void     SetRepeatedEnumValue(Message * message, const FieldDescriptor * field, int index, int value);
Message *MutableRepeatedMessage(Message * message, const FieldDescriptor * field, int index);
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
void                          AddInt32(Message * message, const FieldDescriptor * field, int32 value);
void                          AddInt64(Message * message, const FieldDescriptor * field, int64 value);
void                          AddUInt32(Message * message, const FieldDescriptor * field, uint32 value);
void                          AddUInt64(Message * message, const FieldDescriptor * field, uint64 value);
void                          AddFloat(Message * message, const FieldDescriptor * field, float value);
void                          AddDouble(Message * message, const FieldDescriptor * field, double value);
void                          AddBool(Message * message, const FieldDescriptor * field, bool value);
void                          AddString(Message * message, const FieldDescriptor * field, const string & value);
void                          AddEnum(Message * message, const FieldDescriptor * field, const EnumValueDescriptor * value);
void                          AddEnumValue(Message * message, const FieldDescriptor * field, int value);
Message                      *AddMessage(Message * message, const FieldDescriptor * field, MessageFactory * factory = NULL);
RepeatedFieldRef< T >         GetRepeatedFieldRef(const Message & message, const FieldDescriptor * field);
MutableRepeatedFieldRef< T >  GetMutableRepeatedFieldRef(Message * message, const FieldDescriptor * field);
const RepeatedField< T >     &GetRepeatedField(const Message & , const FieldDescriptor * );
RepeatedField< T >           *MutableRepeatedField(Message * , const FieldDescriptor * );
const RepeatedPtrField< T >  &GetRepeatedPtrField(const Message & , const FieldDescriptor * );
RepeatedPtrField< T >        *MutableRepeatedPtrField(Message * , const FieldDescriptor * );
// ////////////////////////////////////////////////////////////////////////////
*/

} // namespace ProtoBuf

} // namespace MLB

#endif // #ifndef HH___MLB__ProtoBuf__VVV_Type2CppType_hpp___HH

