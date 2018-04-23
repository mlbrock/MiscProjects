//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
/*
	File Name			:	TimeSupport.cpp

	File Description	:	Unix epoch time support logic.

	Revision History	:	2005-02-28 --- Creation.
									Michael L. Brock

		Copyright Michael L. Brock 2005 - 2018.

		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
//	////////////////////////////////////////////////////////////////////////////

namespace MLB {

namespace UtilityCli {

namespace {

//	////////////////////////////////////////////////////////////////////////////
public ref class UnixEpochTimeSupport {
public:
	static System::DateTime GetUnixEpochDateTimeUtc()
	{
		return(UnixEpochDateTimeUtc);
	}	

//private:
	static System::DateTime UnixEpochDateTimeUtc =
		System::DateTime(1970, 1, 1, 0, 0, 0, System::DateTimeKind::Utc);
};
//	////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

//	////////////////////////////////////////////////////////////////////////////
public ref class ConvertTime_timespec {
public:
	static System::DateTime GetUnixEpochDateTimeUtc()
	{
		return(UnixEpochTimeSupport::UnixEpochDateTimeUtc);
	}

	static struct timespec Convert(System::DateTime %src_datetime)
	{
		struct timespec dst_datetime;
		long long       unix_ticks =
			src_datetime.Ticks - UnixEpochTimeSupport::UnixEpochDateTimeUtc.Ticks;

		dst_datetime.tv_sec  =  unix_ticks / 10000000;
		dst_datetime.tv_nsec = (unix_ticks % 10000000) * 100;

		return(dst_datetime);
	}

	static System::DateTime Convert(System::DateTime &src)
	{
		return(Convert(src.tv_sec, src.tv_nsec));
	}

	static System::DateTime Convert(std::time_t secs, long nsecs)
	{
		return(UnixEpochTimeSupport::UnixEpochDateTimeUtc.AddTicks(
			(secs * TimeSpan::TicksPerSecond) +
			(nsecs / (1000000000 / TimeSpan::TicksPerSecond))));
	}
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
public ref class ConvertTime_timeval {
public:
	static System::DateTime GetUnixEpochDateTimeUtc()
	{
		return(UnixEpochTimeSupport::UnixEpochDateTimeUtc);
	}

	static struct timeval Convert(System::DateTime %src_datetime)
	{
		timespec tmp_timespec = ConvertTime_timespec::Convert(src_datetime);
		timeval  tmp_timeval;

		tmp_timeval.tv_sec  = tmp_timespec.tv_sec;
		tmp_timeval.tv_usec = tmp_timespec.tv_usec / 1000;

		return(tmp_timeval);
	}

	static System::DateTime Convert(System::DateTime &src)
	{
		return(Convert(src.tv_sec, src.tv_usec));
	}

	static System::DateTime Convert(std::time_t secs, long usecs)
	{
		return(UnixEpochTimeSupport::UnixEpochDateTimeUtc.AddTicks(
			(secs * TimeSpan::TicksPerSecond) +
			(usecs / (1000000 / TimeSpan::TicksPerSecond))));
	}
};
//	////////////////////////////////////////////////////////////////////////////

} // namespace UtilityCli
 
} //	namespace MLB

