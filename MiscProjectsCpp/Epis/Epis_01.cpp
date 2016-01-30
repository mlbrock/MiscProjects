// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
//	MLB Utility Library Module File
// ////////////////////////////////////////////////////////////////////////////
/*
	File Name			:	Epis_03.cpp

	File Description	:	Percentiles estimation.

	Revision History	:	2015-10-08 --- Creation.
									Michael L. Brock

	Reference(s)		:	Calculating Percentiles
								Ian Robertson, January 09, 2004
								http://web.stanford.edu/class/archive/anthsci/anthsci192/anthsci192.1064/handouts/calculating%20percentiles.pdf

		Copyright Michael L. Brock 2015 - 2016.
		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
/**
	\file MiscProjects/MiscProjectsCpp/Epis_01.cpp
*/
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
//	Required include files...
// ////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <set>
#include <vector>

#ifdef _Windows
# pragma warning(push)
# pragma warning(disable:4625 4626)
#endif // #ifdef _Windows

#include <boost/multi_index_container_fwd.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/ranked_index.hpp>
#include <boost/multi_index/key_extractors.hpp>

#ifdef _Windows
# pragma warning(pop)
#endif // #ifdef _Windows

//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef unsigned int ObservationType;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
class Epis_02;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
/*
	Simple implementation replicates results of Ian Robertson's page. Used
	to test other implementations.
*/
class Epis_00 {
	typedef std::multiset<ObservationType> MySet;
	typedef MySet::const_iterator          MySetIterC;
public:
	Epis_00()
		:obs_set_()
	{
	}

	void AddObservation(ObservationType obs)
	{
		obs_set_.insert(obs);
	}

	ObservationType GetValueForPercentile(ObservationType percentile) const
	{
		if (obs_set_.empty())
			return(0);

		MySetIterC iter_b(obs_set_.begin());
		MySetIterC iter_e(obs_set_.end());

		ObservationType last_value = 0;

		for (std::size_t count_1 = 1; iter_b != iter_e; ++iter_b, ++count_1) {
			double this_percentile = 100.0 *
				(static_cast<double>(count_1) - .5) /
				static_cast<double>(obs_set_.size());
			if (this_percentile > static_cast<double>(percentile))
				break;
			last_value = *iter_b;
		}

		return(last_value);
	}

private:
	MySet obs_set_;
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
struct DatumPoint {
	DatumPoint(ObservationType obs = 0, std::size_t occurrences = 0)
		:value_(obs)
		,occurrences_(occurrences)
	{
	}

	bool operator < (const DatumPoint &other) const
	{
		return(value_ < other.value_);
	}

	ObservationType value_;
	unsigned int    occurrences_;
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef std::set<DatumPoint>                DatumPointSet;
typedef DatumPointSet::const_iterator       DatumPointSetIterC;
typedef std::pair<DatumPointSetIterC, bool> DatumPointSetInsResult;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
class Epis_01 {
public:
	Epis_01()
		:obs_set_()
		,obs_set_total_(0)
	{
	}

	void AddObservation(ObservationType obs)
	{
		DatumPointSetInsResult ins_result(obs_set_.insert(DatumPoint(obs, 1)));

		if (!ins_result.second)
			++const_cast<DatumPoint *>(&(*ins_result.first))->occurrences_;

		++obs_set_total_;
	}

	ObservationType GetValueForPercentile(ObservationType percentile) const
	{
		if (obs_set_.empty())
			return(0);

		DatumPointSetIterC iter_b(obs_set_.begin());
		DatumPointSetIterC iter_e(obs_set_.end());

		ObservationType last_value = 0;

		for (std::size_t count_1 = 1; (count_1 <= 100) && (iter_b != iter_e);
			++iter_b) {
			unsigned int occurrences = iter_b->occurrences_;
			for (unsigned int count_2 = 0; count_2 < occurrences; ++count_2) {
				double this_percentile = 100.0 *
					(static_cast<double>(count_1) - .5) /
					static_cast<double>(obs_set_total_);
				if (this_percentile > static_cast<double>(percentile))
					return(last_value);
				++count_1;
			}
			last_value = iter_b->value_;
		}

		return(last_value);
	}

private:
	DatumPointSet obs_set_;
	unsigned int  obs_set_total_;

	friend class Epis_02;
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
struct DatumPointItem {
	DatumPointItem()
		:base_count_(0)
		,obs_count_(0)
		,obs_set_()
	{
	}

	unsigned int  base_count_;
	unsigned int  obs_count_;
	DatumPointSet obs_set_;
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef std::vector<DatumPointItem>          DatumPointItemVector;
typedef DatumPointItemVector::const_iterator DatumPointItemVectorIterC;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
class Epis_02 {
public:
	Epis_02()
		:tmp_obs_set_()
		,obs_set_total_(0)
		,obs_vector_()
	{
	}

	/*
		Complexity up to n observation inserts = 100 is O(log n).

		Conversion at that point to ~ linear (n = 100)

		Post the above, O(log n) until a percentile bucket reaches an excess/
		overflow trigger (100) at which point the excess is rebalanced to
		the nodes immediately adjacent. That operation is ~ O(n = trigger = 100).

		Note that rebalancings cascade only at the time a percentile bucket
		is the target of an observation insert for which the excess trigger
		is detected.
	*/
	void AddObservation(ObservationType obs)
	{
		if (obs_vector_.empty()) {
			tmp_obs_set_.AddObservation(obs);
			if (tmp_obs_set_.obs_set_total_ < 100)
				return;
			//	Populate;
			obs_vector_.resize(100);
			obs_set_total_ = tmp_obs_set_.obs_set_total_;
			unsigned int       interval_size = obs_set_total_ / 100;
			std::size_t        current_size  = 0;
			std::size_t        current_index = 0;
			DatumPointSetIterC iter_b(tmp_obs_set_.obs_set_.begin());
			DatumPointSetIterC iter_e(tmp_obs_set_.obs_set_.end());
			for ( ; iter_b != iter_e; ) {
				if ((current_size + iter_b->occurrences_) <= interval_size) {
					current_size += iter_b->occurrences_;
					obs_vector_[current_index].obs_set_.insert(*iter_b);
					obs_vector_[current_index].base_count_ += iter_b->occurrences_;
					obs_vector_[current_index].obs_count_  += iter_b->occurrences_;
					const_cast<DatumPoint *>(&(*iter_b))->occurrences_ = 0;
					if (current_size == interval_size) {
						current_size = 0;
						++current_index;
					}
					else
						current_size += iter_b->occurrences_;
					++iter_b;
				}
				else {
					//	Split the observation...
					DatumPoint tmp_point(*iter_b);
					tmp_point.occurrences_ -= interval_size - current_size;
					obs_vector_[current_index].obs_set_.insert(tmp_point);
					obs_vector_[current_index].base_count_ += tmp_point.occurrences_;
					obs_vector_[current_index].obs_count_  += tmp_point.occurrences_;
					const_cast<DatumPoint *>(&(*iter_b))->occurrences_ -=
						(interval_size - current_size);
					current_size = 0;
					++current_index;
				}
			}
			--iter_b;
			if (iter_b->occurrences_) {
				DatumPointSetIterC iter_f(
					obs_vector_[99].obs_set_.find(*iter_b));
				if (iter_f == obs_vector_[99].obs_set_.end())
					obs_vector_[99].obs_set_.insert(*iter_b);
				else
					const_cast<DatumPoint *>(&(*iter_f))->occurrences_ +=
						iter_b->occurrences_;
				obs_vector_[99].base_count_ += iter_b->occurrences_;
				obs_vector_[99].obs_count_  += iter_b->occurrences_;
			}
			tmp_obs_set_.obs_set_.clear();
			return;
		}

		/*
			Obs are ordered within vec, so a bisection search is possible...
			though not done in this implementation.
		*/
		for (std::size_t count_1 = 0; count_1 < 100; ++count_1) {
			DatumPointSet &this_set(obs_vector_[count_1].obs_set_);
			if (((!count_1) && (this_set.begin()->value_ >= obs)) ||
				 ((count_1 == 99) && (this_set.rbegin()->value_ <= obs)) ||
				 ((this_set.begin()->value_ <= obs) &&
				  (this_set.rbegin()->value_ >= obs))) {
				DatumPointSetInsResult ins_result(
					this_set.insert(DatumPoint(obs, 1)));
				if (!ins_result.second)
					++const_cast<DatumPoint *>(&(*ins_result.first))->occurrences_;
				++obs_vector_[count_1].obs_count_;
				if ((obs_vector_[count_1].obs_count_ -
					obs_vector_[count_1].base_count_) >= 100) {
					// Partial re-balancing...
					if (!count_1)
						RebalanceRight(count_1, 100 - 1);
					else if (count_1 == 99)
						RebalanceLeft(count_1, 100 - 1);
					else {
						RebalanceRight(count_1, 99 - count_1);
						RebalanceLeft(count_1, count_1);
					}
					obs_vector_[count_1].base_count_ =
						obs_vector_[count_1].obs_count_;
				}
			}
		}
	}

	/*
		Sums the total occurrences for unique values in the percentile
		bucket (in the call to GetElementMean(). An optimization
		(unimplemented) would be to maintain a per-bucket total value
		(adjusted with re-balancings, et cetera), which would make this
		constant-time.
	*/
	ObservationType GetValueForPercentile(ObservationType percentile) const
	{
		if ((percentile < 1) || (percentile > 99))
			return(0);

		if (obs_vector_.empty())
			return(tmp_obs_set_.GetValueForPercentile(percentile));

		return(GetElementMean(percentile));
	}

private:
	Epis_01              tmp_obs_set_;
	unsigned int         obs_set_total_;
	DatumPointItemVector obs_vector_;

	ObservationType GetElementMean(std::size_t element_index) const
	{
		double             working_total = 0.0;
		DatumPointSetIterC iter_b(obs_vector_[element_index].obs_set_.begin());
		DatumPointSetIterC iter_e(obs_vector_[element_index].obs_set_.end());

		for ( ; iter_b != iter_e; ++iter_b)
			working_total += iter_b->value_ * iter_b->occurrences_;

		return(static_cast<ObservationType>(working_total /
			static_cast<double>(obs_vector_[element_index].obs_count_)));
	}

	void RebalanceLeft(std::size_t src_index, std::size_t rebalance_count)
	{
		RebalanceHelper(src_index, rebalance_count, -1);
	}

	void RebalanceRight(std::size_t src_index, std::size_t rebalance_count)
	{
		RebalanceHelper(src_index, rebalance_count, 1);
	}

	void RebalanceHelper(std::size_t src_index, std::size_t rebalance_count,
		int direction)
	{
		DatumPointItem &src_element(obs_vector_[src_index]);
		DatumPointItem &dst_element(obs_vector_[(direction == -1) ?
			(src_index - 1) : (src_index + 1)]);
		std::size_t     remaining_size = rebalance_count;

		while (remaining_size) {
			DatumPointSetIterC src_iter((direction == -1) ?
				src_element.obs_set_.begin() : --src_element.obs_set_.end());
			if (src_iter->occurrences_ <= remaining_size) {
				remaining_size -= src_iter->occurrences_;
				DatumPointSetInsResult ins_result(
					dst_element.obs_set_.insert(*src_iter));
				if (!ins_result.second)
					const_cast<DatumPoint *>(&(*ins_result.first))->occurrences_ +=
						src_iter->occurrences_;
				dst_element.obs_count_ += src_iter->occurrences_;
				src_element.obs_set_.erase(src_iter);
			}
			else {
				DatumPoint tmp_point(*src_iter);
				tmp_point.occurrences_ -= remaining_size;
				remaining_size          = 0;
				DatumPointSetInsResult ins_result(
					dst_element.obs_set_.insert(tmp_point));
				if (!ins_result.second)
					const_cast<DatumPoint *>(&(*ins_result.first))->occurrences_ +=
						tmp_point.occurrences_;
				dst_element.obs_count_ += tmp_point.occurrences_;
			}
		}
	}
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
struct ObsDataByRank { };
struct ObsDataByAge  { };
//	----------------------------------------------------------------------------
typedef boost::multi_index_container<
	ObservationType,
	boost::multi_index::indexed_by<
		boost::multi_index::ranked_non_unique<
			boost::multi_index::tag<ObsDataByRank>,
			boost::multi_index::identity<ObservationType>
		>
		,
		boost::multi_index::sequenced<
			boost::multi_index::tag<ObsDataByAge>
		>
	>
> ObsDataMISet;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
class Epis_03 {
	typedef boost::multi_index::index<ObsDataMISet, ObsDataByRank>::type
		ObsDataMISetIdxByRank;
	typedef boost::multi_index::index<ObsDataMISet, ObsDataByAge>::type
		ObsDataMISetIdxByAge;

public:
	/*
		Zero means no size limit imposed.

		Policy is to remove the oldest observation from the set.
	*/
	explicit Epis_03(std::size_t max_size = 10000)
		:max_size_(max_size)
		,obs_set_()
	{
	}

	void AddObservation(ObservationType obs)
	{
		obs_set_.get<ObsDataByRank>().insert(obs);

		if ((max_size_ > 0) && (obs_set_.size() > max_size_))
			obs_set_.get<ObsDataByAge>().erase(
				obs_set_.get<ObsDataByAge>().begin());
	}

	ObservationType GetValueForPercentile(ObservationType percentile) const
	{
		if ((percentile < 1) || (percentile > 99) || obs_set_.empty())
			return(0);

		ObsDataMISetIdxByRank::const_iterator iter_f(
			obs_set_.get<ObsDataByRank>().nth(((obs_set_.size() - 1) *
			percentile) / 100));

		return(*iter_f);
	}

private:
	std::size_t  max_size_;
	ObsDataMISet obs_set_;
};
//	////////////////////////////////////////////////////////////////////////////

namespace {

//	////////////////////////////////////////////////////////////////////////////
ObservationType MyTestList_1[] = { 5, 1, 9, 3, 14, 9, 7 };
std::size_t     MyTestCount_1  =
	sizeof(MyTestList_1) / sizeof(MyTestList_1[0]);
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
std::size_t     MyTestCount_2     = 200;
ObservationType MyTestList_2[200] = {1, 3, 5, 7, 9, 9, 14, 14, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211};
// ////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType>
	void TEST_Report(const DatumType &datum, std::size_t src_count,
		ObservationType percentile)
{
	std::cout << "Src count=" << std::setw(4) << src_count << ", percentile=" <<
		std::setw(3) << percentile << " --- Result=" << std::setw(4) <<
		datum.GetValueForPercentile(percentile) << std::endl;
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType>
	DatumType TEST_BasicTest(std::size_t src_count,
		const ObservationType *src_list, ObservationType percentile,
		bool shuffle_flag)
{
	DatumType                    datum;
	std::vector<ObservationType> src_data(src_list, src_list + src_count);

	if (shuffle_flag)
		std::random_shuffle(src_data.begin(), src_data.end());

	for (std::size_t count_1 = 0; count_1 < src_count; ++count_1)
		datum.AddObservation(src_data[count_1]);

	TEST_Report(datum, src_count, percentile);

	return(datum);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_RunMeForSrc(std::size_t src_count, const ObservationType *src_list,
	ObservationType percentile, bool shuffle_flag)
{
	Epis_00 epis_00(TEST_BasicTest<Epis_00>(src_count, src_list, percentile,
		shuffle_flag));
	Epis_01 epis_01(TEST_BasicTest<Epis_01>(src_count, src_list, percentile,
		shuffle_flag));
	Epis_02 epis_02(TEST_BasicTest<Epis_02>(src_count, src_list, percentile,
		shuffle_flag));
	Epis_03 epis_03(TEST_BasicTest<Epis_03>(src_count, src_list, percentile,
		shuffle_flag));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_RunMe(bool shuffle_flag)
{
	TEST_RunMeForSrc(MyTestCount_1, MyTestList_1, 50, shuffle_flag);
	TEST_RunMeForSrc(MyTestCount_2, MyTestList_2, 50, shuffle_flag);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_RunPopulate(int src_count)
{
	Epis_02 datum;

	for (int count_1 = 0; count_1 < src_count; ++count_1)
		datum.AddObservation(static_cast<ObservationType>(::rand()));

	TEST_Report(datum, static_cast<std::size_t>(src_count), 50);
}
//	////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

//	////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	int return_code = EXIT_SUCCESS;

	try {
		if (argc < 2)
			TEST_RunMe(true);
#ifdef __GNUC__
		else if (!::strcasecmp(argv[1], "noshuffle"))
#else
		else if (!::stricmp(argv[1], "noshuffle"))
#endif // #ifndef __GNUC__
			TEST_RunMe(false);
		else
			TEST_RunPopulate(::atoi(argv[1]));
	}
	catch (const std::exception &except) {
		std::cerr << "ERROR: " << except.what() << std::endl;
		return_code = EXIT_FAILURE;
	}

	return(return_code);
}
//	////////////////////////////////////////////////////////////////////////////





