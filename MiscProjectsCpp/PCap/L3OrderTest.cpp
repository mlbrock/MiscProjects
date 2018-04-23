//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	MLB Test Module File
//	////////////////////////////////////////////////////////////////////////////
/*
	File Name  			:	L3OrderTest.cpp

	File Description	:	Test of a new level three order book implementation.

	Revision History	:	2016-12-22 --- Creation
									Michael L. Brock

		Copyright Michael L. Brock 2016 - 2018.
		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
//	////////////////////////////////////////////////////////////////////////////
 
//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	Required include files...
//	////////////////////////////////////////////////////////////////////////////

#include <stdexcept>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>

#include <mbtypes.h>

#include <boost/shared_ptr.hpp>

#if defined(_MSC_VER)
# include <Windows.h>
# ifndef _Windows
#  define _Windows	1
# endif // # ifndef _Windows
#endif // #if defined(_MSC_VER)

#ifdef _Windows
# pragma warning(push)
# pragma warning(disable:4625 4626)
#endif // #ifdef _Windows

#include <boost/multi_index_container_fwd.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/key_extractors.hpp>

#ifdef _Windows
# pragma warning(pop)
#endif // #ifdef _Windows

//	////////////////////////////////////////////////////////////////////////////

namespace MLB {

namespace Utility {

namespace {

//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	Used to get high-granularity times under Windows...
//	////////////////////////////////////////////////////////////////////////////
#ifdef _Windows

# if defined(_MSC_VER) && (_MSC_VER < 1900)
struct timespec {
	time_t tv_sec;
	long   tv_nsec;
};
# endif // # if defined(_MSC_VER) && (_MSC_VER < 1900)

typedef union {
	FILETIME         struct_data;
	unsigned __int64 scalar_data;
} WIN32_FILETIME_64;
# ifdef __MINGW32__
#  define WIN32_FILETIME_EPOCH_BIAS	116444736000000000LL
# else
#  define WIN32_FILETIME_EPOCH_BIAS	116444736000000000i64
# endif // # ifdef __MINGW32__
#endif // #ifdef _Windows
//	////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

//	////////////////////////////////////////////////////////////////////////////
typedef timeval  TimeVal;
typedef timespec TimeSpec;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
inline Native_UInt64 GetHRTime()
{
	TimeSpec out_time;

#ifdef __MSDOS__
	struct timeb struct_timeb;

	ftime(&struct_timeb);
	out_time.tv_sec  = struct_timeb.time;
	out_time.tv_nsec = ((long) struct_timeb.millitm) * 1000000L;
#elif _Windows
# ifdef __MINGW32__
	WIN32_FILETIME_64 sys_time;

	GetSystemTimeAsFileTime(&sys_time.struct_data);

	out_time.tv_sec  = ((long) ((sys_time.scalar_data -
		WIN32_FILETIME_EPOCH_BIAS) / 10000000LL));
	out_time.tv_nsec = ((long) ((sys_time.scalar_data * 100LL) %
		1000000000LL));
# else
	WIN32_FILETIME_64 sys_time;

	GetSystemTimeAsFileTime(&sys_time.struct_data);

	out_time.tv_sec  = ((long) ((sys_time.scalar_data -
		WIN32_FILETIME_EPOCH_BIAS) / 10000000i64));
	out_time.tv_nsec = ((long) ((sys_time.scalar_data * 100i64) %
		1000000000i64));
# endif // # ifdef __MINGW32__
#elif _POSIX_TIMERS
	clock_gettime(CLOCK_MONOTONIC, &out_time);
#else
	TimeVal tmp_timeval;

	::gettimeofday(&tmp_timeval, NULL);

	out_time.tv_sec  = tmp_timeval.tv_sec;
	out_time.tv_nsec = tmp_timeval.tv_usec * 1000L;
#endif // #ifdef __MSDOS__

	return((static_cast<Native_UInt64>(out_time.tv_sec) *
		static_cast<Native_UInt64>(1000000000)) +
		static_cast<Native_UInt64>(out_time.tv_nsec));
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace Utility

} // namespace MLB

//	////////////////////////////////////////////////////////////////////////////
struct MbOrder {
	explicit MbOrder(Native_UInt64 order_id = 0, Native_UInt64 price = 0,
		Native_UInt64 priority = 0, Native_UInt32 security_id = 0,
		Native_UInt32 quantity = 0)
		:order_id_(order_id)
		,price_(price)
		,priority_(priority)
		,security_id_(security_id)
		,quantity_(quantity)
	{
	}

	Native_UInt64 order_id_;
	Native_UInt64 price_;
	Native_UInt64 priority_;
	Native_UInt32 security_id_;
	Native_UInt32 quantity_;
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
struct MbOrderByOid   { };
struct MbOrderByPrice { };
//	----------------------------------------------------------------------------
typedef boost::multi_index_container<
	MbOrder,
	boost::multi_index::indexed_by<
		boost::multi_index::ordered_unique<
			boost::multi_index::tag<MbOrderByOid>,
			BOOST_MULTI_INDEX_MEMBER(MbOrder, Native_UInt64, order_id_)
		>
		,
		boost::multi_index::ordered_non_unique<
			boost::multi_index::tag<MbOrderByPrice>,
			BOOST_MULTI_INDEX_MEMBER(MbOrder, Native_UInt64, price_)
		>
	>
> MbOrderXXXMISet;
//	////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
typedef boost::multi_index::index<MbOrderXXXMISet,
	MbOrderByOid>::type                       MbOrderXXXMISetIdxByOid;
typedef boost::multi_index::index<MbOrderXXXMISet,
	MbOrderByPrice>::type                     MbOrderXXXMISetIdxByPrice;
// ////////////////////////////////////////////////////////////////////////////

#ifdef __linux__
# include <strings.h>
#else
int strcasecmp(const char *lhs, const char *rhs)
{
	return(::stricmp(lhs, rhs));
}
#endif // #ifdef __linux__


//	////////////////////////////////////////////////////////////////////////////
void TEST_StressTest(std::size_t order_count = 10000,
	std::size_t iteration_count = 1)
{
	order_count     = (order_count < 1) ? 10000 : order_count;
	iteration_count = (iteration_count < 1) ? 1 : iteration_count;

	MbOrder       order;
	Native_UInt64 insert_time = 0;
	Native_UInt64 erase_time  = 0;

	for (std::size_t count_1 = 0; count_1 < iteration_count; ++count_1) {
		MbOrderXXXMISet side_orders;
		Native_UInt64   start_time = MLB::Utility::GetHRTime();
		for (std::size_t count_2 = 0; count_2 < order_count; ++count_2) {
			order.order_id_ = count_2 + 1;
			side_orders.insert(order);
		}
		Native_UInt64 end_time = MLB::Utility::GetHRTime();
		insert_time += end_time - start_time;
		start_time   = MLB::Utility::GetHRTime();
		for (std::size_t count_2 = 0; count_2 < order_count; ++count_2) {
			MbOrderXXXMISetIdxByOid::const_iterator iter_f(
				side_orders.get<MbOrderByOid>().find(
				static_cast<Native_UInt64>(count_2 + 1)));
			if (iter_f == side_orders.get<MbOrderByOid>().end())
				throw std::logic_error("Unable to locate key!");
			side_orders.get<MbOrderByOid>().erase(iter_f);
		}
		end_time    = MLB::Utility::GetHRTime();
		erase_time += end_time - start_time;
	}

	std::cout << "Iterations    : " << iteration_count   << "\n";
	std::cout << std::fixed << std::setprecision(6);
	std::cout << "Insertion Time: " << std::setw(13) <<
		(static_cast<double>(insert_time) / 1000000000.0) << "\n";
	std::cout << "Erasure Time  : " << std::setw(13) <<
		(static_cast<double>(erase_time) / 1000000000.0)  << "\n";
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	int return_code = EXIT_SUCCESS;

	try {
/*
		if (argc < 2)
			throw std::invalid_argument("Invalid command line --- use '-h' for "
				"help.");
		for (int count_1 = 1; count_1 < argc; ++count_1) {
			const char *argv_ptr = argv[count_1];
			argv_ptr += ((*argv_ptr == '-') && (argv_ptr[1] == '-')) ? 1 : 0;
			if ((!::strcasecmp(argv_ptr, "-h")) ||
				 (!::strcasecmp(argv_ptr, "-help"))) {
				std::cout << "USAGE:\n"
					"   " << argv[0] << " <cme-market-data-pcap-file> "
					"[ <cme-market-data-pcap-file> ... ]\n\n";
				return(EXIT_SUCCESS);
			}
		}
		for (int count_1 = 1; count_1 < argc; ++count_1) {
			PacketProcessorBase packet_processor;
			ProcessPCapFile(argv[count_1], packet_processor);
		}
*/
		TEST_StressTest();
	}
	catch (const std::exception &except) {
		return_code = EXIT_FAILURE;
		std::cerr << "ERROR: " << except.what() << std::endl;
	}

	return(return_code);
}
//	////////////////////////////////////////////////////////////////////////////

