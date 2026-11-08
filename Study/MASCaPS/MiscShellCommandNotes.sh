# #############################################################################
# -----------------------------------------------------------------------------
# ----- Find source files which contain SCCS macro definitions:
grep -P '%(M|I|D|T|E|U)%' `find . -type f|grep -iP '\.(c|h|i)((pp)|(xx))?$'`
# -----------------------------------------------------------------------------
# #############################################################################

# #############################################################################
# -----------------------------------------------------------------------------
# ----- Grep build results files for warnings and errors:
grep -P '((warning)|(error)):' BuildResults.*
# -----------------------------------------------------------------------------
# #############################################################################

# #############################################################################
# -----------------------------------------------------------------------------
# ----- Grep the most-recent build results file for warnings and errors:
grep -P '((warning)|(error)):' `ls -1 BuildResults.*|sort -r|head -n 1`
# -----------------------------------------------------------------------------
# #############################################################################

# #############################################################################
# -----------------------------------------------------------------------------
# ----- Live tail of the most-recent build results file:
tail -f `ls -1 BuildResults.*|sort -r|head -n 1`
# -----------------------------------------------------------------------------
# #############################################################################

# #############################################################################
# -----------------------------------------------------------------------------
# ----- Do the following:
#          1) Clean all targets;
#          2) Build all targets;
#          3) Build all tests; and,
#          4) Grep for warnings and errors.
cls ; \
   ./SimpleBuildDriver.sh clean ; \
        rm BuildResults.* ; \
        ./SimpleBuildDriver.sh ; \
        ./SimpleBuildDriver.sh tests_all ; \
        grep -P '((warning)|(error)):' BuildResults.*
# -----------------------------------------------------------------------------
# #############################################################################

# #############################################################################
# -----------------------------------------------------------------------------
# ----- Filter repo example for Utility (before moving Utility.hpp):
git filter-repo \
        --path .MASCaPS \
        --path SimpleBuildDriver.sh \
        --path SimpleBuildDriverLocalList.sh \
        --path .gitattributes \
        --path .gitignore \
        --path include/mbcompat.h \
        --path include/mbtypes.h \
        --path Utility \
        --path include/Utility \
        --path include/Utility.hpp
# -----------------------------------------------------------------------------
# #############################################################################

# #############################################################################
# -----------------------------------------------------------------------------
# ----- Find files which include include/Utility.hpp:
grep -P '(<|")Utility\.hpp(>|")' \
   `find . -type f|grep -iP '\.(c|h|i)((pp)|(xx))?$'`
# -----------------------------------------------------------------------------
# ----- Find files which include include/Utility.hpp, but exclude those which
#       are within directory include/Utility (because they won't need to be
#       changed when include/Utility.hpp is moved within include/Utility.
grep -P '(<|")Utility\.hpp(>|")' \
   `find . -type f|grep -iP '\.(c|h|i)((pp)|(xx))?$'` | \
        grep -v '/include/Utility/'
# -----------------------------------------------------------------------------
# #############################################################################

# #############################################################################
# -----------------------------------------------------------------------------
# ----- Filter repo example for Utility:
( pushd UtilityOld ; git filter-repo \
        --path .MASCaPS \
        --path SimpleBuildDriver.sh \
        --path SimpleBuildDriverLocalList.sh \
        --path .gitattributes \
        --path .gitignore \
        --path include/mbcompat.h \
        --path include/mbtypes.h \
        --path Utility \
        --path include/Utility \
        --path include/Utility.hpp )
#
# ----- Filter repo example for SocketIo:
( pushd SocketIo ; git filter-repo \
        --path .MASCaPS \
        --path SimpleBuildDriver.sh \
        --path SimpleBuildDriverLocalList.sh \
        --path .gitattributes \
        --path .gitignore \
        --path include/mbcompat.h \
        --path include/mbtypes.h \
        --path SocketIo \
        --path include/SocketIo \
        --path include/SocketIo.hpp )
#
# ----- Filter repo example for XercesUtils:
( pushd XercesUtils ; git filter-repo \
        --path .MASCaPS \
        --path SimpleBuildDriver.sh \
        --path SimpleBuildDriverLocalList.sh \
        --path .gitattributes \
        --path .gitignore \
        --path include/mbcompat.h \
        --path include/mbtypes.h \
        --path XercesUtils \
        --path include/XercesUtils )
#
# ----- Filter repo example for VFast:
( pushd VFast ; git filter-repo \
        --path .MASCaPS \
        --path SimpleBuildDriver.sh \
        --path SimpleBuildDriverLocalList.sh \
        --path .gitattributes \
        --path .gitignore \
        --path include/mbcompat.h \
        --path include/mbtypes.h \
        --path VFast \
        --path include/VFast )
# #############################################################################

# #############################################################################
# -----------------------------------------------------------------------------
# ----- Enable the USE_XLIBS_FLAG macro in .MASCaPS/MakePrefix.mk:
#       Modifications to stdout:
sed -E 's/^(\s*USE_XLIBS_FLAG\s*=\s*)0(\s*)$/\11\2/' \
   .MASCaPS/MakePrefix.mk
# -----------------------------------------------------------------------------
# ----- Enable the USE_XLIBS_FLAG macro in .MASCaPS/MakePrefix.mk:
#       Modifications to the source input file:
( chmod u+w <repo-dir>/.MASCaPS/MakePrefix.mk && \
   sed -i -E 's/^(\s*USE_XLIBS_FLAG\s*=\s*)0(\s*)$/\11\2/' \
   <repo-dir>/.MASCaPS/MakePrefix.mk \
)
# #############################################################################

# #############################################################################
# NOTE: Run only if SimpleBuildDriverLocalList.TEMPLATE.sh is available.
# -----------------------------------------------------------------------------
# ----- Use sed to create SimpleBuildDriverLocalList.sh files for filter-repos:
( chmod u+w <repo-dir>/SimpleBuildDriverLocalList.sh ; \
   sed -E \
      -e "s/XLIB_REPO_DIR/<repo-dir>/g" \
      -e "s/XLIB_REPO_NAME/<repo-name>/g" \
      -e "s/XLIB_EXTRACT_DATE/<filter-repo-date>/g" \
      -e "s/XLIB_EXTRACT_YEAR/<filter-repo-date-year>/g" \
      SimpleBuildDriverLocalList.TEMPLATE.sh > \
      <repo-dir>/SimpleBuildDriverLocalList.sh
)
# -----------------------------------------------------------------------------
# ----- EXAMPLE (written to stdout):
sed -E \
   -e "s/XLIB_REPO_DIR/MyRepoDir/g" \
   -e "s/XLIB_REPO_NAME/MyRepoName/g" \
   -e "s/XLIB_EXTRACT_DATE/2027-12-31/g" \
   -e "s/XLIB_EXTRACT_YEAR/2027/g" \
   SimpleBuildDriverLocalList.TEMPLATE.sh
# #############################################################################

