//	////////////////////////////////////////////////////////////////////////////
struct IceSecurityLink {
	bool operator < (const IceSecurityLink &other) const
	{
		return(security_id_ < other.security_id_);
	}

	Native_SInt32                   security_id_;
	std::shared_ptr<ProductDefInfo> security_info_;
	OrderBook<{MBO|MBP}>            order_book_;
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef boost::flat_set<IceSecurityLink> IceSecurityLinkSet;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
struct IceMcMPayload {
	Native_UInt32 first_seq_number_;
	Native_UInt32 last_seq_number_;
	char          payload_[SOME_KINDA_MTU_VALUE];
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
struct IceSessionLink {

	IceMcMPayload *GetSecIdPtr(Native_SInt32 market_id)
	{
		//	Lookup security in sec_id_link_set_
		return(???);	//	Pointer or NULL if not found.
	}

	typedef std::tuple<bool, MLB::SocketIo::SocketSpecSPtr,

	IceMarketType                              market_type_;
	std::string                                market_type_sub_name_;
	IceSecurityLinkSet                         sec_id_link_set_;
	std::vector<MLB::SocketIo::SocketSpec>     delta_spec_list_;
	std::vector<MLB::SocketIo::SocketSpec>     snaps_spec_list_;
	std::vector<MLB::SocketIo::SocketSpecSPtr> delta_mc_list_;
	std::vector<MLB::SocketIo::SocketSpecSPtr> snaps_mc_list_;
	Native_SInt16                              session_number_;
	Native_SInt32                              next_sequence_number_;
	bool                                       recovery_flag_;
	boost::circular_buffer<IceMcMPayload>      pending_deltas_;
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void ProcessAnMcM(IceSessionLink &session_link, bool is_snapshot,
	const IceMcmData &payload)
{
	if (!recovery_flag_) {
		if (is_snapshot)
			return;
		if (payload.getSessionNumber() != session_link.session_number_) {
			LogWarn << "***** RECOVERY MODE STARTED: Session was invalidated "
				"because the session number changed.\n";
			session_link.session_number_       = session_link.session_number_;
			session_link.next_sequence_number_ = -1;
			session_link.recovery_flag_        = true;
			session_link.InvalidateMySecurityLinks();
			return;
		}
		if (payload.getSequenceNumber() < session_link.next_sequence_number_)
			return;
		if (payload.getSequenceNumber() > session_link.next_sequence_number_) {
			LogWarn << "***** RECOVERY MODE STARTED: Expected sequence number " <<
				session_link.next_sequence_number_ << ", but received sequence "
				"number " << payload.getSequenceNumber() << ".\n";
			session_link.next_sequence_number_ = -1;
			session_link.recovery_flag_        = true;
			session_link.InvalidateMySecurityLinks();
			return;
		}
		Native_SInt16 message_count = payload.getMessageCount();
		for (Native_SInt16 count_1 = 0; count_1 < message_count; ++count_1) {
			//	Progress to next message, et cetera...
			registry.DispatchEvent(/* BLAH */);'
		}
	}
	else
		ProcessAnMcM_RecoveryMode(session_link, is_snapshot, payload);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
void ProcessAnMcM(IceSessionLink &session_link, bool is_snapshot,
	const IceMcmData &payload)
{
	if (is_snapshot) {
	}
}
//	////////////////////////////////////////////////////////////////////////////

