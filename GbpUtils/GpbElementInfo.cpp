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
#include <boost/algorithm/string.hpp>

//	////////////////////////////////////////////////////////////////////////////

namespace MLB {

namespace ProtoBufSupport {

//	////////////////////////////////////////////////////////////////////////////
struct GpbElementInfoDescriptors {
	const ::google::protobuf::Descriptor      *descriptor_;
	const ::google::protobuf::FieldDescriptor *field_descriptor_;
	const ::google::protobuf::FileDescriptor  *file_descriptor_;
	const ::google::protobuf::EnumDescriptor  *enum_descriptor_;
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
class GpbElementInfo {
	typedef ::google::protobuf::Descriptor      GPB_Descriptor;
	typedef ::google::protobuf::FieldDescriptor GPB_FieldDescriptor;
	typedef ::google::protobuf::FileDescriptor  GPB_FileDescriptor;
	typedef ::google::protobuf::EnumDescriptor  GPB_EnumDescriptor;
public:
	typedef std::vector<GpbElementInfo>            GpbElementInfoVector_I;
	typedef GpbElementInfoVector_I::iterator       GpbElementInfoVectorIter_I;
	typedef GpbElementInfoVector_I::const_iterator GpbElementInfoVectorIterC_I;

	explicit GpbElementInfo();
	explicit GpbElementInfo(const GPB_Descriptor &descriptor);
	explicit GpbElementInfo(const GPB_Descriptor *descriptor);
	explicit GpbElementInfo(const std::string &message_name);

	const char *GetTypeNameFull() const;

	const char *GetTypeName() const;

	const char *GetName() const;

	const char *GetFileName() const;

	const char *GetLabelName() const;

	::google::protobuf::FieldDescriptor::Label GetLabel() const;

	bool GetSourceLocationType(
		::google::protobuf::SourceLocation &dst) const;

	::google::protobuf::SourceLocation GetSourceLocationType() const;

	bool GetSourceLocationMember(
		::google::protobuf::SourceLocation &dst) const;

	::google::protobuf::SourceLocation GetSourceLocationMember() const;

	GpbElementInfoDescriptors GetDescriptors() const;

	static std::string SourceLocationToString(
		const ::google::protobuf::SourceLocation &datum);

	template <typename DerivedMessageType>
		static GpbElementInfo GetInstance()
	{
		return(GpbElementInfo(DerivedMessageType::descriptor(), 0));
	}

	void TestFileName() const;

private:
	const GPB_Descriptor      *descriptor_;
	const GPB_FieldDescriptor *field_descriptor_;
	const GPB_FileDescriptor  *file_descriptor_;
	const GPB_EnumDescriptor  *enum_descriptor_;
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
		::google::protobuf::FieldDescriptor::Label label);

	//	Really just to support debugging...
	static void ClearSourceLocation(::google::protobuf::SourceLocation &datum);

	friend std::ostream & operator << (std::ostream &o_str,
		const GpbElementInfo &data);
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::ostream & operator << (std::ostream &o_str, const GpbElementInfo &data);
//	////////////////////////////////////////////////////////////////////////////

} // namespace ProtoBufSupport

} // namespace MLB

//	////////////////////////////////////////////////////////////////////////////
//	****************************************************************************
//	****************************************************************************
//	****************************************************************************
//	////////////////////////////////////////////////////////////////////////////

namespace MLB {

namespace ProtoBufSupport {

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
GpbElementInfo::GpbElementInfo(const GPB_Descriptor *descriptor)
	//	CODE NOTE: Fix me!!!
{
	GpbElementInfo tmp(descriptor, NULL, 0, -1);

	*this = tmp;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
/*
	CODE NOTE: Perhaps should enhance to support non-message types by
	searching for first for messages, then for enum types
	(with FindEnumTypeByName()).

	Perhaps finally could search for any name where it's possible to do
	fit the result into a GpbElementInfo: FindFieldByName(),
	FindExtensionByName(), FindOneofByName(), FindEnumValueByName() .
*/
GpbElementInfo::GpbElementInfo(const std::string &message_name)
	:descriptor_(NULL)
	,field_descriptor_(NULL)
	,file_descriptor_(NULL)
	,enum_descriptor_(NULL)
	,depth_(0)
	,member_index_(-1)
	,max_depth_(0)
	,member_list_()
{
  const ::google::protobuf::Descriptor *descriptor =
    ::google::protobuf::DescriptorPool::generated_pool()->
		FindMessageTypeByName(message_name);

	if (!descriptor) {
		std::string::size_type position;
		if ((position = message_name.find_first_of(':')) !=
			std::string::npos) {
			std::string tmp_name(message_name);
			boost::replace_all(tmp_name, "::", ".");
			descriptor = ::google::protobuf::DescriptorPool::generated_pool()->
				FindMessageTypeByName(tmp_name);
		}
	}

	if (!descriptor) {
		std::ostringstream o_str;
		o_str << "Unable to locate message name '" << message_name <<
			"' within the pool of generated descriptors.";
		throw std::invalid_argument(o_str.str());
	}

	GpbElementInfo tmp(descriptor, NULL, 0, -1);

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
const char *GpbElementInfo::GetTypeNameFull() const
{
	return((enum_descriptor_) ? enum_descriptor_->full_name().c_str() :
		((descriptor_) ? descriptor_->full_name().c_str() :
		field_descriptor_->cpp_type_name()));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *GpbElementInfo::GetTypeName() const
{
	return((enum_descriptor_) ? enum_descriptor_->name().c_str() :
		((descriptor_) ? descriptor_->name().c_str() :
		field_descriptor_->cpp_type_name()));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *GpbElementInfo::GetName() const
{
	return((field_descriptor_) ? field_descriptor_->name().c_str() :
		descriptor_->name().c_str());
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *GpbElementInfo::GetFileName() const
{
	return((field_descriptor_) ? field_descriptor_->file()->name().c_str() :
		descriptor_->file()->name().c_str());
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *GpbElementInfo::GetLabelName() const
{
	return(GetLabelName(GetLabel()));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
::google::protobuf::FieldDescriptor::Label GpbElementInfo::GetLabel() const
{
	return((field_descriptor_) ? field_descriptor_->label() :
		::google::protobuf::FieldDescriptor::LABEL_OPTIONAL);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
bool GpbElementInfo::GetSourceLocationType(
	::google::protobuf::SourceLocation &dst) const
{
	return(GetSourceLocationHelper(enum_descriptor_, dst) ||
		GetSourceLocationHelper(descriptor_, dst));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
::google::protobuf::SourceLocation GpbElementInfo::GetSourceLocationType()
	const
{
	::google::protobuf::SourceLocation tmp_dst;

	GetSourceLocationType(tmp_dst);

	return(tmp_dst);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
bool GpbElementInfo::GetSourceLocationMember(
	::google::protobuf::SourceLocation &dst) const
{
	return(GetSourceLocationHelper(field_descriptor_, dst));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
::google::protobuf::SourceLocation GpbElementInfo::GetSourceLocationMember()
	const
{
	::google::protobuf::SourceLocation tmp_dst;

	GetSourceLocationMember(tmp_dst);

	return(tmp_dst);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbElementInfoDescriptors GpbElementInfo::GetDescriptors() const
{
	GpbElementInfoDescriptors out_descriptors;

	out_descriptors.descriptor_       = descriptor_;
	out_descriptors.field_descriptor_ = field_descriptor_;
	out_descriptors.file_descriptor_  = file_descriptor_;
	out_descriptors.enum_descriptor_  = enum_descriptor_;

	return(out_descriptors);
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
void GpbElementInfo::TestFileName() const
{
	std::string pad;

	{
		std::ostringstream o_str;
		o_str << std::setw(depth_ * 3) << "";
		pad = o_str.str();
	}

	std::cout << pad << GetTypeName() << " " << GetName() << ": " <<
		std::endl <<
		pad << "FILE : " << ((file_descriptor_)  ?
			file_descriptor_->name()          : "") << std::endl <<
		pad << "ENUM : " << ((enum_descriptor_)  ?
			enum_descriptor_->file()->name()  : "") << std::endl <<
		pad << "FIELD: " << ((field_descriptor_) ?
			field_descriptor_->file()->name() : "") << std::endl <<
		pad << "TYPE : " << ((descriptor_)       ?
			descriptor_->file()->name()       : "");

	for (std::size_t count_1 = 0; count_1 < member_list_.size(); ++count_1) {
		std::cout << std::endl;
		member_list_[count_1].TestFileName();
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
const char *GpbElementInfo::GetLabelName(
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
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void GpbElementInfo::ClearSourceLocation(
	::google::protobuf::SourceLocation &datum)
{
	datum.start_line   = 0;
	datum.end_line     = 0;
	datum.start_column = 0;
	datum.end_column   = 0;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::ostream & operator << (std::ostream &o_str, const GpbElementInfo &datum)
{
	o_str << std::setw(datum.depth_ * 3) << "" << "{"
		"Depth="                << datum.depth_            << ", "
		"TypeNameFull="         << datum.GetTypeNameFull() << ", "
		"TypeName="             << datum.GetTypeName()     << ", "
		"Name="                 << datum.GetName()         << ", "
		"Label="                << datum.GetLabel()        << ", "
		"LabelName="            << datum.GetLabelName()    << ", "
		"FileName="             << datum.GetFileName()     <<
		"}";

	for (std::size_t count_1 = 0; count_1 < datum.member_list_.size();
		++count_1)
		o_str << std::endl << datum.member_list_[count_1];

	return(o_str);
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace ProtoBufSupport

} // namespace MLB

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

#include <boost/io/ios_state.hpp>

//	////////////////////////////////////////////////////////////////////////////
void TEST_EmitSep(char sep_char, std::streamsize sep_width = 79,
	std::ostream &o_str = std::cout)
{
	boost::io::ios_all_saver io_state(o_str);

	o_str << std::setfill(sep_char) << std::setw(sep_width) << "" << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_EmitDatum(const MLB::ProtoBufSupport::GpbElementInfo &datum)
{
//	std::cout << datum << std::endl;
datum.TestFileName();
std::cout << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename MessageType>
	bool TEST_RunTest_1(int &return_code)
{
	bool test_passed = true;

	TEST_EmitSep('=');
	TEST_EmitSep('=');
	std::cout << "TEST for a templated messaged type:" << std::endl;
	TEST_EmitSep('=');

	try {
		MessageType                          test_msg;
		MLB::ProtoBufSupport::GpbElementInfo element(*test_msg.descriptor());
		TEST_EmitDatum(element);
	}
	catch (const std::exception &except) {
		std::cerr << "TEST FAILED: " << except.what() << std::endl;
		test_passed = false;
	}

	TEST_EmitSep('=');
	std::cout << std::endl;

	if (!test_passed)
		return_code = EXIT_FAILURE;

	return(test_passed);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
bool TEST_RunTest_2(int &return_code, const char *message_name)
{
	TEST_EmitSep('=');
	TEST_EmitSep('=');
	std::cout << "TEST for a message name ('" << message_name << "'):" <<
		std::endl;
	TEST_EmitSep('=');

	bool test_passed = true;

	try {
		MLB::ProtoBufSupport::GpbElementInfo element(message_name);
		TEST_EmitDatum(element);
	}
	catch (const std::exception &except) {
		std::cerr << "TEST FAILED: " << except.what() << std::endl;
		test_passed = false;
	}

	TEST_EmitSep('=');
	std::cout << std::endl;

	if (!test_passed)
		return_code = EXIT_FAILURE;

	return(test_passed);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
int main()
{
	int return_code = EXIT_SUCCESS;

	try {
		TEST_RunTest_1<GpbElementInfoTest::AddressBook>(return_code);
		TEST_RunTest_2(return_code, "GpbElementInfoTest::AddressBook");
		TEST_RunTest_2(return_code, "GpbElementInfoTest.AddressBook");
		TEST_RunTest_2(return_code, "GpbElementInfoTest::Person::PhoneNumber");
		TEST_RunTest_2(return_code, "GpbElementInfoTest.Person.PhoneNumber");
		TEST_RunTest_2(return_code, "GPBELEMENTINFOTEST.PERSON.PHONENUMBER");
		TEST_RunTest_2(return_code, "gpbelementinfotest.person.phonenumber");
	}
	catch (const std::exception &except) {
		std::cerr << "ERROR: " << except.what() << std::endl;
		return_code = EXIT_FAILURE;
	}

	return(return_code);
}
//	////////////////////////////////////////////////////////////////////////////

#endif // #ifdef TEST_MAIN

