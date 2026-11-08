#!/bin/bash
# #############################################################################
# #############################################################################
# Miscellaneous Utility Script
# #############################################################################
#
# File Name       : FilterRepo.MlbDev.BuildRepos.sh
#
# File Description: Build the independent repositories in order from least
#                   dependent.
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
MY_REPO_LIST="\
Utility \
RapidXmlUtils \
XercesUtils \
SocketIo \
SockLibX \
LbmWrapper \
LSFWrapper \
RvUtilX \
VFast \
MlbUtilities
"
# #############################################################################

# #############################################################################
for this_repo_name in ${MY_REPO_LIST}
do
    this_repo_dir=${this_repo_name}
    if [ ${this_repo_dir} == "Utility" ]; then
	this_repo_dir="${this_repo_name}Old"
    fi
    if [ ! -e ${this_repo_dir} ]; then
	echo "Unable to build: directory '${this_repo_dir}' does not exist."
	exit 1
    fi
    ( pushd ${this_repo_dir} ; \
	clear ; printf "\\033c;\\033[2J;\\033[H" ; \
	./SimpleBuildDriver.sh clean ; \
	rm BuildResults.* ; \
	./SimpleBuildDriver.sh ; \
	./SimpleBuildDriver.sh tests_all ; \
	grep -P '((warning)|(error)):' BuildResults.* \
    )
#    echo pushd ${this_repo_dir}
done
# #############################################################################

