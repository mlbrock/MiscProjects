// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
//	MLB Utility Library Module File
// ////////////////////////////////////////////////////////////////////////////
/*
	File Name			:	Epis_01.cpp

	File Description	:	Percentiles estimation.

	Revision History	:	2015-10-08 --- Creation.
									Michael L. Brock

	Reference(s)		:	Calculating Percentiles
								Ian Robertson, January 09, 2004
								http://web.stanford.edu/class/archive/anthsci/anthsci192/anthsci192.1064/handouts/calculating%20percentiles.pdf

		Copyright Michael L. Brock 2015 - 2015.
		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
// ////////////////////////////////////////////////////////////////////////////


// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
//	Required include files...
// ////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <set>
#include <vector>

#include <algorithm>
#include <numeric>

//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef unsigned int ObservationType;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
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
typedef DatumPointSet::const_iterator       DatumPointSetIterC;
typedef std::pair<DatumPointSetIterC, bool> DatumPointSetInsResult;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
class Epis_01 {
public:
	Epis_01()
		:obs_set_()
	{
	}

	void AddObservation(ObservationType obs)
	{
		DatumPointSetInsResult ins_result(obs_set_.insert(DatumPoint(obs, 1)));

		if (!ins_result.second)
			const_cast<DatumPoint *>(&(*ins_result.first))->occurrences_++;
	}

private:
	DatumPointSet obs_set_;
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
		:obs_set_()
		,obs_set_total_(0)
		,obs_vector_()
	{
	}

	void AddObservation(ObservationType obs)
	{
		if (obs_vector_.empty()) {
			AddObservationPrep(obs);
			if (obs_set_.size() < 100)
				return;
			//	Populate;
			obs_vector_.resize(100);
			unsigned int       interval_size = obs_set_total_ / 100;
			std::size_t        current_size  = 0;
			std::size_t        current_index = 0;
			DatumPointSetIterC iter_b(obs_set_.begin());
			DatumPointSetIterC iter_e(obs_set_.end());
			for ( ; iter_b != iter_e; ) {
				if ((current_size + iter_b->occurrences_) <= interval_size) {
					current_size += iter_b->occurrences_;
					obs_vector_[current_index].obs_set_.insert(*iter_b);
					const_cast<DatumPoint *>(&(*iter_b))->occurrences_ = 0;
					obs_vector_[current_index].base_count_ += iter_b->occurrences_;
					obs_vector_[current_index].obs_count_  += iter_b->occurrences_;
					if (current_size == interval_size)
						++current_index;
					++iter_b;
				}
				else {
					//	Split the observation...
					DatumPoint tmp_point(*iter_b);
					tmp_point.occurrences_ -= current_size;
					obs_vector_[current_index].obs_set_.insert(tmp_point);
					obs_vector_[current_index].base_count_ += tmp_point.occurrences_;
					obs_vector_[current_index].obs_count_  += tmp_point.occurrences_;
					const_cast<DatumPoint *>(&(*iter_b))->occurrences_ -=
						(interval_size - current_size);
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

			obs_set_.clear();

			return;
		}

		//	Obs are ordered within vec, so a bisection search is possible...
		for (std::size_t count_1 = count_1; count_1 < 100; ++count_1) {
			if ((obs_vector_[count_1].obs_set_.begin()->value_ <= obs) &&
				 (obs_vector_[count_1].obs_set_.rbegin()->value_ >= obs)) {
				DatumPointSetInsResult ins_result(
					obs_vector_[count_1].obs_set_.insert(DatumPoint(obs, 1)));
				if (!ins_result.second)
					++const_cast<DatumPoint *>(&(*ins_result.first))->occurrences_;
				++obs_vector_[count_1].obs_count_;
				if ((obs_vector_[count_1].obs_count_ -
					obs_vector_[count_1].base_count_) >= 100) {
					// Partial re-balancing...
					if (!count_1)
						RebalanceRight(count_1, 100);
					else if (count_1 == 99)
						RebalanceLeft(count_1, 100);
					else {
						RebalanceRight(count_1, 99 - count_1);
						RebalanceLeft(count_1, count_1 - 1);
					}
				}
			}
		}
	}
#if 0
#endif // #if 0

	ObservationType GetValueForPercentile(ObservationType percentile) const
	{
		if ((obs_set_.size() < 100) || (percentile < 1) || (percentile > 100))
			return(0);

		return(GetElementMean(percentile));
	}

private:
	DatumPointSet        obs_set_;
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

	void AddObservationPrep(ObservationType obs)
	{
		DatumPointSetInsResult ins_result(obs_set_.insert(DatumPoint(obs, 1)));

		if (!ins_result.second)
			const_cast<DatumPoint *>(&(*ins_result.first))->occurrences_++;

		++obs_set_total_;
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
#if 0
#endif // #if 0

};
//	////////////////////////////////////////////////////////////////////////////

/*
//	////////////////////////////////////////////////////////////////////////////
class Epis_03 {
	std::size_t PercGran = 100;
public:
	Epis_03()
		:obs_set_()
	{
	}

	void AddObservation(ObservationType obs)
	{
		DatumPointSetInsResult ins_result(obs_set_.insert(DatumPoint(obs, 1)));

		if (!ins_result.second)
			const_cast<DatumPoint *>(&(*ins_result.first))->occurrences_++;
	}

	ObservationType GetValueForPercentile(ObservationType percentile) const
	{
		if ((percentile < 1) || (percentile > 100))
			return(0);

		for ()
		return(obs_vector_[percentile])
	}

private:
	DatumPointSet obs_set_;
}
//	////////////////////////////////////////////////////////////////////////////
*/

namespace {

//	////////////////////////////////////////////////////////////////////////////
const ObservationType MyTestList[] = { 5, 1, 9, 3, 14, 9, 7 };
const std::size_t     MyTestCount  = sizeof(MyTestList) / sizeof(MyTestList[0]);
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_RunPopulate(int value_count)
{
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType> DatumType &TEST_BasicTest()
{
	DatumType datum;

	for (std::size_t count_1 = 0; count_1 < MyTestCount; ++count_1)
		datum.AddObservation(MyTestList[count_1]);

	std::cout << datum.GetValueForPercentile(50) << std::endl;

	return(datum);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_RunMe()
{
	Epis_00 epis_00(TEST_BasicTest<Epis_00>());
	Epis_02 epis_02(TEST_BasicTest<Epis_02>());
}
//	////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

//	////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	int return_code = EXIT_SUCCESS;

	try {
		if (argc < 2)
			TEST_RunMe();
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

