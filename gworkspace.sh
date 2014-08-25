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

cmd_help=false
cmd_show_repo=false
cmd_update_repo=false
cmd_config_info=false
cmd_show_info=false
MY_PATH="`dirname \"$0\"`"              # relative
MY_PATH="`( cd \"$MY_PATH\" && pwd )`"  # absolutized and normalized
cd ${MY_PATH}
CWD=$(pwd)

#-----------------------------------------------------#
# check "-d" to see if to build with debug binaries   #
#-----------------------------------------------------#
usage() { echo "${bldred}Usage: $0 -h -i -s -u -c"
    echo "  -h       help and show command list"
    echo "  -s       show repository status"
    echo "  -i       show repository information"
    echo "  -c       config information"
    echo "  -u       update or initialize the repositories using ssh"
    echo "  -x  https  [username password]"
    echo "           update or initialize the repositories using https"
    echo "${txtrst}"
}

while getopts ":hiscux:" opt; do
    case $opt in
        h)
            cmd_help=true
            ;;
        s)
            cmd_show_repo=true
            ;;
        i)
            cmd_show_info=true
            ;;
        u)
            cmd_update_repo=true
            gitclone_cmd="git@github.com:GraphSQL/"
            ;;
        x)
            cmd_update_repo=true
            clone_use_https=$OPTARG
            ;;
        c)
            cmd_config_info=true
            ;;
        \?)
            echo "Invalid option: -$OPTARG"
            usage
            exit 1
            ;;
    esac
done

shift $((OPTIND-1))

if [ $cmd_update_repo ];
then
    if [[ "$clone_use_https" == "https" ]];
    then
        user_password=""
        if [ -n "$1" ];
        then
            if [ -n "$2" ];
            then
                user_password="$1:$2@"
            else
                user_password="$1@"
            fi
        fi
        gitclone_cmd="https://${user_password}github.com/GraphSQL/"
    else
        gitclone_cmd="git@github.com:GraphSQL/"
        if [[ "$clone_use_https" == "-h" ]];
        then
            cmd_help=true
            echo "$clone_use_https"
        fi
    fi
fi

#######################################################
# This function will execute  cmds one by one         #
#######################################################
# execution function
safeRunCommand() {
    typeset cmnd="$*"
    typeset ret_code
    now=$(date +"%T")
    if [ $cmd_show_repo == false ] && [ $cmd_show_info == false ]; then
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
# trim a string                                       #
#######################################################
trim() {
    local var=$@
    var="${var#"${var%%[![:space:]]*}"}"   # remove leading whitespace characters
    var="${var%"${var##*[![:space:]]}"}"   # remove trailing whitespace characters
    echo -n "$var"
}

#######################################################
# Read in repo config file                            #
#######################################################
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

#######################################################
# For new project, populate dirs and clone repos      #
#######################################################
populate_dir() {
    for i in "${!DIRECTORY[@]}"; do 
        if ! [ -d "${DIRECTORY[$i]}" ]; then
            cmdarr=("${cmdarr[@]}" "mkdir -p ${DIRECTORY[$i]}")
            if ! [ "${PUSHABLE[$i]}" == "x" ]; then
                cmdarr=("${cmdarr[@]}" "git clone ${gitclone_cmd}${REPO[$i]}.git  ${DIRECTORY[$i]}")
            fi
        fi
    done
}

#######################################################
# Populate or update repos                            #
#######################################################
populate_repos() {
    for i in "${!DIRECTORY[@]}"; do
        if ! [ "${PUSHABLE[$i]}" == "x" ]; then
            if [ "${VERSION[$i]}" == "latest" ]; then
                cmdarr=("${cmdarr[@]}" "cd ${DIRECTORY[$i]}; git checkout ${BRANCH[$i]}; git fetch --tags; git pull; cd ${CWD}")
            else
                if ! [ "${VERSION[$i]}" == "*" ]; then
                    cmdarr=("${cmdarr[@]}" "cd ${DIRECTORY[$i]}; git checkout ${BRANCH[$i]}; git fetch --tags; git pull; git checkout ${VERSION[$i]} -q; cd ${CWD}")
                fi
            fi
        fi
    done
}

############################################
# show if a branch is dirty                #
############################################
print_git_dirty() {
  local status=$(git status --porcelain 2> /dev/null)
  if [[ "$status" != "" ]]; then
    printf ${bldred}
  fi
}

############################################
# show if a branch need push               #
############################################
print_git_need_push() {
  local status=$(git status | grep "commit\.\|commits\.")
  if [[ "$status" != "" ]]; then
    printf "    ${bldred}(${status})${txtrst}\n"
  fi  
}

############################################
# get branch name                          #
############################################
function parse_git_branch() {
  git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/ \1/'
}

############################################
# get tag of a branch                      #
############################################
parse_git_tag () {
  git describe --tags 2> /dev/null
#    local tag=`git name-rev --tags --name-only $(git rev-parse HEAD)|sed 's/\^.*//'`
#    echo "${tag#*^ }"
}
############################################
# get commit of a branch                   #
############################################
parse_git_commit() {
  git log --pretty=format:'%h' -n 1
}

############################################
# get branch, or tag, or commit            #
############################################
parse_git_branch_or_tag() {
  local OUT="$(parse_git_branch)"
  if [[ $OUT == *detached* ]]
  then
    OUT="$(parse_git_tag)";
    if [ -z "$OUT" ]
    then
      OUT="$(parse_git_commit)"
    fi
  fi
  echo $OUT
}


#######################################################
# Show repos                                          #
#######################################################
show_repos() {
    for i in "${!DIRECTORY[@]}"; do
        cd "${DIRECTORY[$i]}"
        cmdarr=("${cmdarr[@]}" "echo; echo ${bldblu}REPO:${bldgre}${REPO[$i]}")
        cmdarr=("${cmdarr[@]}" "echo ${bldblu}DIR :${bldgre}${DIRECTORY[$i]}")
        cmdarr=("${cmdarr[@]}" "echo -ne ${bldblu}TAG :${bldgre}")
        cmdarr=("${cmdarr[@]}" "cd ${DIRECTORY[$i]}; print_git_dirty; parse_git_branch_or_tag; print_git_need_push")
        cmdarr=("${cmdarr[@]}" "echo -ne ${bldblu}SHA :${bldgre}")
        cmdarr=("${cmdarr[@]}" "git name-rev $(git rev-parse HEAD); cd ${CWD}")
        cd "${CWD}"
    done
}

#######################################################
# Show repos info in details                          #
#######################################################
show_info() {
    for i in "${!DIRECTORY[@]}"; do
        cd "${DIRECTORY[$i]}"
        cmdarr=("${cmdarr[@]}" "echo; echo ${bldblu}REPO:${bldgre}${REPO[$i]}")
        cmdarr=("${cmdarr[@]}" "echo ${bldblu}DIR :${bldgre}${DIRECTORY[$i]}")
        cmdarr=("${cmdarr[@]}" "echo -ne ${bldblu}TAG :${bldgre}")
        cmdarr=("${cmdarr[@]}" "cd ${DIRECTORY[$i]}; print_git_dirty; parse_git_branch_or_tag; print_git_need_push")
        cmdarr=("${cmdarr[@]}" "printf \"${txtrst}\"; git status; cd ${CWD}")
        cd "${CWD}"
    done
}

#######################################################
# Show current config info. Similar to show_repos but #
# the output is concise to be included in package     #
#######################################################
config_info() {
    printf '%-20s %-20s %s  ' "product" "$(parse_git_branch_or_tag)" $(git rev-parse HEAD)
    echo "$(git log --pretty=format:%ci -1)"
    for i in "${!DIRECTORY[@]}"; do
        cd "${DIRECTORY[$i]}"
        local timestr=$(git log --pretty=format:%ci -1)
        local tag="$(parse_git_branch_or_tag)"  #$(git name-rev --tags --name-only $(git rev-parse HEAD))
        local formatedTag="${tag%^0}"
        printf '%-20s %-20s %s  ' ${REPO[$i]} $formatedTag $(git rev-parse HEAD)
        echo "$timestr"
        cd "${CWD}"
    done
}

#######################################################
# Ignore the other repose as they are read only       #
#######################################################
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
getRepoConfig "config/proj.config"

if [ $cmd_update_repo == true ]; then
    update_gitignore_file
    populate_dir
    populate_repos
fi

if [ $cmd_show_repo == true ]; then
    show_repos
fi

if [ $cmd_show_info == true ]; then
    show_info
fi

if [ $cmd_config_info == true ]; then
    config_info
    exit
fi

if [ ${#cmdarr[@]} == 0 ]; then
    usage
    exit
fi

# run all the commands defines in cmdarr
for (( i = 0; i < ${#cmdarr[@]} ; i++ )); do
    if $cmd_help ; then
        echo "${bldblu}${cmdarr[$i]}${txtrst}"
    else
        safeRunCommand ${cmdarr[$i]}
    fi
done

END=$(date +%s)
DIFF=$(( $END - $START ))
echo
echo "${bldgre}$0 finished successfully in ${DIFF} seconds. ${txtrst}"





