#!/bin/bash
#######################################################
# repo populating                                     #
#######################################################

#-----------------------------------------------------#
# Common definitions                                  #
#-----------------------------------------------------#
txtbld=$(tput bold)             # Bold
bldred=${txtbld}$(tput setaf 1) # red
bldgre=${txtbld}$(tput setaf 2) # green
bldblu=${txtbld}$(tput setaf 4) # blue
txtrst=$(tput sgr0)             # Reset

START=$(date +%s)
declare -a cmdarr=()
declare -a PUSHABLE
declare -a DIRECTORY
declare -a REPO
declare -a BRANCH
declare -a VERSION
declare -a SRC_TYPE

build_help=false
cmd_show_repo=false
cmd_update_repo=false
CWD=$(pwd)
#-----------------------------------------------------#
# check "-d" to see if to build with debug binaries   #
#-----------------------------------------------------#
usage() { echo "${bldred}Usage: $0 -h -s -u"
		  echo "  -h	help and show command list"
		  echo "  -s	show repository info"
		  echo "  -u	update or initialize the repositories"
		  echo "${txtrst}"
}

while getopts ":hus" opt; do
    case $opt in
        h)
            usage
            build_help=true
            ;;
        s)
            cmd_show_repo=true
            ;;
        u)
            cmd_update_repo=true
            ;;
        \?)
            echo "Invalid option: -$OPTARG"
            usage
            exit 1
            ;;
    esac
done

#######################################################
# This function will execute  cmds one by one         #
#######################################################
# execution function
safeRunCommand() {
    typeset cmnd="$*"
    typeset ret_code
    now=$(date +"%T")
    if [ $cmd_show_repo == false ]; then
	    echo "${bldred}[${now}]Running: ${bldblu}${cmnd}${txtrst}"
	fi
    eval $cmnd
    ret_code=$?
    if [ $ret_code != 0 ]; then
        printf "${bldred}Error[%d]: ${bldblu}${cmnd}${txtrst}\n" $ret_code
        exit $ret_code
    fi
}

#######################################################
# This function will execute  cmds one by one         #
#######################################################
trim() {
    local var=$@
    var="${var#"${var%%[![:space:]]*}"}"   # remove leading whitespace characters
    var="${var%"${var##*[![:space:]]}"}"   # remove trailing whitespace characters
    echo -n "$var"
}

getRepoConfig() {
    i=0
    while read line || [ -n "$line" ] # Read a line
    do
        trimedLine=`trim "$line"`
        if [ $cmd_show_repo == true ]; then
	        echo "$trimedLine"
	    fi
        if  ! [[ $trimedLine == \#* ]] && [ -n "$trimedLine" ]; then
            cnf_array[i]="$trimedLine" # Put it into the cnf_array
            i=$(($i + 1))
        fi
    done < $1
    
	for str in "${cnf_array[@]}"
	do
		IFS=', ' read -a cnfg_details <<< "$str"
		if [[ "${#cnfg_details[@]}" -lt 6 ]]; then
			echo "wrong configuration: need at least 6 elements"
			exit -1
		fi
		PUSHABLE=("${PUSHABLE[@]}" "${cnfg_details[0]}")
		DIRECTORY=("${DIRECTORY[@]}" "${cnfg_details[1]}")
		REPO=("${REPO[@]}" "${cnfg_details[2]}")
		BRANCH=("${BRANCH[@]}" "${cnfg_details[3]}")
		VERSION=("${VERSION[@]}" "${cnfg_details[4]}")
		SRC_TYPE=("${SRC_TYPE[@]}" "${cnfg_details[5]}")
	done
}

populate_dir() {
	for i in "${!DIRECTORY[@]}"; do 
	    if ! [ -d "${DIRECTORY[$i]}" ]; then
    	    cmdarr=("${cmdarr[@]}" "mkdir -p ${DIRECTORY[$i]}")
    	    if ! [ "${PUSHABLE[$i]}" == "x" ]; then
	    	    cmdarr=("${cmdarr[@]}" "git clone git@github.com:GraphSQL/${REPO[$i]}.git  ${DIRECTORY[$i]}")
	    	fi
	    fi
	done
}

populate_repos() {
	for i in "${!DIRECTORY[@]}"; do
		if [ "${VERSION[$i]}" == "latest" ]; then
	    	cmdarr=("${cmdarr[@]}" "cd ${DIRECTORY[$i]}; git checkout ${BRANCH[$i]}; git fetch --tags; git fetch; cd ${CWD}")
	    else
	    	if ! [ "${VERSION[$i]}" == "*" ]; then
		    	cmdarr=("${cmdarr[@]}" "cd ${DIRECTORY[$i]}; git checkout ${BRANCH[$i]}; git fetch --tags; git fetch; git checkout ${VERSION[$i]} -q; cd ${CWD}")
		    fi
	    fi
	done
}


show_repos() {
	for i in "${!DIRECTORY[@]}"; do
		cd "${DIRECTORY[$i]}"
		cmdarr=("${cmdarr[@]}" "echo; echo ${bldblu}REPO:${bldgre}${REPO[$i]}")
		cmdarr=("${cmdarr[@]}" "echo ${bldblu}DIR :${bldgre}${DIRECTORY[$i]}")
    	cmdarr=("${cmdarr[@]}" "echo -ne ${bldblu}TAG :${bldgre}")
		cmdarr=("${cmdarr[@]}" "cd ${DIRECTORY[$i]}; git name-rev --tags --name-only $(git rev-parse HEAD)")
	    cmdarr=("${cmdarr[@]}" "echo -ne ${bldblu}SHA :${bldgre}")
		cmdarr=("${cmdarr[@]}" "git name-rev $(git rev-parse HEAD); cd ${CWD}")
		cd "${CWD}"
	done
}


update_gitignore_file()
{

     # initialize a local var
     local gitignore_file=".gitignore"
     local temp_ignore_file=".gitignore_tmp"

     if [ ! -f "$gitignore_file" ] ; then
         # if not create the file
         touch "$gitignore_file"
     fi

    local i=0
    while read line || [ -n "$line" ] # Read a line
    do
        trimedLine=`trim "$line"`
		ignore_array[i]="$trimedLine" # Put ignore into the ignore_array
		i=$(($i + 1))
    done < $gitignore_file


	echo -ne "" > "$temp_ignore_file"

	# write out existing ignore rules minus other repos
	for str in "${ignore_array[@]}"
	do
	    local found=0
		for repo_dir in "${DIRECTORY[@]}"
		do
			if [ "$str" == "$repo_dir" ]; then
				found=1
			fi
		done

		if [ "$found" == 0 ]; then
			echo "$str" >> "$temp_ignore_file"
		fi
	done

	# write out other repo dirs
	for i in "${!DIRECTORY[@]}"; do
		if [ "${PUSHABLE[$i]}" != "+" ] && [ "${PUSHABLE[$i]}" != "x" ]; then
			echo "${DIRECTORY[$i]}" >> "$temp_ignore_file"
		fi
	done
	
	#update ignore file
	mv  $temp_ignore_file $gitignore_file
}


#######################################
# Start from here                     #
#######################################
getRepoConfig "proj.config"

if [ $cmd_update_repo == true ]; then
	update_gitignore_file
	populate_dir
	populate_repos
fi

if [ $cmd_show_repo == true ]; then
	show_repos
fi


# run all the commands defines in cmdarr
for (( i = 0; i < ${#cmdarr[@]} ; i++ )); do
    if $build_help ; then
        echo "${bldblu}${cmdarr[$i]}${txtrst}"
    else
        safeRunCommand ${cmdarr[$i]}
    fi
done

END=$(date +%s)
DIFF=$(( $END - $START ))
echo
echo "${bldgre}$0 finished successfully in ${DIFF} seconds. ${txtrst}"





