//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////

#include <xlcall.h>

#include <intrin.h>	//	For popcnt(), et cetera.

//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
//	****************************************************************************
//	****************************************************************************
//	****************************************************************************
//	////////////////////////////////////////////////////////////////////////////

namespace MLB {

namespace XllInterface {

//	CODE NOTE: Use short name = Xli?

//	////////////////////////////////////////////////////////////////////////////
#ifdef ExcelInterface_4
typedef WORD           XlOperType;
typedef double         XlOperNum;
typedef char           XlOperChar;
typedef char          *XlOperStr;
typedef Native_UInt8   XlOperStrLen;
typedef WORD           XlOperBool;
typedef WORD           XlOperErr;
typedef Native_SInt16  XlOperWInt;
typedef XLREF12        XlOperSRefItem;
typedef XLMREF12       XlOperMRefItem;
typedef WORD           XlOperRows;
typedef WORD           XlOperCols;
typedef FP             XlOperFp;
typedef Native_UInt16  XlOperFpRow;
typedef Native_UInt16  XlOperFpCol;
typedef IDSHEET        XlOperSheetId;
#else
typedef DWORD          XlOperType;
typedef double         XlOperNum;
typedef XCHAR          XlOperChar;
typedef XCHAR         *XlOperStr;
typedef Native_SInt16  XlOperStrLen;
typedef BOOL           XlOperBool;
typedef Native_SInt32  XlOperErr;
typedef Native_SInt32  XlOperWInt;
typedef XLREF12        XlOperSRefItem;
typedef XLMREF12       XlOperMRefItem;
typedef RW             XlOperRows;
typedef COL            XlOperCols;
typedef FP12           XlOperFp;
typedef INT32          XlOperFpRow;
typedef INT32          XlOperFpCol;
typedef IDSHEET        XlOperSheetId;
#endif // #ifdef ExcelInterface_4
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const int XlOperTypeMask_Data = 
	xltypeNum		|
	xltypeStr		|
	xltypeBool		|
	xltypeRef		|
	xltypeErr		|
	xltypeFlow		|
	xltypeMulti		|
	xltypeMissing	|
	xltypeNil		|
	xltypeSRef		|
	xltypeInt;

const int XlOperTypeMask_Free = 
	xlbitXLFree	|
	xlbitDLLFree;

const int XlOperTypeMask_All = XlOperTypeMask_Data | XlOperTypeMask_Free;

//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
inline int GetXlOperTypeData(int xl_oper_type)
{
	return(xl_oper_type & XlOperTypeMask_Data);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
inline int GetXlOperTypeFree(int xl_oper_type)
{
	return(xl_oper_type & XlOperTypeMask_Free);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
bool IsValidXlOperTypeData(int xl_oper_type, bool throw_on_error = true);
bool CheckValidXlOperTypeData(int xl_oper_type);
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *GetXlOperTypeStringData(int xl_oper_type)
const char *GetXlOperTypeStringFree(int xl_oper_type)

const char *GetXlOperTypeStringDataShort(int xl_oper_type)
const char *GetXlOperTypeStringFreeShort(int xl_oper_type)
//	////////////////////////////////////////////////////////////////////////////

} // namespace XllInterface

} // namespace MLB

//	////////////////////////////////////////////////////////////////////////////
//	****************************************************************************
//	****************************************************************************
//	****************************************************************************
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
bool IsValidXlOperTypeData(int xl_oper_type, bool throw_on_error = true)
{
	using namespace MLB::Utility;

	bool return_code       = true;
	int  xl_oper_type_data = GetXlOperTypeData(xl_oper_type);
	int  xl_oper_type_free = GetXlOperTypeFree(xl_oper_type);

	try {
		if (!xl_oper_type_data) {
			if (!throw_on_error)
				return(false);
			ThrowInvalidArgument("No xltype data bits are set.");
		}
		if (xl_oper_type_data != xltypeBigData) {
			unsigned int bits_set = __popcnt(
				static_cast<unsigned int>(xl_oper_type_data));
			if (bits_set > 1) {
				if (!throw_on_error)
					return(false);
				std::ostringstream o_str;
				o_str << "There are " << bits_set << " bits set among the " <<
					"xltype free indicator bits --- a maximum of one was expected "
					"(except for the combination which specifies 'xltypeBigData'.";
				ThrowInvalidArgument(o_str.str());
			}
		}
		if (xl_oper_type_free) {
			unsigned int bits_set = __popcnt(
				static_cast<unsigned int>(xl_oper_type_free));
			if (bits_set > 1) {
				if (!throw_on_error)
					return(false);
				std::ostringstream o_str;
				o_str << "There are " << bits_set << " bits set among the " <<
					"xltype free indicator bits --- a maximum of one was expected.";
				ThrowInvalidArgument(o_str.str());
			}
		}
		if (xl_oper_type & ~XlOperTypeMask_All) {
			if (!throw_on_error)
				return(false);
			unsigned int bits_set = __popcnt(
				static_cast<unsigned int>(xl_oper_type & ~XlOperTypeMask_All));
			std::ostringstream o_str;
			o_str << "There " << ((bits_set == 1) ? "is" : "are") <<
				" " << bits_set << " bit" << ((bits_set == 1) ? "" : "s") <<
				" set outside of the valid range for xltype bits."
			ThrowInvalidArgument(o_str.str());
		}
	}
	catch (const std::exception &except) {
		std::ostringstream o_str;
		o_str << "The specified xltype value (" << xl_oper_type <<
			" = " << ValueToStringBin(xl_oper_type) << ") is invalid: " <<
			except.what();
		Rethrow(except, o_str.str());
	}

	return(xl_oper_type_data);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
bool CheckValidXlOperTypeData(int xl_oper_type)
{
	return(IsValidXlOperTypeData(xl_oper_type, true));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *GetXlOperTypeStringData(int xl_oper_type)
{
	if (xl_oper_type & xltypeBigData)
		return("xltypeBigData");
	else if (xl_oper_type & xltypeNum)
		return("xltypeNum");
	else if (xl_oper_type & xltypeStr)
		return("xltypeStr");
	else if (xl_oper_type & xltypeBool)
		return("xltypeBool");
	else if (xl_oper_type & xltypeRef)
		return("xltypeRef");
	else if (xl_oper_type & xltypeErr)
		return("xltypeErr");
	else if (xl_oper_type & xltypeFlow)
		return("xltypeFlow");
	else if (xl_oper_type & xltypeMulti)
		return("xltypeMulti");
	else if (xl_oper_type & xltypeMissing)
		return("xltypeMissing");
	else if (xl_oper_type & xltypeNil)
		return("xltypeNil");
	else if (xl_oper_type & xltypeSRef)
		return("xltypeSRef");
	else if (xl_oper_type & xltypeInt)
		return("xltypeInt");

	return("***UNKNOWN-DATA-TYPE***");
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *GetXlOperTypeStringFree(int xl_oper_type)
{
	if (xl_oper_type & xlbitXLFree)
		return("xlbitXLFree");
	else if (xl_oper_type & xlbitDLLFree)
		return("xlbitDLLFree");

	return("***UNKNOWN-FREE-TYPE***")
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *GetXlOperTypeStringDataShort(int xl_oper_type)
{
	const char *ptr = GetXlOperTypeStringData(xl_oper_type);

	return((*ptr = 'x') ? (ptr + 6) : "UNKNOWN");
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const char *GetXlOperTypeStringFreeShort(int xl_oper_type)
{
	const char *ptr = GetXlOperTypeStringFreeShort(xl_oper_type);

	return((*ptr = 'x') ? (ptr + 5) : "UNKNOWN");
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	****************************************************************************
//	****************************************************************************
//	****************************************************************************
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
struct XlOper : public XLOPER12 {
	typedef XLOPER12 BaseXlOperType;

	         XlOper();
	explicit XlOper(bool src);
	explicit XlOper(double src);
	explicit XlOper(const char *src, bool extended_storage_flag = false);
	explicit XlOper(const wchar_t *src, bool extended_storage_flag = false);
	explicit XlOper(std::size_t src_length, const char *src,
		bool extended_storage_flag = false);
	explicit XlOper(std::size_t src_length, const wchar_t *src,
		bool extended_storage_flag = false);
	explicit XlOper(const std::string &src, bool extended_storage_flag = false);
	explicit XlOper(const std::wstring &src, bool extended_storage_flag = false);

	XlOper(const XLOPER12 &other);

	XlOper(const XlOper &other);

	XlOper & operator = (XlOper &other)
	{
		XlOper tmp(other);

		std::swap(*this, tmp);

		return(*this);
	}

	xlType GetType() const
	{
		return(xltype);
	}

	friend static void swap(XlOper &lhs, XlOper &rhs)
	{
		std::swap(static_cast<XLOPER12 &>(lhs), static_cast<XLOPER12 &>(rhs));
		std::swap(lhs.allocated_length_, rhs.allocated_length_);
		lhs.swap(rhs.str_);
	}

	static const char *MyTypeName() const
	{
		return("XLOPER12");
	}

private:
	std::size_t             allocated_length_;
	boost::shared_ptr<char> str_;

	void AllocateBuffer_str(const char *str_ptr, std::size_t str_length);
	void AllocateBuffer_str(const char *str_ptr);
	void AllocateBuffer_val_str(const char *val_str_ptr);
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
XlOper::XlOper()
	:XLOPER12{}
	,allocated_length_(0)
	,str_()
{
	::memset(this, '\0', sizeof(XLOPER));

	xltype = xltypeMissing;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
XlOper::XlOper(const char *src)
	:XLOPER12{}
	,allocated_length_(0)
	,str_()
{
	::memset(this, '\0', sizeof(XLOPER));

	AllocateBuffer_str(src);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
XlOper::XlOper(const std::string &src)
	:XLOPER12()
	,allocated_length_(0)
	,str_()
{
	XlOper(src.c_str()).swap(*this);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
XlOper::XlOper(const XLOPER12 &other)
	:XLOPER12(other)
	,allocated_length_(0)
	,str_()
{
	switch (xltype) {
		case xltypeNum  		:
			break;
		case xltypeStr  		:
			AllocateBuffer_val_str(val.str);
			break;
		case xltypeBool 		:
			break;
		case xltypeRef  		:
			break;
		case xltypeErr  		:
			break;
		case xltypeFlow 		:
			break;
		case xltypeMulti		:
			break;
		case xltypeMissing	:
			break;
		case xltypeNil  		:
			break;
		case xltypeSRef 		:
			break;
		case xltypeInt  		:
			break;
		default					:
***
			break;
	}
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
XlOper::XlOper(const XlOper &other)
	:XLOPER12(other)
	,allocated_length_(0)
	,str_()
{
	switch (xltype) {
		case xltypeNum  		:
			break;
		case xltypeStr  		:
			Validate(other);
			if (other.allocated_length_) {
				allocated_length_ = other.allocated_length_;
				str_.reset(new char[allocated_length_]);
				str_.get()[0] = static_cast<unsigned char>(allocated_length_);
				::memcpy(str_.get(), other.str_.get(), allocated_length_);
				str_.get()[allocated_length_ - 1] = '\0';
			}
			break;
		case xltypeBool 		:
			break;
		case xltypeRef  		:
			break;
		case xltypeErr  		:
			break;
		case xltypeFlow 		:
			break;
		case xltypeMulti		:
			break;
		case xltypeMissing	:
			break;
		case xltypeNil  		:
			break;
		case xltypeSRef 		:
			break;
		case xltypeInt  		:
			break;
		default					:
***
			break;
	}
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void XlOper::AllocateBuffer_str(const char *str_ptr, std::size_t str_length)
{
	if (str_length > std::numeric_limits<unsigned char>::max()) {
		std::ostringstream o_str;
		o_str << "The specified string content length (" << str_length <<
			") exceeds the maximum permissible for the '" << MyTypeName() <<
			"' data 'val.str' member (" <<
			std::numeric_limits<unsigned char>::max() << ").";
		MLB::Utility::ThrowInvalidArgument(o_str.str());
	}
	else if (!str_ptr) {
		str_.reset();
		allocated_length_ = 0;
		val.str           = NULL;
	}
	else {
		std::size_t allocated_length = 1 + str_length + 1;
		if (allocated_length != allocated_length_)
			str_.reset(new char[allocated_length]);
		str_.get()[0] = static_cast<unsigned char>(allocated_length);
		if (str_length)
			::memcpy(str_.get() + 1, str_ptr, str_length - 2);
		str_.get()[allocated_length - 1] = '\0';
		allocated_length_                = allocated_length;
		val.str                          = str_.get();
	}

	xltype = xltypeStr;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void XlOper::AllocateBuffer_str(const char *str_ptr)
{
	(str_ptr) ? AllocateBuffer_str(str_ptr, ::strlen(src_ptr)) :
		AllocateBuffer_str(NULL, 0);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void XlOper::AllocateBuffer_val_str(const char *val_str_ptr)
{
	(val_str_ptr) ? AllocateBuffer_str(val_str_ptr + 1,
		static_cast<std::size_t>(static_cast<unsigned char>(*val_str_ptr))) :
		AllocateBuffer_str(NULL, 0);

//	////////////////////////////////////////////////////////////////////////////

