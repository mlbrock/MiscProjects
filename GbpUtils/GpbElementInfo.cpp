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

#include "GpbElementInfo.hpp"

#include <iomanip>
#include <sstream>

#include <boost/algorithm/string.hpp>

//	////////////////////////////////////////////////////////////////////////////

namespace MLB {

namespace ProtoBufSupport {

namespace {

//	////////////////////////////////////////////////////////////////////////////
const ::google::protobuf::FieldDescriptor::CppType InitialCppTypeValue =
	static_cast<::google::protobuf::FieldDescriptor::CppType>(
	::google::protobuf::FieldDescriptor::MAX_CPPTYPE + 1);
//	////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

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
	,cpp_type_(InitialCppTypeValue)
	,datum_type_(GpbDatumType_Count)
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
	,cpp_type_(InitialCppTypeValue)
	,datum_type_(GpbDatumType_Count)
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
	,cpp_type_(InitialCppTypeValue)
	,datum_type_(GpbDatumType_Count)
	,depth_(depth)
	,member_index_(member_index)
	,max_depth_(depth_)
	,member_list_()
{
	using namespace ::google::protobuf;

	if (field_descriptor_) {
		cpp_type_ = field_descriptor_->cpp_type();
		if (field_descriptor_->label() == FieldDescriptor::LABEL_REPEATED) {
			if ((cpp_type_ == FieldDescriptor::CPPTYPE_STRING) ||
				 (cpp_type_ == FieldDescriptor::CPPTYPE_MESSAGE))
				datum_type_ = GpbDatumType_RepeatedPtr;
			else
				datum_type_ = GpbDatumType_Repeated;
		}
		else
			datum_type_ = static_cast<GpbDatumType>(cpp_type_);
	}

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
const char *GpbElementInfo::GetMemberName() const
{
	return((field_descriptor_) ? field_descriptor_->name().c_str() : "");
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
const char *GpbElementInfo::GetTypeFileName() const
{
	return((enum_descriptor_) ? enum_descriptor_->file()->name().c_str() :
		((descriptor_) ? descriptor_->file()->name().c_str() : ""));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *GpbElementInfo::GetMemberFileName() const
{
	return((field_descriptor_) ? field_descriptor_->file()->name().c_str() :
		"");
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

namespace {

//	////////////////////////////////////////////////////////////////////////////
struct CompareByNameAndType {
	bool operator () (const GpbElementInfo &lhs, const GpbElementInfo &rhs)
	{
		int cmp;

		if ((cmp = ::strcmp(lhs.GetMemberName(), rhs.GetMemberName())) == 0)
			cmp = ::strcmp(lhs.GetTypeNameFull(), rhs.GetTypeNameFull());

		return(cmp < 0);
	}
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbElementInfoVector SetOperationHelper(bool is_diff,
	const GpbElementInfoVector &in_lhs, const GpbElementInfoVector &in_rhs,
	GpbElementInfoVector &out_lhs, GpbElementInfoVector &out_rhs)
{
	out_lhs = in_lhs;
	out_rhs = in_rhs;

	GpbElementInfoVector result(in_lhs.size());

	std::sort(out_lhs.begin(), out_lhs.end(), CompareByNameAndType());
	std::sort(out_rhs.begin(), out_rhs.end(), CompareByNameAndType());

	GpbElementInfoVectorIter iter_result((!is_diff) ?
		std::set_intersection(out_lhs.begin(), out_lhs.end(),
		out_rhs.begin(), out_rhs.end(), result.begin(), CompareByNameAndType()) :
		std::set_difference(out_lhs.begin(), out_lhs.end(),
		out_rhs.begin(), out_rhs.end(), result.begin(), CompareByNameAndType()));

	result.resize(iter_result - result.begin());

	return(result);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbElementInfoVector SetOperationHelper(bool is_diff,
	const GpbElementInfoVector &in_lhs, const GpbElementInfoVector &in_rhs)
{
	GpbElementInfoVector out_lhs;
	GpbElementInfoVector out_rhs;

	return(SetOperationHelper(is_diff, in_lhs, in_rhs, out_lhs, out_rhs));
}
//	////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

//	////////////////////////////////////////////////////////////////////////////
GpbElementInfoVector GpbElementInfo::SetIntersection(
	const GpbElementInfo &other) const
{
	return(SetOperationHelper(false, member_list_, other.member_list_));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbElementInfoVector GpbElementInfo::SetDifference(
	const GpbElementInfo &other) const
{
	return(SetOperationHelper(true, member_list_, other.member_list_));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
GpbElementInfoPairVector GpbElementInfo::SetIntersectionPair(
	const GpbElementInfo &other) const
{
	GpbElementInfoVector     tmp_lhs;
	GpbElementInfoVector     tmp_rhs;
	GpbElementInfoVector     inte_list(SetOperationHelper(false, member_list_,
		other.member_list_, tmp_lhs, tmp_rhs));
	GpbElementInfoPairVector result_list(inte_list.size());

	//	Can't fail --- we just matched lhs and rhs elements.
	for (std::size_t count_1 = 0; count_1 < result_list.size(); ++count_1)
		result_list[count_1] = GpbElementInfoPair(inte_list[count_1],
			*std::lower_bound(tmp_rhs.begin(), tmp_rhs.end(),
			inte_list[count_1], CompareByNameAndType()));

	return(result_list);

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
		"Depth="                << datum.depth_              << ", "
		"TypeNameFull="         << datum.GetTypeNameFull()   << ", "
		"TypeName="             << datum.GetTypeName()       << ", "
		"MemberName="           << datum.GetMemberName()     << ", "
		"Label="                << datum.GetLabel()          << ", "
		"LabelName="            << datum.GetLabelName()      << ", "
		"TypeFileName="         << datum.GetTypeFileName()   << ", "
		"MemberFileName="       << datum.GetMemberFileName() <<
		"}";

	for (std::size_t count_1 = 0; count_1 < datum.member_list_.size();
		++count_1)
		o_str << std::endl << datum.member_list_[count_1];

	return(o_str);
/*
	o_str << std::setw(datum.depth_ * 3) << "" << "{"
		"\"Depth\": "            << datum.depth_              << ", "
		"\"TypeNameFull\": \""   << datum.GetTypeNameFull()   << "\", "
		"\"TypeName\": \""       << datum.GetTypeName()       << "\", "
		"\"MemberName\": \""     << datum.GetMemberName()     << "\", "
		"\"Label\": "            << datum.GetLabel()          << ", "
		"\"LabelName\": \""      << datum.GetLabelName()      << "\", "
		"\"TypeFileName\": \""   << datum.GetTypeFileName()   << "\", "
		"\"MemberFileName\": \"" << datum.GetMemberFileName() << "\"";

	for (std::size_t count_1 = 0; count_1 < datum.member_list_.size();
		++count_1)
		o_str << std::endl << datum.member_list_[count_1];

	o_str << "}";

	return(o_str);
*/
/*
	std::string pad;

	{
		std::ostringstream tmp_o_str;
		tmp_o_str << std::setw((datum.depth_ + 1) * 3) << "";
		pad = tmp_o_str.str();
	}

	o_str << std::setw(datum.depth_ * 3) << "" << "{\n"
		<< pad << "\"Depth\": "            << datum.depth_              << ",\n"
		<< pad << "\"TypeNameFull\": \""   << datum.GetTypeNameFull()   << "\",\n"
		<< pad << "\"TypeName\": \""       << datum.GetTypeName()       << "\",\n"
		<< pad << "\"MemberName\": \""     << datum.GetMemberName()     << "\",\n"
		<< pad << "\"Label\": "            << datum.GetLabel()          << ",\n"
		<< pad << "\"LabelName\": \""      << datum.GetLabelName()      << "\",\n"
		<< pad << "\"TypeFileName\": \""   << datum.GetTypeFileName()   << "\",\n"
		<< pad << "\"MemberFileName\": \"" << datum.GetMemberFileName() << "\"\n";

	o_str << pad << "\"MemberList\": [\n";

	for (std::size_t count_1 = 0; count_1 < datum.member_list_.size();
		++count_1)
		o_str << ((count_1) ? ",\n" : "") << datum.member_list_[count_1];

	if (datum.member_list_.empty())
		o_str << "]\n";
	else
		o_str << "\n" << pad << "]\n";

	o_str << std::setw(datum.depth_ * 3) << "" << "}";

	return(o_str);
*/
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace ProtoBufSupport

} // namespace MLB

#ifdef TEST_MAIN

#include <cstdlib>
#include <iostream>

#include <boost/io/ios_state.hpp>

namespace {

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
#if 1
	std::cout << datum << std::endl;
#else
	datum.TestFileName();
	std::cout << std::endl;
#endif // #if 1
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
void TEST_RunTest_3_HelperInner(
	const MLB::ProtoBufSupport::GpbElementInfo &element_1,
	const MLB::ProtoBufSupport::GpbElementInfo &element_2,
	MLB::ProtoBufSupport::GpbElementInfoVector &inte_list,
	MLB::ProtoBufSupport::GpbElementInfoVector &diff_list)
{
	inte_list = element_1.SetIntersection(element_2);
	diff_list = element_1.SetDifference(element_2);

	std::cout << "Source     : " << element_1.GetTypeNameFull() << std::endl;
	std::cout << "Destination: " << element_2.GetTypeNameFull() << std::endl;
	TEST_EmitSep('-');
	std::cout << "Same in source and in destination: " << inte_list.size() <<
		std::endl;
	std::copy(inte_list.begin(), inte_list.end(), std::ostream_iterator<
		MLB::ProtoBufSupport::GpbElementInfoVector::value_type>(
		std::cout, "\n"));
	TEST_EmitSep('-');
	std::cout << "In source, not in destination    : " << diff_list.size() <<
		std::endl;
	std::copy(diff_list.begin(), diff_list.end(), std::ostream_iterator<
		MLB::ProtoBufSupport::GpbElementInfoVector::value_type>(
		std::cout, "\n"));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_RunTest_3_Helper(
	const MLB::ProtoBufSupport::GpbElementInfo &element_1,
	const MLB::ProtoBufSupport::GpbElementInfo &element_2,
	MLB::ProtoBufSupport::GpbElementInfoVector inte_lists[2],
	MLB::ProtoBufSupport::GpbElementInfoVector diff_lists[2])
{
	TEST_RunTest_3_HelperInner(element_1, element_2, inte_lists[0],
		diff_lists[0]);
	TEST_EmitSep('=');
	TEST_RunTest_3_HelperInner(element_2, element_1, inte_lists[1],
		diff_lists[1]);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
bool TEST_Compare(int &return_code, const std::string &message_name_1,
	const std::string &message_name_2)
{
	TEST_EmitSep('=');
	TEST_EmitSep('=');
	std::cout << "TEST of conversion logic generation:" << std::endl;
	std::cout << "   From: " << message_name_1 << std::endl;
	std::cout << "   To  : " << message_name_2 << std::endl;
	TEST_EmitSep('=');

	bool test_passed = true;

	try {
		MLB::ProtoBufSupport::GpbElementInfo       element_1(message_name_1);
		MLB::ProtoBufSupport::GpbElementInfo       element_2(message_name_2);
		MLB::ProtoBufSupport::GpbElementInfoVector inte_lists[2];
		MLB::ProtoBufSupport::GpbElementInfoVector diff_lists[2];
		TEST_RunTest_3_Helper(element_1, element_2, inte_lists, diff_lists);
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
bool TEST_RunTest_3(int &return_code, const std::string &message_name_1,
	const std::string &message_name_2)
{
	return(TEST_Compare(return_code, message_name_1, message_name_2));
}
//	////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

//	////////////////////////////////////////////////////////////////////////////
int main()
{
	int return_code = EXIT_SUCCESS;

	try {
		TEST_RunTest_2(return_code, "GpbElementInfoTestOne::AddressBook");
		TEST_RunTest_2(return_code, "GpbElementInfoTestTwo::AddressBook");
		TEST_RunTest_2(return_code, "GpbElementInfoTestTwo::Person");
		TEST_RunTest_2(return_code, "GpbElementInfoTestTwo::PhoneNumber");
		TEST_RunTest_2(return_code, "GpbElementInfoTestThree::AddressBookOne");
		TEST_RunTest_2(return_code, "GpbElementInfoTestThree::AddressBookTwo");
		TEST_RunTest_3(return_code,
			"GpbElementInfoTestThree::PersonOne",
			"GpbElementInfoTestThree::PersonTwo");
	}
	catch (const std::exception &except) {
		std::cerr << "ERROR: " << except.what() << std::endl;
		return_code = EXIT_FAILURE;
	}

	return(return_code);
}
//	////////////////////////////////////////////////////////////////////////////

#endif // #ifdef TEST_MAIN

