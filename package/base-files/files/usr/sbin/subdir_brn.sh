
#
# $1 - root directory of subdirectory
# $2 - output file name
# $3 - 1: single layer sub-directories, others: all sub-directories
#

if [ -z "$2" ] ; then
	exit 1
fi

PWD_TMP=`pwd`

if [ "$3" != "1" ] ; then
	cd "$1"; ( find ./ -type d | sort ) > "$2"
else
	cd "$1"; ( ls -1l | awk 'BEGIN { print "./" } /\<d/ { print "./"$7}' ) > "$2"
fi

cd "$PWD_TMP"


