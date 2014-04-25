#!/bin/bash

change_proj()
{
	PWD=`pwd`
	CLEAN_LEVEL=$2

	if [ -f ./.project_root ] && [ "`cat ./.project_root`" != "$PWD" ] ; then
		echo ".............................................................."
		echo ".................... Cleaning old project ...................."
		echo ".............................................................."
		
		CLN=Y
		if [ ${CLEAN_LEVEL} != "2" ] && [ -f ./.project_root ] ; then
        	    echo "The toolchain was build at `cat ./.project_root`"
        	    echo "But you are at $PWD"
        	    echo "Do you want to clean all(including toolchain)"
        	    echo
        	    echo "N) Don't clean all"
        	    echo
        	    read -p "Please input your choice -> " CLN
		fi

		if [ N${CLN} != "NN" ] && [ N${CLN} != "Nn" ] ; then
			CLEAN_LEVEL=2
		fi
	fi

	if [ ${CLEAN_LEVEL} == "2" ]; then
			echo "Clean all"
			rm -rf build_dir/
			rm -rf staging_dir/
			rm -rf tmp/
	fi
	if [ ${CLEAN_LEVEL} == "1" ]; then
		echo "Clean target"
		rm -rf build_dir/linux*
		rm -rf build_dir/mips*
		rm -rf build_dir/target*
		rm -rf staging_dir/target*
		rm -rf tmp/
	fi

	echo ".............................................................."
	echo "        Installing Project Dependant Files for $1 "
	echo ".............................................................."

	if [ -e "./projects/common" ] ; then
		cp -rf ./projects/common/. .
	fi

	if [ -e "./projects/$1" ] ; then
		cp -rf ./projects/$1/. .
	fi

	echo -n "$PWD" > "./.project_root"
	echo -n "${PRJ_NAME}" > "./.project_name"
	echo `date` $1 >> "./.project_history"
}

proj_help()
{
	echo "syntax: $1 [Project Name] [Clean Level]"
	echo "    [Project Name]  - The project name that need to have the folder with the same name in project directory"
	echo "    [Clean Level]   - available value as below"
	echo "        0) Don't clean everything, just copy update files"
	echo "        1) Clean all but Keep host util and tooolchains"
	echo "        2) Remove all host utilities and compiler toolchain"
}

#
# Main
#
if [ $# == 2 ]; then
	if [ -e "./projects/$1" ] ; then
		echo "change_proj $1 $2"
		change_proj $1 $2
	else
		proj_help $0
		exit
	fi
elif [ $# == 1 ]; then
	proj_help $0
	exit
else
		USER_CHOICE=0
		CLEAN_LEVEL=0
		# prompt
		echo
		echo "****************************************************"
		echo "a) 904 DSL"
		echo
		echo "0) exit"
		echo
		read -p "Please input your choice -> " USER_CHOICE

		# check choice
		case ${USER_CHOICE} in
		  0)
				exit
				;;
		  "a"|"A")
				PRJ_NAME=904dsl
				;;
		  *)
				echo "bad choice!"
				;;
		esac

		# prompt
		echo
		echo "****************************************************"
		echo "0) Don't clean everything, just copy update files"
		echo "1) Clean all but Keep host util and tooolchains"
		echo "2) Remove all host utilities and compiler toolchain"
		echo
		echo "e) exit"
		echo
		read -p "Please input your choice -> " CLEAN_LEVEL


	if [ e${CLEAN_LEVEL} == "ee" ]; then
	    exit
	fi
	if [ e${CLEAN_LEVEL} == "e" ]; then
	    CLEAN_LEVEL=0
	fi

        echo
        echo "Change to ${PRJ_NAME}, and do clean level ${CLEAN_LEVEL}"
        echo
	change_proj ${PRJ_NAME} ${CLEAN_LEVEL}

fi

