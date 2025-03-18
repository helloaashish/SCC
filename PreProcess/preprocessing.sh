#!/bin/bash
#----------------------------------------------------
# Sample Slurm job script
#   for TACC Lonestar6 AMD Milan nodes
#
#   *** Serial Job in Normal Queue***
# 
# Last revised: October 22, 2021
#
# Notes:
#
#  -- Copy/edit this script as desired.  Launch by executing
#     "sbatch milan.serial.slurm" on a Lonestar6 login node.
#
#  -- Serial codes run on a single node (upper case N = 1).
#       A serial code ignores the value of lower case n,
#       but slurm needs a plausible value to schedule the job.
#
#  -- Use TACC's launcher utility to run multiple serial 
#       executables at the same time, execute "module load launcher" 
#       followed by "module help launcher".
#----------------------------------------------------

#SBATCH -J livejournal           # Job name
#SBATCH -o livejournal.o%j       # Name of stdout output file
#SBATCH -e livejournal.e%j       # Name of stderr error file
#SBATCH -p normal          # Queue (partition) name
#SBATCH -N 1               # Total # of nodes (must be 1 for serial)
#SBATCH -n 1               # Total # of mpi tasks (should be 1 for serial)
#SBATCH -t 01:00:00        # Run time (hh:mm:ss)
# BATCH --mail-type=all    # Send email at begin and end of job
# BATCH -A myproject       # Project/Allocation name (req'd if you have more than 1)
# BATCH --mail-user=apandey@tacc.utexas.edu



# #STEP 1 change the edge list into 0 vertexed graph
# # input format a b form OriginalDataset folder
# # output n m       To OriginalReady folder
# #        0 1 1 ie src 
APP_DIR="/work/08434/apandey/ls6/SCC-new/SCC1"
EXTERNAL_APPS="/work/08434/apandey/ls6/SCC/Bhowmick_SCC"
export PATH=${APP_DIR}/PreProcess:${PATH}
WORK_DIR=$SCRATCH/${1}
mkdir -p ${WORK_DIR}
DATA_DIR="/work/08434/apandey/ls6/SCC-new/Datasets/OriginalDataset"

source /work/08434/apandey/ls6/venvs/scc_env/bin/activate
cd ${WORK_DIR}
cp ${DATA_DIR}/$1 . # copy the original data to the directory

# step 1 renaming
python3 ${APP_DIR}/PreProcess/rename.py $1 #path to originalfile with edgelist only, with no weight and number of nodes,edges at top
echo "Step 1: Renaming Completed renamed_${1} file created!!"
# #STEP 2 Use Multistep to Create SCCx of the created Graph
# # write thus created SCCx into
${EXTERNAL_APPS}/multistep/scc renamed_$1 sccx_renamed_$1
echo "Step 2: Initial SCC Computation Compelted, sccx_renamed_${1} file created!!"
# step 3 
# Create a metagraph that is 0 vertex and metavertex_map file that has has mapping for sccid
python3 ${APP_DIR}/PreProcess/metagraph.py renamed_$1 
echo "Step 3: Metagraph Creation Compelted, metagraph_renamed_${1} and metavertex_map_renamed_${1} file created!!"


## rm $1 # do cleanup 
## mv renamed_$1 $1
#step 4 generate changed edges

# ./generate_CE.sh graph_name
generate_CE.sh renamed_$1 sccx_renamed_$1 metavertex_map_renamed_$1
echo "Step 4: Changed Edges Created and compressed based on SCCx and Metavertex_map"

# # #step 5
# convert_ce.sh renamed_${1}_1M_100
# renamed_baidu_original.txt_1M_2
# # update the graphs
# /work/08434/apandey/ls6/SCC-new/SCC/PreProcess/updateGraph.sh wiki-en

# # make run the ispan
# /work/08434/apandey/ls6/SCC-new/SCC/PostProcess/ispanTiming.sh 