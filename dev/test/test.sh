
#!/bin/bash


cmeta=~/bican/projects/test/h1930001_chrL.cmeta.gz
chrompath=~/scratch2/human_brain_atlas/custom-genome-with_snp/h1930001/h1930001_with_ChrL/h1930001.fa.fai


ballcools=~/local/ballcools/dev/src/ballcools
ballcools=ballcools

## select one pool folder
allc_dir=$(ls -d ~/hba/mapping2/hba_*h1930001*)
allc_dir=${allc_dir%/}
allc_dir=$(echo "$allc_dir" | shuf -n 1)

## select one allc folder
allc_dir=$(ls -d ${allc_dir}/*/allc)
allc_dir=${allc_dir%/}
allc_dir=$(echo "$allc_dir" | shuf -n 1)

echo $allc_dir "selected"

## make output dir
out_dir=out
mkdir -p ${out_dir}





counter_good=0
counter_bad=0

# Function to run the command and echo its return value
test_command() {
    eval "$1"
    if [ $? -eq 0 ]; then
        echo -en "\e[32m[PASS]\e[0m "
        ((counter_good++))
    else
        echo -en "\e[31m[NOTPASS]\e[0m "
        ((counter_bad++))
    fi    
    echo $2
}

show_command() {
    echo $1
}

## check ballcools path
# which $ballcools
# /data/wtian/local/miniconda3/envs/condatest/bin/ballcools

ranges=("chr1:-100000" "chr2:-100000" "chr12:100000-200000" "chrY:57140000-" "chrL" "chr11_rjrjr" "chrUn_KI270312v1")


## test for each allc file
for allc in ${allc_dir}/*.allc.tsv.gz
do
	allcname="${allc##*/}"
    ballcname="${allcname%%.*}.ballc"
	ballc="${out_dir}/${allcname%%.*}.ballc"
    test_command "${ballcools} a2b ${allc} ${ballc} ${chrompath}" "a2b [${ballcname}]"
	test_command "${ballcools} index  ${ballc}" "indexing [${ballcname}]"
	for range in "${ranges[@]}"
	do
		test_command "diff <(tabix ${allc} ${range}) <(${ballcools} query  ${ballc} ${range} -c ${cmeta}) > /dev/null" "query ${range} [${ballcname}]"
	done
done


exit $counter_bad



