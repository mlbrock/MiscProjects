#!/bin/bash
# #############################################################################
# #############################################################################
# Miscellaneous Utility Script
# #############################################################################
#
# File Name       : FilterRepo.MlbDev.ExtractRepos.sh
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
MY_DIR_NAME=${PWD}
MY_BASE_DIR=`basename ${MY_DIR_NAME}`
MY_CURR_DATE=`date -u +%Y-%m-%d`
MY_CURR_YEAR=${MY_CURR_DATE:0:4}
# #############################################################################

# #############################################################################
MY_ORIGIN_URL="git@github.com:mlbrock/MlbDev.git"
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
# Ensure SimpleBuildDriverLocalList.TEMPLATE.sh is available:
if [ ! -f SimpleBuildDriverLocalList.TEMPLATE.sh ]; then
    echo "The template file 'SimpleBuildDriverLocalList.TEMPLATE.sh' is not" \
	"present in the current directory --- processing halted."
    exit 1
fi
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
	exit 1
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
	    --path include/BoostX.hpp \
	    --path ${this_repo_name} \
	    --path include/${this_repo_name} \
	    --path include/${this_repo_name}.hpp \
    )
    if [ ! -f ${this_repo_dir}/.MASCaPS/MakePrefix.mk ]; then
	echo "The file '${this_repo_dir}/.MASCaPS/MakePrefix.mk' is not" \
	   "present within the new filter-repo repository."
	exit 1
    fi
    # ----- Set the USE_XLIBS_FLAG in <repo-dir>/.MASCaPS/MakePrefix.mk:
    ( chmod u+w ${this_repo_dir}/.MASCaPS/MakePrefix.mk && \
	sed -i -E 's/^(\s*USE_XLIBS_FLAG\s*=\s*)0(\s*)$/\11\2/' \
	${this_repo_dir}/.MASCaPS/MakePrefix.mk
    )
    # ----- Update the <repo-dir>/SimpleBuildDriverLocalList.sh file:
    ( chmod u+w ${this_repo_dir}/SimpleBuildDriverLocalList.sh ; \
	sed -E \
	-e "s/XLIB_REPO_DIR/${this_repo_dir}/g" \
	-e "s/XLIB_REPO_NAME/${this_repo_name}/g" \
	-e "s/XLIB_EXTRACT_DATE/${MY_CURR_DATE}/g" \
	-e "s/XLIB_EXTRACT_YEAR/${MY_CURR_YEAR}/g" \
	SimpleBuildDriverLocalList.TEMPLATE.sh > \
	${this_repo_dir}/SimpleBuildDriverLocalList.sh
    )
done
# #############################################################################

