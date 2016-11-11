#include <sys/stat.h>
#include <unistd.h>

void makeplot(TString filetotoutavg = "total-TALYS_OUTPUT-nubar-GEFlmf-GEFTEX-avg"){

  TGraph *gnubar = new TGraph(filetotoutavg);
  TGraphErrors *sum = new TGraphErrors(filetotoutavg,format = "%lg %lg %lg");
  
  TGraph *nubarORG = new TGraph("original_Freya_n-multiplicity.dat");
  
  Double_t avg,tot,avgLF,avgHF;

 avg = 0.;
 tot = 0.;
 avgLF = 0.;
 avgHF = 0.;
for (Int_t i = 0; i < sum->GetN(); i++){

avg += sum->GetY()[i]*sum->GetEY()[i];
tot += sum->GetEY()[i];
 if (sum->GetX()[i] < 118) avgLF += sum->GetY()[i]*sum->GetEY()[i];
 if (sum->GetX()[i] > 118) avgHF += sum->GetY()[i]*sum->GetEY()[i];
}
cout << "tot = " << tot << endl;
cout << "avgtot = " << avg << endl;
avg = avg*2./tot;
cout << "avg = " << avg << endl;
 avgLF *= 2./tot;
 avgHF *= 2./tot;
 cout << "avgLF = " << avgLF << endl;
 cout << "avgHF = " << avgHF << endl;
 getchar();
TPaveText *ptnu = new TPaveText(0.1,0.85,0.3,0.9,"blNDC");
ptnu->SetBorderSize(1);
ptnu->SetFillColor(0);
ptnu->AddText(Form("#bar{#nu} = %2.2f",avg));
gnubar->SetTitle("Freya(Fis)+TALYS(FFDeExc) - "+filetotoutavg+" - #nu(A);Mass Number (A);#nu_{avg}");
gnubar->SetMarkerStyle(21);
gnubar->SetMarkerSize(0.8);
gnubar->SetFillColor(0);
gnubar->SetLineStyle(3);
gnubar->Draw("APL");
ptnu->Draw();

TLegend *leg = new TLegend(0.1,0.7,0.3,0.85);
leg->SetFillColor(0);
leg->AddEntry(gnubar,"TALYS-DeExc");

TGraphErrors *gX4 = new TGraphErrors("XS/u235_n-multipl-vs-A=70-166_36meV_Vorobyev.csv","%lg %lg %lg");
gX4->SetLineColor(kRed);
gX4->SetFillColor(0);
gX4->SetMarkerColor(kRed);
gX4->SetMarkerStyle(22);
gX4->SetMarkerSize(0.5);
gX4->Draw("P");


leg->AddEntry(gX4,"Vorobyev et al. (X4)");


TGraphErrors *gX4b = new TGraphErrors("dataset1_Fraser.dat","%lg %lg %lg");
gX4b->SetLineColor(kRed+2.);
gX4b->SetFillColor(0);
gX4b->SetMarkerColor(kRed+2.);
gX4b->SetMarkerStyle(23);
gX4b->SetMarkerSize(0.5);
gX4b->Draw("P");

TGraphErrors *gX4c = new TGraphErrors("dataset2_Nishio.dat","%lg %lg %lg");
gX4c->SetLineColor(kRed+4.);
gX4c->SetFillColor(0);
gX4c->SetMarkerColor(kRed+4.);
gX4c->SetMarkerStyle(24);
gX4c->SetMarkerSize(0.5);
gX4c->Draw("P");

TGraphErrors *gX4d = new TGraphErrors("dataset3_Boldeman.dat","%lg %lg %lg");
gX4d->SetLineColor(kRed-2.);
gX4d->SetFillColor(0);
gX4d->SetMarkerColor(kRed-2.);
gX4d->SetMarkerStyle(25);
gX4d->SetMarkerSize(0.5);
gX4d->Draw("P");

leg->AddEntry(gX4b,"Fraser et al. (X4)","lp");
leg->AddEntry(gX4c,"Nishio et al. (X4)","lp");
leg->AddEntry(gX4d,"Boldeman et al. (X4)","lp");

FILE *talys18file = fopen("nunA000.001.fis","r");

if (talys18file){
  fclose(talys18file);
  TGraph *talys18 = new TGraph("nunA000.001.fis","%lg %lg");
  talys18->SetLineColor(kGreen);
  talys18->SetFillColor(0);
  talys18->SetMarkerColor(kGreen);
  talys18->SetMarkerStyle(23);
  talys18->SetMarkerSize(0.5);
  talys18->Draw("LP");
  leg->AddEntry(talys18,"Talys 1.8");
 }

 FILE *GEFfile = fopen("total-GEF-N_A-dist.dat","r");
 
 if (GEFfile){
   fclose(GEFfile);
   TGraph *GEFnA = new TGraph("total-GEF-N_A-dist.dat","%lg %lg");
   GEFnA->SetLineColor(kBlue);
   GEFnA->SetFillColor(0);
   GEFnA->SetMarkerColor(kBlue);
   GEFnA->SetMarkerStyle(24);
   GEFnA->SetMarkerSize(0.5);
   GEFnA->SetLineStyle(3);
   GEFnA->Draw("LP");
   leg->AddEntry(GEFnA,"GEF");
 }

 nubarORG->SetLineColor(kBlue);
 nubarORG->SetFillColor(0);
 nubarORG->SetMarkerColor(kBlue);
 nubarORG->SetMarkerStyle(32);
 nubarORG->SetMarkerSize(0.5);
 nubarORG->SetLineStyle(1);
 nubarORG->Draw("LP");
 leg->AddEntry(nubarORG,"FREYA standalone");



 leg->Draw();

c1->Print("./plots/FREYA_U235.pdf","pdf");
}
