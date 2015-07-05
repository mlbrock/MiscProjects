//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	MLB Google Protocol Buffers Support Library
//	////////////////////////////////////////////////////////////////////////////
/*
	File Name			:	GpbElementInfo.hpp

	File Description	:	Definition of the GpbElementInfo class.

	Revision History	:	2015-06-20 --- Creation.
									Michael L. Brock

		Copyright Michael L. Brock 2015 - 2015.
		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
//	////////////////////////////////////////////////////////////////////////////

#ifndef HH___MLB__ProtoBufSupport__GpbElementInfo_hpp___HH

#define HH___MLB__ProtoBufSupport__GpbElementInfo_hpp___HH	1

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
	typedef ::google::protobuf::Reflection      GPB_Reflection;
public:
	typedef std::vector<GpbElementInfo>            GpbElementInfoVector_I;
//	typedef GpbElementInfoVector_I::iterator       GpbElementInfoVectorIter_I;
//	typedef GpbElementInfoVector_I::const_iterator GpbElementInfoVectorIterC_I;

	explicit GpbElementInfo();
	explicit GpbElementInfo(const GPB_Descriptor &descriptor);
	explicit GpbElementInfo(const GPB_Descriptor *descriptor);
	explicit GpbElementInfo(const std::string &message_name);

	const char *GetTypeNameFull() const;

	const char *GetTypeName() const;

	const char *GetMemberName() const;

	const char *GetName() const;

	const char *GetTypeFileName() const;

	const char *GetMemberFileName() const;

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

	GpbElementInfoVector_I SetIntersection(const GpbElementInfo &other) const;
	GpbElementInfoVector_I SetDifference(const GpbElementInfo &other) const;

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
typedef GpbElementInfo::GpbElementInfoVector_I GpbElementInfoVector;
typedef GpbElementInfoVector::iterator         GpbElementInfoVectorIter;
typedef GpbElementInfoVector::const_iterator   GpbElementInfoVectorIterC;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::ostream & operator << (std::ostream &o_str, const GpbElementInfo &data);
//	////////////////////////////////////////////////////////////////////////////

} // namespace ProtoBufSupport

} // namespace MLB

#endif // #ifndef HH___MLB__ProtoBufSupport__GpbElementInfo_hpp___HH

