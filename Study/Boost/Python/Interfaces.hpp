//	****************************************************************************
//	////////////////////////////////////////////////////////////////////////////
//	LOCAL_PythonWrappers.hpp
//	----------------------------------------------------------------------------

//	////////////////////////////////////////////////////////////////////////////
#include <BoostX/BoostPython.hpp>
#include ***LOCAL-INCLUDES***

#ifdef _Windows
# pragma warning(disable: 4100 4121 4217 4244 4267 4511 4512 4625 4626 4640 4668 4686)
#endif // #ifdef _Windows

#include <boost/python.hpp>

#ifdef _Windows
# pragma warning(default: 4100 4121 4217 4244 4267 4511 4512 4625 4626 4640 4668 4686)
#endif // #ifdef _Windows
//	////////////////////////////////////////////////////////////////////////////

namespace LOCAL-NAMESPACE
{

//	////////////////////////////////////////////////////////////////////////////
boost::python::list          WRAP_GetTransportInfo(LOCAL_API_TYPE &api_ref);
boost::python::list          WRAP_GetQueueNameList(LOCAL_API_TYPE &api_ref,
	const std::string &name_pattern, double time_out = OPTIONAL-PARAM-DEFAULT);
std::string                  WRAP_EnqueueMessage(LOCAL_API_TYPE &api_ref,
	const std::string &facility_name, const std::string &datum,
	double time_out = OPTIONAL-PARAM-DEFAULT);
boost::python::list          WRAP_EnqueueMessageList(LOCAL_API_TYPE &api_ref,
	const std::string &facility_name, const boost::python::list &data_list,
	double time_out = OPTIONAL-PARAM-DEFAULT);

//											... 
//	////////////////////////////////////////////////////////////////////////////

} // namespace LOCAL-NAMESPACE

//	****************************************************************************
//	LOCAL_PythonImpl.cpp
//	----------------------------------------------------------------------------

//	////////////////////////////////////////////////////////////////////////////
#include <LOCAL_PythonWrappers.hpp>
//	////////////////////////////////////////////////////////////////////////////

namespace LOCAL-NAMESPACE
{

namespace
{

//	////////////////////////////////////////////////////////////////////////////
BOOST_PYTHON_FUNCTION_OVERLOADS(GetTransportInfo_OLS, WRAP_GetTransportInfo, 1, 1)
BOOST_PYTHON_FUNCTION_OVERLOADS(GetQueueNameList_OLS, WRAP_GetQueueNameList, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(EnqueueMessage_OLS, WRAP_EnqueueMessage, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(EnqueueMessageList_OLS, WRAP_EnqueueMessageList, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(SetMessageStatus_OLS, WRAP_SetMessageStatus, 4, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(GetMessageById_OLS, WRAP_GetMessageById, 4, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(GetFirstMessage_OLS, WRAP_GetFirstMessage, 4, 6)
BOOST_PYTHON_FUNCTION_OVERLOADS(GetNextMessage_OLS, WRAP_GetNextMessage, 5, 7)
BOOST_PYTHON_FUNCTION_OVERLOADS(GetLastMessage_OLS, WRAP_GetLastMessage, 4, 6)
BOOST_PYTHON_FUNCTION_OVERLOADS(GetPreviousMessage_OLS, WRAP_GetPreviousMessage, 5, 7)
BOOST_PYTHON_FUNCTION_OVERLOADS(GetMessageInfoList_OLS, WRAP_GetMessageInfoList, 2, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(GetMessageList_OLS, WRAP_GetMessageList, 2, 7)
BOOST_PYTHON_FUNCTION_OVERLOADS(AcquireMessageLease_OLS, WRAP_AcquireMessageLease, 4, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(ExtendMessageLease_OLS, WRAP_ExtendMessageLease, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(ResetMessageLease_OLS, WRAP_ResetMessageLease, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(DropLeases_OLS, WRAP_DropLeases, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(GetMessageLeaseList_OLS, WRAP_GetMessageLeaseList, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(RecoverQueueFile_OLS, WRAP_RecoverQueueFile, 3, 4)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PingServer_OLS, PingServer, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GetAvailableFlag_OLS, GetAvailableFlag, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SetAvailableFlag_OLS, SetAvailableFlag, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GetApplicationInfo_OLS, GetApplicationInfo, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SetApplicationInfo_OLS, SetApplicationInfo, 3, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CreateQueue_OLS, CreateQueue, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OpenQueue_OLS,   OpenQueue, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CloseQueue_OLS,  CloseQueue, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RemoveQueue_OLS, RemoveQueue, 1, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CommitMessageLease_OLS, CommitMessageLease, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CancelMessageLease_OLS, CancelMessageLease, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RollbackMessageLease_OLS, RollbackMessageLease, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(TerminateMessageLease_OLS, TerminateMessageLease, 1, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ExpireLeases_OLS, ExpireLeases, 1, 2)
//	////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

//	////////////////////////////////////////////////////////////////////////////
void LOCAL_API_TYPEPython()
{
	boost::python::class_<LOCAL_API_TYPE>("LOCAL_API_TYPE",
		boost::python::init<std::string, std::string,
			boost::python::optional<std::string, std::string, std::string> >())
		.def("GetEnvName",
			&LOCAL_API_TYPE::GetEnvName)
		.def("GetDomainName",
			&LOCAL_API_TYPE::GetDomainName)
		.def("GetTransportInfo",
			WRAP_GetTransportInfo,
			GetTransportInfo_OLS())
		.def("SetEnvName",
			&LOCAL_API_TYPE::SetEnvName)
		.def("SetDomainName",
			&LOCAL_API_TYPE::SetDomainName)
		.def("SetTransportInfo",
			&LOCAL_API_TYPE::SetTransportInfo)
		.def("PingServer",
			&LOCAL_API_TYPE::PingServer,
			PingServer_OLS())
		.def("GetQueueNameList",
			WRAP_GetQueueNameList,
			GetQueueNameList_OLS())
		.def("GetAvailableFlag",
			&LOCAL_API_TYPE::GetAvailableFlag,
			GetAvailableFlag_OLS())
		.def("SetAvailableFlag",
			&LOCAL_API_TYPE::SetAvailableFlag,
			SetAvailableFlag_OLS())
		.def("GetApplicationInfo",
			&LOCAL_API_TYPE::GetApplicationInfo,
			GetApplicationInfo_OLS())
		.def("SetApplicationInfo",
			&LOCAL_API_TYPE::SetApplicationInfo,
			SetApplicationInfo_OLS())
		.def("CreateQueue",
			&LOCAL_API_TYPE::CreateQueue,
			CreateQueue_OLS())
		.def("OpenQueue",
			&LOCAL_API_TYPE::OpenQueue,
			OpenQueue_OLS())
		.def("CloseQueue",
			&LOCAL_API_TYPE::CloseQueue,
			CloseQueue_OLS())
		.def("RemoveQueue",
			&LOCAL_API_TYPE::RemoveQueue,
			RemoveQueue_OLS())
		.def("EnqueueMessage",
			WRAP_EnqueueMessage,
			EnqueueMessage_OLS())
		.def("EnqueueMessageList",
			WRAP_EnqueueMessageList,
			EnqueueMessageList_OLS())
		.def("SetMessageStatus",
			WRAP_SetMessageStatus,
			SetMessageStatus_OLS())
		.def("GetMessageById",
			WRAP_GetMessageById,
			GetMessageById_OLS())
		.def("GetFirstMessage",
			WRAP_GetFirstMessage,
			GetFirstMessage_OLS())
		.def("GetNextMessage",
			WRAP_GetNextMessage,
			GetNextMessage_OLS())
		.def("GetLastMessage",
			WRAP_GetLastMessage,
			GetLastMessage_OLS())
		.def("GetPreviousMessage",
			WRAP_GetPreviousMessage,
			GetPreviousMessage_OLS())
		.def("GetMessageInfoList",
			WRAP_GetMessageInfoList,
			GetMessageInfoList_OLS())
		.def("GetMessageList",
			WRAP_GetMessageList,
			GetMessageList_OLS())
		.def("AcquireMessageLease",
			WRAP_AcquireMessageLease,
			AcquireMessageLease_OLS())
		.def("CommitMessageLease",
			&LOCAL_API_TYPE::CommitMessageLease,
			CommitMessageLease_OLS())
		.def("CancelMessageLease",
			&LOCAL_API_TYPE::CancelMessageLease,
			CancelMessageLease_OLS())
		.def("RollbackMessageLease",
			&LOCAL_API_TYPE::RollbackMessageLease,
			RollbackMessageLease_OLS())
		.def("TerminateMessageLease",
			&LOCAL_API_TYPE::TerminateMessageLease,
			TerminateMessageLease_OLS())
		.def("ExtendMessageLease",
			WRAP_ExtendMessageLease,
			ExtendMessageLease_OLS())
		.def("ResetMessageLease",
			WRAP_ResetMessageLease,
			ResetMessageLease_OLS())
		.def("ExpireLeases",
			&LOCAL_API_TYPE::ExpireLeases,
			ExpireLeases_OLS())
		.def("DropLeases",
			WRAP_DropLeases,
			DropLeases_OLS())
		.def("GetMessageLeaseList",
			WRAP_GetMessageLeaseList,
			GetMessageLeaseList_OLS())
		.def("RecoverQueueFile",
			WRAP_RecoverQueueFile,
			RecoverQueueFile_OLS())
		;
}
//	////////////////////////////////////////////////////////////////////////////

} // namespace LOCAL-NAMESPACE

//	****************************************************************************

//	////////////////////////////////////////////////////////////////////////////
//	MlbDev/UtilityPythonWrappers.hpp
//	----------------------------------------------------------------------------

// ////////////////////////////////////////////////////////////////////////////
std::vector<std::string> &PyListToVectorOfStrings(
	const boost::python::list &src_list, std::vector<std::string> &dst_list)
{
	int tmp_count = boost::python::extract<int>(src_list.attr("__len__")());

	std::vector<std::string> tmp_list;

	if (tmp_count > 0) {
		tmp_list.reserve(tmp_count);
		int count_1;
		for (count_1 = 0; count_1 < tmp_count; ++count_1)
			tmp_list.
				push_back(boost::python::extract<std::string>(src_list[count_1]));
	}

	dst_list.swap(tmp_list);

	return(dst_list);
}
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
std::vector<std::string> PyListToVectorOfStrings(
	const boost::python::list &src_list)
{
	std::vector<std::string> dst_list;

	return(PyListToVectorOfStrings(src_list, dst_list));
}
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
boost::python::list &VectorOfStringsToPyList(
	const std::vector<std::string> &src_list, boost::python::list &dst_list)
{
	boost::python::list tmp_list;

	if (!src_list.empty()) {
		MLB::Utility::StringVectorIterC iter_b(src_list.begin());
		MLB::Utility::StringVectorIterC iter_e(src_list.end());
		for ( ; iter_b != iter_e; ++iter_b)
			tmp_list.append(*iter_b);
	}

	dst_list = tmp_list;

	return(dst_list);
}
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
boost::python::list VectorOfStringsToPyList(
	const std::vector<std::string> &src_list)
{
	boost::python::list dst_list;

	return(VectorOfStringsToPyList(src_list, dst_list));
}
// ////////////////////////////////////////////////////////////////////////////

//	****************************************************************************


//	****************************************************************************
//	////////////////////////////////////////////////////////////////////////////
//	LOCAL_PythonWrappers.cpp
//	----------------------------------------------------------------------------
// ////////////////////////////////////////////////////////////////////////////
boost::python::list WRAP_GetTransportInfo(LOCAL_API_TYPE &api_ref)
{
	return(VectorOfStringsToPyList(api_ref.GetTransportInfo()));
}
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
boost::python::list WRAP_GetQueueNameList(LOCAL_API_TYPE &api_ref,
	const std::string &name_pattern, double time_out)
{
	MLB::Utility::StringVector tmp_list;
	boost::python::list        python_list;

	return(VectorOfStringsToPyList(api_ref.GetQueueNameList(name_pattern,
		tmp_list, time_out), python_list));
}
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
std::string WRAP_EnqueueMessage(LOCAL_API_TYPE &api_ref,
	const std::string &facility_name, const std::string &datum, double time_out)
{
  UniqueId tmp_guid;

	{
		MLB::BoostX::PythonEvalSaveRestore gil_context;
		api_ref.EnqueueMessage(facility_name, datum, time_out).swap(tmp_guid);
	}

	return(tmp_guid.ToString());
}
// ////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////
boost::python::list WRAP_EnqueueMessageList(LOCAL_API_TYPE &api_ref,
	const std::string &facility_name, const boost::python::list &data_list,
	double time_out)
{
	std::vector<std::string> tmp_data_list;
	UniqueIdVector           tmp_guid_list;

	PyListToVectorOfStrings(data_list, tmp_data_list);


	{
		MLB::BoostX::PythonEvalSaveRestore gil_context;
		api_ref.EnqueueMessageList(facility_name, tmp_data_list, time_out).
			swap(tmp_guid_list);
	}

	boost::python::list out_list;
	UniqueIdVectorIterC iter_b(tmp_guid_list.begin());
	UniqueIdVectorIterC iter_e(tmp_guid_list.end());

	for ( ; iter_b != iter_e; ++iter_b)
		out_list.append(iter_b->ToString());

	return(out_list);
}
// ////////////////////////////////////////////////////////////////////////////


