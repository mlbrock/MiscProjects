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

#include <boost/regex/v4/regex.hpp>

#include <boost/bimap.hpp>

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
	std::ostream & ToStream(std::ostream &o_str,
		const std::vector<std::pair<DatumType, DatumType> > &datum,
		const std::string &prefix = "")
{
	for (std::size_t count_1 = 0; count_1 < datum.size(); ++count_1)
		o_str << ((count_1) ? "\n" : "") << prefix << datum[count_1];

	return(o_str);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType>
	std::ostream & ToStream(std::ostream &o_str,
		const std::map<DatumType, DatumType> &datum,
		const std::string &prefix = "")
{
	typename std::map<DatumType, DatumType>::const_iterator iter_b(datum.begin());
//	std::map<DatumType, DatumType>::const_iterator iter_e(datum.end());

	for (std::size_t count_1 = 0; count_1 < datum.size(); ++count_1, ++iter_b)
		o_str << ((count_1) ? "\n" : "") << prefix <<
			std::pair<DatumType, DatumType>(iter_b->first, iter_b->second);

	return(o_str);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType>
	std::ostream & operator << (std::ostream &o_str,
		const std::vector<std::pair<DatumType, DatumType> > &datum)
{
/*
	for (std::size_t count_1 = 0; count_1 < datum.size(); ++count_1)
		o_str << ((count_1) ? "\n" : "") << prefix << datum[count_1];

	return(o_str);
*/
	return(ToStream(o_str, datum));
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
/*
	PCRE Meta-Characters:
	---------------------
	\ --> \\\\
	^$.[](){}|?*+
*/
template <typename DatumType> struct IntervalWrapper {
//	typedef DatumType                           DatumType;
	typedef std::pair<DatumType, DatumType>         ValInterval;
	typedef std::vector<ValInterval>                ValIntervalVec;
	typedef std::map<DatumType, DatumType>          ValIntervalMap;
	typedef typename ValIntervalMap::iterator       ValIntervalMapIter;
	typedef typename ValIntervalMap::const_iterator ValIntervalMapIterC;

	typedef boost::bimap<DatumType, DatumType>        ValIntervalBiMap;
	typedef typename ValIntervalBiMap::iterator       ValIntervalBiMapIter;
	typedef typename ValIntervalBiMap::const_iterator ValIntervalBiMapIterC;

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
		ValIntervalMap           range_map;

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

RangeMapInsert(range_map, val_lo, val_hi);
			}
			catch (const std::exception &except) {
				std::ostringstream o_str;
				o_str << "Unable to parse value '" << this_element << "': " <<
					except.what();
				throw std::invalid_argument(o_str.str());
			}
		}
return(ValIntervalVec(range_map.begin(), range_map.end()));
	}

	static ValIntervalMap &RangeMapInsert(ValIntervalMap &range_map,
		const ValInterval &val_interval)
	{
		return(RangeMapInsert(range_map, val_interval.first,
			val_interval.second));
	}

	static ValIntervalMap &RangeMapInsert(ValIntervalMap &range_map,
		DatumType val_lo, DatumType val_hi)
	{
		return(RangeMapInsert(range_map, val_lo, val_hi, 0));
	}

private:

	static ValIntervalMap &RangeMapInsert(ValIntervalMap &range_map,
		DatumType val_lo, DatumType val_hi, unsigned int depth)
	{
		if (val_lo > val_hi)
			std::swap(val_lo, val_hi);

		bool merge_flag = false;

/*
		if (range_map.empty()) {
			range_map.insert(ValInterval(val_lo, val_hi));
			return(range_map);
		}
*/
		if (!range_map.empty()) {
			ValIntervalMapIter iter_b(range_map.begin());
			ValIntervalMapIter iter_e(range_map.end());
			for ( ; iter_b != iter_e ; ++iter_b) {
				if ((val_lo <= iter_b->second) &&
					 (val_hi >= iter_b->first)) {
					//	CASES: #5, #6, #7, #8, #9
					if (val_lo == iter_b->first)
						iter_b->second = std::max(iter_b->second, val_hi);
					else {
						ValInterval tmp(std::min(iter_b->first, val_lo),
							std::max(iter_b->second, val_hi));
						range_map.erase(iter_b);
						iter_b = range_map.insert(tmp).first;
					}
					if (range_map.size() > 1) {
						ValIntervalMap tmp_range_map(range_map);
						ValInterval    tmp_interval(iter_b->first, iter_b->second);
						tmp_range_map.erase(iter_b->first);
						RangeMapInsert(tmp_range_map, tmp_interval.first,
							tmp_interval.second, depth + 1);
						if (tmp_range_map != range_map)
							range_map.swap(tmp_range_map);
					}
	//				return(range_map);
					merge_flag = true;
					break;
				}
			}
		}

		//	Otherwise, it's an independent interval.
		if (!merge_flag) {
			range_map.insert(ValInterval(val_lo, val_hi));
			merge_flag = true;
		}

		/*
			Next, an ajacency search ...

			Not strictly necessary as there's no room for an integer between
			the end and beginning of two adjacent intervals. However, doing so
			does reduce the number of intervals in the map.
		*/
		if (merge_flag && (range_map.size() > 1)) {
			ValIntervalMapIter iter_b(range_map.begin());
			ValIntervalMapIter iter_e(range_map.end());
			for ( ; iter_b != iter_e ; ) {
				if (iter_b->second < std::numeric_limits<DatumType>::max()) {
					ValIntervalMapIter iter_f(range_map.find(iter_b->second + 1));
					if (iter_f == range_map.end())
						++iter_b;
					else {
						iter_b->second = iter_f->second;
						range_map.erase(iter_f);
					}
				}
				else
					++iter_b;
			}
		}

		return(range_map);
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

#define TEST_MAIN
#ifdef TEST_MAIN

//	////////////////////////////////////////////////////////////////////////////
struct TEST_Instance {
	std::string                       input_;
	std::vector<std::pair<int, int> > output_;
	std::vector<int>                  success_list_;
	std::vector<int>                  failure_list_;
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const TEST_Instance TEST_InstanceList[] = {
	{
		"-",
		{
			{         -2147483647 - 1,          2147483647},
		},
	},
	{
		"-0",
		{
			{         -2147483647 - 1,                   0},
		},
		{ -1, -42 },
		{  2,   3 }
	},
	{
		"-1",
		{
			{         -2147483647 - 1,                   1},
		},
	},
	{
		"0-",
		{
			{                   0,          2147483647},
		},
	},
	{
		"1-",
		{
			{                   1,          2147483647},
		},
	},
	{
		"0-0",
		{
			{                   0,                   0},
		},
	},
	{
		"0-1",
		{
			{                   0,                   1},
		},
	},
	{
		"0-9",
		{
			{                   0,                   9},
		},
	},
	{
		"7-9",
		{
			{                   7,                   9},
		},
		{ 7, 8 },
		{ -42,  -1, 0, 1, 42 }
	},
	{
		"7-9,17,18",
		{
			{                   7,                   9},
			{                  17,                  18},
		},
		{ 7, 8 },
		{ -42,  -1, 0, 1, 42 }
	},
	{
		"7-9,19,100-2399",
		{
			{                   7,                   9},
			{                  19,                  19},
			{                 100,                2399},
		},
		{ 7, 8, 19, 2000 },
		{ -42,  -1, 0, 1, 42 }
	},
	{
		"7-9,19,100-2399,6-2400",
		{
			{                   6,                2400},
		},
		{ 7, 8, 19, 2000 },
		{ -42,  -1, 0, 1 }
	},
	{
		"6-2400,7-9,19,100-2399",
		{
			{                   6,                2400},
		},
		{ 7, 8, 19, 2000 },
		{ -42,  -1, 0, 1 }
	},
	{
		"10-19,30-39,50-59,70-79,90-99,1-100",
		{
			{                   1,                 100},
		},
		{10, 100 },
		{ 0, 101 }
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

	if (instance.output_.empty())
		std::cout << "RESULT  :\n" << intv_list << std::endl;
	else if (intv_list == instance.output_)
		std::cout << "EXPECTED:\n" << intv_list << std::endl;
	else {
		std::cout << "EXPECTED:\n" << instance.output_ << std::endl;
		std::cout << "ACTUAL  :\n" << intv_list        << std::endl;
	}

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
typedef int                                       DatumType;
typedef std::pair<DatumType, DatumType>           ValInterval;
typedef boost::bimap<DatumType, DatumType>        ValIntervalBiMap;
typedef typename ValIntervalBiMap::iterator       ValIntervalBiMapIter;
typedef typename ValIntervalBiMap::const_iterator ValIntervalBiMapIterC;
typedef typename ValIntervalBiMap::left_const_iterator ValIntervalBiMapLIterC;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_RunBiMap_Helper(ValIntervalBiMap &range_bimap,
	const ValInterval &new_value)
{
	if (range_bimap.empty()) {
		range_bimap.left.insert(new_value);
		return;
	}

//	ValIntervalBiMapLIterC iter_f_l(range_bimap.left.find(new_value.first));
	ValIntervalBiMapLIterC iter_ub_l(range_bimap.left.upper_bound(new_value.second));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_RunBiMap()
{
	ValIntervalBiMap range_bimap;

//	range_map.insert(ValInterval(5, 10));
	TEST_RunBiMap_Helper(range_bimap, ValInterval(5, 10));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	int return_code = EXIT_SUCCESS;

	try {
//		TEST_RunBiMap();
		TEST_RunInterval(return_code);
	}
	catch (const std::exception &except) {
		return_code = EXIT_FAILURE;
		std::cerr << "\nERROR: " << except.what() << std::endl;
	}

	if (return_code = EXIT_SUCCESS)
		std::cerr << "Regression test failed.\n" << std::endl;

	return(return_code);
}
//	////////////////////////////////////////////////////////////////////////////

#endif // #ifdef TEST_MAIN
