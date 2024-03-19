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

#SBATCH -J wiki-run           # Job name
#SBATCH -o wiki-run.o%j       # Name of stdout output file
#SBATCH -e wiki-run.e%j       # Name of stderr error file
#SBATCH -p normal          # Queue (partition) name
#SBATCH -N 1               # Total # of nodes (must be 1 for serial)
#SBATCH -n 72               # Total # of mpi tasks (should be 1 for serial)
#SBATCH -t 04:00:00        # Run time (hh:mm:ss)
# BATCH --mail-type=all    # Send email at begin and end of job
# BATCH -A myproject       # Project/Allocation name (req'd if you have more than 1)
# BATCH --mail-user=apandey@tacc.utexas.edu



# #STEP 1 change the edge list into 0 vertexed graph
# # input format a b form OriginalDataset folder
# # output n m       To OriginalReady folder
# #        0 1 1 ie src 
# python3 rename.py temp.txt


# #STEP 2 Use Multistep to Create SCCx of the created Graph
# # write thus created SCCx into
# /work/08434/apandey/ls6/SCC/Bhowmick_SCC/multistep/scc /work/08434/apandey/ls6/SCC-new/Datasets/OriginalReady/baidu /work/08434/apandey/ls6/SCC-new/Datasets/SCCx/baidu

# #step 3 generate changed edges
# ./generate_CE.sh graph_name

 
# python3 metagraph.py pokec
# python3 metagraph.py wiki-com
# python3 metagraph.py wiki-en


# update the graphs
/work/08434/apandey/ls6/SCC-new/SCC/PreProcess/updateGraph.sh wiki-en

# make run the ispan
/work/08434/apandey/ls6/SCC-new/SCC/PostProcess/ispanTiming.sh 