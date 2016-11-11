#!/bin/bash
ZZ=$1
AA=$2
ffile=$3
clear
#check that the Nr. of parameters is right 
if [ "$#" -ne 3 ]; then
	echo "Illegal number of parameters!"
	echo "3 paremeters needed to run the script"
	echo " "
	echo "Try something like this:"
	echo "./ana_GEFlmf.sh ZZ AA lmf_file"
	exit
fi
#check that A is less than Z
if [ $AA -lt $ZZ ] ; then
	echo "ERROR! A < Z"
	exit
fi
echo "A = $AA --- Z = $ZZ"

#check if perhaps the input file give as input was already the cleaned-up file
if [[ $ffile != "clean"* ]]; then

#check if cleaned-up lmf already exists. If not, cleanup the lmf from comments *,#/blank lines
	if [ ! -e "clean-$ffile" ]; then
		echo "cleaning up file from comments/blank lines..."
		grep -v '^$\|^\s*\#\|^\*\|^\s*$' $ffile > tmp
		mv tmp "clean-$ffile"
		echo "done! File clean-$ffile created!"
	fi
	echo "renaming variable ffile...";
	ffile="clean-$ffile"
	echo "ffile = $ffile";
fi

#ROOT ANALYSIS
#check if the directory plots already exists (used to print TXE-histograms). If not, create it!
echo "Analysing file $ffile in root..."
if [ ! -d "plots" ]; then
	  # Control will enter here if $DIRECTORY doesn't exist.
	mkdir plots
	echo  "folder ./plots created!"
fi


#read the bool variable spinIO (1,0) -> 1 = prints also the energy file with spin information from GEF -- 0 = prints only energy
#echo "Do you want to print also spin information? (0 = only ene; 1 = both .ene and .jene; 2 = only jene)"
#read spinIO

#
spinIO=2
echo "Spin information added by default"
#run the ana_lmf script (it creates the energy files to be fed to Talys)
#the root script will print an energy file for A-->A+4,Z-->Z+2 (and all combinations), in order to
#take into account nuclides that decay to the chosen nuclide after n-/p-emission

skiproot=0

if [ $spinIO == 2 ]; then 
	geflfmfilename="geflmf_$ZZ-$AA-*.jene"
	ngeflmffiles=$(ls $geflfmfilename | wc -l)
	if [ $ngeflmffiles == 15 ]; then
		skiproot=1
	fi
fi
if [ $spinIO == 0 ]; then 
	geflfmfilename="geflmf_$ZZ-$AA-*.ene"
	ngeflmffiles=$(ls $geflfmfilename | wc -l)
	if [ $ngeflmffiles == 15 ]; then
		skiproot=1
	fi
fi
if [ $spinIO == 1 ]; then 
	geflfmfilename="geflmf_$ZZ-$AA-*.*ene"
	ngeflmffiles=$(ls $geflfmfilename | wc -l)
	if [ $ngeflmffiles == 30 ]; then
		skiproot=1
	fi

fi


if [ $skiproot != 1 ]; then
	root -l -q -p "ana_lmf.cpp+($ZZ,$AA,\"$ffile\",$spinIO,0.)"
	echo "Root analysis completed!"
fi

#check for existence of an old output file and moves it to "$outputfile_old"
if [ -e "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX" ]
then
	echo "moving file TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX to TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX_old"
	mv "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX" "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX_old"
fi
#check for existence of an old output file (short) and moves it to "$outputfile-short_old"
if [ -e "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short" ]
then
	echo "moving file TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short to TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short_old"
	mv "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short" "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short_old"
fi

#write header of the files $outputfile and $outputfile-short
echo "# RESULTS FROM TALYS CALCULATION #" > "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX"
echo "# DECAY to FP $ZZ-$AA #" >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX"
echo "# Also all other residual nuclei are included #" >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX"
echo "# input file (lmf): $3" >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX"

echo "# RESULTS FROM TALYS CALCULATION #" > "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short"
echo "# DECAY to FP $ZZ-$AA #" >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short"
echo "# input file (lmf): $3" >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short"

#loop over all energy files that are found in the folder and that correspond to the input A,Z given to this script
#for file in $(ls geflmf_$ZZ-$AA*.*ene); do #analyse both spin/no-spin files
for file in $(ls geflmf_$ZZ-$AA*.*ene); do  #analyse only spin/file

clear

#extract the values of Z,A of the system to be simulated in Talys from the energy file-name -> save it in the variables AAA and ZZZ
ZZZ=`echo "$file" | cut -d'-' -f3`
tmp=`echo "$file" | cut -d'-' -f4`
AAA=`echo "$tmp" | cut -d'.' -f1`
echo "analysing nuclide $ZZZ $AAA"
echo "TEX file: $file"
echo " "

#create input file to be used in Talys
echo "element $ZZZ" > tmp_input
echo "mass $AAA" >> tmp_input
echo "projectile 0" >> tmp_input
echo "energy $file" >> tmp_input
echo "best y" >> tmp_input
echo "channels y" >> tmp_input
echo "preequilibrium y" >> tmp_input
echo "outpreequilibrium y" >> tmp_input

#print to screen
echo "Creating input file for Talys:"
echo "-----------" 
more tmp_input
echo "-----------"
echo ""

#running Talys calculation. The output for the present energy is saved to TALYS_$ZZ-$AA-$ZZZ-$AAA-GEFlmf-GEFTEX
echo "Running Talys..."
talys < tmp_input > "TALYS_$ZZ-$AA-$ZZZ-$AAA-GEFlmf-GEFTEX"
echo "done!"
echo " "
echo " "
rm tmp_input

#select the section of Talys' output file that prints the Residual Production and print it to a single final output file
echo "------------------------------------------" >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX"
echo "---  decay to $ZZ-$AA from $ZZZ-$AAA   ---" >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX"
echo "-------------- $file --------------- " >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX"
echo "------------------------------------------" >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX"
sed -n '/Residual production cross sections/,/b. Per mass/p' "TALYS_$ZZ-$AA-$ZZZ-$AAA-GEFlmf-GEFTEX" >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX"
echo " " >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX"
echo " " >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX"

#print a shorter version of the output with only the isotope of interest
echo "------------------------------------------" >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short"
echo "---  decay to $ZZ-$AA from $ZZZ-$AAA   ---" >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short"
echo "-------------- $file --------------- " >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short"
echo "------------------------------------------" >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short"

#grep -A1 "$ZZ \+$AA" "TALYS_$ZZ-$AA-$ZZZ-$AAA-GEFlmf-GEFTEX" >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short"
awk -v zzz=$ZZ -v aaa=$AA 'BEGIN{isomio=0;}{ if(($1==zzz)&&($2==aaa)&&(isomio==0)){print $0; isomio=1} else if((isomio==1)&&(length($1)==1)){print $0} else {isomio=0;} } ' "TALYS_$ZZ-$AA-$ZZZ-$AAA-GEFlmf-GEFTEX" >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short"



#sed -n '/$ZZ $AA/p' "TALYS_$ZZ-$AA-$ZZZ-$AAA-GEFlmf-GEFTEX" >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short"
#sed -n '/$ZZ $AA/{N;p}' >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short"
echo " " >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short"
echo " " >> "TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short"


#cleanup folder from Talys output and the energy file
rm "TALYS_$ZZ-$AA-$ZZZ-$AAA-GEFlmf-GEFTEX"
rm $file

done

#end of loop! SUCCESS! :)
echo "-----------------------------------------------------------------------"
echo "The results have been printed to file TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX"
echo "An output with only Z=$ZZ and A=$AA was printed to TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-short"
echo "-----------------------------------------------------------------------"
echo " "


awk -v zzz=$ZZ -v aaa=$AA 'BEGIN{isomio=0;print "z=$zzz\ta=$aaa"}{ if(($1==zzz)&&($2==aaa)&&(isomio==0)){print $0; isomio=1} else if((isomio==1)&&(length($1)<=2)&&(NF<7)){print $0} else {isomio=0;} } ' TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX > tempratio

awk -v zzz=$ZZ -v aaa=$AA 'BEGIN{tot = 0; totground = 0; OFS="\t"}{ if (($1==zzz)&&($2==aaa)&&($(NF-2)==0)){print $(NF-3),$(NF-1),$NF; tot+=$(NF-3);totground += $(NF-1)} else if ($NF=="sec.") {print "\t \t \t"$2,$3; totisom[$1] += $2} } END {print totground; for(i in totisom){print totisom[i]}; print tot; print zzz,aaa,"level=\t0",totground/tot; for(i in totisom){print zzz,aaa,"level=",i,totisom[i]/tot}}' tempratio

awk -v zzz=$ZZ -v aaa=$AA 'BEGIN{tot = 0; totground = 0; OFS="\t"}{ if (($1==zzz)&&($2==aaa)&&($(NF-2)==0)){tot+=$(NF-3);totground += $(NF-1)} else if ($NF=="sec.") {totisom[$1] += $2} } END {print zzz,aaa,"level=\t0",totground/tot; for(i in totisom){print zzz,aaa,"level=",i,totisom[i]/tot}}' tempratio > TALYS_OUTPUT-$ZZ-$AA-GEFlmf-GEFTEX-ratio

rm tempratio


