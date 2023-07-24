#!/bin/bash

# Ensure two arguments are provided
if [ $# -ne 3 ]; then
    echo "Error: This script requires 3 arguments."
    echo "Usage: $0 <source_directory> <target_directory> <chrom_size_path>"
    exit 1
fi

# Variables for source and target directories
src=$1
tgt=$2
chrsz=$3

# Copy .allc.tsv.gz files from source to target directory
cp ${src}/*.allc.tsv.gz ${tgt}
cp ${src}/*.allc.tsv.gz.tbi ${tgt}
cp ${chrsz} ${tgt}

# Loop over each .allc.tsv.gz file in source directory and run command
for file in ${src}/*.allc.tsv.gz
do
    base=$(basename $file .allc.tsv.gz)
    ballcools a2b ${file} ${tgt}/${base}.ballc ${chrsz}
    ballcools index ${tgt}/${base}.ballc
done

# Create a tarball of the target directory
tgt="${tgt%/}"
tar -czvf ${tgt}.tar.gz ${tgt}

echo ""
echo -e  "\e[32m${tgt}.tar.gz is the file generated. you may delete folder ${tgt}\e[0m"
