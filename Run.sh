#!/bin/sh
# module load g++
# module --ignore_cache load "g++"


# Get the hostname
HOSTNAME=$(hostname)

# Check if the hostname contains "tacc"
if [[ "$HOSTNAME" == *"tacc"* ]]; then
    DATA_DIR=${SCRATCH}/${1}
    APP_DIR="/work/08434/apandey/ls6/SCC-new/SCC1"
elif [[ "$HOSTNAME" == *"artemis"* ]]; then
    DATA_DIR="/home/users/apandey/SCC-new"
else
    RUNPATH="/defaultpath"
fi

cd ${APP_DIR}
make all
export PATH=${APP_DIR}:${PATH}
# artemis_path
# tacc_path="/work/08434/apandey/ls6"

if [ $1 ]; then
    echo ./a.out ${DATA_DIR}/metagraph_renamed_${1} ${DATA_DIR}/renamed_ce_renamed_${1}_1M_100 ${2} ${3}
    ./a.out ${DATA_DIR}/metagraph_renamed_${1} ${DATA_DIR}/renamed_ce_renamed_${1}_1M_100 ${2} ${3}
    fi
if [ $1 = "test" ]; then
echo ./a.out ${DATA_DIR}/metagraph_renamed_${1} ${DATA_DIR}/renamed_ce_renamed_${1}_1M_100 ${2} ${3}
    ./a.out ${DATA_DIR}/metagraph_renamed_${1} ${DATA_DIR}/renamed_ce_renamed_${1}_1M_100 ${2} ${3}
    fi
if [ $1 = "livejournal" ]; then
    echo ./a.out ${DATA_DIR}/metagraph_renamed_${1} ${DATA_DIR}/renamed_ce_renamed_${1}_1M_100 ${2} ${3}
    ./a.out ${DATA_DIR}/metagraph_renamed_${1} ${DATA_DIR}/renamed_ce_renamed_${1}_1M_100 ${2} ${3}
    fi
