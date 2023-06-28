
#!/bin/bash


cmeta=~wtian/bican/projects/test/h1930001_chrL.cmeta.gz
chrompath=~wtian/scratch2/human_brain_atlas/custom-genome-with_snp/h1930001/h1930001_with_ChrL/h1930001.fa.fai


ballcools=~wtian/local/ballcools/dev/src/ballcools
ballcools=ballcools

## select one pool folder
allc_dir=$(ls -d ~wtian/hba/mapping2/hba_*h1930001*)
allc_dir=${allc_dir%/}
allc_dir=$(echo "$allc_dir" | shuf -n 1)

## select one allc folder
allc_dir=$(ls -d ${allc_dir}/*/allc)
allc_dir=${allc_dir%/}
allc_dir=$(echo "$allc_dir" | shuf -n 1)

echo "folder " $allc_dir "selected"
echo ""

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


## randomly generate genome ranges
random_range() {
    local chrs=("chr1" "chr2" "chr3" "chr4" "chr5" "chr6" "chr7" "chr8" \
            "chr9" "chr10" "chr11" "chr12" "chr13" "chr14" "chr15" "chr16" \
            "chr17" "chr18" "chr19" "chr20" "chr21" "chr22" "chrM"  "chrX" \
            "chrY" "chrL" 
            "chr1" "chr2" "chr3" "chr4" "chr5" "chr6" "chr7" "chr8" \
            "chr9" "chr10" "chr11" "chr12" "chr13" "chr14" "chr15" "chr16" \
            "chr17" "chr18" "chr19" "chr20" "chr21" "chr22" "chrM"  "chrX" \
            "chrY" "chrL" 
            "chr1_KI270714v1_random" "chr22_KI270731v1_random" \
            "chr22_KI270732v1_random" "chr9_KI270717v1_random" "chr17_GL383563v3_alt" \
            "chr19_GL949746v1_alt" "chr20_GL383577v2_alt" "chrUn_KI270363v1" \
            "chr111" "chrxxx" "chrXY" "chrUn_KI270363v1ddd" "chrUn_KI27036")
    local min_pos=0
    local max_pos=7108863

    # Generate a random chromosome prefix
    local random_chr=${chrs[$RANDOM % ${#chrs[@]}]}

    # Generate random start and end positions
    local random_start=$(shuf -i $min_pos-$max_pos -n 1)
    # local random_end=$(shuf -i $min_pos-$max_pos -n 1)

    # if [ "$random_start" -gt "$random_end" ]; then
    #     local temp="$random_start"
    #     random_start="$random_end"
    #     random_end="$temp"
    # fi
    local random_end=$((random_start + 5000))

    # Construct the genome range
    local grange="${random_chr}:${random_start}-${random_end}"
    echo "$grange"
}
num_ranges=20
ranges=()
for ((i = 0; i < num_ranges; i++))
do
    grange=$(random_range)
    ranges+=("$grange")
done

## append fixed test cases
ranges+=("chr1:-100000" "chr2:-100000" "chr12:100000-200000" "chrY:57140000-" "chrL" "chr11_rjrjr" "chrUn_KI270312v1")



## test for each allc file
for allc in ${allc_dir}/*.allc.tsv.gz
do
	allcname="${allc##*/}"
    ballcname="${allcname%%.*}.ballc"
	ballc="${out_dir}/${allcname%%.*}.ballc"
    test_command "${ballcools} a2b ${allc} ${ballc} ${chrompath} > /dev/null" "a2b [${ballcname}]"
	test_command "${ballcools} index  ${ballc} > /dev/null" "indexing [${ballcname}]"
	for range in "${ranges[@]}"
	do
		test_command "diff <(tabix ${allc} ${range}) <(${ballcools} query  ${ballc} ${range} -c ${cmeta}) > /dev/null" "query ${range} [${ballcname}]"
	done
done

echo -en "\e[32m${counter_good} good test case(s)\e[0m"
echo -en " and "
echo -e  "\e[31m${counter_bad} bad test case(s)\e[0m" 
exit $counter_bad



