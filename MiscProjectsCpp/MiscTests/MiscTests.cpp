// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
//	MLB Test Logic
// ////////////////////////////////////////////////////////////////////////////
/*
	File Name			:	MiscTests.hpp

	File Description	:	A miscellany of C++ tests.

	Revision History	:	2008-05-03 --- Creation.
									Michael L. Brock

		Copyright Michael L. Brock 2008 - 2018.
		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
// ////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	Required include files...
//	////////////////////////////////////////////////////////////////////////////

#include <Utility/ParseNumericString.hpp>

#include <Utility/IsEolChar.hpp>

#include <cstdio>
#include <exception>
#include <iomanip>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/utility/enable_if.hpp>

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4512)
#endif // #ifdef _Windows

#include <boost/io/ios_state.hpp>

#ifdef _MSC_VER
# pragma warning(pop)
#endif // #ifdef _MSC_VER

//	////////////////////////////////////////////////////////////////////////////

namespace {

//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType> void ValueIgnore(const DatumType & /* datum */)
{
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void EmitSep(std::ostream &o_str, char sep_char,
	std::streamsize sep_width = 79, bool nl_flag = true)
{
	o_str << std::setfill(sep_char) << std::setw(sep_width) << "" <<
		std::setfill(' ') << ((nl_flag) ? "\n" : "");
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void EmitSep(char sep_char, std::streamsize sep_width = 79,
	bool nl_flag = true)
{
	EmitSep(std::cout, sep_char, sep_width, nl_flag);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void EmitTextAndSep(const char *text, char sep_char = '-')
{
	std::cout << text << ":" << std::endl;

	EmitSep(sep_char, static_cast<std::streamsize>(::strlen(text) + 1), true);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void EmitTextAndSep(const std::string &text, char sep_char = '-')
{
	EmitTextAndSep(text.c_str(), sep_char);
}
//	////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

namespace TEST_RuntimeStaticCastOverhead {

//	////////////////////////////////////////////////////////////////////////////
int CastToInt(unsigned int value)
{
	return(static_cast<int>(value));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
int CastToInt(double value)
{
	return(static_cast<int>(value));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	int i_1 = CastToInt(static_cast<unsigned int>(42));
	int i_2 = CastToInt(42.1);

	ValueIgnore(i_1);
	ValueIgnore(i_2);
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_RuntimeStaticCastOverhead

namespace TEST_CtorStyleCast {

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	char   c_1 = char(3.14);
	int    i_1 = int(1.23);
	double d_1 = double('A');

	ValueIgnore(c_1);
	ValueIgnore(i_1);
	ValueIgnore(d_1);
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_CtorStyleCast

namespace TEST_ConstantFoldingCheck_1 {

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	int value = 3 + 4 + 5;

	std::cout << value << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_ConstantFoldingCheck_1

namespace TEST_WindowsMemoryBarriers {

#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))

//	////////////////////////////////////////////////////////////////////////////
void DoWindowsX86Test()
{
	LONG destination = 666;
	LONG exchange    = destination - 1;
	LONG comparand   = destination;

	::InterlockedCompareExchange(&destination, exchange, comparand);

	exchange  = destination - 1;
	comparand = destination;
	::InterlockedCompareExchangeAcquire(&destination, exchange, comparand);

	exchange  = destination - 1;
	comparand = destination;
	::InterlockedCompareExchangeRelease(&destination, exchange, comparand);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	std::cout << "Running for X86 architectures under Windows." << std::endl;
	std::cout << "_INTEGRAL_MAX_BITS = " << _INTEGRAL_MAX_BITS << std::endl;

	DoWindowsX86Test();
}
//	////////////////////////////////////////////////////////////////////////////

#else 

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	std::cout << "Not implemented for this platform." << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

#endif // #if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))

} // namespace TEST_WindowsMemoryBarriers

namespace TEST_LifetimeOfLocalsDeclaredInElseIfBlocks {

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	int rand_value = ::rand();

	if (rand_value == 100) {
		std::cout << "rand_value == 100";
	}
	else if (int my_tmp = 42 + rand_value) {
		my_tmp -= rand_value;
		std::cout << "rand_value != 100: A: my_tmp = " << my_tmp;
	}
	else {
		std::cout << "rand_value != 100: B: my_tmp = " << my_tmp;
	}

	std::cout << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_LifetimeOfLocalsDeclaredInElseIfBlocks

namespace TEST_ResolveOverloadOnClassHierarchy {

//	////////////////////////////////////////////////////////////////////////////
struct Base {
	virtual const char *MyName() const
	{
		return("Base");
	}
};
struct Derv : public Base {
	virtual const char *MyName() const
	{
		return("Derv");
	}
};
//	----------------------------------------------------------------------------
void FreeFuncOverload(const Base &datum)
{
	std::cout << "FreeFuncOverload(const Base &datum) = " << datum.MyName() <<
		std::endl;
}
//	----------------------------------------------------------------------------
void FreeFuncOverload(const Derv &datum)
{
	std::cout << "FreeFuncOverload(const Derv &datum) = " << datum.MyName() <<
		std::endl;
}
//	----------------------------------------------------------------------------
struct MyClass {
	void MethodOverload(const Base &datum)
	{
		std::cout << "MethodOverload(const Base &datum) = " << datum.MyName() <<
			std::endl;
	}
	void MethodOverload(const Derv &datum)
	{
		std::cout << "MethodOverload(const Derv &datum) = " << datum.MyName() <<
			std::endl;
	}
};
//	----------------------------------------------------------------------------
void RunTest()
{
	Base base_instance;
	Derv derv_instance;

	FreeFuncOverload(base_instance);
	FreeFuncOverload(derv_instance);

	MyClass method_test;

	method_test.MethodOverload(base_instance);
	method_test.MethodOverload(derv_instance);
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_ResolveOverloadOnClassHierarchy

namespace TEST_InitializeArrayofPodsMember {

#if defined(_MSC_VER) && (_MSC_VER <= 1500)
# pragma warning(push)
# pragma warning(disable: 4351)
#endif // #if defined(_MSC_VER) && (_MSC_VER <= 1500)
//	////////////////////////////////////////////////////////////////////////////
struct MyClass {
	MyClass()
		:array_int_()
		,array_double_()
	{
	}

	int    array_int_[3];
	double array_double_[5];
};
//	----------------------------------------------------------------------------
void RunTest()
{
	MyClass datum;
}
//	////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER) && (_MSC_VER <= 1500)
# pragma warning(pop)
#endif // #if defined(_MSC_VER) && (_MSC_VER <= 1500)

} // namespace TEST_InitializeArrayofPodsMember

namespace TEST_TemplatizedBitFieldSizes {

//	////////////////////////////////////////////////////////////////////////////
template <int Size1, int Size2> struct MyBitFields {
	MyBitFields()
	{
		::memset(this, 0xFF, sizeof(*this));
	}

	unsigned int field_1_ : Size1;
	unsigned int field_2_ : Size2;
};
//	----------------------------------------------------------------------------
void RunTest()
{
	MyBitFields<3, 4> datum_1;
	MyBitFields<4, 5> datum_2;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_TemplatizedBitFieldSizes

namespace TEST_ArrayIdentities {

//	////////////////////////////////////////////////////////////////////////////
#define TEST_ArrayIdentities_MACRO(in_array)											\
	if (sizeof(in_array) != sizeof(array_00_12)) {									\
		std::cout << "Sizeof '" << in_array << "' (" << sizeof(in_array) <<	\
			") is not equal to the sizeof '" << array_00_12 << " (" <<			\
			sizeof(array_00_12) << ")." << std::endl;									\
	}
//	----------------------------------------------------------------------------
void RunTest()
{
	int array_00_12[12];
	int array_01_12[1][12];
	int array_12_01[12][1];
	int array_02_06[2][6];
	int array_06_02[6][2];
	int array_03_04[3][4];
	int array_04_03[4][3];

#if defined(_MSC_VER) && (_MSC_VER <= 1500)
# pragma warning(push)
# pragma warning(disable: 4127)
#endif // #if defined(_MSC_VER) && (_MSC_VER <= 1500)
	TEST_ArrayIdentities_MACRO(array_00_12);
	TEST_ArrayIdentities_MACRO(array_01_12);
	TEST_ArrayIdentities_MACRO(array_12_01);
	TEST_ArrayIdentities_MACRO(array_02_06);
	TEST_ArrayIdentities_MACRO(array_06_02);
	TEST_ArrayIdentities_MACRO(array_03_04);
	TEST_ArrayIdentities_MACRO(array_04_03);
#if defined(_MSC_VER) && (_MSC_VER <= 1500)
# pragma warning(pop)
#endif // #if defined(_MSC_VER) && (_MSC_VER <= 1500)

	int (&array_04_03_ref)[4][3] =  array_04_03;
	int (*array_04_03_ptr)[4][3] = &array_04_03_ref;
	int  *ptr_04_03              = reinterpret_cast<int *>(array_04_03_ptr);

	ptr_04_03 = NULL;	//	Shut the compiler up.
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_ArrayIdentities

namespace TEST_EnableIf {

//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType, typename EnableType = void> struct TestType {
	const EnableType *Non_numeric_type_parameter_to_TestType_class_template;
	TestType()
	{
		std::cout << "TestType non-numeric type holder." << std::endl;
	}
};
//	----------------------------------------------------------------------------
template <typename DatumType>
	struct TestType<DatumType,
		typename boost::enable_if<
			boost::is_integral<DatumType> >::type> {
	TestType()
	{
		std::cout << "TestType integer holder." << std::endl;
	}
};
//	----------------------------------------------------------------------------
template <typename DatumType>
	struct TestType<DatumType,
		typename boost::enable_if<
			boost::is_float<DatumType> >::type> {
	TestType()
	{
		std::cout << "TestType floating-point holder." << std::endl;
	}
};
//	----------------------------------------------------------------------------
struct SomeClass {
};
//	----------------------------------------------------------------------------
void RunTest()
{
	TestType<signed int>   test_si;
	TestType<unsigned int> test_ui;
	TestType<double>       test_d;

	TestType<void *>            test_bad_1;
	TestType<TestType<double> > test_bad_2;
	TestType<SomeClass>         test_bad_3;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_EnableIf

namespace TEST_MemberAccess_1 {

//	////////////////////////////////////////////////////////////////////////////
struct Base {
	virtual std::string Foo() const { return("Base"); }
};
//	----------------------------------------------------------------------------
struct Derived_PUB_PUB_1 : public virtual Base {
	virtual std::string Foo() const { return("Derived_PUB_PUB_1"); }
};
//	----------------------------------------------------------------------------
struct Derived_PUB_PUB_2 : public virtual Base {
	virtual std::string Foo() const { return("Derived_PUB_PUB_2"); }
};
//	----------------------------------------------------------------------------
//	Uses base class implementation.
struct Derived_PUB_PUB_3 : public virtual Base {
};
//	----------------------------------------------------------------------------
/*
	Access through a base class reference or pointer works, but not
	through a pointer of the derived class type.
*/
struct Derived_PUB_PRI : public Base {
private:
	virtual std::string Foo() const { return("Derived_PUB_PRI"); }
};
//	----------------------------------------------------------------------------
/*
	Can only be called by member and friend functions because the
	inheritance relationship isn't accessible otherwise.
*/
struct Derived_PRI_PRI : private Base {
private:
	virtual std::string Foo() const { return("Derived_PRI_PRI"); }
	friend void RunTest();
};
//	----------------------------------------------------------------------------
/*
	Can only be called by member and friend functions because the
	inheritance relationship isn't accessible otherwise.
*/
struct Derived_PRI_PUB : private Base {
	virtual std::string Foo() const { return("Derived_PRI_PUB"); }
	friend void RunTest();
};
//	----------------------------------------------------------------------------
#if 0
/*
	Doesn't compile because of ambiguity. MSVC 2008 reports:
		error C2250: 'TEST_MemberAccess_1::DerivedAmbiguous' : ambiguous
		inheritance of 'std::string TEST_MemberAccess_1::Base::
		Foo(void) const'
*/
struct DerivedAmbiguous_1 : public Derived_PUB_PUB_1,
									 public Derived_PUB_PUB_2 {
};
#endif // #if 0
//	----------------------------------------------------------------------------
/*
	But this compiles because there's no ambiguity as to which base
	class implementation to use (none, uses local method).
*/
struct DerivedAmbiguous_2 : public Derived_PUB_PUB_1,
									 public Derived_PUB_PUB_2 {
	virtual std::string Foo() const { return("DerivedAmbiguous_2"); }
};
//	----------------------------------------------------------------------------
#if defined(_MSC_VER) && (_MSC_VER <= 1500)
# pragma warning(push)
# pragma warning(disable: 4250)
#endif // #if defined(_MSC_VER) && (_MSC_VER <= 1500)
/*
	This also compiles because the Derived_PUB_PUB_1 implementation is
	selected by dominance. MSVC 2008 reports:
		warning C4250: 'TEST_MemberAccess_1::DerivedAmbiguous_3' : inherits
		'TEST_MemberAccess_1::Derived_PUB_PUB_1::TEST_MemberAccess_1::
		Derived_PUB_PUB_1::Foo' via dominance
*/
struct DerivedAmbiguous_3 : public Derived_PUB_PUB_1,
									 public Derived_PUB_PUB_3 {
};
#if defined(_MSC_VER) && (_MSC_VER <= 1500)
# pragma warning(pop)
#endif // #if defined(_MSC_VER) && (_MSC_VER <= 1500)
//	----------------------------------------------------------------------------
#define TEST_MemberAccess_1_MACRO(c_name)											\
	{																							\
		c_name tmp_instance;																\
		Base   &tmp_ref(tmp_instance);												\
		std::cout << "Static type: " << std::left << std::setw(18) <<		\
			#c_name << std::right << " ---> Output: " << tmp_ref.Foo() <<	\
			std::endl;																		\
	}
//	----------------------------------------------------------------------------
void RunTest()
{
	TEST_MemberAccess_1_MACRO(Derived_PUB_PUB_1);
	TEST_MemberAccess_1_MACRO(Derived_PUB_PUB_2);
	TEST_MemberAccess_1_MACRO(Derived_PUB_PUB_3);
//	TEST_MemberAccess_1_MACRO(DerivedAmbiguous_1);
	std::cout << "Static type: " << std::left << std::setw(18) <<
		"DerivedAmbiguous_1" << std::right <<
		" ---> ******  CANNOT BE COMPILED  ******" << std::endl;
	TEST_MemberAccess_1_MACRO(DerivedAmbiguous_2);
	TEST_MemberAccess_1_MACRO(DerivedAmbiguous_3);
	TEST_MemberAccess_1_MACRO(Derived_PUB_PRI);
	TEST_MemberAccess_1_MACRO(Derived_PRI_PRI);
	TEST_MemberAccess_1_MACRO(Derived_PRI_PUB);
}
//	----------------------------------------------------------------------------
/*
	Output:
	-------
	Static type: Derived_PUB_PUB_1  ---> Output: Derived_PUB_PUB_1
	Static type: Derived_PUB_PUB_2  ---> Output: Derived_PUB_PUB_2
	Static type: Derived_PUB_PUB_3  ---> Output: Base
	Static type: DerivedAmbiguous_1 ---> ******  CANNOT BE COMPILED  ******
	Static type: DerivedAmbiguous_2 ---> Output: DerivedAmbiguous_2
	Static type: DerivedAmbiguous_3 ---> Output: Derived_PUB_PUB_1
	Static type: Derived_PUB_PRI    ---> Output: Derived_PUB_PRI
	Static type: Derived_PRI_PRI    ---> Output: Derived_PRI_PRI
	Static type: Derived_PRI_PUB    ---> Output: Derived_PRI_PUB
*/
//	----------------------------------------------------------------------------
#undef TEST_MemberAccess_1_MACRO
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_MemberAccess_1

namespace TEST_MemberAccess_2 {

//	////////////////////////////////////////////////////////////////////////////
struct Base {
private:
	virtual std::string Foo() const { return("Base"); }
};
//	----------------------------------------------------------------------------
struct Derived : public Base {
public:
	virtual std::string Foo() const { return("Derived"); }
};
//	----------------------------------------------------------------------------
void RunTest()
{
	{
		Derived  tmp_instance;
		Base    &tmp_ref(tmp_instance);
		std::cout << "Static type: " << std::left << std::setw(18) <<
			"Derived" << std::right << " ---> Output: " << tmp_instance.Foo() <<
			std::endl;
	}
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_MemberAccess_2

namespace TEST_ReverseAStringRecursively {

//	////////////////////////////////////////////////////////////////////////////
char *ReverseStringRecursively(char * const begin_ptr, char * const end_ptr)
{
	if ((end_ptr - begin_ptr) > 1) {
		char tmp_char = *begin_ptr;
		*begin_ptr  = end_ptr[-1];
		end_ptr[-1] = tmp_char;
		ReverseStringRecursively(begin_ptr + 1, end_ptr - 1);
	}

	return(begin_ptr);
}
//	----------------------------------------------------------------------------
char *ReverseStringRecursively(char *in_string)
{
	if ((!in_string) || (!(*in_string)))
		return(in_string);

	return(ReverseStringRecursively(in_string,
		in_string + ::strlen(in_string)));
}
//	----------------------------------------------------------------------------
char *FunctionalReverseString(char * const begin_ptr, char * const end_ptr)
{
	if ((begin_ptr + 1) < end_ptr) {
		*begin_ptr  ^= end_ptr[-1];
		end_ptr[-1] ^= *begin_ptr;
		*begin_ptr  ^= end_ptr[-1];
		FunctionalReverseString(begin_ptr + 1, end_ptr - 1);
	}

	return(begin_ptr);
}
//	----------------------------------------------------------------------------
char *FunctionalReverseString(char *in_string)
{
	if ((!in_string) || (!(*in_string)))
		return(in_string);

	return(FunctionalReverseString(in_string, in_string + ::strlen(in_string)));
}
//	----------------------------------------------------------------------------
char *ReverseCharsIteratively(char *in_string)
{
	std::size_t string_length = ::strlen(in_string);

	if (string_length > 0) {
		char *begin_ptr = in_string;
		char *end_ptr   = in_string + string_length - 1;
		while (begin_ptr < end_ptr) {
			char tmp_char = *begin_ptr;
			*begin_ptr++ = *end_ptr;
			*end_ptr--   = tmp_char;
		}
	}

	return(in_string);
}
//	----------------------------------------------------------------------------
void RunTestCheck(const boost::shared_ptr<char> &lhs,
	const boost::shared_ptr<char> &rhs)
{
	if (!::strcmp(lhs.get(), rhs.get()))
		std::cout << "OK";
	else {
		std::cout << "FAILED";
		std::cout << " : [" << rhs << "]";
	}

	std::cout << std::endl;
}
//	----------------------------------------------------------------------------
void RunTestHelper(const char *in_string)
{
	boost::shared_ptr<char> tmp_string_1(::strdup(in_string));
	boost::shared_ptr<char> tmp_string_2(::strdup(in_string));
	boost::shared_ptr<char> tmp_string_3(::strdup(in_string));

	std::cout << "A: [" << tmp_string_1 << "]" << std::endl;

	ReverseStringRecursively(tmp_string_1.get());
	std::cout << "B: [" << tmp_string_1 << "] ---> ";
	RunTestCheck(tmp_string_1, tmp_string_1);

	FunctionalReverseString(tmp_string_2.get());
	std::cout << "C: [" << tmp_string_2 << "] ---> ";
	RunTestCheck(tmp_string_1, tmp_string_2);

	ReverseCharsIteratively(tmp_string_3.get());
	std::cout << "D: [" << tmp_string_3 << "] ---> ";
	RunTestCheck(tmp_string_1, tmp_string_3);

	std::cout << std::endl;
}
//	----------------------------------------------------------------------------
void RunTest()
{
	RunTestHelper("The quick brown fox jumped over the lazy dog");
	EmitSep('-');

	RunTestHelper("first second third");
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_ReverseAStringRecursively

namespace TEST_ReverseWordsInAString {

//	////////////////////////////////////////////////////////////////////////////
char *ReverseChars(char *in_string, int string_length)
{
	if (string_length > 0) {
		char *begin_ptr = in_string;
		char *end_ptr   = in_string + string_length - 1;
		while (begin_ptr < end_ptr) {
			char tmp_char = *begin_ptr;
			*begin_ptr++ = *end_ptr;
			*end_ptr--   = tmp_char;
		}
	}

	return(in_string);
}
//	----------------------------------------------------------------------------
char *ReverseWords(char *in_string)
{
	if (!(*in_string))
		return(in_string);

	int string_length = static_cast<int>(::strlen(in_string));

	ReverseChars(in_string, string_length);

	char *begin_ptr = in_string;
	char *end_ptr   = in_string + (string_length - 1);

	while (begin_ptr < end_ptr) {
		while (::isspace(*begin_ptr))
			++begin_ptr;
		if (*begin_ptr) {
			char *tmp_begin_ptr = begin_ptr;
			while (*begin_ptr && (!::isspace(*begin_ptr)))
				++begin_ptr;
			if ((begin_ptr - tmp_begin_ptr) > 1)
				ReverseChars(tmp_begin_ptr, begin_ptr - tmp_begin_ptr);
		}
	}

	return(in_string);
}
//	----------------------------------------------------------------------------
void RunTestHelper(const char *in_string)
{
	boost::shared_ptr<char> tmp_string(::strdup(in_string));

	std::cout << "A: [" << tmp_string << "]" << std::endl;

	ReverseWords(tmp_string.get());

	std::cout << "B: [" << tmp_string << "]" << std::endl;
}
//	----------------------------------------------------------------------------
void RunTest()
{
	RunTestHelper("The quick brown fox jumped over the lazy dog");
	EmitSep('-');

	RunTestHelper("first second third");
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_ReverseWordsInAString

namespace TEST_TemplateTemplateParameters {

//	////////////////////////////////////////////////////////////////////////////
/*
template <template <typename ValueType> typename ContainerType>
	struct IsInSliceWrapper {
	static bool IsInSlice(const ValueType &target_value,
		const ContainerType &container)
	{
		typename ContainerType::const_iterator iter_b(container.begin());
		typename ContainerType::const_iterator iter_e(container.end());

		for ( ; iter_b != iter_e; ++iter_b) {
			if (target_value == *iter_b)
				return(true);
		}

		return(false);
	}
};
//	----------------------------------------------------------------------------
template <> struct IsInSliceWrapper<std::set<ValueType> > {
	static bool IsInSlice(const ValueType &target_value,
		const ContainerType &container)
	{
		return(container.find(target_value) != container.end());
	}
};
//	----------------------------------------------------------------------------
template <> struct IsInSliceWrapper<std::multiset<ValueType> > {
	static bool IsInSlice(const ValueType &target_value,
		const ContainerType &container)
	{
		return(container.find(target_value) != container.end());
	}
};
*/
//	----------------------------------------------------------------------------
template<template<typename, typename> class ContainerType, typename ValueType>
   void DoTest(ContainerType<ValueType, std::allocator<ValueType> > &cont)
{
	ValueType tmp_1(cont.back());
	typename ContainerType<ValueType, std::allocator<ValueType> >::value_type
				 tmp_2(cont.back());
   cont.pop_back();
   std::cout << tmp_1 << std::endl;
}
//	----------------------------------------------------------------------------
template<typename ValueType, template<typename, typename> class ContainerType>
   void DoTest_2(ContainerType<ValueType, std::allocator<ValueType> > &cont)
{
	ValueType tmp_1(cont.back());
	typename ContainerType<ValueType, std::allocator<ValueType> >::value_type
				 tmp_2(cont.back());
   cont.pop_back();
   std::cout << ((tmp_1 == tmp_2) ? "   " : "NOT") << tmp_1 << std::endl;
}
//	----------------------------------------------------------------------------
template<typename ValueType, template <typename, typename> class ContainerType>
   void DoTest_3(ContainerType<ValueType, std::allocator<ValueType> > &cont)
{
	ValueType tmp_1(cont.back());
	typename ContainerType<ValueType, std::allocator<ValueType> >::value_type
				 tmp_2(cont.back());
   cont.pop_back();
   std::cout << tmp_1 << std::endl;
}
//	----------------------------------------------------------------------------
void RunTest()
{
	{
		std::vector<int> int_vec(10, 42);
		DoTest(int_vec);
	}
	{
		std::vector<int> int_vec(10, 42);
		DoTest_2(int_vec);
	}
	{
		std::vector<int> int_vec(10, 42);
		DoTest_3(int_vec);
	}
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_TemplateTemplateParameters

namespace TEST_MyAlignedAllocator {

//	////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t AlignSize = 64> class MyAlignmentAllocator {
public:
	typedef T                 value_type;
	typedef value_type       *pointer;
	typedef const value_type *const_pointer;
	typedef value_type       &reference;
	typedef const value_type &const_reference;
	typedef std::size_t       size_type;
	typedef std::ptrdiff_t    difference_type;

	pointer       address(reference x)       const { return &x; }
	const_pointer address(const_reference x) const { return &x; }

	MyAlignmentAllocator()
	{
	}

	MyAlignmentAllocator(const MyAlignmentAllocator &)
	{
	}

	~MyAlignmentAllocator()
	{
	}

	pointer allocate(size_type n, const_pointer = 0)
	{
#ifdef _MSC_VER
		void *ptr = ::_aligned_malloc(n * sizeof(T), AlignSize);
#else
		void *ptr = ::posix_memalign(n * sizeof(T), AlignSize);
#endif // #ifdef _MSC_VER

		if (!ptr)
			throw std::bad_alloc();

		return(static_cast<pointer>(ptr));
	}

	void deallocate(pointer ptr, size_type)
	{
#ifdef _MSC_VER
		::_aligned_free(ptr);
#else
		std::free(ptr);
#endif // #ifdef _MSC_VER
	}

	size_type max_size() const
	{
		return(std::numeric_limits<size_type>::max() / sizeof(value_type));
	}

	void construct(pointer ptr, const value_type &x)
	{
		new (ptr) value_type(x); 
	}

	void destroy(pointer ptr)
	{
		ptr->~value_type();
#if defined(_MSC_VER) && (_MSC_VER <= 1500)
		/*
			MSVC 2008 reports C4100 ("unreferenced formal parameter") for ptr
			and disabling the warning with #pragma desn't work... One hopes
			assignment to the parameter as the last state will be elided...
		*/
		ptr = NULL;
#endif // #if defined(_MSC_VER) && (_MSC_VER <= 1500)
	}

	template <class U> MyAlignmentAllocator(const MyAlignmentAllocator<U> &)
	{
	}

	/*
		Use typename MyAlignmentAllocator::template rebind<U>::other in
		order to refer to the dependent type.
	*/
	template <class U> struct rebind
	{
		typedef MyAlignmentAllocator<U> other;
	};

private:
	void operator = (const MyAlignmentAllocator &);	//	Not implemented.
};
//	----------------------------------------------------------------------------
template <class T> inline bool operator==(const MyAlignmentAllocator<T> &, 
	const MyAlignmentAllocator<T> &)
{
	return(true);
}
//	----------------------------------------------------------------------------
template <class T> inline bool operator!=(const MyAlignmentAllocator<T> &, 
	const MyAlignmentAllocator<T> &)
{
	return(false);
}
//	----------------------------------------------------------------------------
struct SomeClass {
	SomeClass(int value)
		:value_(value)
	{
	}

	int value_;
};
//	----------------------------------------------------------------------------
void RunTest()
{
	std::cout << "Test of MyAlignmentAllocator<>:" << std::endl;

	{
		std::vector<int, MyAlignmentAllocator<int> > aligned_vector(1000);
		std::cout << "   std::vector<int, MyAlignmentAllocator<...> >  = " <<
			aligned_vector.size() << std::endl << std::endl;
	}

	{
		std::vector<SomeClass, MyAlignmentAllocator<SomeClass> > aligned_vector;
		aligned_vector.push_back(SomeClass(1));
		aligned_vector.push_back(SomeClass(2));
		aligned_vector.push_back(SomeClass(3));
		std::cout << "   std::vector<Some, MyAlignmentAllocator<...> > = " <<
			aligned_vector.size() << std::endl << std::endl;
	}
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_MyAlignedAllocator

namespace TEST_DelegateToSiblingClass {

//	////////////////////////////////////////////////////////////////////////////
class Base {
public:
	Base()
	{
		std::cout << GetMyName() << "::" << GetMyName() << std::endl;
	}

	static const char *GetMyName() { return("Base"); }

	virtual void foo() = 0;
	virtual void bar() = 0;
};
//	----------------------------------------------------------------------------
class Der1 : public virtual Base {
public:
	Der1()
	{
		std::cout << GetMyName() << "::" << GetMyName() << std::endl;
	}

	static const char *GetMyName() { return("Der1"); }

	virtual void foo()
	{
		std::cout << "Der1::foo()" << " ---> ";

		bar();
	}
};
//	----------------------------------------------------------------------------
class Der2 : public virtual Base {
public:
	Der2()
	{
		std::cout << GetMyName() << "::" << GetMyName() << std::endl;
	}

	static const char *GetMyName() { return("Der2"); }

	virtual void bar()
	{
		std::cout << "Der2::bar()" << std::endl;
	}
};
//	----------------------------------------------------------------------------
#if defined(_MSC_VER) && (_MSC_VER <= 1500)
# pragma warning(push)
# pragma warning(disable: 4250)
#endif // #if defined(_MSC_VER) && (_MSC_VER <= 1500)
class Join : public Der1, public Der2 {
public:
	Join()
	{
		std::cout << GetMyName() << "::" << GetMyName() << std::endl;
	}

	static const char *GetMyName() { return("Join"); }
};
#if defined(_MSC_VER) && (_MSC_VER <= 1500)
# pragma warning(pop)
#endif // #if defined(_MSC_VER) && (_MSC_VER <= 1500)
//	----------------------------------------------------------------------------
void RunTest()
{
   boost::shared_ptr<Join> p1(new Join());
   boost::shared_ptr<Der1> p2 = p1;
   boost::shared_ptr<Base> p3 = p1;
 
	std::cout << std::endl;
 
	std::cout << Join::GetMyName() << "::foo() = ";
   p1->foo();

	std::cout << Der1::GetMyName() << "::foo() = ";
   p2->foo();

	std::cout << Base::GetMyName() << "::foo() = ";
   p3->foo();
}
//	////////////////////////////////////////////////////////////////////////////

} // TEST_DelegateToSiblingClass

namespace TEST_UnableToCompletelyFixHidingProblem {

//	////////////////////////////////////////////////////////////////////////////
struct Base {
	virtual double Price_1(double /* coupon */) const
	{
		return(666.0);
	}
	virtual double Price_2(double /* coupon */) const
	{
		return(1666.0);
	}
};
//	----------------------------------------------------------------------------
struct Intermediate : public Base {
	virtual double Price_1(double /* coupon */) const
	{
		return(66.0);
	}
	virtual double Price_2(double /* coupon */) const
	{
		return(1066.0);
	}
};
//	----------------------------------------------------------------------------
struct Derived : public Intermediate {

	using Base::Price_1;
	using Base::Price_2;

	//	Virtual function hides base class virtual function
	virtual double Price_1(int /* coupon */) const
	{
		return(42.0);
	}

	//	Non-virtual function hides base class virtual function
	        double Price_2(int /* coupon */) const
	{
		return(1042.0);
	}
};
//	----------------------------------------------------------------------------
void RunTest()
{
	boost::shared_ptr<Derived> derv_sp(new Derived);

	std::cout << "A:Price_1() = " << derv_sp->Price_1(   1) << std::endl;
	std::cout << "A:Price_2() = " << derv_sp->Price_2(   1) << std::endl;
	std::cout << "A:Price_1() = " << derv_sp->Price_1(1.23) << std::endl;
	std::cout << "A:Price_2() = " << derv_sp->Price_2(1.23) << std::endl;

	std::cout << "B:Price_1() = " << derv_sp->Base::Price_1(   1) << std::endl;
	std::cout << "B:Price_2() = " << derv_sp->Base::Price_2(   1) << std::endl;
	std::cout << "B:Price_1() = " << derv_sp->Base::Price_1(1.23) << std::endl;
	std::cout << "B:Price_2() = " << derv_sp->Base::Price_2(1.23) << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_UnableToCompletelyFixHidingProblem

namespace TEST_ZeroStorageBitCount {

//	////////////////////////////////////////////////////////////////////////////
size_t Masks[] = {
	0x55555555,
	0x33333333,
	0x0F0F0F0F,
	0x00FF00FF,
	0x0000FFFF
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void DumpMasks()
{
	for (size_t count_1 = 0, shift = 1; count_1 < 5; ++count_1, shift *= 2)
		std::cout << count_1 << ": 0x" << std::hex << std::setfill('0') <<
			std::setw(8) << Masks[count_1] << " = " << std::setw(32) <<
			std::bitset<sizeof(Masks[0]) * CHAR_BIT>(Masks[count_1]) <<
			std::dec << std::setfill(' ') << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
size_t LoopTest(size_t value)
{
	std::cout << "    LoopTest(" << value << ") = ";

	for (size_t count_1 = 0, shift = 1; count_1 < 5; ++count_1, shift *= 2)
		value = (value & Masks[count_1]) + ((value >> shift) & Masks[count_1]);

	std::cout << value << std::endl;

	return(value);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
size_t UnrolledTest(size_t value)
{
	std::cout << "UnrolledTest(" << value << ") = ";

	value = (value & 0x55555555) + ((value >>  1) & 0x55555555);
	value = (value & 0x33333333) + ((value >>  2) & 0x33333333);
	value = (value & 0x0F0F0F0F) + ((value >>  4) & 0x0F0F0F0F);
	value = (value & 0x00FF00FF) + ((value >>  8) & 0x00FF00FF);
	value = (value & 0x0000FFFF) + ((value >> 16) & 0x0000FFFF);

	std::cout << value << std::endl;

	return(value);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	EmitSep('=');
	DumpMasks();
	EmitSep('-');
	LoopTest(42);
	EmitSep('-');
	UnrolledTest(42);
	EmitSep('=');
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_ZeroStorageBitCount

namespace TEST_ThrowFromDtor {

//	////////////////////////////////////////////////////////////////////////////
struct DtorThrows {
	~DtorThrows()
	{
		throw std::exception();
	}
};
//	----------------------------------------------------------------------------
void RunTest()
{
	try {
		DtorThrows tmp;
	}
	catch (const std::exception &except) {
		std::cout << "TEST_ThrowFromDtor::RunTest(): Destructor exception "
			"caught: " << except.what() << std::endl;
	}
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_ThrowFromDtor

namespace TEST_NewInitialization {

//	////////////////////////////////////////////////////////////////////////////
//	Test leaks...
void RunTest()
{
	int    *i_ptr = new int(42);
	double *d_ptr = new double(42.12345);

	ValueIgnore(i_ptr);
	ValueIgnore(d_ptr);
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_NewInitialization

namespace TEST_ConversionOperator {

//	////////////////////////////////////////////////////////////////////////////
struct MyString {
	explicit MyString(const char *src = NULL)
		:datum_((src == NULL) ? "" : src)
	{
		if (src != NULL)
			datum_.assign(src);
	}

	operator const char * () const
	{
		return(datum_.c_str());
	}

private:
	std::string datum_;
};
//	----------------------------------------------------------------------------
void RunTest()
{
	MyString src("Some string.");
	char     dst[127 + 1];

	::printf("[%s]\n", ::strcpy(dst, src));
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_ConversionOperator

namespace TEST_CrossCast_1 {

//	////////////////////////////////////////////////////////////////////////////
struct ClassA {
	virtual ~ClassA()
	{
	}

	std::string GetName() const
	{
		return("ClassA");
	}
};
//	----------------------------------------------------------------------------
struct ClassB {
	virtual ~ClassB()
	{
	}

	std::string GetName() const
	{
		return("ClassB");
	}
};
//	----------------------------------------------------------------------------
struct ClassC : ClassA, ClassB {
	virtual ~ClassC()
	{
	}

	std::string GetName() const
	{
		return("ClassC");
	}
};
//	----------------------------------------------------------------------------
//	Test leaks...
void RunTest()
{
	ClassA *a_ptr = new ClassC;
	ClassB *b_ptr = dynamic_cast<ClassB *>(a_ptr);

	std::cout << "a_ptr->GetName() = " << a_ptr->GetName() << " ---> " <<
		typeid(*a_ptr).name() << std::endl;
	std::cout << "b_ptr->GetName() = " << b_ptr->GetName() << " ---> " <<
		typeid(*b_ptr).name() << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_CrossCast_1

namespace TEST_CtorOrder {

//	////////////////////////////////////////////////////////////////////////////
#define MISC_TEST_CTOR_METHOD(in_name)				\
     in_name () { std::cout << "   CTOR " << #in_name << std::endl; }
#define MISC_TEST_DTOR_METHOD(in_name)				\
	virtual ~ in_name () { std::cout << "   DTOR " << #in_name << std::endl; }
#define MISC_TEST_CTOR_ORDER(in_name, in_bases)	\
	class in_name : in_bases {							\
	public:													\
		MISC_TEST_CTOR_METHOD(in_name)				\
		MISC_TEST_DTOR_METHOD(in_name)				\
	};
class Base {
public:
	MISC_TEST_CTOR_METHOD(Base)
	MISC_TEST_DTOR_METHOD(Base)
};
#define MISC_TEST_CTOR_VIRTUAL	virtual
#undef  MISC_TEST_CTOR_VIRTUAL
#define MISC_TEST_CTOR_VIRTUAL
//	----------------------------------------------------------------------------
MISC_TEST_CTOR_ORDER(Derv_1A, public MISC_TEST_CTOR_VIRTUAL Base)
MISC_TEST_CTOR_ORDER(Derv_1B, public MISC_TEST_CTOR_VIRTUAL Base)
MISC_TEST_CTOR_ORDER(Derv_1C, public MISC_TEST_CTOR_VIRTUAL Base)

//#define MISC_TEST_CTOR_VIRTUAL
MISC_TEST_CTOR_ORDER(Derv_1A2A, public MISC_TEST_CTOR_VIRTUAL Derv_1A)
MISC_TEST_CTOR_ORDER(Derv_1A2B, public MISC_TEST_CTOR_VIRTUAL Derv_1A)

MISC_TEST_CTOR_ORDER(Derv_1B2A, public Derv_1B)

MISC_TEST_CTOR_ORDER(Derv_1C2B, public Derv_1C)

class MyFinal : public Derv_1A2A, public Derv_1A2B,
	public Derv_1B2A, public Derv_1C2B {
public:
	MISC_TEST_CTOR_METHOD(MyFinal)
	MISC_TEST_DTOR_METHOD(MyFinal)
};
//	----------------------------------------------------------------------------
#undef MISC_TEST_CTOR_METHOD
#undef MISC_TEST_DTOR_METHOD
#undef MISC_TEST_CTOR_ORDER
#undef MISC_TEST_CTOR_VIRTUAL
//	----------------------------------------------------------------------------
void RunTest()
{
	std::cout << "TEST_CtorOrder: Start" << std::endl;

	{
		MyFinal  my_final;
//		Base    *my_base_ptr  = &my_final;
		Base    *my_base_ptr  = static_cast<Derv_1A2A *>(&my_final);
//		MyFinal *my_final_ptr = my_base_ptr;
		MyFinal *my_final_ptr = dynamic_cast<MyFinal *>(my_base_ptr);
		const std::type_info &my_type(typeid(my_final));
		ValueIgnore(my_final);
		ValueIgnore(my_final_ptr);
		ValueIgnore(my_type);
	}

	std::cout << "TEST_CtorOrder: End" << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_CtorOrder

namespace TEST_FloatingPointNotAssociative {

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	boost::io::ios_all_saver ios_saver(std::cout);

	double value_1 = 1.0;
	double value_2 = 1.0;
	double value_3 = 1.0;

	for (unsigned int count_1 = 0; count_1 < 100; ++count_1) {
		double result_1 = value_1 + value_2 + value_3;
		double result_2 = value_3 + value_1 + value_2;
		if (result_1 != result_2) {
			std::cout << "FP Assoc Test: " << count_1 << ":" << value_1 <<
				" + " << value_2 << " + " << value_3 << " = " << result_1 <<
				std::endl;
			std::cout << "FP Assoc Test: " << count_1 << ":" << value_3 <<
				" + " << value_2 << " + " << value_1 << " = " << result_2 <<
				std::endl;
			std::cout << "FP Assoc Test: NOT ASSOCIATIVE (difference = " <<
				std::scientific << (result_1 - result_2) << ")" << std::endl;
			return;
		}
		if (value_1 < value_2)
			value_1 += std::numeric_limits<double>::epsilon();
		else {
			value_1 = 1.0;
			if (value_2 < value_3)
				value_2 += std::numeric_limits<double>::epsilon();
			else {
				value_2  = 1.0;
				value_3 += std::numeric_limits<double>::epsilon();
			}
		}
	}

	std::cout << "FP Assoc Test: ASSOCIATIVE" << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_FloatingPointNotAssociative

namespace TEST_FibonacciSequence {

//	////////////////////////////////////////////////////////////////////////////
/*
	Scheme:
	-------

		(define (fib n) (if (< n 1) 0 (if (< n 2) 1 (+ (fib (- n 1)) (fib (- n 2))))))

		(define (fib n)
			(if (< n 1)
				0
				(if (< n 2)
					1
					(+ (fib (- n 1))
						(fib (- n 2)))
				)
			)
		)

	Or:

		(define (fib n) (cond ((< n 1) 0) ((< n 2) 1) (else (+ (fib (- n 1)) (fib (- n 2))))))

		(define (fib n)
			(cond
				((< n 1) 0)
				((< n 2) 1)
				(else
					(+ (fib (- n 1))
						(fib (- n 2)))
				)
			)
		)
*/
//	----------------------------------------------------------------------------
template <unsigned int number> struct FibClass {
	static const unsigned int value = FibClass<number - 1>::value +
		FibClass<number - 2>::value;
};
template <> struct FibClass<0> {
	static const unsigned int value = 0;
};
template <> struct FibClass<1> {
	static const unsigned int value = 1;
};
//	----------------------------------------------------------------------------
unsigned int FibIterative(unsigned int number)
{
	if (number < 1)
		return(0);

	if (number > 1) {
		unsigned int prev_fib = 1;
		unsigned int curr_fib = 1;
		for (unsigned int count_1 = 2; count_1 < number; ++count_1) {
			unsigned int tmp_fib = curr_fib;
			curr_fib += prev_fib;
			prev_fib  = tmp_fib;
		}
		return(curr_fib);
	}

	return(1);
}
//	----------------------------------------------------------------------------
unsigned int FibRecursive(unsigned int number)
{
	unsigned int return_value = 0;

	if (number < 1)
		return_value = 0;
	else if (number == 1)
		return_value = 1;
	else if (number > 1)
		return_value = FibRecursive(number - 1) + FibRecursive(number - 2);

	std::cout << "FibRecursive(" << std::setw(3) << number << ") = " <<
		std::setw(10) << return_value << std::endl;

	return(return_value);
}
//	----------------------------------------------------------------------------
void RunTest()
{
	static const unsigned int number = 10;

	std::cout << "FibTemplate( " << std::setw(3) << number << ") = " <<
		std::setw(10) << FibClass<number>::value << std::endl;
	std::cout << "FibIterative(" << std::setw(3) << number << ") = " <<
		std::setw(10) << FibIterative(number) << std::endl;
	std::cout << "FibRecursive(" << std::setw(3) << number << ") = " <<
		std::setw(10) << FibRecursive(number) << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_FibonacciSequence

namespace TEST_RewrittenFactorialCode {

/*
	From Joaquin M Lopez Munoz's blog post at:
		http://bannalia.blogspot.co.uk/2008_10_01_archive.html .
*/

//	////////////////////////////////////////////////////////////////////////////
int MyFactorial_1(int number)
{
	std::cout << " >>> A:MyFactorial_1(" << std::setw(2) << number << ") = " <<
		std::endl;

	int final_result;

	if (number > 0)
		final_result = number * MyFactorial_1(number - 1);
	else
		final_result = 1;

	std::cout << " >>> B:MyFactorial_1(" << std::setw(2) << number << ") = ";
	std::cout << std::setw(20) << final_result << std::endl;

	return(final_result);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
int MyFactorial_2(int number, int result = 1)
{
	std::cout << " >>> A:MyFactorial_2(" << std::setw(2) << number << ", " <<
		std::setw(20) << result << ") = " << std::endl;

	int final_result;

	if (number > 0)
		final_result = MyFactorial_2(number - 1, number * result);
	else
		final_result = result;

	std::cout << " >>> B:MyFactorial_2(" << std::setw(2) << number << ", " <<
		std::setw(20) << result << ") = ";
	std::cout << std::setw(20) << final_result << std::endl;

	return(final_result);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	std::cout << "MyFactorial_1(10) = " << MyFactorial_1(10) << std::endl;
	std::cout << "MyFactorial_2(10) = " << MyFactorial_2(10) << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_RewrittenFactorialCode

namespace TEST_StupidClojureRecursionTest {

//	////////////////////////////////////////////////////////////////////////////
boost::shared_ptr<std::vector<double> > loop(
	boost::shared_ptr<std::vector<double> > tmp, int i) {
  if (i<=0) {
    return tmp;
  } else {
    boost::shared_ptr<std::vector<double> > tmp1(new std::vector<double>(*tmp));
    boost::shared_ptr<std::vector<double> > tmp2 = loop(tmp1, i-1);
    ++(*tmp2)[0];
    return tmp2;
  }
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	boost::shared_ptr<std::vector<double> > tmp_1(new std::vector<double>(100000));

	tmp_1->push_back(42);

	boost::shared_ptr<std::vector<double> > tmp_2(loop(tmp_1, 1000));

	std::cout << (*tmp_1)[0] << std::endl;
	std::cout << (*tmp_2)[0] << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_StupidClojureRecursionTest

namespace TEST_AccessDeclaration {

//#define TEST_AccessDeclaration_DEPRECATED	1

//	////////////////////////////////////////////////////////////////////////////
class Base {
protected:
	Base()
		:public_int_(42)
		,protected_int_(666)
	{
	}

public:
	int public_int_;
protected:
	int protected_int_;
};
//	----------------------------------------------------------------------------
class Derv : private Base {
public:
#ifdef TEST_AccessDeclaration_DEPRECATED
	Base::public_int_;
#else 
	using Base::public_int_;
#endif // #ifdef TEST_AccessDeclaration_DEPRECATED

protected:
#ifdef TEST_AccessDeclaration_DEPRECATED
	Base::protected_int_;
#else
	using Base::protected_int_;
#endif // #ifdef TEST_AccessDeclaration_DEPRECATED

public:
	int GetProtected() const
	{
		return(protected_int_);
	}
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	Derv derv;

	std::cout << "Base::public_int_   : " << derv.public_int_    << std::endl;
	std::cout << "Base::protected_int_: " << derv.GetProtected() << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_AccessDeclaration

namespace TEST_PartialInstantiation {

//	////////////////////////////////////////////////////////////////////////////
template <typename T> struct Test_PI_1 {
	static const char *GetMyName()
	{
		return("Test_PI_1<T>");
	}
};
template <typename T> struct Test_PI_1<T *> {
	static const char *GetMyName()
	{
		return("Test_PI_1<T *>");
	}
};
//	----------------------------------------------------------------------------
template <typename T, typename U> struct Test_PI_2 {
	static const char *GetMyName()
	{
		return("Test_PI_2<T, U>");
	}
};
template <typename U> struct Test_PI_2<int, U> {
	static const char *GetMyName()
	{
		return("Test_PI_2<int, U>");
	}
};
template <typename T> struct Test_PI_2<T, int> {
	static const char *GetMyName()
	{
		return("Test_PI_2<T, int>");
	}
};


template <typename T> struct Test_PI_2<T *, int> {
	static const char *GetMyName()
	{
		return("Test_PI_2<T *, int>");
	}
};


template <> struct Test_PI_2<int, int> {
	static const char *GetMyName()
	{
		return("Test_PI_2<int, int>");
	}
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename TestType>
	void RunTestHelper()
{
	std::cout << TestType::GetMyName() << std::endl;
}
template <typename TestType>
	void RunTestHelper(const std::string &in_name)
{
	std::string            tmp_name(in_name);

/*
	std::string::size_type tmp_name_length = tmp_name.size();

	while ((!tmp_name.empty()) && ((tmp_name[0] == ' ') || (tmp_name[0] == '(')))
		--tmp_name_length;

	if (tmp_name_length < tmp_name.size())
		tmp_name = tmp_name.substr(tmp_name.size() - tmp_name_length);

	while ((!tmp_name.empty()) && ((tmp_name[tmp_name.size() - 1] == ' ') ||
		(tmp_name[tmp_name.size() - 1] == ')')))
		--tmp_name_length;

	if (tmp_name_length < tmp_name.size())
		tmp_name = tmp_name.substr(0, tmp_name_length);
*/
/*
	while ((!tmp_name.empty()) &&
		((tmp_name[0] == ' ') || (tmp_name[0] == '(')))
		tmp_name = tmp_name.substr(1);

	while ((!tmp_name.empty()) && ((tmp_name[tmp_name.size() - 1] == ' ') ||
		(tmp_name[tmp_name.size() - 1] == ')')))
		tmp_name = tmp_name.substr(0, tmp_name.size() - 1);
*/
	std::string::iterator iter;

	while ((!tmp_name.empty()) && ((*(iter = tmp_name.begin()) == ' ') ||
		(*iter == '(')))
		tmp_name = tmp_name.substr(1);

	while ((!tmp_name.empty()) && (((iter = tmp_name.end())[-1] == ' ') ||
		(iter[-1] == ')')))
		tmp_name = tmp_name.substr(0, tmp_name.size() - 1);

	std::cout << "Instantiation " << std::left << std::setw(22) <<
		tmp_name << " from specialization " << TestType::GetMyName() <<
		std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
/*
#define RunTestMacro(instantiation)										\
	std::cout << "Instantiation '" << std::left << std::setw(24) <<	\
		#instantiation << "' uses specialization '" <<				\
		instantiation::GetMyName() << std::endl;
*/
//		TestHelper(instantiation) << std::endl;
template<typename T> struct argument_type;
template<typename T, typename U> struct argument_type<T(U)> { typedef U type; };
#define RunTestMacro(instantiation)										\
	RunTestHelper< argument_type<void(instantiation)>::type >( #instantiation );
//	----------------------------------------------------------------------------
void RunTest()
{
/*
	RunTestHelper<Test_PI_1<char> >();
	RunTestHelper<Test_PI_1<char *> >();

	RunTestHelper<Test_PI_2<char, char> >();
	RunTestHelper<Test_PI_2<int, char> >();
	RunTestHelper<Test_PI_2<char, int> >();
	RunTestHelper<Test_PI_2<char *, int> >();
	RunTestHelper<Test_PI_2<int *, int> >();
	RunTestHelper<Test_PI_2<int, int> >();
*/
/*
	RunTestMacro((Test_PI_1<char>));
	RunTestMacro((Test_PI_1<char *>));
*/
	RunTestMacro((Test_PI_1<char>));
	RunTestMacro((Test_PI_1<char *>));

	RunTestMacro((Test_PI_2<char, char>));
	RunTestMacro((Test_PI_2<int, char>));
	RunTestMacro((Test_PI_2<char, int>));
	RunTestMacro((Test_PI_2<char *, int>));
	RunTestMacro((Test_PI_2<int *, int>));
	RunTestMacro((Test_PI_2<int, int>));
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_PartialInstantiation

namespace TEST_TemplateAsFriend {

//	////////////////////////////////////////////////////////////////////////////
class MyTest;
//	----------------------------------------------------------------------------
void EmitNotFriend(const char *name_1, const char *name_2)
{
	std::cout << "NOT MyTest Class Friend: " << name_1 <<
		"<" << name_2 << ">" << std::endl;
}
//	----------------------------------------------------------------------------
template <typename T> void FriendFunc_1(const char *name_string)
{
	MyTest::EmitFriend("FriendFunc_1", name_string);
}
template <typename T> void FriendFunc_2(const char *name_string)
{
	MyTest::EmitFriend("FriendFunc_2", name_string);
}
template <> void FriendFunc_2<char>(const char *name_string)
{
	EmitNotFriend("FriendFunc_2", name_string);
}
template <> void FriendFunc_2<double>(const char *name_string)
{
	EmitNotFriend("FriendFunc_2", name_string);
}
//	----------------------------------------------------------------------------
template <typename T> class FriendClass_1 {
public:
	FriendClass_1(const char *name_string)
	{
		MyTest::EmitFriend("FriendClass_1", name_string);
	}
};
template <typename T> class FriendClass_2 {
public:
	FriendClass_2(const char *name_string)
	{
		MyTest::EmitFriend("FriendClass_2", name_string);
	}
};
template <> class FriendClass_2<char> {
public:
	FriendClass_2(const char *name_string)
	{
		EmitNotFriend("FriendClass_2", name_string);
	}
};
template <> class FriendClass_2<double> {
public:
	FriendClass_2(const char *name_string)
	{
		EmitNotFriend("FriendClass_2", name_string);
	}
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
class MyTest {
	template <typename> friend void FriendFunc_1(const char *name_string);
	                    friend void FriendFunc_2<int>(const char *name_string);

	template <typename> friend class FriendClass_1;
	                    friend class FriendClass_2<int>;

	static void EmitFriend(const char *name_1, const char *name_2)
	{
		std::cout << "    MyTest Class Friend: " << name_1 <<
			"<" << name_2 << ">" << std::endl;
	}
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	FriendFunc_1<char>("char");
	FriendFunc_1<int>("int");
	FriendFunc_1<double>("double");

	FriendFunc_2<char>("char");
	FriendFunc_2<int>("int");
	FriendFunc_2<double>("double");

	FriendClass_1<char>("char");
	FriendClass_1<int>("int");
	FriendClass_1<double>("double");

	FriendClass_2<char>("char");
	FriendClass_2<int>("int");
	FriendClass_2<double>("double");
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_TemplateAsFriend

namespace TEST_DerivedOverrideAccessChange {

//	////////////////////////////////////////////////////////////////////////////
class Base {
public:
	virtual void EmitInstanceName() const
	{
		std::cout << "Instance Name: Base" << std::endl;
	}

	static void EmitClassName()
	{
		std::cout << "Class Name   : Base" << std::endl;
	}
};
//	----------------------------------------------------------------------------
class Derv : public Base {
	friend void DervFuncsVisibleYes();

	virtual void EmitInstanceName() const
	{
		std::cout << "Instance Name: Derv" << std::endl;
	}

	static void EmitClassName()
	{
		std::cout << "Class Name   : Derv" << std::endl;
	}
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void DervFuncsVisibleNo()
{
	boost::shared_ptr<Base> tmp_sptr(new Derv);

	tmp_sptr->EmitInstanceName();
	tmp_sptr->EmitClassName();
}
//	----------------------------------------------------------------------------
void DervFuncsVisibleYes()
{
	boost::shared_ptr<Base> tmp_sptr(new Derv);

	tmp_sptr->EmitInstanceName();
	tmp_sptr->EmitClassName();
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	DervFuncsVisibleNo();
	DervFuncsVisibleYes();
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_DerivedOverrideAccessChange

namespace TEST_PointerVersusArray_1 {

//	////////////////////////////////////////////////////////////////////////////
void XXX(char **&ref_to_char_pp)
{
//	char **char_pp = new char * [100];

	ref_to_char_pp = new char * [100];

	ref_to_char_pp[0] = "Hello, world!";
	ref_to_char_pp[1] = "The quick brown fox et cetera...";
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void YYY(int *in_ptr, int &in_ref)
{
	std::cout << *in_ptr << std::endl;
	std::cout <<  in_ref << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void ZZZ()
{
	char **char_pp = NULL;

	XXX(char_pp);

	int  datum = 42;
	int *ptr   = &datum;
	int &ref   = datum;

	YYY(ptr, ref);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	int   ia[16] = { 0, 1, 2, 42, 4 };
	int  *ip  = ia;
	int **ipp = &ip;

/*
	But the following statement doesn't work they way one might intuit:

		int **iap = &ia;

	This is because the address of ia is itself the array address. That is:

		ia == &ia

*/
	int (*iap)[16] = &ia;

/*
char  ca_1[]  = "123";
char  ca_2[9] = "123";
char *cp = "123";
*/

	std::cout << "ia[3]    : " << ia[3]     << std::endl;
	std::cout << "3[ia]    : " << 3[ia]     << std::endl;
	std::cout << "ip[3]    : " << ip[3]     << std::endl;
	std::cout << "3[ip]    : " << 3[ip]     << std::endl;
	std::cout << "(*ipp)[3]: " << (*ipp)[3] << std::endl;
	std::cout << "3[*ipp]  : " << 3[*ipp]   << std::endl;
	std::cout << "(*iap)[3]: " << (*iap)[3] << std::endl;
	std::cout << "3[*iap]  : " << 3[*iap]   << std::endl;

	ZZZ();
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_PointerVersusArray_1

namespace TEST_PointerVersusArray_2 {

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	int  ia[5] = { 1, 2, 3, 4, 5 };

	/*
		Used reinterpret_cast<> to convert from pointer-to-array-of-int to
		pointer-to-int.
	*/
	int *ip    = reinterpret_cast<int *>(&ia + 1);

	/*
		Output is: [*(ia + 1)=2][*(ip - 1)=5]

		Because &ia + 1 steps to &ia + (sizeof(int) * 5)
	*/
	std::cout << "[*(ia + 1)=" << (*(ia + 1)) << "]";
	std::cout << "[*(ip - 1)=" << (*(ip - 1)) << "]" << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_PointerVersusArray_2

namespace TEST_PointerVersusArray_3 {

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	char *cp;
	char  ca[10] = { 1, 2, 3, 4, 5, 6, 9, 8 };

	/*
		Used reinterpret_cast<> to convert from integral type to pointer type.
	*/
	cp = reinterpret_cast<char *>((ca + 1)[5]);

	/*
		ca				points to first element '1'.
		ca + 1		points to the second element '2'.
		(ca + 1)[5] resolves to the fifth element after the second element '9'.
	*/
	std::cout << "cp         : " << static_cast<void *>(cp)       << std::endl;
	std::cout << "(ca + 1)[5]: " << static_cast<int>((ca + 1)[5]) << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_PointerVersusArray_3

namespace TEST_ReverseLinkedList_Single {

#define REVERSE_LIST_DEBUG_OUTPUT

//	////////////////////////////////////////////////////////////////////////////
struct ListNode {
	explicit ListNode(int value = 0, ListNode *next_ptr = NULL)
		:value_(value)
		,next_ptr_(next_ptr)
	{
	}

	int       value_;
	ListNode *next_ptr_;

	static void GetTestList(std::vector<ListNode> &out_list)
	{
		std::vector<ListNode> tmp_list(3);

		tmp_list[0] = ListNode(1, &(tmp_list[1]));
		tmp_list[1] = ListNode(2, &(tmp_list[2]));
		tmp_list[2] = ListNode(3, NULL);

		out_list.swap(tmp_list);
	}
};
/*
//	----------------------------------------------------------------------------
ListNode Node_3 = { 3, NULL };
ListNode Node_3 = { 2, NULL };
ListNode Node_3 = { 1, NULL };
*/
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void EmitNodePtrList(const std::vector<ListNode *> &node_ptr_list)
{
	std::cout << "-----------------------------------------------" << std::endl;

	for (size_t count_1 = 0; count_1 < node_ptr_list.size(); ++count_1) {
		std::cout << count_1 << ":" << node_ptr_list[count_1]->value_ << " " <<
			std::hex << std::setfill('0') <<
			"0x" << std::setw(8) << node_ptr_list[count_1] << " ---> " <<
			"0x" << std::setw(8) << node_ptr_list[count_1]->next_ptr_ <<
			std::dec << std::setfill(' ') << std::endl;
	}

	std::cout << "-----------------------------------------------" << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
ListNode *MyReverseListInPlace(ListNode *first_node_ptr)
{
	if ((!first_node_ptr) || (!first_node_ptr->next_ptr_))
		return(first_node_ptr);

	ListNode *tmp_prev_ptr = NULL;
	ListNode *tmp_next_ptr = NULL;

	while (first_node_ptr) {
		tmp_next_ptr = first_node_ptr;
		first_node_ptr = first_node_ptr->next_ptr_;
		tmp_next_ptr->next_ptr_ = tmp_prev_ptr;
		tmp_prev_ptr = tmp_next_ptr;
	}

	return(tmp_next_ptr);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
ListNode *ReverseList(ListNode *first_node_ptr)
{
	if (!first_node_ptr)
		return(first_node_ptr);

#ifdef REVERSE_LIST_DEBUG_OUTPUT
	std::vector<ListNode *> node_ptr_list;

	{
		ListNode *tmp_node_ptr = first_node_ptr;

		while (tmp_node_ptr) {
			node_ptr_list.push_back(tmp_node_ptr);
			tmp_node_ptr = tmp_node_ptr->next_ptr_;
		}
	}

	EmitNodePtrList(node_ptr_list);
#endif // #ifdef REVERSE_LIST_DEBUG_OUTPUT

	ListNode *tmp_next_ptr = NULL;
	ListNode *tmp_prev_ptr = NULL;

	while (first_node_ptr) {
		tmp_next_ptr            = first_node_ptr;
		first_node_ptr          = tmp_next_ptr->next_ptr_;
		tmp_next_ptr->next_ptr_ = tmp_prev_ptr;
		tmp_prev_ptr            = tmp_next_ptr;
#ifdef REVERSE_LIST_DEBUG_OUTPUT
		EmitNodePtrList(node_ptr_list);
#endif // #ifdef REVERSE_LIST_DEBUG_OUTPUT
	}

	return(tmp_next_ptr);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void EmitList(const ListNode *first_node_ptr)
{
	std::cout << "===============================================" << std::endl;

	int this_number = 1;

	while (first_node_ptr != NULL) {
		std::cout << this_number << ":" << first_node_ptr->value_ << std::endl;
		first_node_ptr = first_node_ptr->next_ptr_;
		++this_number;
	}

	std::cout << "===============================================" << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void EmitList_StdList(const std::list<int> &std_list)
{
	std::cout << "===============================================" << std::endl;

	std::copy(std_list.begin(), std_list.end(),
		std::ostream_iterator<int>(std::cout, "\n"));

	std::cout << "===============================================" << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void RunTest_StdList()
{
	std::list<int> std_list;

	std_list.push_back(1);
	std_list.push_back(2);
	std_list.push_back(3);

	EmitList_StdList(std_list);

	std::reverse(std_list.begin(), std_list.end());

	EmitList_StdList(std_list);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
ListNode *ReverseListInPlaceAnswer(ListNode *node_ptr)
{
	if ((!node_ptr) || (!node_ptr->next_ptr_))
		return(node_ptr);

	ListNode *tmp_next_ptr = NULL;
	ListNode *tmp_prev_ptr = NULL;

	while (node_ptr) {
		tmp_next_ptr            = node_ptr;
		node_ptr                = node_ptr->next_ptr_;
		tmp_next_ptr->next_ptr_ = tmp_prev_ptr;
		tmp_prev_ptr            = tmp_next_ptr;
	}

	return(tmp_next_ptr);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
ListNode *ReverseListInPlaceAnswer2(ListNode *node_ptr)
{
	if ((!node_ptr) || (!node_ptr->next_ptr_))
		return(node_ptr);

	ListNode *tmp_prev_ptr = NULL;
	ListNode *tmp_next_ptr = NULL;

	while (node_ptr) {
		tmp_next_ptr = node_ptr;
		node_ptr     = node_ptr->next_ptr_;
		tmp_next_ptr->next_ptr_ = tmp_prev_ptr;
		tmp_prev_ptr = tmp_next_ptr;
	}

	return(tmp_next_ptr);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
ListNode *ReverseListInPlacePrimitive(ListNode *node_ptr)
{
	if ((!node_ptr) || (!node_ptr->next_ptr_))
		return(node_ptr);

	ListNode *tmp_prev_ptr = NULL;
	ListNode *tmp_next_ptr = NULL;

	while (node_ptr) {
		tmp_next_ptr = node_ptr;
		node_ptr = node_ptr->next_ptr_;
		tmp_next_ptr->next_ptr_ = tmp_prev_ptr;
		tmp_prev_ptr = tmp_next_ptr;
	}

	return(tmp_next_ptr);
}
//	////////////////////////////////////////////////////////////////////////////


































//	////////////////////////////////////////////////////////////////////////////
ListNode *ReverseList_SelfTest(ListNode *node_ptr)
{
	if ((!node_ptr) || (!node_ptr->next_ptr_))
		return(node_ptr);

	ListNode *prev_ptr = NULL;

	while (node_ptr) {
		ListNode *next_ptr = node_ptr->next_ptr_;
		node_ptr->next_ptr_ = prev_ptr;
		prev_ptr            = node_ptr;
		node_ptr            = next_ptr;
	}

	return(prev_ptr);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	std::vector<ListNode> my_list;

	ListNode::GetTestList(my_list);

	ListNode *first_node_ptr = &(my_list[0]);

	EmitList(first_node_ptr);
	std::cout << std::endl;
//	first_node_ptr = ReverseList(first_node_ptr);
//	first_node_ptr = MyReverseListInPlace(first_node_ptr);
//first_node_ptr = ReverseListInPlaceAnswer2(first_node_ptr);
//first_node_ptr = ReverseListInPlacePrimitive(first_node_ptr);
first_node_ptr = ReverseList_SelfTest(first_node_ptr);
	EmitList(first_node_ptr);
}
//	////////////////////////////////////////////////////////////////////////////

} // TEST_ReverseLinkedList_Single


namespace TEST_ReverseLinkedList_Recursive {

//	////////////////////////////////////////////////////////////////////////////
struct ListNode {
	explicit ListNode(int value = 0, ListNode *next_ptr = NULL)
		:value_(value)
		,next_ptr_(next_ptr)
	{
	}

	int       value_;
	ListNode *next_ptr_;

	static ListNode *GetTestList(std::vector<ListNode> &out_list);

	static void EmitList(const ListNode *first_node_ptr,
		std::ostream &o_str = std::cout);
	static void EmitListReversedFunctional(const ListNode *first_node_ptr,
		std::ostream &o_str = std::cout);

private:
	static void EmitListReversedFunctionalImpl(const ListNode *first_node_ptr,
		int this_number, std::ostream &o_str);
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
ListNode *ListNode::GetTestList(std::vector<ListNode> &out_list)
{
	std::vector<ListNode> tmp_list(3);

	tmp_list[0] = ListNode(1, &(tmp_list[1]));
	tmp_list[1] = ListNode(2, &(tmp_list[2]));
	tmp_list[2] = ListNode(3, NULL);

	out_list.swap(tmp_list);

	return(&(out_list[0]));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void ListNode::EmitList(const ListNode *first_node_ptr, std::ostream &o_str)
{
	o_str << "===============================================" << std::endl;

	int this_number = 1;

	while (first_node_ptr != NULL) {
		o_str << this_number << ":" << first_node_ptr->value_ << std::endl;
		first_node_ptr = first_node_ptr->next_ptr_;
		++this_number;
	}

	o_str << "===============================================" << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void ListNode::EmitListReversedFunctional(const ListNode *first_node_ptr,
	std::ostream &o_str)
{
	o_str << "===============================================" << std::endl;

	EmitListReversedFunctionalImpl(first_node_ptr, 1, o_str);

	o_str << "===============================================" << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void ListNode::EmitListReversedFunctionalImpl(const ListNode *first_node_ptr,
	int this_number, std::ostream &o_str)
{
	if (first_node_ptr) {
		if (first_node_ptr->next_ptr_)
			EmitListReversedFunctionalImpl(first_node_ptr->next_ptr_,
				this_number + 1, o_str);
		o_str << this_number << ":" << first_node_ptr->value_ << std::endl;
	}
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
ListNode *MyReverseListInPlace(ListNode *first_node_ptr)
{
	if ((!first_node_ptr) || (!first_node_ptr->next_ptr_))
		return(first_node_ptr);

	ListNode *tmp_prev_ptr = NULL;
	ListNode *tmp_next_ptr = NULL;

	while (first_node_ptr) {
		tmp_next_ptr = first_node_ptr;
		first_node_ptr = first_node_ptr->next_ptr_;
		tmp_next_ptr->next_ptr_ = tmp_prev_ptr;
		tmp_prev_ptr = tmp_next_ptr;
	}

	return(tmp_next_ptr);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
ListNode *MyReverseListInPlaceRecursiveImpl(ListNode *this_node_ptr,
	ListNode *prev_node_ptr)
{
	ListNode *tmp_next_ptr = this_node_ptr->next_ptr_;

	this_node_ptr->next_ptr_ = prev_node_ptr;

	if (!tmp_next_ptr)
		return(this_node_ptr);

	return(MyReverseListInPlaceRecursiveImpl(tmp_next_ptr, this_node_ptr));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
ListNode *MyReverseListInPlaceRecursive(ListNode *first_node_ptr)
{
	if (!first_node_ptr)
		return(first_node_ptr);

	return(MyReverseListInPlaceRecursiveImpl(first_node_ptr, NULL));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	std::vector<ListNode>  my_list;
	ListNode              *first_node_ptr = ListNode::GetTestList(my_list);

	std::cout << "Emit Original List:" << std::endl;
	ListNode::EmitList(first_node_ptr, std::cout);
	std::cout << std::endl;

	std::cout << "Emit Original List in Reverse Order:" << std::endl;
	ListNode::EmitListReversedFunctional(first_node_ptr, std::cout);
	std::cout << std::endl;

	first_node_ptr = MyReverseListInPlace(first_node_ptr);
	std::cout << "Emit Reversed List:" << std::endl;
	ListNode::EmitList(first_node_ptr, std::cout);
	std::cout << std::endl;

	//	Re-reverse...
	first_node_ptr = MyReverseListInPlaceRecursive(first_node_ptr);
	std::cout << "Emit Re-Reversed List:" << std::endl;
	ListNode::EmitList(first_node_ptr, std::cout);
	std::cout << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // TEST_ReverseLinkedList_Recursive

namespace TEST_IntermediateBaseAccess {

//	////////////////////////////////////////////////////////////////////////////
struct Base_1 {
	virtual Base_1 *Clone() const
	{
		return(new Base_1(*this));
	}

	const char *GetName() const
	{
		return("Base_1");
	}
};
//	----------------------------------------------------------------------------
struct Base_2 : private Base_1 {

	friend struct Base_3;

	virtual Base_1 *Clone() const
	{
		return(new Base_2(*this));
	}

	const char *GetName() const
	{
		return("Base_2");
	}
};
//	----------------------------------------------------------------------------
struct Base_3 : Base_2 {
	virtual Base_2 *Clone() const
	{
		return(new Base_3(*this));
	}

	const char *GetName() const
	{
		return("Base_3");
	}
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	Base_3  tmp_base_3;
	Base_2 *base_3_ptr(tmp_base_3.Clone());

	std::cout << "NAME: " << base_3_ptr->GetName() << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_IntermediateBaseAccess

namespace TEST_ConstInstances {

//	////////////////////////////////////////////////////////////////////////////
struct MyClass {
	void Foo()
	{
	}

	void Bar() const
	{
	}
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void RunTest()
{
	const MyClass my_1;

	const_cast<MyClass &>(my_1).Foo();
	my_1.Bar();
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace TEST_ConstInstances

namespace TEST_IKM {

namespace PreprocessorTest {

const std::string MyNamespace = "TEST_IKM::PreprocessorTest";

//	////////////////////////////////////////////////////////////////////////////
/*
	ANSWER(S):

		1)	An error or warning will occur on Line 4 because language has already
			been defined.
*/
void RunTest()
{
	//	----------------------------------------------------------------------
	#define language 437            //Line 1 
	#if language < 400  
	# undef language                //Line 2 
	#else                           //Line 3 
	//# define language 850         //Line 4 
	# ifdef language                //Line 5    
		::printf("%d", language);    //Line 6 
	#  define XXX YYY
	# endif 
	#endif 
	//	----------------------------------------------------------------------

	::printf("\n");

	std::cout << "language = [" << language << "]" << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace PreprocessorTest

namespace ConstMutable {

//	////////////////////////////////////////////////////////////////////////////
class Object 
{
  public:
      Object() {}
      void Print() const
      {
			 using namespace std;
          cout << "const" << endl;
      }
      void Print()
      {
			 using namespace std;
          cout << "mutable" << endl;
      }
};
//	----------------------------------------------------------------------------
void print_obj(const Object& obj) 
{
      obj.Print(); 
}
//	----------------------------------------------------------------------------
int RunTest_main() 
{
      Object       obj1;
      const Object obj2;
      Object*const pobj1 = &obj1;

      print_obj(obj1);
      print_obj(obj2);

      obj1.Print();
      obj2.Print();

      pobj1->Print();

      return 0; 
}
//	----------------------------------------------------------------------------
/*
	ANSWER(S):

		1)	const const mutable const mutable
*/
void RunTest()
{
	RunTest_main();
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace ConstMutable

namespace ConstVolatile {

//	////////////////////////////////////////////////////////////////////////////
class TestPrint 
{
  public:
     void Print()
     {
          std::cout << "TestPrint" << std::endl;
     }
     void Print() const
     {
          std::cout << "const TestPrint" << std::endl;
     }
     void Print() volatile
     {
          std::cout << "volatile TestPrint" << std::endl;
     }
     void Print() const volatile
     {
          std::cout << "const volatile TestPrint" << std::endl;
     }
};
//	----------------------------------------------------------------------------
int RunTest_main(/* int argc, char* argv[] */) 
{
      TestPrint normal_test;
      normal_test.Print();

      const TestPrint const_test;
      const_test.Print();

      volatile TestPrint volatile_test;
      volatile_test.Print();

      const volatile TestPrint const_volatile_test;
      const_volatile_test.Print(); 

	return(0);
}
//	----------------------------------------------------------------------------
/*
	ANSWER(S):

		1)	TestPrint 
			const TestPrint 
			volatile TestPrint
			const volatile TestPrint
*/
void RunTest()
{
	RunTest_main();
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace ConstVolatile

namespace OuterInner_1 {

//	////////////////////////////////////////////////////////////////////////////
class Outer 
{
  public:
     static int m_Out;
     class Inner
     {
        public:
          static int m_In;
          void Display();
     };
};
//	----------------------------------------------------------------------------
int Outer::m_Out = 10;
int Outer::Inner::m_In = 25;
//	----------------------------------------------------------------------------
void Outer::Inner::Display() {  std::cout << m_Out << std::endl;  }
//	----------------------------------------------------------------------------
void RunTest_main() 
{
     Outer    objOut;
     Outer::Inner objIn;
     // *****
//	objOut.m_In;    
//	objIn.m_Out;
	objIn.Display();    
//	objIn.Outer::m_Out;

	ValueIgnore(objOut);

#if defined(_MSC_VER) && (_MSC_VER <= 1500)
# pragma warning(push)
# pragma warning(disable: 4101 4555)
#endif // #if defined(_MSC_VER) && (_MSC_VER <= 1500)

	objOut.Inner::m_In;

#if defined(_MSC_VER) && (_MSC_VER <= 1500)
# pragma warning(pop)
#endif // #if defined(_MSC_VER) && (_MSC_VER <= 1500)

	//	MLB
	std::cout << "objOut.Inner::m_In = [" << objOut.Inner::m_In << "]" <<
		std::endl;
}
//	----------------------------------------------------------------------------
/*
	ANSWER(S):

		1)	Outer class cannot access its nested class's private members.

		2) Inner class cannot access its containing class's private members.
			(true until C++11)
*/
void RunTest()
{
	RunTest_main();
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace OuterInner_1

namespace OuterInner_2 {

//	////////////////////////////////////////////////////////////////////////////
class Outer 
{
     int m_o;
     public:
     class Inner
     {
        int m_i;
        public:
          Inner(){}
          Inner(Outer m_outer, int x)
          {
             m_outer.m_o = x;
          }
     };    
     Outer(int /* y */)
     {
			/*
				ERROR: Outer class cannot access its nested class's private members.
				1>.\MiscTests.cpp(1288) : error C2248: 'TEST_IKM::OuterInner_2::Outer::Inner::m_i' : cannot access private member declared in class 'TEST_IKM::OuterInner_2::Outer::Inner'
				1>        .\MiscTests.cpp(1275) : see declaration of 'TEST_IKM::OuterInner_2::Outer::Inner::m_i'
				1>        .\MiscTests.cpp(1274) : see declaration of 'TEST_IKM::OuterInner_2::Outer::Inner'
         m_inner.m_i = y;
			*/
     }
     void Display()
     {
         using namespace std;
			/*
				ERROR: Outer class cannot access its nested class's private members.
				1>.\MiscTests.cpp(1294) : error C2248: 'TEST_IKM::OuterInner_2::Outer::Inner::m_i' : cannot access private member declared in class 'TEST_IKM::OuterInner_2::Outer::Inner'
				1>        .\MiscTests.cpp(1275) : see declaration of 'TEST_IKM::OuterInner_2::Outer::Inner::m_i'
				1>        .\MiscTests.cpp(1274) : see declaration of 'TEST_IKM::OuterInner_2::Outer::Inner'
         cout << m_o << endl
              << m_inner.m_i << endl;
			*/
         cout << m_o << endl
              << "FAILED TO ACCESS 'm_inner.m_i'" << endl;
     }
     Inner m_inner;
};
//	----------------------------------------------------------------------------
void RunTest_main() 
{
     Outer    objOut(10);
     Outer::Inner objIn(objOut, 5);
     objOut.Display(); 
}    
//	----------------------------------------------------------------------------
/*
	ANSWER(S):

		1)	Outer class cannot access its nested class's private members.

		2)	C++98/03: Inner class cannot access its containing class's private
			members. But in C++11+, the inner class has full access as a member.
*/
void RunTest()
{
	RunTest_main();
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace OuterInner_2

namespace AmbiguousOverloads {

//	////////////////////////////////////////////////////////////////////////////
struct base { };
struct der  : public base { };
//	----------------------------------------------------------------------------
/*
void SomeFunc(base& b){ ... } 
void SomeFunc(base b){ ... } 
void SomeFunc(der& b){ ... } 
void SomeFunc(der b){ ... }   
*/
void EmitMe(const char *in_string)
{
	std::cout << "EmitMe(" << in_string << ");" << std::endl;
}
void SomeFunc(base& /* b */){ EmitMe("void SomeFunc(base& b)"); } 
void SomeFunc(base /* b */) { EmitMe("void SomeFunc(base b)");  } 
void SomeFunc(der& /* b */) { EmitMe("void SomeFunc(der& b)");  } 
void SomeFunc(der /* b */)  { EmitMe("void SomeFunc(der b)");   }   
//	----------------------------------------------------------------------------
void RunTest_main() 
{
  der d;
/*
  SomeFunc(d);	// <--- error C2668: 'TEST_IKM::AmbiguousOverloads::SomeFunc' : ambiguous call to overloaded function
*/

	ValueIgnore(d);
}
//	----------------------------------------------------------------------------
/*
	ANSWER(S):

		1)	The compiler will generate an ambiguous call error.
*/
void RunTest()
{
	RunTest_main();
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace AmbiguousOverloads

namespace ExBaseAndDer {

//	////////////////////////////////////////////////////////////////////////////
class ExBase 
{  
private:
     static int stat;
public:
     static int GetStat(){ return stat; }
};
//	----------------------------------------------------------------------------
int ExBase::stat = 25;
//	----------------------------------------------------------------------------
class ExDer1 : public ExBase 
{  
public:
//     friend int Der1Fn(){ return ExBase::stat; } // <---  error C2248: 'TEST_IKM::ExBaseAndDer::ExBase::stat' : cannot access private member declared in class 'TEST_IKM::ExBaseAndDer::ExBase'
     friend int Der1Fn(){ return 0; }
};
//	----------------------------------------------------------------------------
class ExDer2 : public ExBase{};   
class ExDer : public ExDer1, public ExDer2{};
//	----------------------------------------------------------------------------
void Answer_A_main() 
{
	using namespace std;

      ExDer d;

//      cout << d.Der1Fn() << endl; 

	ValueIgnore(d);
}   
// will result in an ambiguity error from the compiler.
/*
	MLB:

	Not an ambiguity error, but:

	Der1Fn() is not a member, but a friend free function defined inline. Also,
	it can't be compiled anyway as it doesn't have access to ExBase::stat.
*/
//	----------------------------------------------------------------------------
void Answer_B_main() 
{
	using namespace std;

      ExDer d;

      cout << d.GetStat() << endl; 

	ValueIgnore(d);
}  
// will display an output as "25".  
//	----------------------------------------------------------------------------
void Answer_C_main() 
{
	using namespace std;

      cout << ExDer1::GetStat() << endl; 
}  
// will result in an ambiguity error from the compiler.  
//	----------------------------------------------------------------------------
/*
	MLB:

	The inline friend free function Der1Fn() does not have access to the private
	member of base class ExBase. And so it fails to compile with an access error.

class ExDer1 : public ExBase 
{  
public:
     friend int Der1Fn(){ return ExBase::stat; } 
};
// will result in an access error from the compiler.  
*/
//	----------------------------------------------------------------------------
void Answer_E_main() 
{
	using namespace std;

      cout << ExDer1::ExBase::GetStat() << endl; 
}
// will display an output as "25".    
//	----------------------------------------------------------------------------
void RunTest_main() 
{
}
//	----------------------------------------------------------------------------
/*
	ANSWER(S):

		1)	
			class ExDer1 : public ExBase 
			{  
				public:
				  friend int Der1Fn(){ return ExBase::stat; } 
			};
			will result in an access error from the compiler.
*/
void RunTest()
{
	Answer_A_main();
	Answer_B_main();
	Answer_C_main();
//	Answer_D_main();
	Answer_E_main();
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace ExBaseAndDer

namespace DynamicCastTest {

//	////////////////////////////////////////////////////////////////////////////
// MLB support for the question...
struct IIcon {
};
//	----------------------------------------------------------------------------
//Someone else's code, e.g. library 
class IGlyph 
{  
public:
      virtual ~IGlyph(){}
      virtual std::string Text()=0;
      virtual IIcon*      Icon()=0;
      //... 
};   
//	----------------------------------------------------------------------------
class IWidgetSelector 
{  
public:
      virtual ~IWidgetSelector(){}
      virtual void    AddItem(IGlyph*)=0;
      virtual IGlyph* Selection()=0; 
};    
//	----------------------------------------------------------------------------
// MLB support for the question...
struct SpiffyForm {
	static void OnDoubleClick(IWidgetSelector* ws);
};
class MyIWidgetSelector : public IWidgetSelector {
public:
		MyIWidgetSelector()
			:IWidgetSelector()
			,item_(NULL)
		{
		}

      virtual void    AddItem(IGlyph *in_item)
		{
			item_ = in_item;
		}

      virtual IGlyph* Selection()
		{
			return(item_);
		}

		IGlyph *item_;
};
//	----------------------------------------------------------------------------
//Your code  
class MyItem : public IGlyph 
{  
public:
      virtual std::string Text()
      {
          return this->text;
      }
      virtual IIcon* Icon()
      {
          return this->icon.get();
      }
      void Activate()
      {
          std::cout << "My Item Activated" << std::endl;
      }
      std::string          text;
      std::auto_ptr<IIcon> icon; 
};
//	----------------------------------------------------------------------------
void SpiffyForm::OnDoubleClick(IWidgetSelector* ws) 
{
      IGlyph* gylph = ws->Selection();
      MyItem* item  = dynamic_cast<MyItem*>(gylph);
     if(item)
          item->Activate();
}    
//	----------------------------------------------------------------------------
/*
	ANSWER(S):

		1)	The dynamic_cast is necessary since we cannot know for certain what
			concrete type is returned by IWidgetSelector::Selection().   
*/
void RunTest()
{
	MyIWidgetSelector tmp_iws;
	MyItem            my_item;

	tmp_iws.AddItem(&my_item);

	SpiffyForm::OnDoubleClick(&tmp_iws);
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace DynamicCastTest

namespace ConstCorrectness {

//	////////////////////////////////////////////////////////////////////////////
template<typename T> 
class MyArray 
{  
public:
      MyArray() { }
//		WAS:
//		MyArray(MyArray& copy);
		MyArray(const MyArray& copy) { }
      MyArray& operator=(MyArray& copy) { }
      //... 
};   
//	----------------------------------------------------------------------------
class MyData 
{  
public:
//		WAS:
//		MyData(MyArray<int>& x, MyArray<int>& y);
      MyData(const MyArray<int>& /* x */, const MyArray<int>& /* y */) { }
      //...
      const MyArray<int>& x() { }
      const MyArray<int>& y() { } 
};
//	----------------------------------------------------------------------------
MyArray<int> read_data(int*, char**) { return(MyArray<int>()); }
//	----------------------------------------------------------------------------
void make_changes(MyData* /* edit */) { }
//	----------------------------------------------------------------------------
void RunTest_main(int argc, char* argv[]) 
{
      const MyArray<int> read_x = read_data(&argc, argv);
      const MyArray<int> read_y = read_data(&argc, argv);
      MyData user_data(read_x, read_y);
      MyData edit_buffer(user_data);
      make_changes(&edit_buffer); 
}    
//	----------------------------------------------------------------------------
/*
	ANSWER(S):

	1) MyData(MyArray<int>& x, MyArray<int>& y); should be
		MyData(const MyArray<int>& x, const MyArray<int>& y);

	2)	MyArray(MyArray& copy); should be MyArray(const MyArray& copy);
*/
void RunTest()
{
	int argc     = 3;
	char *argv[] = { "A", "B", "C", "\0" };

	RunTest_main(argc, argv);
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace ConstCorrectness

namespace EnumAssignment {

//	////////////////////////////////////////////////////////////////////////////
/*
	ANSWER(S):

		1)	Mon of enum DOW cannot be assigned because the compiler will give a
			redefinition error.
*/
void RunTest()
{
	const int Mon = 1, Tue = 2;

	ValueIgnore(Mon + Tue);

	/*
		1>.\MiscTests.cpp(1554) : error C2365: 'Mon' : redefinition; previous definition was 'data variable'
		1>        .\MiscTests.cpp(1553) : see declaration of 'Mon'
		1>.\MiscTests.cpp(1554) : error C2365: 'Tue' : redefinition; previous definition was 'data variable'
		1>        .\MiscTests.cpp(1553) : see declaration of 'Tue'
	enum DOW{ Mon = 11, Tue = 12 };
	*/
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace EnumAssignment

namespace CtorOrder {

//	////////////////////////////////////////////////////////////////////////////
class Base 
{
      public:
      Base(){ std::cout << "In Base Ctor\n"; }
      class Nest
      {
          public:
          Nest(){ std::cout << "In Nest Ctor\n"; }
      };    
};
//	----------------------------------------------------------------------------
class Derive : public Base 
{
      public:
      Derive(){ std::cout << "In Derive Ctor\n"; }
};   
//	----------------------------------------------------------------------------
void RunTest_main() 
{
      Derive obj; 
}
//	----------------------------------------------------------------------------
/*
	ANSWER(S):

		1)	Base constructor 
			Derive constructor
*/
void RunTest()
{
	RunTest_main();
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace CtorOrder

namespace PtrToArrayofPtrs {

//	////////////////////////////////////////////////////////////////////////////
//	----------------------------------------------------------------------------
/*
	ANSWER(S):

		1)	p = new int *[3][5];
*/
const int MyRows = 3;
const int MyCols = 5;
void RunTest()
{
	int     value_list[MyRows * MyCols];
	char    barrier_1[128];
	int  *(*p)[5];	//	Pointer to an array of five int pointers.
	char    barrier_2[128];

	for (int count_1 = 0; count_1 < (MyRows * MyCols); ++count_1)
		value_list[count_1] = count_1;

	::memset(barrier_1, '\0', sizeof(barrier_1));
	::memset(barrier_2, '\0', sizeof(barrier_2));

	p = new int * [3][5];

	for (int count_1 = 0; count_1 < MyRows; ++count_1) {
		for (int count_2 = 0; count_2 < MyCols; ++count_2)
			p[count_1][count_2] = &(value_list[(count_1 * MyCols) + MyCols]);
	}

	delete [] p;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace PtrToArrayofPtrs

} // namespace TEST_IKM

namespace TEST_praetor_web_elte_hu {
	// From http://praetor.web.elte.hu/

namespace LAndRValueAssignment {

//TEST_praetor_web_elte_hu::LAndRValueAssignment::RunTest();
//	////////////////////////////////////////////////////////////////////////////
void RunTest_main()
{
	int a = 5;
	int& b = ++a;
	int& c = b;

	std::cout << c++ << std::endl;
}
//	----------------------------------------------------------------------------
/*
	ANSWER(S):

		1)	This code will display an output as “6”.
*/
void RunTest()
{
	RunTest_main();
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace LAndRValueAssignment

namespace CallPureVirtualThroughBase {

//	////////////////////////////////////////////////////////////////////////////
class Base
{
	public:
	virtual int f() = 0;
	virtual ~Base(){}
};
//	----------------------------------------------------------------------------
int Base::f()
{
	return 0;
}
//	----------------------------------------------------------------------------
class Derived : public Base
{
public:
	int f()
	{
		return 1;
	}

	virtual ~Derived(){}
};
//	----------------------------------------------------------------------------
void RunTest_main()
{
	Base* b = new Derived();

	std::cout << b->Base::f() << ' ' << b->f() << std::endl;
}
//	----------------------------------------------------------------------------
/*
	ANSWER(S):

		1)	The program will display an output as “0 1”
*/
void RunTest()
{
	RunTest_main();
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace CallPureVirtualThroughBase

namespace PointerToMemberData {

//	////////////////////////////////////////////////////////////////////////////
class SomeClass
{
	public:
		int iData;
		//...
};
//	----------------------------------------------------------------------------
/*
	ANSWER(S):

		1)	int SomeClass::*pData = &SomeClass::iData;
*/
void RunTest()
{
	int       SomeClass::*pData = &SomeClass::iData;

	SomeClass sc;

	sc.iData = 42;
	std::cout << "sc.iData = " << sc.iData << std::endl;

	sc.*pData = 666;
	std::cout << "sc.*pData = " << sc.iData << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace PointerToMemberData

namespace InlineFriendFunction {

//	////////////////////////////////////////////////////////////////////////////
class SomeClass
{
	public:
		friend int f() { return SomeClass::data; }
	private:
		static int data;
};
//	----------------------------------------------------------------------------
int SomeClass::data = 5;
//	----------------------------------------------------------------------------
/*
	ANSWER(S):

		1)	Compiling the previous code will result in a compiler error, since the
			name f() does not exist in the global namespace.
*/
void RunTest()
{
	//	MSVC 2008: error C2039: 'f' : is not a member of 'SomeClass'
	//	g++ 4.8.2: error: ‘f’ was not declared in this scope
//	std::cout << f() << std::endl;

	//	MSVC 2008: error C2039: 'f' : is not a member of 'SomeClass'
	//	MSVC 2008: error C3767: 'f': candidate function(s) not accessible
	//	g++ 4.8.2: error: ‘f’ is not a member of ‘SomeClass’
//	std::cout << SomeClass::f() << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace InlineFriendFunction

namespace VirtualFuncsDifferentDefaults {

//	////////////////////////////////////////////////////////////////////////////
struct Base
{
	virtual void f(int i = 1) { std::cout << "Base::f("<< i << ")\n"; }
	virtual ~Base(){};
};
//	----------------------------------------------------------------------------
struct Der : Base
{
	void f(int i = 2) { std::cout << "Der::f("<< i << ")\n"; }
	~Der(){};
};
//	----------------------------------------------------------------------------
void RunTest_main()
{
	Base *bp = new Der;

	bp->f();
}
//	----------------------------------------------------------------------------
/*
	ANSWER(S):

		1)	The program will display an output as "Der::f(1)".
*/
void RunTest()
{
	RunTest_main();
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace VirtualFuncsDifferentDefaults

namespace ReverseIteratorBack {

//	////////////////////////////////////////////////////////////////////////////
std::string EmitTerm(const std::vector<int> &in_vec,
	const std::vector<int>::reverse_iterator &iter)
{
	std::ostringstream o_str;

	if (iter == in_vec.rend())
		o_str << "<END>";
	else
		o_str << *iter;

	return(o_str.str());
}
//	----------------------------------------------------------------------------
std::string EmitTerm(const std::vector<int> &in_vec,
	const std::vector<int>::iterator &iter)
{
	std::ostringstream o_str;

	if (iter == in_vec.end())
		o_str << "<END>";
	else
		o_str << *iter;

	return(o_str.str());
}
//	----------------------------------------------------------------------------
void MyHelper(const std::vector<int> &in_vec,
	const std::vector<int>::reverse_iterator &iter, const char *name)
{
	std::cout << std::left << std::setw(17) << name << std::right << " = ";

	std::cout << EmitTerm(in_vec, iter) << " (base() = " <<
		EmitTerm(in_vec, iter.base()) << ")";

	std::cout << std::endl;
}
//	----------------------------------------------------------------------------
void MyHelper(const std::vector<int> &in_vec,
	const std::vector<int>::iterator &iter, const char *name)
{
	std::cout << std::left << std::setw(17) << name << std::right << " = ";

	std::cout << EmitTerm(in_vec, iter);

	std::cout << std::endl;
}
//	----------------------------------------------------------------------------
/*
	ANSWER(S):

		1)	The program will display an output as “4”.
*/
void RunTest()
{
	std::vector<int> v;

	for(int i = 1; i <= 5; ++i) v.push_back(i);

	{
		std::vector<int>::reverse_iterator ri_0 = v.rbegin();
		std::vector<int>::reverse_iterator ri_1 = v.rbegin() + 1;
		std::vector<int>::reverse_iterator ri_2 = v.rbegin() + 2;
		MyHelper(v, ri_0, "ri_0");
		MyHelper(v, ri_1, "ri_1");
		MyHelper(v, ri_2, "ri_2");
		MyHelper(v, v.rbegin().base(), "v.rbegin().base()");
	}

	std::vector<int>::reverse_iterator ri = v.rbegin() + 2;
	std::vector<int>::iterator iter       = ri.base();

	MyHelper(v, ri, "ri");
	MyHelper(v, iter, "iter");

	std::cout << *iter;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace ReverseIteratorBack

namespace StdRemoveOnVector {

//	////////////////////////////////////////////////////////////////////////////
/*
	ANSWER(S):

		1)	The program will display an output as “6”.
*/
void RunTest()
{
	std::vector<int> v;
	for(int i = 0; i <= 5; ++i) v.push_back(i);
	std::remove( v.begin(), v.end(), 2 );
	std::cout << v.size();
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace StdRemoveOnVector

} // namespace TEST_praetor_web_elte_hu

namespace TEST_DominicConner {

namespace EnumValueArithmetic {

//	////////////////////////////////////////////////////////////////////////////
void RunTest_1()
{
	EmitTextAndSep(__FUNCTION__);

	unsigned long sum;

	enum Days {Monday,Tuesday,Wednesday,Thursday,Friday,Saturday,Sunday};

	sum = Monday + Sunday;

	std::cout << sum << "\n";
}
//	----------------------------------------------------------------------------
void RunTest_2()
{
	EmitTextAndSep(__FUNCTION__);

	unsigned long sum;

	enum Days {Monday,Tuesday,Wednesday=0,Thursday,Friday,Saturday,Sunday };

	sum = Monday + Thursday;

	std::cout << sum << "\n";
}
//	----------------------------------------------------------------------------
void RunTest_3()
{
	EmitTextAndSep(__FUNCTION__);

	unsigned long sum;

	enum Days {Monday,Tuesday,Wednesday=Monday,Thursday,Friday,Saturday,
		Sunday};

	sum = Tuesday + Thursday;

	std::cout << sum << "\n";

	sum = static_cast<unsigned long>(Monday + Tuesday - Sunday);

	std::cout << sum << "\n";
}
//	----------------------------------------------------------------------------
void RunTest()
{
	RunTest_1();
	EmitSep('-');

	RunTest_2();
	EmitSep('-');

	RunTest_3();
	EmitSep('-');
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace EnumValueArithmetic

namespace HidingAndObsfuscation {

//	////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
# pragma warning(push)
# pragma warning(disable: 4100 4263 4244 4264 4946)
#endif // #if defined(_MSC_VER)
//	----------------------------------------------------------------------------
class Bond {
public:
	double virtual Price(double rate) { return rate *25; }
};
//	----------------------------------------------------------------------------
class Warrant : public Bond
{
public:
	double Price(int WarID ) { return 1; }
};
//	----------------------------------------------------------------------------
void RunTest()
{
	Warrant * w = new Warrant;
	Bond * b = dynamic_cast <Bond *> (w);	//	MLB: No need for any cast here.

	w = reinterpret_cast<Warrant *> (b);	//	MLB: Static cast would suffice.
	b=static_cast<Bond *> (w);

	double rate = 0.15;

	/*
		MLB: Static type of w is Warrant *. Class Warrant has a non-virtual
			  class method Rprice() which hides the base class virtual
			  function Price(), so we'll dispatch to based upon the static
			  type of the pointer to Warrant::Price().

           The floating-point argument of 0.15 is implicitly covnverted
			  to an int as per the formal parameter type, which results in
			  the function receiving the value 0. The integer return value 1
			  is implicitly converted to 1.0 because the formal return type
           is double.

			  Upshot being, output is:

					Price=1.0
	*/
	std::cout << "Price=" << w->Price(rate) << "\n";
}
//	----------------------------------------------------------------------------
#if defined(_MSC_VER)
# pragma warning(pop)
#endif // #if defined(_MSC_VER)
//	////////////////////////////////////////////////////////////////////////////

} // namespace HidingAndObsfuscation

} // namespace TEST_DominicConner

#include <boost/tti/has_data.hpp>
#include <boost/tti/has_member_data.hpp>

//	////////////////////////////////////////////////////////////////////////////
struct SomeMessage_1 {
	unsigned int u_int_32_;
	signed int   s_int_32_;
};
//	----------------------------------------------------------------------------
struct SomeMessage_2 {
	signed int   s_int_32_;
	unsigned int u_int_32_;
};
//	----------------------------------------------------------------------------
#define TEST_TTI_Test_001(param)	\
	std::cout << #param << '=' << param << '\n';
//	----------------------------------------------------------------------------
BOOST_TTI_HAS_DATA(u_int_32_)
BOOST_TTI_HAS_MEMBER_DATA(u_int_32_)
BOOST_TTI_HAS_MEMBER_DATA(s_int_32_)
BOOST_TTI_TRAIT_HAS_MEMBER_DATA(CheckIceMarketIDName, u_int_32_)
//	----------------------------------------------------------------------------
template <std::size_t> struct CheckIceMarketIDOffset {
	static const bool value = false;
};
//	----------------------------------------------------------------------------
template <> struct CheckIceMarketIDOffset<0> {
	static const bool value = true;
};
//	----------------------------------------------------------------------------
template <typename IceMessageType> struct CheckIceMarketID {
	static const bool value =
		CheckIceMarketIDName<IceMessageType, unsigned int>::value &&
		CheckIceMarketIDOffset<offsetof(IceMessageType, u_int_32_)>::value;
};
//	----------------------------------------------------------------------------
void TEST_TTI_001()
{
	TEST_TTI_Test_001( (has_member_data_u_int_32_<SomeMessage_1, unsigned int>::value) )
	TEST_TTI_Test_001( (has_member_data_u_int_32_<SomeMessage_1,   signed int>::value) )
	TEST_TTI_Test_001( (has_member_data_s_int_32_<SomeMessage_1, unsigned int>::value) )
	TEST_TTI_Test_001( (has_member_data_s_int_32_<SomeMessage_1,   signed int>::value) )

	TEST_TTI_Test_001( (CheckIceMarketIDName<SomeMessage_1, unsigned int>::value) )
	TEST_TTI_Test_001( (CheckIceMarketIDName<SomeMessage_1,   signed int>::value) )

	TEST_TTI_Test_001( (CheckIceMarketID<SomeMessage_1>::value) )

	TEST_TTI_Test_001( (has_member_data_u_int_32_<SomeMessage_2, unsigned int>::value) )
	TEST_TTI_Test_001( (has_member_data_u_int_32_<SomeMessage_2,   signed int>::value) )
	TEST_TTI_Test_001( (has_member_data_s_int_32_<SomeMessage_2, unsigned int>::value) )
	TEST_TTI_Test_001( (has_member_data_s_int_32_<SomeMessage_2,   signed int>::value) )

	TEST_TTI_Test_001( (CheckIceMarketIDName<SomeMessage_2, unsigned int>::value) )
	TEST_TTI_Test_001( (CheckIceMarketIDName<SomeMessage_2,   signed int>::value) )

	TEST_TTI_Test_001( (CheckIceMarketID<SomeMessage_2>::value) )
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
int main()
{
	int return_code = EXIT_SUCCESS;

TEST_TTI_001();

	try {
//	TEST_WindowsMemoryBarriers::RunTest();
//	TEST_ResolveOverloadOnClassHierarchy::RunTest();
//	TEST_InitializeArrayofPodsMember::RunTest();
//	TEST_TemplatizedBitFieldSizes::RunTest();
//	TEST_ArrayIdentities::RunTest();
//	TEST_EnableIf::RunTest();
//	TEST_MemberAccess_1::RunTest();
//	TEST_MemberAccess_2::RunTest();
//	TEST_TemplateTemplateParameters::RunTest();
//TEST_ReverseAStringRecursively::RunTest();
//TEST_DelegateToSiblingClass::RunTest();
TEST_ReverseLinkedList_Single::RunTest();
//TEST_ReverseLinkedList_Recursive::RunTest();
//TEST_FibonacciSequence::RunTest();
//TEST_MyAlignedAllocator::RunTest();
//TEST_DominicConner::HidingAndObsfuscation::RunTest();
//TEST_ReverseWordsInAString::RunTest();
//TEST_IKM::PreprocessorTest::RunTest();
//TEST_IKM::ConstMutable::RunTest();
//TEST_IKM::ConstVolatile::RunTest();
//TEST_IKM::OuterInner_1::RunTest();
//TEST_IKM::OuterInner_2::RunTest();
//TEST_IKM::AmbiguousOverloads::RunTest();
//TEST_IKM::ExBaseAndDer::RunTest();
//TEST_IKM::DynamicCastTest::RunTest();
//TEST_IKM::ConstCorrectness::RunTest();
//TEST_IKM::EnumAssignment::RunTest();
//TEST_IKM::CtorOrder::RunTest();
//TEST_IKM::PtrToArrayofPtrs::RunTest();
//TEST_praetor_web_elte_hu::LAndRValueAssignment::RunTest();
//TEST_praetor_web_elte_hu::CallPureVirtualThroughBase::RunTest();
//TEST_praetor_web_elte_hu::PointerToMemberData::RunTest();
//TEST_praetor_web_elte_hu::InlineFriendFunction::RunTest();
//TEST_praetor_web_elte_hu::VirtualFuncsDifferentDefaults::RunTest();
//TEST_praetor_web_elte_hu::ReverseIteratorBack::RunTest();
//TEST_praetor_web_elte_hu::StdRemoveOnVector::RunTest();
		TEST_RuntimeStaticCastOverhead::RunTest();
		TEST_CtorStyleCast::RunTest();
		TEST_ConstantFoldingCheck_1::RunTest();
		TEST_WindowsMemoryBarriers::RunTest();
		TEST_LifetimeOfLocalsDeclaredInElseIfBlocks::RunTest();
		TEST_ResolveOverloadOnClassHierarchy::RunTest();
		TEST_InitializeArrayofPodsMember::RunTest();
		TEST_TemplatizedBitFieldSizes::RunTest();
		TEST_ReverseAStringRecursively::RunTest();
		TEST_ReverseWordsInAString::RunTest();
		TEST_MyAlignedAllocator::RunTest();
		TEST_DelegateToSiblingClass::RunTest();
		TEST_UnableToCompletelyFixHidingProblem::RunTest();
		TEST_ZeroStorageBitCount::RunTest();
		TEST_ThrowFromDtor::RunTest();
		TEST_NewInitialization::RunTest();
		TEST_ConversionOperator::RunTest();
		TEST_CrossCast_1::RunTest();
//		TEST_CtorOrder::RunTest();
//		TEST_FloatingPointNotAssociative::RunTest();
		TEST_FibonacciSequence::RunTest();
		TEST_RewrittenFactorialCode::RunTest();
//		TEST_StupidClojureRecursionTest::RunTest();
//		TEST_AccessDeclaration::RunTest();
		TEST_PartialInstantiation::RunTest();
//		TEST_TemplateAsFriend::RunTest();
		TEST_DerivedOverrideAccessChange::RunTest();
//		TEST_PointerVersusArray_1::RunTest();
//		TEST_PointerVersusArray_2::RunTest();
//		TEST_PointerVersusArray_3::RunTest();
//		TEST_ReverseLinkedList_Single::RunTest();
//		TEST_ReverseLinkedList_Single::RunTest_StdList();
		TEST_ReverseLinkedList_Recursive::RunTest();
//		TEST_IntermediateBaseAccess::RunTest();
		TEST_ConstInstances::RunTest();
		//	IKM
		TEST_IKM::PreprocessorTest::RunTest();
		TEST_IKM::ConstMutable::RunTest();
		TEST_IKM::ConstVolatile::RunTest();
		TEST_IKM::OuterInner_1::RunTest();
		TEST_IKM::OuterInner_2::RunTest();
		TEST_IKM::AmbiguousOverloads::RunTest();
		TEST_IKM::ExBaseAndDer::RunTest();
		TEST_IKM::DynamicCastTest::RunTest();
		TEST_IKM::ConstCorrectness::RunTest();
		TEST_IKM::EnumAssignment::RunTest();
		TEST_IKM::CtorOrder::RunTest();
		TEST_IKM::PtrToArrayofPtrs::RunTest();
		// From http://praetor.web.elte.hu/
		TEST_praetor_web_elte_hu::LAndRValueAssignment::RunTest();
		TEST_praetor_web_elte_hu::CallPureVirtualThroughBase::RunTest();
		TEST_praetor_web_elte_hu::PointerToMemberData::RunTest();
		TEST_praetor_web_elte_hu::InlineFriendFunction::RunTest();
		TEST_praetor_web_elte_hu::VirtualFuncsDifferentDefaults::RunTest();
		TEST_praetor_web_elte_hu::ReverseIteratorBack::RunTest();
		TEST_praetor_web_elte_hu::StdRemoveOnVector::RunTest();
		//	http://www.wilmott.com/pdfs/DomincCpp2.pdf
		TEST_DominicConner::EnumValueArithmetic::RunTest();
		TEST_DominicConner::HidingAndObsfuscation::RunTest();
	}
	catch (const std::exception &except) {
		std::cerr << "ERROR: " << except.what() << std::endl;
		return_code = EXIT_FAILURE;
	}

	return(return_code);
}
//	////////////////////////////////////////////////////////////////////////////

