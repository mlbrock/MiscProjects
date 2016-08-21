//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	MLB Google Protocol Buffers Support Library
//	////////////////////////////////////////////////////////////////////////////
/*
	File Name			:	GpbTypeFlags.hpp

	File Description	:	Definition of the GpbTypeFlags class.

	Revision History	:	2016-07-05 --- Creation.
									Michael L. Brock

		Copyright Michael L. Brock 2016 - 2016.
		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
//	////////////////////////////////////////////////////////////////////////////

#ifndef HH___MLB__ProtoBuf__GpbTypeFlags_hpp___HH

#define HH___MLB__ProtoBuf__GpbTypeFlags_hpp___HH	1

//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	Include necessary header files...
//	////////////////////////////////////////////////////////////////////////////

#include <ProtoBuf/GpbElementInfo.hpp>

#include <Utility/Utility_Exception.hpp>

//	////////////////////////////////////////////////////////////////////////////

namespace MLB {

namespace ProtoBuf {

//	////////////////////////////////////////////////////////////////////////////
class GpbTypeFlags {
public:
	enum Flag {
		Flag_None           = 0x0000,
		Flag_IntS           = 0x0001,
		Flag_IntU           = 0x0002,
		Flag_Int            = Flag_IntS | Flag_IntU,
		Flag_Float          = 0x0004,
		Flag_Numeric        = Flag_Int | Flag_Float,
		Flag_Bool           = 0x0008,
		Flag_Enum           = 0x0010,
		Flag_Simple         = Flag_Numeric | Flag_Bool | Flag_Enum,
		Flag_String         = 0x0020,
		Flag_Scalar         = Flag_Simple | Flag_String,
		Flag_Message        = 0x0040,
		Flag_OneOf          = 0x0080,
		Flag_Datum          = Flag_Scalar | Flag_Message | Flag_OneOf,
		Flag_RepeatedSimple = 0x4000,
		Flag_RepeatedPtr    = 0x8000,
		Flag_RepeatedAny    = Flag_RepeatedSimple | Flag_RepeatedPtr,
		Flag_Repeated       = Flag_RepeatedAny,	//	Synonym
		Flag_MaskDatum      = Flag_Datum,
		Flag_MaskRepeated   = Flag_Repeated,
		Flag_Mask           = Flag_MaskDatum | Flag_MaskRepeated
	};

	static bool IsValidDatum(Flag src_flags, bool throw_on_error = false);
	static bool IsValidRepeated(Flag src_flags, bool throw_on_error = false);
	static bool IsValid(Flag src_flags, bool throw_on_error = false);
	static Flag GetFlags(::google::protobuf::FieldDescriptor::CppType cpp_type,
		bool is_repeating);
	static Flag GetFlags(::google::protobuf::FieldDescriptor::CppType cpp_type,
		::google::protobuf::FieldDescriptor::Label label);
	static Flag GetFlags(
		const ::google::protobuf::FieldDescriptor &field_descriptor);
	static Flag GetFlags(
		const ::google::protobuf::FieldDescriptor *field_descriptor);
	static Flag GetFlags(const ::google::protobuf::Descriptor &descriptor);
	static Flag GetFlags(const ::google::protobuf::Descriptor *descriptor);
	static Flag GetFlags(const GpbElementInfo &msg_info);
	static Flag GetFlags(const GpbElementInfo *msg_info_ptr);
	static Flag GetFlags(const GpbElementInfoSPtr &msg_info_ptr);
	static Flag GetFlags(const GpbElementInfoUPtr &msg_info_ptr);

private:
	GpbTypeFlags();
	GpbTypeFlags(const GpbTypeFlags &);
	GpbTypeFlags & operator = (const GpbTypeFlags &);
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::ostream & operator << (std::ostream &o_str,
	const GpbTypeFlags::Flag &datum);
//	////////////////////////////////////////////////////////////////////////////

} // namespace ProtoBuf

} // namespace MLB

#endif // #ifndef HH___MLB__ProtoBuf__GpbTypeFlags_hpp___HH

