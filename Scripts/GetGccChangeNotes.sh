#!/bin/bash
# #############################################################################
# #############################################################################
# Miscellaneoys Utility Script
# #############################################################################
#
# File Name       : GetGccChangeNotes.sh
#
# File Description: Get GCC change notes.
#
# Revision History: 2026-09-19 --- Creation.
#                       Michael L. Brock
#
#       Copyright Michael L. Brock 2026.
#
#       Distributed under the Boost Software License, Version 1.0.
#       (See accompanying file LICENSE_1_0.txt or copy at
#       http://www.boost.org/LICENSE_1_0.txt)
#
# #############################################################################

# #############################################################################
MY_VERSION_LIST="\
2.95 \
3.0 \
3.1 \
3.2 \
3.3 \
3.4 \
4.0 \
4.1 \
4.2 \
4.3 \
4.4 \
4.5 \
4.6 \
4.7 \
4.8 \
4.9 \
5 \
6 \
7 \
8 \
9 \
10 \
11 \
12 \
13 \
14 \
15 \
16 \
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
for this_version in ${MY_VERSION_LIST}
do
    ( mkdir -p ${GCC_DIR_NAME}/${this_version} && pushd ${GCC_DIR_NAME}/${this_version} && wget https://gcc.gnu.org/gcc-${this_version}/{index,changes}.html )
done
# #############################################################################


