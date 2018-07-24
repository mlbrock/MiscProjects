//	////////////////////////////////////////////////////////////////////////////

#define _SCL_SECURE_NO_WARNINGS

#include <functional>

#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

//	For Interval logic.
#include <limits>
#include <sstream>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
//#include <boost/algorithm/string/replace.hpp>

#include <boost/regex/v4/regex.hpp>

#include <typeinfo>

//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
//	****************************************************************************
//	****************************************************************************
//	****************************************************************************
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType>
	DatumType ParseNumericString(const char *src,
		DatumType min_value = std::numeric_limits<DatumType>::min(),
		DatumType max_value = std::numeric_limits<DatumType>::max())
{
	return(static_cast<DatumType>(::atoi(src)));
}
template <typename DatumType>
	DatumType ParseNumericString(const std::string &src,
		DatumType min_value = std::numeric_limits<DatumType>::min(),
		DatumType max_value = std::numeric_limits<DatumType>::max())
{
	return(ParseNumericString<DatumType>(src.c_str(), min_value, max_value));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
/*
	Key:
		Existing: |=|
		New     : |-|
	----------------------------------------------------------------------------
	#1
   |===================|
	                         |----------------|
	----------------------------------------------------------------------------
	#2
                            |===================|
	|----------------|
	----------------------------------------------------------------------------
	#3
   |===================|
	                    |----------------|
	----------------------------------------------------------------------------
	#4
                    |===================|
	|----------------|
	----------------------------------------------------------------------------
	#5
   |===================|
                 |----------------|
	----------------------------------------------------------------------------
	#6
                |===================|
	|----------------|
	----------------------------------------------------------------------------
	#7
   |===================|
	    |---------|
	----------------------------------------------------------------------------
	#8
       |===================|
	|---------------------------------|
	----------------------------------------------------------------------------
	#9
   |===================|
	|-------------------|
	----------------------------------------------------------------------------
*/
//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType> struct IntervalWrapper {
//	typedef DatumType                           DatumType;
	typedef std::pair<DatumType, DatumType>     ValInterval;
	typedef std::vector<ValInterval>            ValIntervalVec;
	typedef std::multimap<DatumType, DatumType> ValIntervalMap;

	static ValIntervalVec ParseInterval(const std::string &src,
		DatumType min_value = std::numeric_limits<DatumType>::min(),
		DatumType max_value = std::numeric_limits<DatumType>::max())
	{
		if (min_value > max_value)
			throw std::invalid_argument("The specified minimum permissible value "
				"is greater than the maximum permissible value.");

		using namespace boost::algorithm;

		ValIntervalVec           dst;
		std::string              tmp_src(src);
		std::vector<std::string> tmp_list;
		ValIntervalVec           range_list;

		split(tmp_list, tmp_src, is_any_of(","), token_compress_off);

		for (const auto &this_element : tmp_list) {
			try {
				std::string tmp(trim_copy(this_element));
				if (tmp.find('-') != std::string::npos)
					boost::regex_replace(tmp, boost::regex("\\s*\\-\\s*",
						boost::regex::perl | boost::regex::icase), "-");
				boost::regex_replace(tmp, boost::regex("\\s+",
					boost::regex::perl | boost::regex::icase), "");
				if (tmp.empty())
					continue;
				std::size_t sep_pos = tmp.find_first_of("-");
				DatumType val_lo;
				DatumType val_hi;
				if (sep_pos == std::string::npos) {
					val_lo = ParseNumericString<DatumType>(tmp,
						min_value, max_value);
					val_hi = val_lo;
				}
				else if (::strchr(tmp.c_str() + sep_pos + 1, '-') == NULL) {
					if (tmp.size() == 1) {
						val_lo = min_value;
						val_hi = max_value;
					}
					else if (!sep_pos) {
						val_lo = min_value;
						val_hi = ParseNumericString<DatumType>(tmp.c_str() + 1,
							min_value, max_value);
					}
					else if (sep_pos == (tmp.size() - 1)) {
						tmp[sep_pos] = '\0';
						val_lo       = ParseNumericString<DatumType>(tmp.c_str(),
							min_value, max_value);
						val_hi       = max_value;
					}
					else {
						tmp[sep_pos] = '\0';
						val_lo       = ParseNumericString<DatumType>(
							tmp.c_str(), min_value, max_value);
						val_hi       = ParseNumericString<DatumType>(
							tmp.c_str() + sep_pos + 1, min_value, max_value);
						if (val_lo > val_hi)
							std::swap(val_lo, val_hi);
					}
				}
				else
					throw std::invalid_argument("Multiple interval element "
						"separator characters were encountered.");
				range_list.push_back(ValInterval(val_lo, val_hi));
			}
			catch (const std::exception &except) {
				std::ostringstream o_str;
				o_str << "Unable to parse value '" << this_element << "': " <<
					except.what();
				throw std::invalid_argument(o_str.str());
			}
		}

		for (const auto &src_intv : range_list) {
			if (dst.empty())
				dst.push_back(src_intv);
			else {
				bool merged_flag = false;
				for (auto &dst_intv : dst) {
					if ((src_intv.first <= dst_intv.second) &&
						 (src_intv.second >= dst_intv.first)) {
						//	CASES: #3, #4, #5, #6, #7, #8, #9
						dst_intv.first  = std::min(dst_intv.first, src_intv.first);
						dst_intv.second = std::max(dst_intv.second, src_intv.second);
						merged_flag     = true;
						break;
					}
				}
				if (!merged_flag)
					dst.push_back(ValInterval(src_intv.first, src_intv.second));
			}
		}

		return(dst);
	}
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType>
	std::vector<std::pair<DatumType, DatumType> > ParseInterval(
		const std::string &src,
		DatumType min_value = std::numeric_limits<DatumType>::min(),
		DatumType max_value = std::numeric_limits<DatumType>::max())
{
	return(IntervalWrapper<DatumType>::ParseInterval(src, min_value, max_value));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType> bool IsInInterval(
	const std::vector<std::pair<DatumType, DatumType> > &intv_list,
	const DatumType &value)
{
	for (const auto &this_intv : intv_list) {
		if ((value >= this_intv.first) && (value <= this_intv.second))
			return(true);
	}

	return(false);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType>
	std::ostream & operator << (std::ostream &o_str,
		const std::pair<DatumType, DatumType> &datum)
{
	o_str <<
		'[' << std::setw(20) << datum.first  <<
		',' << std::setw(20) << datum.second << ']';

	return(o_str);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType>
	std::ostream & operator << (std::ostream &o_str,
		const std::vector<std::pair<DatumType, DatumType> > &datum)
{
	for (std::size_t count_1 = 0; count_1 < datum.size(); ++count_1)
		o_str << ((count_1) ? "\n" : "") << datum[count_1];

	return(o_str);
}
//	////////////////////////////////////////////////////////////////////////////

#define TEST_MAIN
#ifdef TEST_MAIN

//	////////////////////////////////////////////////////////////////////////////
struct TEST_Instance {
	std::string      input_;
	std::vector<int> success_list_;
	std::vector<int> failure_list_;
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const TEST_Instance TEST_InstanceList[] = {
	{
		"-",
	},
	{
		"-0",
		{ -1, -42 },
		{  2,   3 }
	},
	{
		"-1",
	},
	{
		"0-",
	},
	{
		"1-",
	},
	{
		"0-0",
	},
	{
		"0-1",
	},
	{
		"0-9",
	},
	{
		"7-9",
		{ 7, 8 },
		{ -42,  -1, 0, 1, 42 }
	},
	{
		"7-9,19,100-2399",
		{ 7, 8, 19, 2000 },
		{ -42,  -1, 0, 1, 42 }
	},
	{
		"7-9,19,100-2399,6-2400",
		{ 7, 8, 19, 2000 },
		{ -42,  -1, 0, 1 }
	},
};
const std::size_t   TEST_InstanceCount  =
	sizeof(TEST_InstanceList) / sizeof(TEST_InstanceList[0]);
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::string MyEmitterSep(char sep_char = '=', std::size_t sep_length = 79,
	bool nl_flag = true)
{
	std::ostringstream o_str;

	if (sep_length > 0)
		o_str << std::setfill(sep_char) << std::setw(sep_length) << ' ' <<
			((nl_flag) ? "\n" : "");

	return(o_str.str());
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType>
	void TEST_RunInterval_Helper_2(
		const std::vector<std::pair<DatumType, DatumType> > &intv_list,
		const std::vector<DatumType> &value_list, bool success_flag,
		int &return_code)
{
	for (std::size_t count_1 = 0; count_1 < value_list.size(); ++count_1) {
		bool found_value = IsInInterval(intv_list, value_list[count_1]);
		std::cout << std::setw(11) << value_list[count_1] <<
			((found_value == success_flag) ? ": SUCCESS (" : "FAILURE (") <<
			((found_value)  ? "    " : "NOT ") << "FOUND, WAS " <<
			((found_value == success_flag) ? "    " : "NOT ") << "EXPECTED)\n";
		if (found_value != success_flag)
			return_code = EXIT_FAILURE;
	}
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_RunInterval_Helper_1(const TEST_Instance &instance,
	int &return_code)
{
	std::cout << MyEmitterSep('=');

	std::cout << '{' << instance.input_ << '}' << std::endl;

	std::vector<std::pair<int, int> > intv_list(
		ParseInterval<int>(instance.input_));

	std::cout << intv_list << std::endl;

	TEST_RunInterval_Helper_2(intv_list, instance.success_list_,
		true, return_code);
	TEST_RunInterval_Helper_2(intv_list, instance.failure_list_,
		false, return_code);

	std::cout << MyEmitterSep('=') << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_RunInterval(int &return_code)
{
	char **src_list = ((char **) TEST_InstanceList);

	for (std::size_t count_1 = 0; count_1 < TEST_InstanceCount; ++count_1)
		TEST_RunInterval_Helper_1(TEST_InstanceList[count_1], return_code);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	int return_code = EXIT_SUCCESS;

	try {
		TEST_RunInterval(return_code);
	}
	catch (const std::exception &except) {
		return_code = EXIT_SUCCESS;
		std::cerr << "\nERROR: " << except.what() << std::endl;
	}

	return(return_code);
}
//	////////////////////////////////////////////////////////////////////////////

#endif // #ifdef TEST_MAIN

