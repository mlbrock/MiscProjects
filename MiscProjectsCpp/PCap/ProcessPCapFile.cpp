//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	MLB Utility Library Module File
//	////////////////////////////////////////////////////////////////////////////
/*
	File Name  			:	%M%

	File Version      :	%I%

	Last Extracted		:	%D%	%T%

	Last Updated		:	%E%	%U%

	File Description	:	Implementation of the argument parsing and validation
								class ParseCmdLineArg.

	Revision History	:	1993-10-02 --- Creation
									Michael L. Brock

		Copyright Michael L. Brock 1993 - 2016.
		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		http://www.boost.org/LICENSE_1_0.txt)

*/
//	////////////////////////////////////////////////////////////////////////////
 
//	////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////
//	Required include files...
//	////////////////////////////////////////////////////////////////////////////

#include <pcap.h>

#include <stdexcept>
#include <ostream>
#include <sstream>

#include <net/ethernet.h>			//	ether_header
#include <netinet/ip.h>				//	ip	--- for IP_HEADER_STRUCT.ip_hl * 4
#include <netinet/udp.h>			//	udphdr

#include <strings.h>

#include <boost/shared_ptr.hpp>

//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
typedef boost::shared_ptr<pcap_t> pcap_t_SPtr;
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
//	Useful but not required...
class PacketProcessorBase {
public:

	virtual void DoCallback(const struct pcap_pkthdr *header_ptr,
		const u_char *packet_ptr)
	{
		const u_char       *packet_data_ptr  = packet_ptr;
		const ether_header *ether_header_ptr =
			reinterpret_cast<const ether_header *>(packet_data_ptr);
		std::size_t         remaining_length =
			static_cast<std::size_t>(header_ptr->caplen);

		if (remaining_length < sizeof(ether_header))
			throw std::runtime_error("Remaining capture length < Ethernet header "
				"structure length.");

		remaining_length -= sizeof(ether_header);
		packet_data_ptr  += sizeof(ether_header);

		if (remaining_length < sizeof(ip))
			throw std::runtime_error("Remaining capture length < IP header "
				"structure length.");

		const ip *ip_header_ptr = reinterpret_cast<const ip *>(packet_data_ptr);

//		remaining_length -= sizeof(ip);
//		packet_data_ptr  += sizeof(ip);

		std::size_t ip_header_length = ip_header_ptr->ip_hl * 4;

		if (remaining_length < ip_header_length)
			throw std::runtime_error("Remaining capture length < indicated "
				"length of the IP header.");

		if (ip_header_ptr->ip_p != IPPROTO_UDP)
			throw std::runtime_error("Captured packet IP protocol is not UDP.");

		remaining_length -= ip_header_length;
		packet_data_ptr  += ip_header_length;

		if (remaining_length < sizeof(udphdr))
			throw std::runtime_error("Remaining capture length < UDP header "
				"structure length.");

		const udphdr *udp_header_ptr =
			reinterpret_cast<const udphdr *>(packet_data_ptr);

		remaining_length -= sizeof(udphdr);
		packet_data_ptr  += sizeof(udphdr);

		std::cout << "DoCallback(): Payload length = " << remaining_length <<
			"\n";
	}

	static void MyHackyPCapCallback(::u_char *user,
		const struct pcap_pkthdr *h, const u_char *bytes)
	{
		reinterpret_cast<PacketProcessorBase *>(user)->DoCallback(h, bytes);
	}
};
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
//	Because silly pcap_close() will try to close NULL pointers...
void SensibleShimFor_pcap_close(pcap_t *pcap_ptr)
{
	if (pcap_ptr)
		::pcap_close(pcap_ptr);
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
template <typename PacketProcessor>
	void ProcessPCapFile(const std::string &pcap_file,
		PacketProcessor &packet_processor)
{
	char error_text[PCAP_ERRBUF_SIZE];

	try {
		pcap_t_SPtr pcap_sptr(::pcap_open_offline_with_tstamp_precision(
			pcap_file.c_str(), PCAP_TSTAMP_PRECISION_NANO, error_text),
			SensibleShimFor_pcap_close);
		if (!pcap_sptr.get()) {
			std::ostringstream o_str;
			o_str << "Open attempt of the PCAP file with "
				"'::pcap_open_offline_with_tstamp_precision() failed: " <<
				error_text;
			throw std::runtime_error(o_str.str());
		}
		int pcap_return_code;
/*
		pcap_return_code = ::pcap_set_snaplen(pcap_sptr.get(), 16384);
		if (pcap_return_code) {
			std::ostringstream o_str;
			o_str << "An error occurred in the call to '::pcap_set_snaplen()' "
				"which resulted in a return code of " << pcap_return_code <<
				": " << ::pcap_geterr(pcap_sptr.get());
			throw std::runtime_error(o_str.str());
		}
*/
		pcap_return_code = ::pcap_datalink(pcap_sptr.get());
		if (pcap_return_code == PCAP_ERROR_NOT_ACTIVATED) {
			std::ostringstream o_str;
			o_str << "An error occurred in the call to '::pcap_datalink()' "
				"which resulted in a return code of " << pcap_return_code <<
				" (PCAP_ERROR_NOT_ACTIVATED).";
			throw std::runtime_error(o_str.str());
		}
		pcap_return_code = ::pcap_loop(pcap_sptr.get(), 0,
			PacketProcessor::MyHackyPCapCallback,
			reinterpret_cast<u_char *>(&packet_processor));
		if (pcap_return_code && (pcap_return_code != 2)) {
			std::ostringstream o_str;
			o_str << "An error occurred in the call to '::pcap_loop()' which "
				"resulted in a return code of " << pcap_return_code << ": " <<
				((pcap_return_code == -1) ? ::pcap_geterr(pcap_sptr.get()) :
				"*** UNKNOWN PCAP ERROR TYPE ***");
			throw std::runtime_error(o_str.str());
		}
	}
	catch (const std::exception &except) {
		std::ostringstream o_str;
		o_str << "Unable to process PCAP file '" << pcap_file << "': " <<
			except.what();
		throw std::runtime_error(o_str.str());
	}
}
//	////////////////////////////////////////////////////////////////////////////

//	////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	int return_code = EXIT_SUCCESS;

	try {
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
	}
	catch (const std::exception &except) {
		return_code = EXIT_FAILURE;
		std::cerr << "ERROR: " << except.what() << std::endl;
	}

	return(return_code);
}
//	////////////////////////////////////////////////////////////////////////////

