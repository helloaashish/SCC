#!/bin/bash

# This script convert the changed edges into reduced mapped form
# Input is changed_edges(a b x), sccx: original sccx file from multistep, map_file: new mapping after metagraph renaming
# Check if exactly three arguments are provided

if [[ $# -ne 3 ]]; then
    echo "Usage: $0 <a.txt> <sccx> <map.txt>"
    exit 1
fi

# Assign input filenames from command-line arguments
CE_FILE=$1
SCCX_FILE=$2
MAP_FILE=$3

# Check if files exist
if [[ ! -f "$CE_FILE" || ! -f "$SCCX_FILE" || ! -f "$MAP_FILE" ]]; then
    echo "Error: One or more input files are missing."
    exit 1
fi

# First Pass: Replace values in a.txt using sccx
awk '
    NR==FNR { scc[NR-1] = $1; next }  # Read sccx into an array with line numbers as keys
    NR>FNR { 
        # Create the map (if required, or apply further transformations here)
        if ($1 in scc) $1 = scc[$1];  # Replace column 1 with mapped value from scc
        if ($2 in scc) $2 = scc[$2];  # Replace column 2 with mapped value from scc
        print $1, $2, $3  # Print the transformed values with original third column
    }
' "$SCCX_FILE" "$CE_FILE" > temp_output.txt  # Save intermediate output

awk '
    NR==FNR { map[$1] = $2; next }  # Read map.txt into dictionary
        { 
        if ($1 in map) $1 = map[$1];  # Replace if found in map.txt
        if ($2 in map) $2 = map[$2];  # Replace if found in map.txt
        if ($1 == $2) next;           # Skip rows where column 1 == column 2
        output[$1, $2, $3] = $1 " " $2 " " $3;  # Store unique rows in an associative array
    }
    END {
        for (row in output) print output[row];  # Print only unique rows
    }
' "$MAP_FILE" temp_output.txt > renamed_ce_$CE_FILE
# Cleanup temporary file
rm temp_output.txt
echo "Processing complete! Unique output saved in renamed_ce_$CE_FILE."


