//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	MLB Google Protocol Buffers Support Library
//	////////////////////////////////////////////////////////////////////////////
/*
	File Name			:	GpbElementInfo.cpp

	File Description	:	Implementation of the GpbElementInfo class.

	Revision History	:	2015-06-14 --- Creation.
									Michael L. Brock

		Copyright Michael L. Brock 2015 - 2015.
		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	Include necessary header files...
//	////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:4100 4512)
#endif // #ifdef _MSC_VER

#include <google/protobuf/Descriptor.pb.h>

#ifdef _MSC_VER
# pragma warning(pop)
#endif // #ifdef _MSC_VER

//	Needed for the .cpp only.
#include <iomanip>
#include <sstream>

//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename MessageType>
	const MessageType &GpbGetMessageDescriptorPtr(const MessageType *message_ptr)
{
	return(GpbGetMessageDescriptorPtrHelper<MessageType>::
		GetDescriptorPtr(message_ptr));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
struct GpbElementInfoDescriptors {
	const ::google::protobuf::Descriptor      *descriptor_;
	const ::google::protobuf::FieldDescriptor *field_descriptor_;
	const ::google::protobuf::FileDescriptor  *file_descriptor_;
	const ::google::protobuf::EnumDescriptor  *enum_descriptor_;
	const ::google::protobuf::OneofDescriptor *oneof_descriptor_;
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
class GpbElementInfo {
	typedef ::google::protobuf::Descriptor      GPB_Descriptor;
	typedef ::google::protobuf::FieldDescriptor GPB_FieldDescriptor;
	typedef ::google::protobuf::FileDescriptor  GPB_FileDescriptor;
	typedef ::google::protobuf::EnumDescriptor  GPB_EnumDescriptor;
	typedef ::google::protobuf::OneofDescriptor GPB_OneofDescriptor;
public:
	typedef std::vector<GpbElementInfo>            GpbElementInfoVector_I;
	typedef GpbElementInfoVector_I::iterator       GpbElementInfoVectorIter_I;
	typedef GpbElementInfoVector_I::const_iterator GpbElementInfoVectorIterC_I;

	explicit GpbElementInfo();

	explicit GpbElementInfo(const GPB_Descriptor &descriptor);

	const char *GetTypeNameFull() const
	{
		return((enum_descriptor_) ? enum_descriptor_->full_name().c_str() :
			((descriptor_) ? descriptor_->full_name().c_str() :
			field_descriptor_->cpp_type_name()));
	}

	const char *GetTypeName() const
	{
		return((enum_descriptor_) ? enum_descriptor_->name().c_str() :
			((descriptor_) ? descriptor_->name().c_str() :
			field_descriptor_->cpp_type_name()));
	}

	const char *GetName() const
	{
		return((field_descriptor_) ? field_descriptor_->name().c_str() :
			descriptor_->name().c_str());
	}

	const char *GetFileName() const
	{
		return((field_descriptor_) ? field_descriptor_->file()->name().c_str() :
			descriptor_->file()->name().c_str());
	}

	const char *GetLabelName() const
	{
		return(GetLabelName(GetLabel()));
	}

	::google::protobuf::FieldDescriptor::Label GetLabel() const
	{
		return((field_descriptor_) ? field_descriptor_->label() :
			::google::protobuf::FieldDescriptor::LABEL_OPTIONAL);
	}

	bool GetTypeSourceLocation(
		::google::protobuf::SourceLocation &dst) const
	{
		bool return_code;

		if (enum_descriptor_)
			return_code = GetSourceLocationHelper(enum_descriptor_, dst);
		else if (field_descriptor_)
			return_code = GetSourceLocationHelper(field_descriptor_, dst);
		else if (descriptor_)			
			return_code = GetSourceLocationHelper(descriptor_, dst);
		else
			return_code = false;

		return(return_code);
	}

	::google::protobuf::SourceLocation GetTypeSourceLocation() const
	{
		::google::protobuf::SourceLocation tmp_dst;

		GetTypeSourceLocation(tmp_dst);

		return(tmp_dst);
	}

	bool GetMemberSourceLocation(
		::google::protobuf::SourceLocation &dst) const
	{
		bool return_code;

		if (enum_descriptor_)
			return_code = GetSourceLocationHelper(enum_descriptor_, dst);
		else if (field_descriptor_)
			return_code = GetSourceLocationHelper(field_descriptor_, dst);
		else if (descriptor_)			
			return_code = GetSourceLocationHelper(descriptor_, dst);
		else
			return_code = false;

		return(return_code);
	}

	::google::protobuf::SourceLocation GetMemberSourceLocation() const
	{
		::google::protobuf::SourceLocation tmp_dst;

		GetTypeSourceLocation(tmp_dst);

		return(tmp_dst);
	}

	GpbElementInfoDescriptors GetDescriptors() const
	{
		GpbElementInfoDescriptors out_descriptors;

		out_descriptors.descriptor_       = descriptor_;
		out_descriptors.field_descriptor_ = field_descriptor_;
		out_descriptors.file_descriptor_  = file_descriptor_;
		out_descriptors.enum_descriptor_  = enum_descriptor_;
		out_descriptors.oneof_descriptor_ = oneof_descriptor_;

		return(out_descriptors);
	}

	static std::string SourceLocationToString(
		const ::google::protobuf::SourceLocation &datum);

	template <typename DerivedMessageType>
		static GpbElementInfo GetInstance()
	{
		return(GpbElementInfo(DerivedMessageType::descriptor(), 0));
	}


private:
	const GPB_Descriptor      *descriptor_;
	const GPB_FieldDescriptor *field_descriptor_;
	const GPB_FileDescriptor  *file_descriptor_;
	const GPB_EnumDescriptor  *enum_descriptor_;
	const GPB_OneofDescriptor *oneof_descriptor_;
	std::size_t                depth_;
	int                        member_index_;
	std::size_t                max_depth_;
	GpbElementInfoVector_I     member_list_;

	explicit GpbElementInfo(const GPB_Descriptor *descriptor,
		std::size_t depth);
	explicit GpbElementInfo(const GPB_Descriptor *descriptor,
		const GPB_FieldDescriptor *field_descriptor,
		std::size_t depth, int member_index);

	static const GPB_FileDescriptor *DetermineFileDescriptorPtr(
		const GPB_Descriptor *descriptor,
		const GPB_FieldDescriptor *field_descriptor);

	template <typename DescriptorType>
		static bool GetSourceLocationHelper(const DescriptorType &descriptor,
			::google::protobuf::SourceLocation &dst)
	{
		return((descriptor) ? descriptor->GetSourceLocation(&dst) : false);
	}

	static const char *GetLabelName(
		::google::protobuf::FieldDescriptor::Label label)
	{
		if (label == ::google::protobuf::FieldDescriptor::LABEL_OPTIONAL)
			return("optional");
		else if (label == ::google::protobuf::FieldDescriptor::LABEL_REQUIRED)
			return("required");
		else if (label == ::google::protobuf::FieldDescriptor::LABEL_REPEATED)
			return("repeated");

		return("*INVALID*");
	}

	//	Really just to support debugging...
	static void ClearSourceLocation(::google::protobuf::SourceLocation &datum)
	{
		datum.start_line   = 0;
		datum.end_line     = 0;
		datum.start_column = 0;
		datum.end_column   = 0;
	}

	friend std::ostream & operator << (std::ostream &o_str,
		const GpbElementInfo &data);

};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::ostream & operator << (std::ostream &o_str, const GpbElementInfo &data);
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbElementInfo::GpbElementInfo()
	//	CODE NOTE: Fix me!!!
{
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbElementInfo::GpbElementInfo(const GPB_Descriptor &descriptor)
	//	CODE NOTE: Fix me!!!
{
	GpbElementInfo tmp(&descriptor, NULL, 0, -1);

	*this = tmp;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbElementInfo::GpbElementInfo(const GPB_Descriptor *descriptor,
	std::size_t depth)
	:descriptor_(descriptor)
	,field_descriptor_(NULL)
	,file_descriptor_(descriptor_->file())
	,enum_descriptor_(NULL)
	,oneof_descriptor_(NULL)
	,depth_(depth)
	,member_index_(-1)
	,max_depth_(depth_)
	,member_list_()
{
	GpbElementInfo tmp(descriptor, NULL, depth, -1);

	*this = tmp;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbElementInfo::GpbElementInfo(const GPB_Descriptor *descriptor,
	const GPB_FieldDescriptor *field_descriptor,
	std::size_t depth, int member_index)
	:descriptor_(descriptor)
	,field_descriptor_(field_descriptor)
	,file_descriptor_(DetermineFileDescriptorPtr(descriptor_, field_descriptor))
	,enum_descriptor_((field_descriptor_ && (field_descriptor_->cpp_type() ==
		::google::protobuf::FieldDescriptor::CPPTYPE_ENUM)) ?
		field_descriptor->enum_type() : NULL)
	,oneof_descriptor_((field_descriptor_) ? field_descriptor_->containing_oneof() : NULL)
	,depth_(depth)
	,member_index_(member_index)
	,max_depth_(depth_)
	,member_list_()
{
	if ((!descriptor) || (!descriptor->field_count()))
		return;

	member_list_.reserve(descriptor->field_count());

	for (int count_1 = 0; count_1 < descriptor_->field_count(); ++count_1) {
		const GPB_FieldDescriptor *this_field_descriptor =
			descriptor->field(count_1);
		const GPB_Descriptor      *this_descriptor       =
			this_field_descriptor->message_type();
		GpbElementInfo new_field(this_descriptor, this_field_descriptor,
			depth + 1, count_1);
		max_depth_ = std::max(max_depth_, new_field.max_depth_);
		member_list_.push_back(new_field);
	}
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const GpbElementInfo::GPB_FileDescriptor
	*GpbElementInfo::DetermineFileDescriptorPtr(
	const GPB_Descriptor *descriptor,
	const GPB_FieldDescriptor *field_descriptor)
{
	if (descriptor)
		return(descriptor->file());
	else if (!field_descriptor) {
		throw std::invalid_argument("Unable to determine the file descriptor "
			"value.");
	}

	return(field_descriptor->file());
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::string GpbElementInfo::SourceLocationToString(
	const ::google::protobuf::SourceLocation &datum)
{
	std::ostringstream o_str;

	o_str << "{"
		"StartLine="        << datum.start_line        << ", "
		"EndLine="          << datum.end_line          << ", "
		"StartColumn="      << datum.start_column      << ", "
		"EndColumn="        << datum.end_column        << ", "
		"LeadingComments="  << datum.leading_comments  << ", "
		"TrailingComments=" << datum.trailing_comments <<
		"}";

	return(o_str.str());
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::ostream & operator << (std::ostream &o_str, const GpbElementInfo &datum)
{
	::google::protobuf::SourceLocation s_l_1;
	::google::protobuf::SourceLocation s_l_2;

	GpbElementInfo::ClearSourceLocation(s_l_1);
	GpbElementInfo::ClearSourceLocation(s_l_2);
	datum.GetTypeSourceLocation(s_l_1);
	datum.GetMemberSourceLocation(s_l_2);

	o_str << std::setw(datum.depth_ * 3) << "" << "{"
		"Depth="                << datum.depth_            << ", "
		"TypeNameFull="         << datum.GetTypeNameFull() << ", "
		"TypeName="             << datum.GetTypeName()     << ", "
		"Name="                 << datum.GetName()         << ", "
		"Label="                << datum.GetLabel()        << ", "
		"LabelName="            << datum.GetLabelName()    << ", "
		"FileName="             << datum.GetFileName()     << ", "
		"TypeSourceLocation="   << datum.SourceLocationToString(s_l_1) << ", "
		"MemberSourceLocation=" << datum.SourceLocationToString(s_l_2) <<
		"}";

	for (std::size_t count_1 = 0; count_1 < datum.member_list_.size();
		++count_1)
		o_str << std::endl << datum.member_list_[count_1];

	return(o_str);
}
//	////////////////////////////////////////////////////////////////////////////

#ifdef TEST_MAIN

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:4100 4512)
#endif // #ifdef _MSC_VER

#include "./Tests/CppOut/GpbElementInfoTest.pb.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif // #ifdef _MSC_VER

#include <cstdlib>

#include <iostream>

//	////////////////////////////////////////////////////////////////////////////
template <typename MessageType>
	bool TEST_RunTest(int &return_code)
{
	try {
		MessageType    test_msg;
		GpbElementInfo element_1(*test_msg.descriptor());
		std::cout << element_1 << std::endl << std::endl;
		GpbElementInfo element_2(GpbElementInfo::GetInstance<MessageType>());
		{
			std::ostringstream o_str_1;
			std::ostringstream o_str_2;
			o_str_1 << element_1;
			o_str_2 << element_2;
			if (o_str_1.str() != o_str_2.str())
				throw std::logic_error("The string representations of "
					"GpbElementInfo instances are not identical for a one "
					"constructed from a message instance descriptor and one "
					"constructed from the class descriptor.");
		}
		return(true);
	}
	catch (const std::exception &except) {
		std::cerr << "TEST FAILED: " << except.what() << std::endl;
	}

	return_code = EXIT_FAILURE;

	return(false);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
int main()
{
	int return_code = EXIT_SUCCESS;

	try {
		TEST_RunTest<GpbElementInfoTest::AddressBook>(return_code);
	}
	catch (const std::exception &except) {
		std::cerr << "ERROR: " << except.what() << std::endl;
		return_code = EXIT_FAILURE;
	}

	return(return_code);
}
//	////////////////////////////////////////////////////////////////////////////

#endif // #ifdef TEST_MAIN



/*
	CODE NOTE: My GPB internals study. To be removed.
*/
#if defined(THIS_IS_NOT_DEFINED)

//	////////////////////////////////////////////////////////////////////////////
E:\DevEnv\GitRepos\MiscProjects\GbpUtils\Tests\CppOut\GpbElementInfoTest.pb.cc
Line 37
//	----------------------------------------------------------------------------
void protobuf_AssignDesc_GpbElementInfoTest_2eproto() {
  protobuf_AddDesc_GpbElementInfoTest_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "GpbElementInfoTest.proto");
  GOOGLE_CHECK(file != NULL);
  Person_descriptor_ = file->message_type(0);
  static const int Person_offsets_[4] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Person, name_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Person, id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Person, email_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Person, phone_),
  };
  Person_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Person_descriptor_,
      Person::default_instance_,
      Person_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Person, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Person, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Person));
  Person_PhoneNumber_descriptor_ = Person_descriptor_->nested_type(0);
  static const int Person_PhoneNumber_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Person_PhoneNumber, number_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Person_PhoneNumber, type_),
  };
  Person_PhoneNumber_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Person_PhoneNumber_descriptor_,
      Person_PhoneNumber::default_instance_,
      Person_PhoneNumber_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Person_PhoneNumber, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Person_PhoneNumber, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Person_PhoneNumber));
  Person_PhoneType_descriptor_ = Person_descriptor_->enum_type(0);
  AddressBook_descriptor_ = file->message_type(1);
  static const int AddressBook_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AddressBook, person_),
  };
  AddressBook_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      AddressBook_descriptor_,
      AddressBook::default_instance_,
      AddressBook_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AddressBook, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AddressBook, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(AddressBook));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
E:\DevEnv\GitRepos\MiscProjects\GbpUtils\Tests\CppOut\GpbElementInfoTest.pb.cc
Line 124
//	----------------------------------------------------------------------------
void protobuf_AddDesc_GpbElementInfoTest_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\030GpbElementInfoTest.proto\022\022GpbElementIn"
    "foTest\"\356\001\n\006Person\022\014\n\004name\030\001 \002(\t\022\n\n\002id\030\002 "
    "\002(\005\022\r\n\005email\030\003 \001(\t\0225\n\005phone\030\004 \003(\0132&.GpbE"
    "lementInfoTest.Person.PhoneNumber\032W\n\013Pho"
    "neNumber\022\016\n\006number\030\001 \002(\t\0228\n\004type\030\002 \001(\0162$"
    ".GpbElementInfoTest.Person.PhoneType:\004HO"
    "ME\"+\n\tPhoneType\022\n\n\006MOBILE\020\000\022\010\n\004HOME\020\001\022\010\n"
    "\004WORK\020\002\"9\n\013AddressBook\022*\n\006person\030\001 \003(\0132\032"
    ".GpbElementInfoTest.Person", 346);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "GpbElementInfoTest.proto", &protobuf_RegisterTypes);
  Person::default_instance_ = new Person();
  Person_PhoneNumber::default_instance_ = new Person_PhoneNumber();
  AddressBook::default_instance_ = new AddressBook();
  Person::default_instance_->InitAsDefaultInstance();
  Person_PhoneNumber::default_instance_->InitAsDefaultInstance();
  AddressBook::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_GpbElementInfoTest_2eproto);
}
//	////////////////////////////////////////////////////////////////////////////

#endif // #if defined(THIS_IS_NOT_DEFINED)

