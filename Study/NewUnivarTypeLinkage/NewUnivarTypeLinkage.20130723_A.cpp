//	////////////////////////////////////////////////////////////////////////////

#include <mbtypes.h>

#include <Utility.hpp>
#include <Utility/ParseNumericString.hpp>

#include <set>
#include <string>

//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
class TypeDirect_Base {
protected:
	typedef std::string                  ValueType;
	typedef std::set<std::string>        ValueTypeSet;
	typedef ValueTypeSet::iterator       ValueTypeSetIter;
	typedef ValueTypeSet::const_iterator ValueTypeSetIterC;

	TypeDirect_Base()
		:select_all_flag_(false)
		,select_include_set_string_()
		,select_exclude_set_string_()
	{
	}

	virtual ~TypeDirect_Base()
	{
	}

public:
	virtual bool HasValue() const = 0;
	virtual void ParseSelectSpec(const ValueType &select_string);
	virtual bool IsSelected() const;
	virtual void ClearAccumulator();
	virtual void SetAccumulator(const ValueType &set_value);
	virtual void GetAccumulator(ValueType &get_value);
	virtual void Accumulate(const ValueType &add_value);

protected:
	bool         select_all_flag_;
	ValueTypeSet select_include_set_string_;
	ValueTypeSet select_exclude_set_string_;

	template <typename DerivedType>
		bool IsSelectedSupport(const std::set<DerivedType> &include_set,
			const std::set<DerivedType> &exclude_set,
			const DerivedType &value) const;
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename DerivedType> bool TypeDirect_Base::IsSelectedSupport(
	const std::set<DerivedType> &include_set,
	const std::set<DerivedType> &exclude_set, const DerivedType &value) const
{
	//	No criteria (+).
	if (select_all_flag_)
		return(true);

	//	Include set specified, but not found in that set...
	if ((!include_set.empty()) &&
		(include_set.find(value) == include_set.end()))
		return(false);

	//	Exclude set specified and found in that set...
	if ((!exclude_set.empty()) &&
		(exclude_set.find(value) != exclude_set.end()))
		return(false);

	return(true);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType>
	class TypeDirect_Numeric : public TypeDirect_Base {
public:
	typedef DatumType                             ValueType;
	typedef std::set<ValueType>                   ValueTypeSet;
	typename typedef ValueTypeSet::iterator       ValueTypeSetIter;
	typename typedef ValueTypeSet::const_iterator ValueTypeSetIterC;

	TypeDirect_Numeric()
		:TypeDirect_Base()
		,value_ptr_(NULL)
		,accumulator_(static_cast<ValueType>(0))
		,select_include_set_()
		,select_exclude_set_()
	{
	}

	virtual void ParseSelectSpec(const std::string &select_string)
	{
		select_include_set_.clear();
		select_exclude_set_.clear();

		TypeDirect_Base::ParseSelectSpec(select_string);

		if (select_all_flag_)
			return;

		ParseSelectSpecToValueType(select_include_set_string_,
			select_include_set_);
		ParseSelectSpecToValueType(select_exclude_set_string_,
			select_exclude_set_);
	}

	virtual bool IsSelected() const
	{
		return(HasValue() && IsSelectedSupport(select_include_set_,
			select_exclude_set_, *value_ptr_));
	}

	virtual bool HasValue() const
	{
		return(value_ptr_ != NULL);
	}

	virtual void ClearAccumulator()
	{
		accumulator_ = static_cast<ValueType>(0);
	}

	virtual void SetAccumulator(const ValueType &set_value)
	{
		accumulator_ = set_value;
	}

	virtual void GetAccumulator(ValueType &get_value) const
	{
		get_value = accumulator_;
	}

	virtual void Accumulate(const ValueType &add_value)
	{
		accumulator_ += add_value;
	}

protected:
	const ValueType *value_ptr_;
	ValueType        accumulator_;
	ValueTypeSet     select_include_set_;
	ValueTypeSet     select_exclude_set_;

private:
	static void ParseSelectSpecToValueType(
		const TypeDirect_Base::ValueTypeSet &src_set, ValueTypeSet &dst_set)
	{
		typename TypeDirect_Base::ValueTypeSetIterC iter_b(src_set.begin());
		typename TypeDirect_Base::ValueTypeSetIterC iter_e(src_set.end());

		for ( ; iter_b != iter_e; ++iter_b)
			dst_set.insert(MLB::Utility::CheckIsNumericString<ValueType>(*iter_b));
	}
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef TypeDirect_Numeric<Native_SInt8>   TypeDirect_SInt8;
typedef TypeDirect_Numeric<Native_UInt8>   TypeDirect_UInt8;
typedef TypeDirect_Numeric<Native_SInt16>  TypeDirect_SInt16;
typedef TypeDirect_Numeric<Native_UInt16>  TypeDirect_UInt16;
typedef TypeDirect_Numeric<Native_SInt32>  TypeDirect_SInt32;
typedef TypeDirect_Numeric<Native_UInt32>  TypeDirect_UInt32;
typedef TypeDirect_Numeric<Native_SInt64>  TypeDirect_SInt64;
typedef TypeDirect_Numeric<Native_UInt64>  TypeDirect_UInt64;
typedef TypeDirect_Numeric<Native_Float32> TypeDirect_Float32;
typedef TypeDirect_Numeric<Native_Float64> TypeDirect_Float64;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_SimpleSInt()
{
	TypeDirect_SInt64 try_me;
}
//	////////////////////////////////////////////////////////////////////////////

