#!/bin/bash
# #############################################################################
# #############################################################################
# Miscellaneous Utility Script
# #############################################################################
#
# File Name       : FilterRepo.MlbDev.sh
#
# File Description: Use git-filter-repo to create independent repositories
#                   from individual library and executable directory targets
#                   within the github.com/MlbDev repository.
#
# Revision History: 2026-11-06 --- Creation.
#                       Michael L. Brock
#
#       Copyright Michael L. Brock 2026 - 2026.
#
#       Distributed under the Boost Software License, Version 1.0.
#       (See accompanying file LICENSE_1_0.txt or copy at
#       http://www.boost.org/LICENSE_1_0.txt)
#
# #############################################################################

# #############################################################################
MY_ORIGIN_URL="git@github.com:mlbrock/MlbDev.git"
# #############################################################################

# #############################################################################
MY_REPO_LIST="\
LbmWrapper \
LSFWrapper \
MlbUtilities
RapidXmlUtils \
RvUtilX \
SocketIo \
SockLibX \
Utility \
VFast \
XercesUtils \
"
# #############################################################################

# #############################################################################
MY_DIR_NAME=${PWD}
MY_BASE_DIR=`basename ${MY_DIR_NAME}`
MY_CURR_DATE=`date -u +%Y_%m_%d_%H_%M_%S_%N`
MY_HOST_NAME=`hostname -s`
MY_USER_NAME=`whoami`
GCC_DIR_NAME="./VersionChangeNotesForGcc/${MY_CURR_DATE}.${MY_HOST_NAME}.${MY_USER_NAME}.${BASHPID}"
# #############################################################################

# #############################################################################
for this_repo_name in ${MY_REPO_LIST}
do
    this_repo_dir=${this_repo_name}
    if [ ${this_repo_dir} == "Utility" ]; then
	this_repo_dir="${this_repo_name}Old"
    fi
    if [ -e ${this_repo_dir} ]; then
	echo "Unable to clone from '${MY_ORIGIN_URL}':" \
	    "directory '${this_repo_dir}' already exists."
	exit
    fi
    ( git clone ${MY_ORIGIN_URL} ${this_repo_dir} && \
	pushd ${this_repo_dir} && \
	git filter-repo \
	    --path .MASCaPS \
	    --path SimpleBuildDriver.sh \
	    --path SimpleBuildDriverLocalList.sh \
	    --path .gitattributes \
	    --path .gitignore \
	    --path include/mbcompat.h \
	    --path include/mbtypes.h \
	    --path ${this_repo_name} \
	    --path include/${this_repo_name} \
	    --path include/${this_repo_name}.hpp \
    )
#    echo git clone ${MY_ORIGIN_URL} ${this_repo_dir}
#    echo pushd ${this_repo_dir}
done
# #############################################################################

