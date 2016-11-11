#!/bin/bash
#AA=$1
clear

#check that the Nr. of parameters is right 
if [ "$#" -ne 0 ]; then
	echo "Illegal number of parameters!"
	echo "0 parameters needed to run the script"
	echo " "
	echo "Try something like this:"
	echo "./ana_bellanubar.sh"
	exit
fi


#ROOT ANALYSIS
#check if the directory plots already exists (used to print TXE-histograms). If not, create it!
echo "Analysing file $ffile in root..."
if [ ! -d "plots" ]; then
	  # Control will enter here if $DIRECTORY doesn't exist.
	mkdir plots
	echo  "folder ./plots created!"
fi

root -l <<ROOTANASTOP
.L ana_anabella.cpp
ana_bella("FF_energies_FG.DAT",236,92)

ROOTANASTOP

#read the bool variable spinIO (1,0) -> 1 = prints also the energy file with spin information from GEF -- 0 = prints only energy
#echo "Do you want to print also spin information? (0 = only ene; 1 = both .ene and .jene; 2 = only jene)"
#read spinIO

#

#echo "Spin information added by default"
#run the ana_lmf script (it creates the energy files to be fed to Talys)
#the root script will print an energy file for A-->A+4,Z-->Z+2 (and all combinations), in order to
#take into account nuclides that decay to the chosen nuclide after n-/p-emission

	geflfmfilename="geflmf_*-*.ene"
	ngeflmffiles=0
	if [ -e $geflfmfilename ]; then
		ngeflmffiles=$(ls $geflfmfilename | wc -l)
	fi


#check for existence of an old output file and moves it to "$outputfile_old"
if [ -e "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX" ]
then
	echo "moving file TALYS_OUTPUT-nubar-GEFlmf-GEFTEX to TALYS_OUTPUT-nubar-GEFlmf-GEFTEX_old"
	mv "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX" "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX_old"
fi
#check for existence of an old output file (short) and moves it to "$outputfile-short_old"
if [ -e "TALYS_OUTPUT-GEFlmf-GEFTEX-short" ]
then
	echo "moving file TALYS_OUTPUT-nubar-GEFlmf-GEFTEX-short to TALYS_OUTPUT-nubar-GEFlmf-GEFTEX-short_old"
	mv "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX-short" "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX-short_old"
fi

#write header of the files $outputfile and $outputfile-short
echo "# RESULTS FROM TALYS CALCULATION #" > "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX"
echo "# Also all other residual nuclei are included #" >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX"

echo "# RESULTS FROM TALYS CALCULATION #" > "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX-short"

echo "# RESULTS FROM TALYS CALCULATION #" > "data_file.dat"
echo "# nubar vs A : " >> "data_file.dat"

for file in $(ls $geflfmfilename); do  #analyse only spin/file

TOTTOT=0
AVGTOT=0

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

echo "# RESULTS FROM TALYS CALCULATION #" > "TALYS_OUTPUT-nubar-GEFlmf-$AAA-GEFTEX-avg"
echo "# nubar vs A : " >> "TALYS_OUTPUT-nubar-GEFlmf-$AAA-GEFTEX-avg"

#select the section of Talys' output file that prints the Residual Production and print it to a single final output file
echo "------------------------------------------" >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX"
echo "---  $ZZZ-$AAA   ---" >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX"
echo "-------------- $file --------------- " >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX"
echo "------------------------------------------" >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX"
sed -n '/Initial population distribution/,/Q-values/p' "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX"
echo " " >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX"
sed -n '/Residual production cross sections/,/b. Per mass/p' "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX"
echo " " >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX"
echo " " >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX"

totXS=$(sed -n '/Initial population distribution/,/Q-values/p' "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" | awk 'BEGIN{tot=0}($1~/[0-9]/){for (ll=2;ll <= NF;ll++) {tot+=$(ll)}} END{print tot;}')

echo "------------------------------------------" >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX-short"
echo "---  $ZZZ-$AAA   ---" >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX-short"
echo "-------------- $file --------------- " >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX-short"
totout=$(sed -n '/Residual production cross sections/,/b. Per mass/p' "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" |  sed 's/( /(/' | sed 's/ )/)/' | awk -v zzz=$ZZZ 'BEGIN{tot=0;}{if($5==0){ tot += $(NF-3)}} END{print tot}')
echo "--totin = $totXS - totout = $totout" >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX-short"
echo "------------------------------------------" >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX-short"

#for some reason the total input XS (nr. of events) is not the same as the output. So we rescale the output with the input to make things work.
corrXS=$(sed -n '/Residual production cross sections/,/b. Per mass/p' "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" |  sed 's/( /(/' | sed 's/ )/)/' | awk -v zzz=$ZZZ -v totcorr=$totXS 'BEGIN{tot=0;}{if($5==0){ tot += $(NF-3)}} END{print totcorr/tot}')
#print short version of the output with number of neutrons:
sed -n '/Residual production cross sections/,/b. Per mass/p' "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" |  sed 's/( /(/' | sed 's/ )/)/' | awk -v aaa=$AAA -v zzz=$ZZZ -v corr=$corrXS 'BEGIN{OFS="\t"}{if($5==0){ print "Z=",$1,"A=",$2,"n=",(aaa-$2)-(zzz-$1),$(NF-3)*corr} } ' >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX-short"
echo -e "\n" >> "TALYS_OUTPUT-nubar-GEFlmf-GEFTEX-short"

tempTOT=$(sed -n '/Residual production cross sections/,/b. Per mass/p' "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" |  sed 's/( /(/' | sed 's/ )/)/'  | awk -v oldtot=$TOTTOT -v aaa=$AAA -v zzz=$ZZZ -v corr=$corrXS 'BEGIN{avg=0;tot=0;OFS="\t"}{if($5==0){avg += $(NF-3)*corr*((aaa-$2)-(zzz-$1)); tot += $(NF-3)*corr}} END{print tot+oldtot} ')
TOTTOT=$tempTOT
tempAVG=$(sed -n '/Residual production cross sections/,/b. Per mass/p' "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" |  sed 's/( /(/' | sed 's/ )/)/' | awk -v oldavg=$AVGTOT -v aaa=$AAA -v zzz=$ZZZ -v corr=$corrXS 'BEGIN{avg=0;tot=0;OFS="\t"}{if($5==0){avg += $(NF-3)*corr*((aaa-$2)-(zzz-$1)); tot += $(NF-3)*corr}} END{print avg+oldavg} ')
AVGTOT=$tempAVG
#cleanup folder from Talys output and the energy file
mv "TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX" "./raw/TALYS-nubar_$ZZZ-$AAA-GEFlmf-GEFTEX"
rm $file

echo "" | awk -v avgsum=$AVGTOT -v tot=$TOTTOT -v aaa=$AAA 'BEGIN{OFS="\t"}END{print "A = "aaa,"nubar = "avgsum/tot}'
echo "" | awk -v avgsum=$AVGTOT -v tot=$TOTTOT -v aaa=$AAA 'BEGIN{OFS="\t"}END{print aaa,avgsum/tot,tot}' >> "data_file.dat"
echo "" | awk -v avgsum=$AVGTOT -v tot=$TOTTOT -v aaa=$AAA 'BEGIN{OFS="\t"}END{print aaa,avgsum/tot,tot}' >> "TALYS_OUTPUT-nubar-GEFlmf-$AAA-GEFTEX-avg"


done


#end of loop! SUCCESS! :)
echo "-----------------------------------------------------------------------"
echo "The results have been printed to file TALYS_OUTPUT-nubar-GEFlmf-GEFTEX"
#echo "An output with only Z=$ZZ and A=$AA was printed to TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short"
echo "-----------------------------------------------------------------------"
echo " "



filetotoutavg="total-TALYS_OUTPUT-ene-nubar-GEFlmf-GEFTEX-avg"
echo "# RESULTS FROM TALYS CALCULATION #" > "$filetotoutavg"
echo "# input file (lmf): $ffile" >> "$filetotoutavg"
echo "# nubar vs A : " >> "$filetotoutavg"


for AA in $(seq 71 1 165) ; do

awk -v AAA=$AA '($1 == AAA){print $0}' "TALYS_OUTPUT-nubar-GEFlmf-$AA-GEFTEX-avg" >> "$filetotoutavg"
rm "TALYS_OUTPUT-nubar-GEFlmf-$AA-GEFTEX-avg"

done

root -l <<ROOTANASTOP

//TGraph *gnubar = new TGraph("$filetotoutavg");
TGraph *gnubar = new TGraph("data_file.dat");
TGraphErrors *sum = new TGraphErrors("data_file.dat",format = "%lg %lg %lg");

Double_t avg,tot;

avg = 0.;
tot = 0.;
 
for (Int_t i = 0; i < sum->GetN(); i++){

avg += sum->GetY()[i]*sum->GetEY()[i];
tot += sum->GetEY()[i];

}
cout << "tot = " << tot << endl;
cout << "avgtot = " << avg << endl;
avg = avg*2./tot;
cout << "avg = " << avg << endl;

TPaveText *ptnu = new TPaveText(0.1,0.85,0.3,0.9,"blNDC");
ptnu->SetBorderSize(1);
ptnu->SetFillColor(0);
ptnu->AddText(Form("#bar{#nu} = %2.2f",avg));
gnubar->SetTitle("GEF(Fis)+TALYS(FFDeExc) - $ffile - #nu(A);Mass Number (A);#nu_{avg}");
gnubar->SetMarkerStyle(21);
gnubar->SetMarkerSize(0.8);
gnubar->SetFillColor(0);
gnubar->Draw("AP");
ptnu->Draw();

TGraphErrors *gX4 = new TGraphErrors("XS/u235_n-multipl-vs-A=70-166_36meV_Vorobyev.csv","%lg %lg %lg");
gX4->SetLineColor(kRed);
gX4->SetFillColor(0);
gX4->SetMarkerColor(kRed);
gX4->SetMarkerStyle(22);
gX4->SetMarkerSize(0.5);
gX4->Draw("LP");

TString filenameFY = Form("XS/u235_n-fis_FY.csv");
TGraph *FYinfo = new TGraph(filenameFY);
Double_t scaleFY = gPad->GetUymax()/(TMath::MaxElement(FYinfo->GetN(),FYinfo->GetY())*1.1);
for (Int_t ii = 0; ii < FYinfo->GetN() ; ii++) FYinfo->GetY()[ii] *= scaleFY;

FYinfo->SetLineColor(4);
FYinfo->Draw("L");
FYinfo->SetLineWidth(2);
FYinfo->SetLineStyle(3);

TString filenameFY2 = Form("XS/FY_GEFdat_236_092.dat");
TGraph *FYinfo2 = new TGraph(filenameFY2);
Double_t scaleFY2 = gPad->GetUymax()/(TMath::MaxElement(FYinfo2->GetN(),FYinfo2->GetY())*1.1);
for (Int_t ii = 0; ii < FYinfo2->GetN() ; ii++) FYinfo2->GetY()[ii] *= scaleFY2;

FYinfo->SetLineColor(6);
FYinfo->Draw("L");
FYinfo->SetLineWidth(2);
FYinfo->SetLineStyle(3);


TLegend *leg = new TLegend(0.1,0.7,0.3,0.85);
leg->SetFillColor(0);
leg->AddEntry(gnubar,"A.Tudora-TALYS-DeExc");
leg->AddEntry(gX4,"Vorobyev et al. (X4)");


leg->Draw();

c1->Print("anabella.pdf","pdf");

ROOTANASTOP

okular anabella.pdf &
