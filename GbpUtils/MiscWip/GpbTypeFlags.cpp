//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	MLB Google Protocol Buffers Support Library
//	////////////////////////////////////////////////////////////////////////////
/*
	File Name			:	GpbTypeFlags.cpp

	File Description	:	Implementation of the GpbTypeFlags class and related
								functions.

	Revision History	:	2016-07-05 --- Creation.
									Michael L. Brock

		Copyright Michael L. Brock 2016 - 2016.
		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	Include necessary header files...
//	////////////////////////////////////////////////////////////////////////////

#include <ProtoBuf/GpbTypeFlags.hpp>

//	////////////////////////////////////////////////////////////////////////////

namespace MLB {

namespace ProtoBuf {

//	////////////////////////////////////////////////////////////////////////////
bool GpbTypeFlags::IsValidDatum(GpbTypeFlags::Flag src_flags,
	bool throw_on_error)
{
	if (src_flags & ~GpbTypeFlags::Flag_MaskDatum) {
		if (throw_on_error)
			MLB::Utility::ThrowInvalidArgument("Unspecified bits in type mask.");
		return(false);
	}
	else if (MyPopCount(src_flags) != 1) {
		if (throw_on_error)
			MLB::Utility::ThrowInvalidArgument("Multiple datum type bits in use.");
		return(false);
	}

	return(true);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
bool GpbTypeFlags::IsValidRepeated(GpbTypeFlags::Flag src_flags,
	bool throw_on_error)
{
	if (src_flags & ~GpbTypeFlags::Flag_MaskRepeated) {
		if (throw_on_error)
			MLB::Utility::ThrowInvalidArgument("Unspecified bits in type mask.");
		return(false);
	}
	else if (MyPopCount(src_flags) > 1) {
		if (throw_on_error)
			MLB::Utility::ThrowInvalidArgument("Multiple repeated type bits in use.");
		return(false);
	}

	return(true);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
bool GpbTypeFlags::IsValid(GpbTypeFlags::Flag src_flags, bool throw_on_error)
{
	if (src_flags & ~GpbTypeFlags::Flag_Mask) {
		if (throw_on_error)
			MLB::Utility::ThrowInvalidArgument("Unspecified bits in type mask.");
		return(false);
	}

	return(IsValidDatum(src_flags) && IsValidRepeated(src_flags));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbTypeFlags::Flag GpbTypeFlags::GetFlags(
	::google::protobuf::FieldDescriptor::CppType cpp_type, bool is_repeating)
{
	GpbTypeFlags::Flag flags = GpbTypeFlags::Flag_None;

	switch (cpp_type) {
		case ::google::protobuf::FieldDescriptor::CPPTYPE_INT32		:
		case ::google::protobuf::FieldDescriptor::CPPTYPE_INT64		:
			flags |= GpbTypeFlags::Flag_IntS;
			flags |= GpbTypeFlags::Flag_Int;
			break;
		case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT32		:
		case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT64		:
			flags |= GpbTypeFlags::Flag_IntU;
			flags |= GpbTypeFlags::Flag_Int;
			break;
		case ::google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE		:
		case ::google::protobuf::FieldDescriptor::CPPTYPE_FLOAT		:
			flags |= GpbTypeFlags::Flag_Float;
			break;
		case ::google::protobuf::FieldDescriptor::CPPTYPE_BOOL		:
			flags |= GpbTypeFlags::Flag_Bool;
			break;
		case ::google::protobuf::FieldDescriptor::CPPTYPE_ENUM		:
			flags |= GpbTypeFlags::Flag_Enum;
			break;
		case ::google::protobuf::FieldDescriptor::CPPTYPE_STRING		:
			flags |= GpbTypeFlags::Flag_String;
			break;
		case ::google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE	:
			flags |= GpbTypeFlags::Flag_Message;
			break;
	}

	if (is_repeating)
		flags |=  (flags & GpbTypeFlags::Flag_Simple) ?
			GpbTypeFlags::Flag_RepeatedSimple : GpbTypeFlags::Flag_RepeatedPtr;

	return(flags);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbTypeFlags::Flag GpbTypeFlags::GetFlags(
	::google::protobuf::FieldDescriptor::CppType cpp_type,
	::google::protobuf::FieldDescriptor::Label label)
{
	return(GetFlags(cpp_type,
		(label == ::google::protobuf::FieldDescriptor::LABEL_REPEATED)));

}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbTypeFlags::Flag GpbTypeFlags::GetFlags(
	const ::google::protobuf::FieldDescriptor &field_descriptor)
{
	return(GetFlags(field_descriptor.label(),
		field_descriptor.cpp_type()));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbTypeFlags::Flag GpbTypeFlags::GetFlags(
	const ::google::protobuf::FieldDescriptor *field_descriptor)
{
	return((field_descriptor) ? GetFlags(*field_descriptor) :
		GpbTypeFlags::Flag_None);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbTypeFlags::Flag GpbTypeFlags::GetFlags(
	const ::google::protobuf::Descriptor &descriptor)
{
	return(GetFlags(::google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE,
		false));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbTypeFlags::Flag GpbTypeFlags::GetFlags(
	const ::google::protobuf::Descriptor *descriptor)
{
	return((descriptor) ? GetFlags(*descriptor) : GpbTypeFlags::Flag_None);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbTypeFlags::Flag GpbTypeFlags::GetFlags(const GpbElementInfo &msg_info)
{
	return(GetFlags(msg_info.GetFieldDescriptor()));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbTypeFlags::Flag GpbTypeFlags::GetFlags(const GpbElementInfo *msg_info_ptr)
{
	return(GetFlags((msg_info_ptr) ? GetFlags(*msg_info_ptr) :
		GpbTypeFlags::Flag_None));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbTypeFlags::Flag GpbTypeFlags::GetFlags(const GpbElementInfoSPtr &msg_info_ptr)
{
	return(GetFlags(msg_info_ptr.get()));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbTypeFlags::Flag GpbTypeFlags::GetFlags(const GpbElementInfoUPtr &msg_info_ptr)
{
	return(GetFlags(msg_info_ptr.get()));
}
//	////////////////////////////////////////////////////////////////////////////

namespace {

//	////////////////////////////////////////////////////////////////////////////
typedef std::pair<GpbTypeFlags::Flag, const char *> FlagNameItem;

const FlagNameItem FlagNameList[] = {
		FlagNameItem(GpbTypeFlags::Flag_IntS,           "IntS"),
		FlagNameItem(GpbTypeFlags::Flag_IntU,           "IntU"),
		FlagNameItem(GpbTypeFlags::Flag_Float,          "Float"),
		FlagNameItem(GpbTypeFlags::Flag_Bool,           "Bool"),
		FlagNameItem(GpbTypeFlags::Flag_Enum,           "Enum"),
		FlagNameItem(GpbTypeFlags::Flag_String,         "String"),
		FlagNameItem(GpbTypeFlags::Flag_Message,        "Message"),
		FlagNameItem(GpbTypeFlags::Flag_OneOf,          "OneOf"),
		FlagNameItem(GpbTypeFlags::Flag_RepeatedSimple, "RepeatedSimple"),
		FlagNameItem(GpbTypeFlags::Flag_RepeatedPtr,    "RepeatedPtr")
};

const std::size_t  FlagNameCount  =
	sizeof(FlagNameList) / sizeof(FlagNameList[0]);
//	////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

//	////////////////////////////////////////////////////////////////////////////
std::ostream & operator << (std::ostream &o_str,
	const GpbTypeFlags::Flag &datum)
{
	if (!datum)
		o_str << "None";
	else {
		GpbTypeFlags::Flag value = datum;
		for (std::size_t count_1 = 0; (count_1 < FlagNameCount) && value;
			++count_1) {
			if (value & FlagNameList[count_1].first) {
				o_str << ((value == datum) ? "" : "|") <<
					FlagNameList[count_1].second;
				value &= ~FlagNameList[count_1].first;
			}
		}
	}

	return(o_str);
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace ProtoBuf

} // namespace MLB

