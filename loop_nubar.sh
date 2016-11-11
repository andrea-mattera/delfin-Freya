#!/bin/bash
ffile=$1
spinIO=$2

#check that the Nr. of parameters is right 
if [ "$#" -ne 2 ]; then
	echo "Illegal number of parameters!"
	echo "2 parameters needed to run the script"
	echo " "
	echo "Try something like this:"
	echo "./ana_GEFlmf.sh lmf_file spinIO"
	exit
fi

filetotoutavg="total-TALYS_OUTPUT-nubar-GEFlmf-GEFTEX-avg"
filetotoutshort="total-TALYS_OUTPUT-nubar-GEFlmf-GEFTEX-short"
filetotout="total-TALYS_OUTPUT-nubar-GEFlmf-GEFTEX"
filerootpdf="plots/gnubar-TALYS_OUTPUT.pdf"

if [ $spinIO -eq 2 ]; then
	filetotoutavg="total-TALYS_OUTPUT-jene-nubar-GEFlmf-GEFTEX-avg"
	filetotoutshort="total-TALYS_OUTPUT-jene-nubar-GEFlmf-GEFTEX-short"
	filetotout="total-TALYS_OUTPUT-jene-nubar-GEFlmf-GEFTEX"
	filerootpdf="plots/gnubar-jene-TALYS_OUTPUT.pdf"
	echo "spinIO = $spinIO"
	echo "filetotout = $filetotout"

fi
if [ $spinIO -eq 0 ]; then
	filetotoutavg="total-TALYS_OUTPUT-ene-nubar-GEFlmf-GEFTEX-avg"
	filetotoutshort="total-TALYS_OUTPUT-ene-nubar-GEFlmf-GEFTEX-short"
	filetotout="total-TALYS_OUTPUT-ene-nubar-GEFlmf-GEFTEX"
	filerootpdf="plots/gnubar-ene-TALYS_OUTPUT.pdf"
	echo "spinIO = $spinIO"
	echo "filetotout = $filetotout"
fi

echo "# RESULTS FROM TALYS CALCULATION #" > "$filetotoutavg"
echo "# input file (lmf): $ffile" >> "$filetotoutavg"
echo "# nubar vs A : " >> "$filetotoutavg"

echo "# RESULTS FROM TALYS CALCULATION #" > "$filetotoutshort"
echo "# input file (lmf): $ffile" >> "$filetotoutshort"
echo "# nubar vs A : " >> "$filetotoutshort"

echo "# RESULTS FROM TALYS CALCULATION #" > "$filetotout"
echo "# input file (lmf): $ffile" >> "$filetotout"
echo "# nubar vs A : " >> "$filetotout"

for AA in $(seq 71 1 165) ; do
#for AA in $(seq 166 -1 70) ; do

echo "executing ./ana_GEFnubar.sh $AA $ffile $spinIO"
./ana_GEFnubar.sh $AA $ffile $spinIO

awk -v AAA=$AA '($1 == AAA){print $0}' "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-avg" >> "$filetotoutavg"

echo -e "# MASS NUMBER:\t$AA" >> "$filetotoutshort"
more "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short" >> "$filetotoutshort"
echo "#" >> "$filetotoutshort"
echo "#" >> "$filetotoutshort"
rm "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX-short"

echo -e "# MASS NUMBER:\t$AA" >> "$filetotout"
more "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX" >> "$filetotout"
echo "#" >> "$filetotout"
echo "#" >> "$filetotout"
rm "TALYS_OUTPUT-nubar-$AA-GEFlmf-GEFTEX"


done

sort -t$'\t' -k1 -g -o "$filetotoutavg" "$filetotoutavg"
echo "sorted by increasing mass number"



root -l <<ROOTANASTOP

TGraph *gnubar = new TGraph("$filetotoutavg");
TGraphErrors *sum = new TGraphErrors("$filetotoutavg",format = "%lg %lg %lg");

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


TLegend *leg = new TLegend(0.1,0.7,0.3,0.85);
leg->SetFillColor(0);
leg->AddEntry(gnubar,"TALYS-DeExc");
leg->AddEntry(gX4,"Vorobyev et al. (X4)");


leg->Draw();

c1->Print("$filerootpdf","pdf");

ROOTANASTOP

okular "$filerootpdf" &


