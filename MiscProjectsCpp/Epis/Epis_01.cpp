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
class Epis_02;
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

	void AddObservation(ObservationType obs)
	{
		if (obs_vector_.empty()) {
			tmp_obs_set_.AddObservation(obs);
//			AddObservationPrep(obs);
/*
			if (tmp_obs_set_.obs_set_.size() < 100)
				return;
*/
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
					current_index = 0;
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
		if ((percentile < 1) || (percentile > 100))
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

	void AddObservationPrep(ObservationType obs)
	{
		tmp_obs_set_.AddObservation(obs);
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
const ObservationType MyTestList_1[] = { 5, 1, 9, 3, 14, 9, 7 };
const std::size_t     MyTestCount_1  =
	sizeof(MyTestList_1) / sizeof(MyTestList_1[0]);
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
const std::size_t     MyTestCount_2     = 200;
const ObservationType MyTestList_2[200] = {1, 3, 5, 7, 9, 9, 14, 14, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211};
const ObservationType MyTestIdx_2[200]  = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200};
const double          MyTestPerc_2[200] = {0.25, 0.75, 1.25, 1.75, 2.25, 2.75, 3.25, 3.75, 4.25, 4.75, 5.25, 5.75, 6.25, 6.75, 7.25, 7.75, 8.25, 8.75, 9.25, 9.75, 10.25, 10.75, 11.25, 11.75, 12.25, 12.75, 13.25, 13.75, 14.25, 14.75, 15.25, 15.75, 16.25, 16.75, 17.25, 17.75, 18.25, 18.75, 19.25, 19.75, 20.25, 20.75, 21.25, 21.75, 22.25, 22.75, 23.25, 23.75, 24.25, 24.75, 25.25, 25.75, 26.25, 26.75, 27.25, 27.75, 28.25, 28.75, 29.25, 29.75, 30.25, 30.75, 31.25, 31.75, 32.25, 32.75, 33.25, 33.75, 34.25, 34.75, 35.25, 35.75, 36.25, 36.75, 37.25, 37.75, 38.25, 38.75, 39.25, 39.75, 40.25, 40.75, 41.25, 41.75, 42.25, 42.75, 43.25, 43.75, 44.25, 44.75, 45.25, 45.75, 46.25, 46.75, 47.25, 47.75, 48.25, 48.75, 49.25, 49.75, 50.25, 50.75, 51.25, 51.75, 52.25, 52.75, 53.25, 53.75, 54.25, 54.75, 55.25, 55.75, 56.25, 56.75, 57.25, 57.75, 58.25, 58.75, 59.25, 59.75, 60.25, 60.75, 61.25, 61.75, 62.25, 62.75, 63.25, 63.75, 64.25, 64.75, 65.25, 65.75, 66.25, 66.75, 67.25, 67.75, 68.25, 68.75, 69.25, 69.75, 70.25, 70.75, 71.25, 71.75, 72.25, 72.75, 73.25, 73.75, 74.25, 74.75, 75.25, 75.75, 76.25, 76.75, 77.25, 77.75, 78.25, 78.75, 79.25, 79.75, 80.25, 80.75, 81.25, 81.75, 82.25, 82.75, 83.25, 83.75, 84.25, 84.75, 85.25, 85.75, 86.25, 86.75, 87.25, 87.75, 88.25, 88.75, 89.25, 89.75, 90.25, 90.75, 91.25, 91.75, 92.25, 92.75, 93.25, 93.75, 94.25, 94.75, 95.25, 95.75, 96.25, 96.75, 97.25, 97.75, 98.25, 98.75, 99.25, 99.75};
//, ////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename DatumType>
	DatumType TEST_BasicTest(std::size_t src_count,
		const ObservationType *src_list, ObservationType percentile)
{
	DatumType datum;

	std::cout << "Src count=" << src_count;

	for (std::size_t count_1 = 0; count_1 < src_count; ++count_1)
		datum.AddObservation(src_list[count_1]);

	std::cout << " percentile=" << percentile << " --- Result=" <<
		datum.GetValueForPercentile(percentile) << std::endl;

	return(datum);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_RunMeForSrc(std::size_t src_count, const ObservationType *src_list,
	ObservationType percentile)
{
	Epis_00 epis_00(TEST_BasicTest<Epis_00>(src_count, src_list, percentile));
	Epis_01 epis_01(TEST_BasicTest<Epis_01>(src_count, src_list, percentile));
	Epis_02 epis_02(TEST_BasicTest<Epis_02>(src_count, src_list, percentile));
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_RunMe()
{
	TEST_RunMeForSrc(MyTestCount_1, MyTestList_1, 50);
	TEST_RunMeForSrc(MyTestCount_2, MyTestList_2, 50);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void TEST_RunPopulate(int /* rand_seed */)
{
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

