// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
//	MLB Utility Library Module File
// ////////////////////////////////////////////////////////////////////////////
/*
	File Name			:	StackTrace.cpp

	File Description	:	Lightweight stack trace facility.

	Revision History	:	2015-07-20 --- Creation.
									Michael L. Brock

		Copyright Michael L. Brock 1998 - 2015.
		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
// ////////////////////////////////////////////////////////////////////////////


// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
//	Required include files...
// ////////////////////////////////////////////////////////////////////////////

#include <execinfo.h>

#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>

#include <iomanip>

// ////////////////////////////////////////////////////////////////////////////

namespace MLB {

namespace Utility {

// ////////////////////////////////////////////////////////////////////////////
/*
	backtrace_symbols() output formats:

		<invocation-file-name>(<symbol-name>+0x<hex-offset>) [0x<hex-address>]

		PCRE: /^(.+)\(((.+)\+0x([\dA-Fa-f]+))*\)\s+\[0x([\dA-Fa-f]+)\]$

		Print the following captures: $1 $3 $4 $5

	Parentheses are empty if not compiled with -rdynamic
*/
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
struct StackTraceShallow {
	typedef std::vector<StackTraceShallow> StackTraceShallowVector_I;

	StackTraceShallow(
		std::size_t file_name_size = 0, const char *file_name_ptr = NULL,
		std::size_t func_name_size = 0, const char *func_name_ptr = NULL,
		std::size_t func_offset_size = 0, const char *func_offset_ptr = NULL,
		std::size_t func_address_size = 0, const char *func_address_ptr = NULL)
		:file_name_size_(file_name_size)
		,file_name_ptr_(file_name_ptr)
		,func_name_size_(func_name_size)
		,func_name_ptr_(func_name_ptr)
		,func_offset_size_(func_offset_size)
		,func_offset_ptr_(func_offset_ptr)
		,func_address_size_(func_address_size)
		,func_address_ptr_(func_address_ptr)
		,func_offset_value_(0)
		,func_address_value_(0)
	{
	}

	void swap(StackTraceShallow &other);

	std::string GetFileName() const
	{
		return((!file_name_ptr_) ? "" :
			std::string(file_name_ptr_, file_name_size_));
	}

	std::string GetFunctionName() const
	{
		return((!func_name_ptr_) ? "" :
			std::string(func_name_ptr_, func_name_size_));
	}

	std::size_t GetFunctionOffset() const
	{
		return(func_offset_value_);
	}

	const void *GetFunctionAddress() const
	{
		return(func_address_value_);
	}

	std::size_t ParseFunctionOffset()
	{
		if (!func_offset_ptr_)
			return(0);
		else if (func_offset_value_)
			return(func_offset_value_);

		char **end_ptr = NULL;

#ifdef _MSC_VER
		return(func_offset_value_ = ::_strtoui64(func_offset_ptr_, end_ptr, 16));
#else
		return(func_offset_value_ = ::strtoull(func_offset_ptr_, end_ptr, 16));
#endif // #ifdef _MSC_VER
	}

	const void *ParseFunctionAddress()
	{
		if (!func_address_ptr_)
			return(0);
		else if (func_address_value_)
			return(func_address_value_);

		char **end_ptr = NULL;

#ifdef _MSC_VER
		return(func_address_value_ = static_cast<const void *>(
			::_strtoui64(func_address_ptr_, end_ptr, 16)));
#else
		return(func_address_value_ = reinterpret_cast<const void *>(
			::strtoull(func_address_ptr_, end_ptr, 16)));
#endif // #ifdef _MSC_VER
	}

	void ParseNumericValues()
	{
		ParseFunctionOffset();
		ParseFunctionAddress();
	}

	std::size_t  file_name_size_;
	const char  *file_name_ptr_;
	std::size_t  func_name_size_;
	const char  *func_name_ptr_;
	std::size_t  func_offset_size_;
	const char  *func_offset_ptr_;
	std::size_t  func_address_size_;
	const char  *func_address_ptr_;
	std::size_t  func_offset_value_;
	const void  *func_address_value_;

	static StackTraceShallow &Extract(const char *src, StackTraceShallow &dst);
	static StackTraceShallow  Extract(const char *src);

	static StackTraceShallowVector_I &Extract(int src_count,
		const char **src_list, StackTraceShallowVector_I &dst_list);
	static StackTraceShallowVector_I  Extract(int src_count, 
		const char **src_list);

	static StackTraceShallowVector_I &FixUpNumericValues(
		StackTraceShallowVector_I &trace_list);

	static std::pair<std::size_t, std::size_t> GetMaxStringLengths(
		const StackTraceShallowVector_I &trace_list);

	static void Emit(const StackTraceShallowVector_I &trace_list,
		std::ostream &o_str = std::cout);
};
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
typedef StackTraceShallow::StackTraceShallowVector_I StackTraceShallowVector;
typedef StackTraceShallowVector::iterator 			  StackTraceShallowVectorIter;
typedef StackTraceShallowVector::const_iterator 	  StackTraceShallowVectorIterC;
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
void StackTraceShallow::swap(StackTraceShallow &other)
{
	std::swap(file_name_size_,     other.file_name_size_);
	std::swap(file_name_ptr_,      other.file_name_ptr_);
	std::swap(func_name_size_,     other.func_name_size_);
	std::swap(func_name_ptr_,      other.func_name_ptr_);
	std::swap(func_offset_size_,   other.func_offset_size_);
	std::swap(func_offset_ptr_,    other.func_offset_ptr_);
	std::swap(func_address_size_,  other.func_address_size_);
	std::swap(func_address_ptr_,   other.func_address_ptr_);
	std::swap(func_offset_value_,  other.func_offset_value_);
	std::swap(func_address_value_, other.func_address_value_);
}
// ////////////////////////////////////////////////////////////////////////////

namespace {

// ////////////////////////////////////////////////////////////////////////////
const boost::regex MyBackTraceMatcher(
	"^(.+)\\(((.+)\\+0x([\\dA-Fa-f]+))*\\)\\s+\\[0x([\\dA-Fa-f]+)\\]$",
	boost::regex::perl | boost::regex::icase);
// ////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

// ////////////////////////////////////////////////////////////////////////////
StackTraceShallow &StackTraceShallow::Extract(const char *src,
	StackTraceShallow &dst)
{
//		PCRE: /^(.+)\(((.+)\+0x([\dA-Fa-f]+))*\)\s+\[0x([\dA-Fa-f]+)\]$

	if ((!src) || (!(*src)))
		throw std::invalid_argument("Null src parameter.");

	StackTraceShallow tmp_dst;
	boost::cmatch     results;

	if (boost::regex_match(src, results, MyBackTraceMatcher,
		boost::match_default) && (results.size() == 6) && results[1].matched &&
		results[5].matched) {
		if (results[3].matched && results[4].matched)
			StackTraceShallow(
				results[1].second - results[1].first,
				results[1].first,
				results[3].second - results[3].first,
				results[3].first,
				results[4].second - results[4].first,
				results[4].first,
				results[5].second - results[5].first,
				results[5].first).swap(dst);
		else
			StackTraceShallow(
				results[1].second - results[1].first,
				results[1].first,
				0,
				NULL,
				0,
				NULL,
				results[5].second - results[5].first,
				results[5].first).swap(dst);
	}
	else
		StackTraceShallow().swap(dst);

	return(dst);
}
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
StackTraceShallow StackTraceShallow::Extract(const char *src)
{
	StackTraceShallow dst;

	return(Extract(src, dst));
}
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
StackTraceShallowVector &StackTraceShallow::Extract(int src_count,
	const char **src_list, StackTraceShallowVector &dst_list)
{
	StackTraceShallowVector tmp_dst_list;

	if (src_count > 0) {
		for (int count_1 = 0; count_1 < src_count; ++count_1)
			tmp_dst_list.push_back(Extract(src_list[count_1]));
	}

	dst_list.swap(tmp_dst_list);

	return(dst_list);
}
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
StackTraceShallowVector StackTraceShallow::Extract(int src_count, 
	const char **src_list)
{
	StackTraceShallowVector dst_list;

	return(Extract(src_count, src_list, dst_list));
}
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
StackTraceShallowVector &StackTraceShallow::FixUpNumericValues(
	StackTraceShallowVector &trace_list)
{
	StackTraceShallowVectorIter iter_b(trace_list.begin());
	StackTraceShallowVectorIter iter_e(trace_list.end());

	for ( ; iter_b != iter_e; ++iter_b)
		iter_b->ParseNumericValues();

	return(trace_list);
}
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
std::pair<std::size_t, std::size_t> StackTraceShallow::GetMaxStringLengths(
	const StackTraceShallowVector &trace_list)
{
	std::pair<std::size_t, std::size_t> results(0, 0);
	StackTraceShallowVectorIterC  iter_b(trace_list.begin());
	StackTraceShallowVectorIterC  iter_e(trace_list.end());

	for ( ; iter_b != iter_e; ++iter_b) {
		results.first  = std::max(results.first,  iter_b->file_name_size_);
		results.second = std::max(results.second, iter_b->func_name_size_);
	}

	return(results);
}
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
void StackTraceShallow::Emit(const StackTraceShallowVector &trace_list,
	std::ostream &o_str)
{
	std::pair<std::size_t, std::size_t> max_lengths(
		GetMaxStringLengths(trace_list));

	StackTraceShallowVectorIterC iter_b(trace_list.begin());
	StackTraceShallowVectorIterC iter_e(trace_list.end());

	o_str << std::setfill(' ');

	for ( ; iter_b != iter_e; ++iter_b) {
		if (iter_b->file_name_ptr_) {
			o_str
				<< std::left
				<< std::setw(max_lengths.first) << iter_b->GetFileName() << " "
				<< "0x" << std::right << std::setfill('0') << std::hex
				<< std::setw(sizeof(void *) * 2) <<
					reinterpret_cast<unsigned long long>(
					iter_b->GetFunctionAddress())
				<< " " << std::dec << std::setfill(' ')
					;
			if (iter_b->func_name_ptr_)
				o_str
					<< std::left
					<< std::setw(max_lengths.second) << iter_b->GetFunctionName()
					<< " "
					<< "0x" << std::right << std::setfill('0') << std::hex
					<< std::setw(4) << iter_b->GetFunctionOffset()
					<< " " << std::dec << std::setfill(' ')
						;
			o_str << std::endl;
		}
	}
}
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
void GetBackTrace()
{
	const int  list_count = 1024;
	void      *list_data[list_count];
	int        trace_count;

	if ((trace_count = ::backtrace(list_data, list_count)) < 1) {
		std::ostringstream o_str;
		o_str << "Invocation of ::backtrace() returned " << trace_count << ".";
		throw std::runtime_error(o_str.str());
	}

	boost::shared_ptr<const char *> trace_list(const_cast<const char **>(
		::backtrace_symbols(list_data, trace_count)));

	StackTraceShallowVector trace_vector(
		StackTraceShallow::Extract(trace_count, trace_list.get()));
	StackTraceShallow::FixUpNumericValues(trace_vector);
	StackTraceShallow::Emit(trace_vector);

	std::cout << std::endl;
}
// ////////////////////////////////////////////////////////////////////////////

} // namespace Utility

} // namespace MLB

#ifdef TEST_MAIN

#include <cstdlib>

namespace SomeNamespaceName {

// ////////////////////////////////////////////////////////////////////////////
struct TEST_ClassBottom {
	void NonStaticNonConstFunc()
	{
		NonStaticConstFunc();
	}

	void NonStaticConstFunc() const
	{
		StaticFunc();
	}

	static void StaticFunc()
	{
		MLB::Utility::GetBackTrace();
	}
};
//	----------------------------------------------------------------------------
void TEST_InterveningFreeFunc_3()
{
	TEST_ClassBottom bottom_instance;

	bottom_instance.NonStaticConstFunc();
}
//	----------------------------------------------------------------------------
void TEST_InterveningFreeFunc_2()
{
	TEST_InterveningFreeFunc_3();
}
//	----------------------------------------------------------------------------
void TEST_InterveningFreeFunc_1()
{
	TEST_InterveningFreeFunc_2();
}
//	----------------------------------------------------------------------------
struct TEST_ClassTop {
	void NonStaticNonConstFunc()
	{
		NonStaticConstFunc();
	}

	void NonStaticConstFunc() const
	{
		StaticFunc();
	}

	static void StaticFunc()
	{
		TEST_InterveningFreeFunc_1();
	}
};
//	----------------------------------------------------------------------------
void TEST_RunTest(int &return_code)
{
	try {
		TEST_ClassTop top_instance;
		top_instance.NonStaticNonConstFunc();
	}
	catch (const std::exception &except) {
		std::cerr << "ERROR in TEST_RunTest(): " << except.what() << std::endl;
		return_code = EXIT_FAILURE;
	}
}
// ////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

int main()
{
	int return_code = EXIT_SUCCESS;

	try {
		SomeNamespaceName::TEST_RunTest(return_code);
	}
	catch (const std::exception &except) {
		std::cerr << "ERROR: " << except.what() << std::endl;
		return_code = EXIT_FAILURE;
	}

	return(return_code);
}

#endif // #ifdef TEST_MAIN

