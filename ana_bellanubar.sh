#!/bin/bash
AA=$1


geflfmfilename="geflmf_*-$AA.ene"
ngeflmffiles=0
if [ -e $geflfmfilename ]; then
	ngeflmffiles=$(ls $geflfmfilename | wc -l)
fi
#	if [ $ngeflmffiles == 15 ]; then
#		skiproot=1
#	fi


# OBS  old call for ana_lmf_nu: 
# ana_lmf_nu($AA,"$ffile",$spinIO,0.) 
# new call, adds one parameter that takes the avg excitation energy. (exitIO was by default set to 1., but now I have to specify it, that's why I add 2 parameters! :) )


#check for existence of an old output file and moves it to "$outputfile_old"
if [ -e "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX" ]
then
	echo "moving file TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX to TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX_old"
	mv "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX" "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX_old"
fi
#check for existence of an old output file (short) and moves it to "$outputfile-short_old"
if [ -e "TALYS_OUTPUT-$AA-GEFlmf-GEFTEX-short" ]
then
	echo "moving file TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short to TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short_old"
	mv "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short" "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short_old"
fi

#write header of the files $outputfile and $outputfile-short
echo "# RESULTS FROM TALYS CALCULATION #" > "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX"
echo "# Also all other residual nuclei are included #" >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX"
echo "# input file (lmf): $2" >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX"

echo "# RESULTS FROM TALYS CALCULATION #" > "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short"
echo "# input file (lmf): $2" >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short"


echo "# RESULTS FROM TALYS CALCULATION #" > "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-avg"
echo "# input file (lmf): $2" >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-avg"
echo "# nubar vs A : " >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-avg"

#loop over all energy files that are found in the folder and that correspond to the input A,Z given to this script
#for file in $(ls geflmf_$AA*.*ene); do #analyse both spin/no-spin files

for file in $(ls $geflfmfilename); do  #analyse only spin/file

clear

#extract the values of Z,A of the system to be simulated in Talys from the energy file-name -> save it in the variables AAA and ZZZ
tmp=$(echo "$file" | cut -d'_' -f2)
ZZZ=$(echo "$tmp" | cut -d'-' -f1)

tmp=$(echo "$file" | cut -d'-' -f2)
AAA=$(echo "$tmp" | cut -d'.' -f1)
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
echo "maxchannel 4" >> tmp_input #maximum number of outgoing particles in exclusive channel description

#print to screen
echo "Creating input file for Talys:"
echo "-----------" 
more tmp_input
echo "-----------"
echo ""

#running Talys calculation. The output for the present energy is saved to TALYS_$ZZ-$AA-$ZZZ-$AAA-GEFlmf-GEFTEX
echo "Running Talys 1.8 ..."
talys-1.8 < tmp_input > "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX"
echo "done!"
echo " "
echo " "
rm tmp_input

#select the section of Talys' output file that prints the Residual Production and print it to a single final output file
echo "------------------------------------------" >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX"
echo "---  $ZZZ-$AAA   ---" >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX"
echo "-------------- $file --------------- " >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX"
echo "------------------------------------------" >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX"
sed -n '/Initial population distribution/,/Q-values/p' "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX"
echo " " >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX"
sed -n '/Residual production cross sections/,/b. Per mass/p' "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX"
echo " " >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX"
echo " " >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX"

totXS=$(sed -n '/Initial population distribution/,/Q-values/p' "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" | awk 'BEGIN{tot=0}($1~/[0-9]/){for (ll=2;ll <= NF;ll++) {tot+=$(ll)}} END{print tot;}')

echo "------------------------------------------" >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short"
echo "---  $ZZZ-$AAA   ---" >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short"
echo "-------------- $file --------------- " >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short"
totout=$(sed -n '/Residual production cross sections/,/b. Per mass/p' "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" |  sed 's/( /(/' | sed 's/ )/)/' | awk -v zzz=$ZZZ 'BEGIN{tot=0;}{if($5==0){ tot += $(NF-3)}} END{print tot}')
echo "--totin = $totXS - totout = $totout" >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short"
echo "------------------------------------------" >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short"

#for some reason the total input XS (nr. of events) is not the same as the output. So we rescale the output with the input to make things work.
corrXS=$(sed -n '/Residual production cross sections/,/b. Per mass/p' "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" |  sed 's/( /(/' | sed 's/ )/)/' | awk -v zzz=$ZZZ -v totcorr=$totXS 'BEGIN{tot=0;}{if($5==0){ tot += $(NF-3)}} END{print totcorr/tot}')
#print short version of the output with number of neutrons:
sed -n '/Residual production cross sections/,/b. Per mass/p' "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" |  sed 's/( /(/' | sed 's/ )/)/' | awk -v aaa=$AAA -v zzz=$ZZZ -v corr=$corrXS 'BEGIN{OFS="\t"}{if($5==0){ print "Z=",$1,"A=",$2,"n=",(aaa-$2)-(zzz-$1),$(NF-3)*corr} } ' >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short"
echo -e "\n" >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short"


tempTOT=$(sed -n '/Residual production cross sections/,/b. Per mass/p' "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" |  sed 's/( /(/' | sed 's/ )/)/'  | awk -v oldtot=$TOTTOT -v aaa=$AAA -v zzz=$ZZZ -v corr=$corrXS 'BEGIN{avg=0;tot=0;OFS="\t"}{if($5==0){avg += $(NF-3)*corr*((aaa-$2)-(zzz-$1)); tot += $(NF-3)*corr}} END{print tot+oldtot} ')
TOTTOT=$tempTOT
tempAVG=$(sed -n '/Residual production cross sections/,/b. Per mass/p' "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" |  sed 's/( /(/' | sed 's/ )/)/' | awk -v oldavg=$AVGTOT -v aaa=$AAA -v zzz=$ZZZ -v corr=$corrXS 'BEGIN{avg=0;tot=0;OFS="\t"}{if($5==0){avg += $(NF-3)*corr*((aaa-$2)-(zzz-$1)); tot += $(NF-3)*corr}} END{print avg+oldavg} ')
AVGTOT=$tempAVG
#cleanup folder from Talys output and the energy file
rm "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX"
rm $file

done

echo "" | awk -v avgsum=$AVGTOT -v tot=$TOTTOT -v aaa=$AAA 'BEGIN{OFS="\t"}END{print "A = "aaa,"nubar = "avgsum/tot}'
echo "" | awk -v avgsum=$AVGTOT -v tot=$TOTTOT -v aaa=$AAA 'BEGIN{OFS="\t"}END{print aaa,avgsum/tot,tot}' >> "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-avg"

#end of loop! SUCCESS! :)
echo "-----------------------------------------------------------------------"
echo "The results have been printed to file TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX"
echo "An output with only Z=$ZZ and A=$AA was printed to TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short"
echo "-----------------------------------------------------------------------"
echo " "



