# #############################################################################
# Boost C++ Libraries Support
# -----------------------------------------------------------------------------
# ----- Include directory:
INC_BOOST_DIR	=	/usr/include
# -----------------------------------------------------------------------------
# ----- Library directory:
LIB_BOOST_DIR	=	${LIB_OTHER_DIR}
# -----------------------------------------------------------------------------
# ----- Static library names:
BOOST_LIB_NAMES	=	\
			libboost_filesystem.a   \
			libboost_date_time.a \
			libboost_thread.a \
			libboost_regex.a
# -----------------------------------------------------------------------------
# ----- Handle libboost_system.a in Boost version 1.89 and above:
# Boost 1.89 made Boost System header-only. A good thing, but of course it
# broke builds which had included the library explicitly. The following
# make logic bridges the gap.
ifeq (1, ${words ${LIB_BOOST_DIR}})
	TMP_LIB_NAME     = ${wildcard ${LIB_BOOST_DIR}/libboost_system.a}
	BOOST_LIB_NAMES += ${notdir ${TMP_LIB_NAME}}
endif
# -----------------------------------------------------------------------------
# ----- Static link directives:
BOOST_LIBS	=       ${addprefix ${LIB_BOOST_DIR}/,${BOOST_LIB_NAMES}}
# #############################################################################


# #############################################################################
# SSL Support
# -----------------------------------------------------------------------------
# ----- Include and library directories:
INC_SSL_DIR	=	/usr/include/openssl
LIB_SSL_DIR	=	/usr/lib64
LIB_SSL_NAMES_A	=
LIB_SSL_NAMES_SO=	ssl
# -----------------------------------------------------------------------------
# ----- Include directives:
CPPFLAGS	+=	\
			<...> \
			-I${INC_SSL_DIR}	\
			<...>
# -----------------------------------------------------------------------------
# ----- Link directives:
LDLIBS		=	\
			<...> \
			${addprefix -l,${LIB_SSL_NAMES_SO}}	\
			<...>

LDFLAGS		+=	\
			<...> \
			${addprefix -L,${LIB_SSL_DIR}}	\
			<...>
# #############################################################################


# #############################################################################
# Crypto Support
# -----------------------------------------------------------------------------
# ----- Include and library directories:
#       Includes are within the OpenSSL directory
INC_CRYPTO_DIR	=	${INC_SSL_DIR}
LIB_CRYPTO_DIR	=	/usr/lib64
LIB_CRYPTO_NAMES_A	=
LIB_CRYPTO_NAMES_SO	=	crypto
# -----------------------------------------------------------------------------
# ----- Include directives:
CPPFLAGS	+=	\
			<...> \
			-I${INC_CRYPTO_DIR}	\
			<...>
# -----------------------------------------------------------------------------
# ----- Link directives:
LDLIBS		=	\
			<...> \
			${addprefix -l,${LIB_CRYPTO_NAMES_SO}}	\
			<...>

LDFLAGS		+=	\
			<...> \
			${addprefix -L,${LIB_CRYPTO_DIR}}	\
			<...>
# #############################################################################


# #############################################################################
# PCAP Support
# -----------------------------------------------------------------------------
# ----- Include directory:
INC_PCAP_DIR	=	/usr/include/pcap
# -----------------------------------------------------------------------------
# ----- Library directory / Approach #1:
LIB_PCAP_DIR	=	/usr/lib64
# -----------------------------------------------------------------------------
# ----- Library directory / Approach #2:
LIB_PCAP_DIR	=	${LIB_BASE}
# -----------------------------------------------------------------------------
# ----- Library name:
PCAP_LIB_NAME	=	pcap
# -----------------------------------------------------------------------------
# ----- Include directives:
CPPFLAGS	+=	\
			<...> \
			-I${INC_PCAP_DIR} \
			<...>
# -----------------------------------------------------------------------------
# ----- Link directives:
LDLIBS		=	\
			<...> \
			${addprefix -l,${PCAP_LIB_NAME}}	\
			<...>

LDFLAGS		+=	\
			<...> \
			-L${LIB_PCAP_DIR}	\
			<...>
# #############################################################################


# #############################################################################
# Google ProtoBuf Support
# -----------------------------------------------------------------------------
# ----- Include and library directories for the basic protobuf support
#       Here supporting shared libraries only
INC_PROTOBUF_DIR	=	/usr/include
LIB_PROTOBUF_DIR	=	/usr/lib64
LIB_PROTOBUF_NAMES_A	=
LIB_PROTOBUF_NAMES_SO	=	protobuf
# -----------------------------------------------------------------------------
# ----- Include and library directories for the protobuf-c support
#       Here supporting shared libraries only
INC_PROTOBUF_C_DIR	=	/usr/include/google/protobuf-c
LIB_PROTOBUF_C_DIR	=	/usr/lib64
LIB_PROTOBUF_C_NAMES_A	=
LIB_PROTOBUF_C_NAMES_SO	=	protobuf-c
# -----------------------------------------------------------------------------
# ----- Include directives:
CPPFLAGS	+=	\
			<...> \
			-I${INC_PROTOBUF_DIR} \
			-I${INC_PROTOBUF_C_DIR} \
			<...>
# -----------------------------------------------------------------------------
# ----- Link directives:
LDLIBS		=	\
			<...> \
			${addprefix -l,${LIB_PROTOBUF_NAMES_SO}}	\
			${addprefix -l,${LIB_PROTOBUF_C_NAMES_SO}}	\
			<...>

LDFLAGS		+=	\
			<...> \
			${addprefix -L,${LIB_PROTOBUF_DIR}}	\
			${addprefix -L,${LIB_PROTOBUF_C_DIR}}	\
			<...>
# #############################################################################


# #############################################################################
# NATS Support
# -----------------------------------------------------------------------------
# ----- Include and library directories:
INC_NATS_DIR		=	${MASCaPS_DIR_PARENT}/../nats-io/nats.c/src
LIB_NATS_DIR		=	${MASCaPS_DIR_PARENT}/../nats-io/nats.c/___MLB_BuildDir/lib
LIB_NATS_NAMES_A	=	nats_static
LIB_NATS_NAMES_SO	=	nats
# -----------------------------------------------------------------------------
# ----- Include directives:
CPPFLAGS	+=	\
			<...> \
			-I${INC_NATS_DIR}	\
			<...>
# -----------------------------------------------------------------------------
# ----- Link directives:
LDLIBS		=	\
			<...> \
			${addprefix -l,${LIB_NATS_NAMES_A}}	\
			<...>

LDFLAGS		+=	\
			<...> \
			${addprefix -L,${LIB_NATS_DIR}}	\
			<...>
# #############################################################################


# #############################################################################
# Tib/Rendezvous Support
# -----------------------------------------------------------------------------
# ----- To support multiple Tib Rendezvous versions:
VER_TIBRV	=	8.1
# -----------------------------------------------------------------------------
# ----- Include directory:
INC_TIBRV_DIR	=	/opt/tibco/tibrv/${VER_TIBRV}/include
# -----------------------------------------------------------------------------
# ----- Library directory:
LIB_TIBRV_DIR	=	/opt/tibco/tibrv/${VER_TIBRV}/lib
# -----------------------------------------------------------------------------
# ----- Static link directives / Approach #1:
TIBRV_LIBS	=	\
			${LIB_TIBRV_DIR}/libtibrvcmq64.a	\
			${LIB_TIBRV_DIR}/libtibrvcm64.a		\
			${LIB_TIBRV_DIR}/libtibrvft64.a		\
			${LIB_TIBRV_DIR}/libtibrvcpp64.a	\
			${LIB_TIBRV_DIR}/libtibrv64.a
# -----------------------------------------------------------------------------
# ----- Static link directives / Approach #2:
TIBRV_LIB_NAMES	=	\
			libtibrvcmq64.a	\
			libtibrvcm64.a	\
			libtibrvft64.a	\
			libtibrvcpp64.a	\
			libtibrv64.a

TIBRV_LIBS	=       ${addprefix ${LIB_TIBRV_DIR}/,${TIBRV_LIB_NAMES}}
# -----------------------------------------------------------------------------
# ----- Include directives:
CPPFLAGS	+=	\
			<...> \
			-I${INC_TIBRV_DIR} \
			<...>
# -----------------------------------------------------------------------------
# ----- Link directives:
LDLIBS		=	\
			<...> \
			${TIBRV_LIBS}	\
			<...>
# #############################################################################


# #############################################################################
# 
# -----------------------------------------------------------------------------

# #############################################################################

