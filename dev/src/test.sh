LD_LIBRARY_PATH=${CONDA_PREFIX}/lib
ballcools=~/local/ballcools/dev/src/ballcools
allc=~/hba/mapping2/hba_pool0_h1930001_CBV/HBA_191114_H1930001_CB60_CBV_1_P2-4-D7/allc/HBA_191114_H1930001_CB60_CBV_1_P2-4-D7-K8.allc.tsv.gz
ballc=test/test.ballc
cmeta=~/bican/projects/test/h1930001_chrL.cmeta.gz
chrompath=~/scratch2/human_brain_atlas/custom-genome-with_snp/h1930001/h1930001_with_ChrL/h1930001.fa.fai

make clean
make
rm test/test.ballc* test/HBA_*

${ballcools} a2b ${allc} ${ballc} ${chrompath} && echo "pass a2b"
${ballcools} index  ${ballc} && echo "pass indexing"



ranges=("chr1:-100000" "chr2:-100000" "chr12:100000-200000" "chrY:57140000-" "chrL" "chr11_rjrjr" "chrUn_KI270312v1")

for range in "${ranges[@]}"
do
	diff <(tabix ${allc} ${range}) <(${ballcools} query  ${ballc} ${range} -c ${cmeta} )|head -5
	if [ $? -eq 0 ]; then
		echo -en "\e[32mPASS\e[0m"
	else
		echo -en "\e[31mNOTPASS\e[0m"
	fi
	echo " " $range
done

for allc in ~/hba/mapping2/hba_pool0_h1930001_CBV/HBA_191114_H1930001_CB60_CBV_1_P2-4-D7/allc/HBA_191114_H1930001_CB60_CBV_1_P2-4-D7-K*.allc.tsv.gz
do
	fn="${allc##*/}"
	ballc="test/${fn%%.*}.ballc"
	${ballcools} a2b ${allc} ${ballc} ${chrompath} && echo "pass a2b [${allc}]"
	${ballcools} index  ${ballc} && echo "pass indexing [${ballc}]"
	for range in "${ranges[@]}"
	do
		diff <(tabix ${allc} ${range}) <(${ballcools} query  ${ballc} ${range} -c ${cmeta} )|head -5
		if [ $? -eq 0 ]; then
			echo -en "\e[32mPASS\e[0m"
		else
			echo -en "\e[31mNOTPASS\e[0m"
		fi
		echo " query" $range " [${ballc}]"
	done
done

${ballcools} merge test/merged.HBA_191114_H1930001_CB60_CBV_1_P2-4-D7-K.ballc \
	test/HBA_191114_H1930001_CB60_CBV_1_P2-4-D7-K*.ballc && echo "pass merge"
${ballcools} index test/merged.HBA_191114_H1930001_CB60_CBV_1_P2-4-D7-K.ballc \
	&& echo "pass index merge"
for range in "${ranges[@]}"
do
	diff <(tabix test/merged.HBA_191114_H1930001_CB60_CBV_1_P2-4-D7-K.allc.tsv.gz ${range}) \
		<(${ballcools} query  test/merged.HBA_191114_H1930001_CB60_CBV_1_P2-4-D7-K.ballc ${range} -c ${cmeta} )
	if [ $? -eq 0 ]; then
		echo -en "\e[32mPASS\e[0m"
	else
		echo -en "\e[31mNOTPASS\e[0m"
	fi
	echo " query" $range " [${ballc}]"
done
