//	////////////////////////////////////////////////////////////////////////////

#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>

//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef unsigned long long UidType;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
struct SomeData {
	SomeData(UidType uid = 0, const std::string &name = "",
		const std::string &other_data = "")
		:uid_(uid)
		,name_(name)
		,other_data_(other_data)
	{
	}

	bool operator == (const SomeData &other) const
	{
		return(uid_ == other.uid_);
	}

	UidType     uid_;
	std::string name_;
	std::string other_data_;
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef std::shared_ptr<SomeData>       SomeDataSPtr;

typedef std::map<UidType, SomeData>     SomeDataMap;
typedef std::map<UidType, SomeDataSPtr> SomeDataSPtrMap;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
SomeDataMap TEST_CreateSomeDataMap(std::size_t instance_count = 10)
{
	instance_count = (instance_count > 0) ? instance_count : 10;

	SomeDataMap dst;

	for (std::size_t count_1 = 0; count_1 < instance_count; ++count_1) {
		std::ostringstream o_str;
		o_str << "ITEM-" << std::setfill('0') << std::setw(5) << (count_1 + 1);
		dst[count_1 + 1] = SomeData(count_1 + 1, o_str.str(),
			o_str.str() + "-OTHER-DATA");
	}

	return(dst);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
SomeDataSPtrMap TEST_CreateSomeDataSPtrMap(std::size_t instance_count = 10)
{
	SomeDataMap     src(TEST_CreateSomeDataMap(instance_count));
	SomeDataSPtrMap dst;

	for (const auto &this_element : src) {
		dst[this_element.first] = std::make_shared<SomeData>(this_element.second);
	}

	return(dst);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType> struct DatumValueShim {
	static DatumType Copy(const DatumType &src)
	{
		return(DatumType(src));
	}

	static bool Equals(const DatumType &lhs, const DatumType &rhs)
	{
		return(lhs == rhs);
	}
};
//	----------------------------------------------------------------------------
template <typename DatumType>
	struct DatumValueShim<std::shared_ptr<DatumType> > {
	typedef std::shared_ptr<DatumType> MyDatumType;

	static std::shared_ptr<DatumType> Copy(
		const std::shared_ptr<DatumType> &src)
	{
		return((src) ? std::make_shared<DatumType>(*src) :
			std::make_shared<DatumType>());
	}

	static bool Equals(const std::shared_ptr<DatumType> &lhs,
		const std::shared_ptr<DatumType> &rhs)
	{
		return((lhs == rhs) ? true : ((lhs && rhs) ? (*lhs == *rhs) : false));
	}
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename ContainerType>
	bool TEST_CopyDatumShim_Helper(const ContainerType &src)
{
	ContainerType dst;

	for (const auto &this_element : src)
		dst[this_element.first] = DatumValueShim<
			typename ContainerType::mapped_type>::Copy(this_element.second);

	ContainerType::const_iterator src_begin(src.begin());
	ContainerType::const_iterator dst_begin(dst.begin());
	
	while (src_begin != src.end()) {
		if (!DatumValueShim<typename ContainerType::mapped_type>::Equals(
			src_begin->second, dst_begin->second)) {
			std::ostringstream o_str;
			o_str << "Mapped value difference encountered at key " <<
				src_begin->first << ".";
			throw std::logic_error(o_str.str());
		}
		++src_begin;
		++dst_begin;
	}

	return(true);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
bool TEST_CopyDatumShim(std::size_t instance_count = 10)
{
	if (!TEST_CopyDatumShim_Helper(TEST_CreateSomeDataMap(instance_count)))
		throw std::logic_error("TEST_CopyDatumShim() map<DATUM> failed.");

	if (!TEST_CopyDatumShim_Helper(TEST_CreateSomeDataSPtrMap(instance_count)))
		throw std::logic_error("TEST_CopyDatumShim() map<shared_ptr<DATUM>> "
			"failed.");

	return(true);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
int main()
{
 	int return_code = EXIT_SUCCESS;

	try {
		TEST_CopyDatumShim(10);
	}
	catch (const std::exception &except) {
		return_code = EXIT_FAILURE;
		std::cerr << "ERROR: " << except.what() << std::endl;
	}

	return(return_code);
}
//	////////////////////////////////////////////////////////////////////////////

