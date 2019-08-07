protected:
	virtual const LineInfo & ReadLineNextImpl() const;
//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	MLB Utility Library Include File
//	////////////////////////////////////////////////////////////////////////////
/*
   File Name  			:	FileReaderMMap.cpp

   File Description	:	Include file for the FileReaderMMap class.

	Revision History	:	2019-08-04 --- Creation.
									Michael L. Brock

		Copyright Michael L. Brock 2019 - 2019.
		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
//	////////////////////////////////////////////////////////////////////////////
 
#ifndef HH__MLB__Utility__Utility__FileReaderMMap_hpp__HH

#define HH__MLB__Utility__Utility__FileReaderMMap_hpp__HH	1

//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	Required include files...
//	////////////////////////////////////////////////////////////////////////////

#ifdef _Windows
# pragma warning(push)
# pragma warning(disable:4061 4365)
#endif // #ifdef _Windows

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#ifdef _Windows
# pragma warning(pop)
#endif // #ifdef _Windows

//	////////////////////////////////////////////////////////////////////////////

namespace MLB {

namespace Utility {

//	////////////////////////////////////////////////////////////////////////////
/*
   IMPL NOTE: Or boost::string_view or std::string_view .
*/
typedef boost::string_ref MyStringView;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
struct LineInfo
{
	LineInfo(const char *src, std::size_t src_length, 
		std::size_t line_number, std::size_t line_offset)
		:line_view_(src, src_length)
		,line_data_()
		,line_number_(line_number)
		,line_offset_(line_offset)
	{
		if (!src)
			throw std::invalid_argument("LineInfo constructor src pointer "
				"parameter is NULL.");
	}

	LineInfo(const LineInfo &other)
		:line_view_(other.line_view_)
		,line_data_()
		,line_number_(other.line_number_)
		,line_offset_(other.line_offset_)
	{
		if (other.IsReified())
			Reify();
	}

	~LineInfo()
	{
	}

	const char *begin() const
	{
		return(line_view_.begin());
	}

	const char *c_str() const
	{
		return(line_view_.c_str());
	}

	void clear()
	{
		line_view_.clear();
		line_data_.clear();
	}

	const char *data() const
	{
		return(line_view_.data());
	}

	bool empty() const
	{
		return(line_view_.empty());
	}

	const char *end() const
	{
		return(line_view_.end());
	}

	const char *rbegin() const
	{
		return(line_view_.rbegin());
	}

	const char *rend() const
	{
		return(line_view_.rend());
	}

	std::size_t size() const
	{
		return(line_view_.size());
	}

	LineInfo & operator = (const LineInfo &other)
	{
		LineInfo tmp(other);

		swap(tmp);

		return(*this);
	}

	bool IsReified() const
	{
		return(line_view_.c_str() == line_data_.c_str());
	}

	void Reify()
	{
		if (!IsReified())
		{
			std::string(line_view_.c_str(), line_view_.size()).swap(line_data_);
			MyStringView(line_data_.c_str(), line_data_.size()).swap(line_view_);
		}
	}

private:
	MyStringView line_view_;
	std::string  line_data_;
	std::size_t  line_number_;
	std::size_t  line_offset_;
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
class FileReaderBase
{
public:
	FileReaderBase()
		:file_name_()
		,last_size_(0)
		,curr_size_(0)
		,line_number_(0)
		,line_offset_(0)
		,line_info_()
	{
	}

	explicit FileReaderBase(const std::string &file_name)
		:file_name_(file_name)
		,last_size_(0)
		,curr_size_(0)
		,line_number_(0)
		,line_offset_(0)
		,line_info_()
	{
	}

	virtual ~FileReader()
	{
	}

	const LineInfo &ReadLineNext() const
	{
		return(ReadLineNextImpl());
	}

	const LineInfo &ReadLinePrev() const
	{
		return(ReadLinePrevImpl());
	}

protected:
	virtual const LineInfo &ReadLineNextImpl() const = 0;

	virtual const LineInfo &ReadLinePrevImpl() const = 0;

protected:
	std::string file_name_;
	std::size_t last_size_;
	std::size_t curr_size_;
	std::size_t line_number_;
	std::size_t line_offset_;
	LineInfo    line_info_;
};
//	//////////////////////////////////////////////////////////////////////////// 

//	//////////////////////////////////////////////////////////////////////////// 
class FileReader : FileReaderBase
{
	FileReader()
		:FileReaderBase()
	{
	}

	explicit FileReader(const std::string &file_name)
		:FileReaderBase(file_name)
	{
	}

protected:
	virtual const LineInfo &ReadLineNextImpl() const
	{
	}

	virtual const LineInfo &ReadLinePrevImpl() const
	{
		if (!line_offset_)
			LineInfo(file_data_.c_str(), 0, 1, 0).swap(line_info_);

		return(line_info_);
	}

private:
	std::string file_data_;
};
//	//////////////////////////////////////////////////////////////////////////// 

// namespace Utility

// namespace MLB

#endif // #ifndef HH__MLB__Utility__Utility__FileReaderMMap_hpp__HH

