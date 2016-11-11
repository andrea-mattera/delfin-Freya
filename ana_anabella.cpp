#include <TH1.h>
#include <TH2.h>
#include <TTree.h>
#include <TApplication.h>
#include <TCanvas.h>
#include "TROOT.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <TNtuple.h>
#include <TMath.h>
#include <TFile.h>
#include "test_wahl_quantities.cpp"


#define print 0

//version 2016-03-10

using namespace std;

/*
//    _      __  __ ______   _   _ _    _ ____          _____  
//   | |    |  \/  |  ____| | \ | | |  | |  _ \   /\   |  __ \ 
//   | |    | \  / | |__    |  \| | |  | | |_) | /  \  | |__) |
//   | |    | |\/| |  __|   | . ` | |  | |  _ < / /\ \ |  _  / 
//   | |____| |  | | |      | |\  | |__| | |_) / ____ \| | \ \ 
//   |______|_|  |_|_|      |_| \_|\____/|____/_/    \_\_|  \_\
//                                                             
//                                                             
*/

TGraphErrors* ana_bella(TString infile = "FF_energies_FG.DAT",Int_t Acn, Int_t Zcn,Double_t talysVersion = 1.8){
  //                                                 _FG.DAT
  /*
  Eex = new TNtuple("Eex","Eex","A:Eexc:Dedef:Efullacc");
  Eex->ReadFile(infile);
  Int_t Amin = Eex->GetMinimum("A");
  Int_t Amax = Eex->GetMaximum("A");
  */
  //  TString filenameFY = Form("XS/FY_GEFdat_%3.3i_%3.3i.dat",Acn,Zcn);
  TString filenameFY = infile+"_FY-ENDFB7.1"; //Form("XS/u%3.3i_n-fis_FY.csv",Acn-1);
  //  TFile fileFY(filenameFY);
  
  //if (fileFY.Sizeof() == 0.) {
  //  cout << "*******************************" << endl;
  //  cout << "ERROR! No information about FY!" << endl;
  //  return;
  // }

  TGraph *gFYA = new TGraph(filenameFY);
 

  //  TGraph *Zavg = new TGraph("Zavg.dat");
  //(infile,"%lg %lg %*lg %*lg"); // AT SCISSION
  //(infile,"%lg %*lg %*lg %lg"); // AT FULL ACCELERATION
  //  TGraphErrors *Eex = new TGraphErrors(infile,"%lg %*lg %*lg %lg"); // FULL ACCELERATION
  TGraphErrors *Eex = new TGraphErrors(infile,"%lg %lg %*lg %*lg"); // SCISSION
  //  Eex->Draw("AWLP");
  // return Eex;



  Double_t AA,eneAvg,ZZ,eneRMS;
  Double_t Zmin = 24;
  Double_t Zmax = 70;

  Int_t Na = Eex->GetN();
  Double_t Amin = Eex->GetX()[0];
  Double_t Amax = Eex->GetX()[Na-1];

  TGraph *gNN = fNNcreate(Amin,Amax);
     


  for(Int_t ii = 0; ii < Na; ii++){
    
    AA = Eex->GetX()[ii];
    eneAvg = Eex->GetY()[ii];

    //    eneAvg *= 1.2;
    eneRMS = 0.35*eneAvg;
    
    Double_t Aprod = fAprod((Double_t) AA);
    Double_t NN = gNN->Eval(Aprod);
    cout << "NN = " << NN << endl;
    TF1 *fWahl = new TF1("fFI",Form("FI(x,%f,%f)",Aprod,NN),(Double_t) Zmin,(Double_t) Zmax);
    Double_t intWahl = fWahl->Integral((Double_t) Zmin,(Double_t) Zmax);
    Double_t intEexAA = gFYA->Eval(Aprod)*10000.;
    cout << "intEexAA = " << intEexAA << endl; 
    cout << "intfWahl = " << intWahl << endl; 
    TF1 *fWahlNorm = new TF1("fFINorm",Form("%f*FI(x,%f,%f)",intEexAA/intWahl,Aprod,NN),(Double_t) Zmin,(Double_t) Zmax);
    
    for (Double_t ZZ = Zmin; ZZ <= Zmax; ZZ++){
      
      cout << "ZZ = " << ZZ << endl;
      cout << "fWahlNorm(" << ZZ << ") = " << fWahlNorm(ZZ) << endl;
      if (fWahlNorm(ZZ) == 0) {
	cout << "No events found. I skip to the next ZZ" << endl;
      }
      else {
#if print
      cout << "AA = " << AA << endl;
      cout << "eneAvg = " << eneAvg << endl;
      cout << "ZZ = " << ZZ << endl;
      cout << "eneRMS = " << eneRMS << endl;
      
      cout << "filename: " << Form("geflmf_%i-%i.ene",TMath::Nint(ZZ),TMath::Nint(AA)) << endl;
      getchar();
#endif
      //Create energy-file
      ofstream file;
      
      file.open (Form("geflmf_%i-%i.ene",TMath::Nint(ZZ),TMath::Nint(AA)));
      
      //      file << hEne_nbins << " 0 1 " << hEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;
      
      
      file << "3 0 1" ;
      
      switch (talysVersion) {
      case 1.8:
	file << endl;
	cout << "Creating .ene input for Talys-1.8" << endl;
	break;
      case 1.6:
	file << " " << (eneAvg + 4*eneRMS) << endl;
	cout << "Creating .ene input for Talys-1.6" << endl;
	break;
      default:
	file << " " << (eneAvg + 4*eneRMS) << endl;
	break;
      }
      
      Double_t tot_bincontent = 0.;
      tot_bincontent = fWahlNorm( (Double_t) ZZ );
      file << eneAvg - 2*eneRMS << " " << 0. << endl;
      file << eneAvg << " " << tot_bincontent << endl;
      file << eneAvg + 2*eneRMS << " " << 0. << endl;
      file.close();
      
      }}}
  
  return;
  
}



// void ana_lmf_nu(Int_t AA,TString infile = "clean-U235+n_th.lmf", Int_t spinIO = 0/*0 == print only .ene; 1== print .ene + .jene; 2 == print only .jene */, Int_t printIO = 0, Int_t exitIO = 1, Int_t avgIO = 0,  Double_t talysVersion = 1.8)
// {

//   TString rootinfile = infile+".root";


//   TFile rootfile(rootinfile);
//   TNtuple *Eex;
//   if (rootfile.IsZombie()){
//     cout << "File "<< rootinfile << " doesn't exist. Reading file " << infile << " and creating Ntuple..." << endl;
    
//     Eex = new TNtuple("Eex","Eex","Z1:Z2:A1pre:A2pre:A1post:A2post:I1pre:I2pre:I1gs:I2gs:Eexc1:Eexc2:n1:n2:TKEpre:TKEpost");
//     cout << "Ntuple created. " << endl;
//     Eex->ReadFile(infile);
//     cout << "Ntuple filled from file " << infile << ". " << endl;
//     TFile rootfile2(rootinfile,"recreate");
//     Eex->Write();
//     rootfile2.Close();
//     cout << "File "<< rootinfile << " created! :)" << endl;
//   }
//   else {
//     Eex = (TNtuple *) rootfile.Get("Eex");
//   }
  
//   Int_t Zmin = TMath::Min(Eex->GetMinimum("Z1"),Eex->GetMinimum("Z2"));
//   Int_t Zmax = TMath::Max(Eex->GetMaximum("Z1"),Eex->GetMaximum("Z2"));

  

//   TCanvas *c1 = new TCanvas("c1","c1");
//   c1->Update();
  
//   cout << "A = " << AA << " | Zmin = " << Zmin << " | Zmax = " << Zmax << endl;

//   if (avgIO == 2.) {
//     Zmax = Zmin;
//       Eex->Draw("(A1pre+A2pre)>>hAcn");
//       Double_t Acn = hAcn->GetMean();
//       Eex->Draw("(Z1+Z2)>>hZcn");
//       Double_t Zcn = hZcn->GetMean();
      
//       cout << "Compound Nucleus: Z="<<Zcn << "  - A="<< Acn << endl;
      
//   }
  
//   for (Int_t ZZ = Zmin; ZZ <= Zmax; ZZ++){
    

//     Double_t enemax = TMath::Max(Eex->GetMaximum("Eexc1"),Eex->GetMaximum("Eexc2"));
    
//     TH1F *hEne1 = new TH1F("hEne1","hEne1",50,0.,enemax);
//     TH1F *hEne2 = new TH1F("hEne2","hEne2",50,0.,enemax);
    

//     //**************************************************************
//     //                        ### #######               #   
//     //     ##   #    #  ####   #  #     #              ##   
//     //    #  #  #    # #    #  #  #     #    #####    # #   
//     //   #    # #    # #       #  #     #               #   
//     //   ###### #    # #  ###  #  #     #    #####      #   
//     //   #    #  #  #  #    #  #  #     #               #   
//     //   #    #   ##    ####  ### #######             ##### 
//     //                                                      
//     //**************************************************************
//     //
//     // IN CASE WE WANT THE AVERAGE, WE JUST WRITE ONE LINE IN THE TALYS .energy FILE:
//     if (avgIO==1){
//       cout << "We are in the new avg analysis. (avgIO = 1)" << endl;
    
//       Eex->Draw("Eexc1>>hEne1",Form("(Z1==%i)&&(A1pre==%i)",ZZ,AA));
//       Eex->Draw("Eexc2>>+hEne1",Form("(Z2==%i)&&(A2pre==%i)",ZZ,AA));
//       getchar();
//       if (hEne1->GetEntries() == 0) {
	
// 	cout << "No events found!!! Skipping A = " << AA << " Z = " << ZZ << "." << endl;
	
// 	delete hEne1;
// 	delete hEne2;
// 	getchar();
// 	continue; //if Nr. of events == 0 doesn't create the .ene/.jene files
//       }    
      
//       cout << " I found some events! I continue..." <<endl;
//       getchar();
//       Double_t eneAvg = hEne1->GetMean(1);
//       Double_t eneRMS = hEne1->GetRMS(1);
      
//     //Create energy-file
//     ofstream file;
    
//     Int_t hEne_nbins = hEne1->GetNbinsX();
    
//     if (spinIO!=2){
//       file.open (Form("geflmf_%i-%i.ene",ZZ,AA));

//       //      file << hEne_nbins << " 0 1 " << hEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;


//       file << "3 0 1" ;

//       switch (talysVersion) {
//       case 1.8:
// 	file << endl;
// 	cout << "Creating .ene input for Talys-1.8" << endl;
// 	break;
//       case 1.6:
// 	file << " " << (eneAvg + 4*eneRMS) << endl;
// 	cout << "Creating .ene input for Talys-1.6" << endl;
// 	break;
//       default:
// 	file << " " << (eneAvg + 4*eneRMS) << endl;
// 	break;
//       }

//       Int_t tot_bincontent = 0.;
//       tot_bincontent = hEne1->GetEntries();
//       file << eneAvg - 2*eneRMS << " " << 0. << endl;
//       file << eneAvg << " " << tot_bincontent << endl;
//       file << eneAvg + 2*eneRMS << " " << 0. << endl;
//       file.close();
//     }
    
//     if (spinIO!=0){

//       cerr << "SpinIO must be 0!" << endl;

//       return;
    
//     /* ofstream filespin;
      
//       Int_t hEne_nbins2 = hJEne1->GetNbinsX();
//       Int_t hJ_nbins = hJEne1->GetNbinsY();
      
//       filespin.open (Form("geflmf_%i-%i.jene",ZZ,AA));

//       // n_energy_bins n_J_bins n_parities ene_max

//       //      filespin << hEne_nbins2 << " " << hJ_nbins <<" 1 " << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;

//       //ALTERNATIVE CODE TO ACCOUNT FOR THE DIFFERENCE OF THE NEW TALYS VERSION!!!
//       filespin << hEne_nbins2 << " " << hJ_nbins <<" 1 " ;

//       switch (talysVersion) {
//       case 1.8:
// 	filespin << endl;
// 	cout << "Creating .jene input for Talys-1.8" << endl;
// 	break;
//       case 1.6:
// 	filespin << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins2) << endl;
// 	cout << "Creating .jene input for Talys-1.6" << endl;
// 	break;
//       default:
// 	filespin << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins2) << endl;
// 	break;
//       }

//       Int_t tot_bincontent2 = 0.;
//       for(Int_t i = 1; i <= hEne_nbins2; i++) {
// 	filespin << hEne1->GetBinCenter(i);
// 	for (Int_t j = 1; j <= hJ_nbins; j++){
// 	  tot_bincontent2 = hJEne1->GetBinContent(i,j) + hJEne2->GetBinContent(i,j);
// 	  filespin << " " << tot_bincontent2;
// 	}
// 	filespin << endl;
//       }
      
//       filespin.close();
//     */
//       }
    
//     //      continue;
//     //      hEne->Delete();
    
//     delete hEne1;
//     delete hEne2;

//     //hEne->Reset();



      
      
//       continue; //with this we should go to the next iteration.
//     }



//     //**************************************************************
//     //                        ### #######              #### 
//     //     ##   #    #  ####   #  #     #             #    #
//     //    #  #  #    # #    #  #  #     #    #####        # 
//     //   #    # #    # #       #  #     #                #  
//     //   ###### #    # #  ###  #  #     #    #####      #   
//     //   #    #  #  #  #    #  #  #     #              #    
//     //   #    #   ##    ####  ### #######             ######
//     //                                                      
//     //**************************************************************
//     //
//     // IN CASE WE WANT THE AVERAGE, WE JUST WRITE ONE LINE IN THE TALYS .energy FILE and only 1 Z
//     if (avgIO == 2){
//       cout << "We are in the new avg analysis. (avgIO = 2)" << endl;
    
//       Eex->Draw("Eexc1>>hEne1",Form("(A1pre==%i)",AA));
//       Eex->Draw("Eexc2>>+hEne1",Form("(A2pre==%i)",AA));
//       getchar();
//       if (hEne1->GetEntries() == 0) {
	
// 	cout << "No events found!!! Skipping A = " << AA << "." << endl;
	
// 	delete hEne1;
// 	delete hEne2;
// 	getchar();
// 	continue; //if Nr. of events == 0 doesn't create the .ene/.jene files
//       }    
      


//       cout << " I found some events! I continue..." <<endl;
//       getchar();
//       Double_t eneAvg = hEne1->GetMean(1);
//       Double_t eneRMS = hEne1->GetRMS(1);
      
//     //Create energy-file
//     ofstream file;
    
//     Int_t hEne_nbins = hEne1->GetNbinsX();

//     ZZavg = TMath::Nint(Zcn*AA/Acn);
//     cout << "Zavg = " << ZZavg << endl;

//     if (spinIO!=2){
//       file.open (Form("geflmf_%i-%i.ene",ZZavg,AA));

//       //      file << hEne_nbins << " 0 1 " << hEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;


//       file << "3 0 1" ;

//       switch (talysVersion) {
//       case 1.8:
// 	file << endl;
// 	cout << "Creating .ene input for Talys-1.8" << endl;
// 	break;
//       case 1.6:
// 	file << " " << (eneAvg + 4*eneRMS) << endl;
// 	cout << "Creating .ene input for Talys-1.6" << endl;
// 	break;
//       default:
// 	file << " " << (eneAvg + 4*eneRMS) << endl;
// 	break;
//       }

//       Int_t tot_bincontent = 0.;
//       tot_bincontent = hEne1->GetEntries();
//       file << eneAvg - 2*eneRMS << " " << 0. << endl;
//       file << eneAvg << " " << tot_bincontent << endl;
//       file << eneAvg + 2*eneRMS << " " << 0. << endl;
//       file.close();
//     }
    
//     if (spinIO!=0){

//       cerr << "SpinIO must be 0!" << endl;

//       return;
    
//     /* ofstream filespin;
      
//       Int_t hEne_nbins2 = hJEne1->GetNbinsX();
//       Int_t hJ_nbins = hJEne1->GetNbinsY();
      
//       filespin.open (Form("geflmf_%i-%i.jene",ZZ,AA));

//       // n_energy_bins n_J_bins n_parities ene_max

//       //      filespin << hEne_nbins2 << " " << hJ_nbins <<" 1 " << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;

//       //ALTERNATIVE CODE TO ACCOUNT FOR THE DIFFERENCE OF THE NEW TALYS VERSION!!!
//       filespin << hEne_nbins2 << " " << hJ_nbins <<" 1 " ;

//       switch (talysVersion) {
//       case 1.8:
// 	filespin << endl;
// 	cout << "Creating .jene input for Talys-1.8" << endl;
// 	break;
//       case 1.6:
// 	filespin << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins2) << endl;
// 	cout << "Creating .jene input for Talys-1.6" << endl;
// 	break;
//       default:
// 	filespin << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins2) << endl;
// 	break;
//       }

//       Int_t tot_bincontent2 = 0.;
//       for(Int_t i = 1; i <= hEne_nbins2; i++) {
// 	filespin << hEne1->GetBinCenter(i);
// 	for (Int_t j = 1; j <= hJ_nbins; j++){
// 	  tot_bincontent2 = hJEne1->GetBinContent(i,j) + hJEne2->GetBinContent(i,j);
// 	  filespin << " " << tot_bincontent2;
// 	}
// 	filespin << endl;
//       }
      
//       filespin.close();
//     */
//       }
    
//     //      continue;
//     //      hEne->Delete();
    
//     delete hEne1;
//     delete hEne2;

//     //hEne->Reset();



      
      
//       continue; //with this we should go to the next iteration.
//     }







//     //                        ### #######               ###   
//     //     ##   #    #  ####   #  #     #              #   #  
//     //    #  #  #    # #    #  #  #     #    #####    #     # 
//     //   #    # #    # #       #  #     #             #     # 
//     //   ###### #    # #  ###  #  #     #    #####    #     # 
//     //   #    #  #  #  #    #  #  #     #              #   #  
//     //   #    #   ##    ####  ### #######               ###   
//     //                                                        
//     cout << "We are in the default non-avg analysis." << endl;
//     Double_t spinmax = TMath::Max(Eex->GetMaximum("I1pre"),Eex->GetMaximum("I2pre"));
//     if (spinmax >= 30.) spinmax=29;
//     Int_t binspin = TMath::FloorNint(spinmax) + 1;
//     Double_t *binYspin;
//     binYspin = (Double_t *)  malloc((binspin + 1) * sizeof(Double_t));
    
//     for (Int_t ll = 0; ll < binspin+1 ; ll++) binYspin[ll] = ll - 0.25;
    
    
//     TH2F *hJEne1 = new TH2F("hJEne1","hJEne1",50,0.,enemax,binspin,binYspin);
//     TH2F *hJEne2 = new TH2F("hJEne2","hJEne2",50,0.,enemax,binspin,binYspin);
    
//     Eex->Draw("Eexc1>>hEne1",Form("(Z1==%i)&&(A1pre==%i)",ZZ,AA));
//     Eex->Draw("Eexc2>>hEne2",Form("(Z2==%i)&&(A2pre==%i)",ZZ,AA));
    
//     Eex->Draw("I1pre:Eexc1>>hJEne1",Form("(Z1==%i)&&(A1pre==%i)",ZZ,AA));
//     Eex->Draw("I2pre:Eexc2>>hJEne2",Form("(Z2==%i)&&(A2pre==%i)",ZZ,AA));
//     //Eex->Draw("Eexc1:I1pre>>hJEne1",Form("(Z1==%i)&&(A1pre==%i)",ZZ,AA));
//     //Eex->Draw("Eexc2:I2pre>>hJEne2",Form("(Z2==%i)&&(A2pre==%i)",ZZ,AA));
    
//     if ((hEne1->GetEntries() + hEne2->GetEntries()) == 0) {
      
//       cout << "No events found!!! Skipping A = " << AA << " Z = " << ZZ << "." << endl;
      
//       delete hEne1;
//       delete hEne2;
//       delete hJEne1;
//       delete hJEne2;

//       continue; //if Nr. of events == 0 doesn't create the .ene/.jene files
//     }    
    
//     hEne1->SetTitle(Form("hEne %i-%i;Energy (MeV);Nr. events",ZZ,AA));
//     hEne2->SetTitle(Form("hEne %i-%i;Energy (MeV);Nr. events",ZZ,AA));
    
//     hJEne1->SetTitle(Form("hJEne %i-%i;Energy (MeV);Spin Jpre;Nr. events",ZZ,AA));
//     hJEne2->SetTitle(Form("hJEne %i-%i;Energy (MeV);Spin Jpre;Nr. events",ZZ,AA));
    
//     if (printIO){
//     if ((hEne1->GetEntries()*hEne2->GetEntries()) != 0.){
//       TH1F *hEne1clone = (TH1F*)hEne1->Clone("hEneclone");
//       hEne1clone->SetTitle(Form("hEne %i-%i;Energy (MeV);Nr. events",ZZ,AA));
//       hEne1clone->Add(hEne2,1.);
//       hEne1clone->Draw();
//       c1->Update();
//       c1->Print(Form("plots/hEne12-%i-%i.pdf",ZZ,AA),"pdf");
//     }
//     if (hEne1->GetEntries() != 0.){
//       hEne1->Draw();
//       c1->Update();
//       if (printIO) c1->Print(Form("plots/hEne1-%i-%i.pdf",ZZ,AA),"pdf");
//       hJEne1->Draw();
//       c1->Update();
      
//       if (printIO) c1->Print(Form("plots/hJEne1-%i-%i.pdf",ZZ,AA),"pdf");
//     }
//     if (hEne2->GetEntries() != 0.){
//       hEne2->Draw();
//       c1->Update();
//       if (printIO)	c1->Print(Form("plots/hEne2-%i-%i.pdf",ZZ,AA),"pdf");
//       hJEne2->Draw();
//       c1->Update();
//       if (printIO) c1->Print(Form("plots/hJEne2-%i-%i.pdf",ZZ,AA),"pdf");
//     }}
//     //Create energy-file
//     ofstream file;
    
//     Int_t hEne_nbins = hEne1->GetNbinsX();
    
//     if (spinIO!=2){
//       file.open (Form("geflmf_%i-%i.ene",ZZ,AA));

//       //      file << hEne_nbins << " 0 1 " << hEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;

            
//       file << hEne_nbins << " 0 1 " ;

//       switch (talysVersion) {
//       case 1.8:
// 	file << endl;
// 	cout << "Creating .ene input for Talys-1.8" << endl;
// 	break;
//       case 1.6:
// 	file << hEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;
// 	cout << "Creating .ene input for Talys-1.6" << endl;
// 	break;
//       default:
// 	file << hEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;
// 	break;
//       }

      
//       Int_t tot_bincontent = 0.;
//       for(Int_t i = 1; i <= hEne_nbins; i++) {
// 	tot_bincontent = hEne1->GetBinContent(i) + hEne2->GetBinContent(i);
// 	file << hEne1->GetBinCenter(i) << " " << tot_bincontent << endl;
//       }
      
//       file.close();
//     }
    
//     if (spinIO!=0){
//       ofstream filespin;
      
//       Int_t hEne_nbins2 = hJEne1->GetNbinsX();
//       Int_t hJ_nbins = hJEne1->GetNbinsY();
      
//       filespin.open (Form("geflmf_%i-%i.jene",ZZ,AA));

//       // n_energy_bins n_J_bins n_parities ene_max

//       //      filespin << hEne_nbins2 << " " << hJ_nbins <<" 1 " << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;

    
//       //ALTERNATIVE CODE TO ACCOUNT FOR THE DIFFERENCE OF THE NEW TALYS VERSION!!!


//       filespin << hEne_nbins2 << " " << hJ_nbins <<" 1 " ;

//       switch (talysVersion) {
//       case 1.8:
// 	filespin << endl;
// 	cout << "Creating .jene input for Talys-1.8" << endl;
// 	break;
//       case 1.6:
// 	filespin << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins2) << endl;
// 	cout << "Creating .jene input for Talys-1.6" << endl;
// 	break;
//       default:
// 	filespin << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins2) << endl;
// 	break;
//       }
  


//       Int_t tot_bincontent2 = 0.;
//       for(Int_t i = 1; i <= hEne_nbins2; i++) {
// 	filespin << hEne1->GetBinCenter(i);
// 	for (Int_t j = 1; j <= hJ_nbins; j++){
// 	  tot_bincontent2 = hJEne1->GetBinContent(i,j) + hJEne2->GetBinContent(i,j);
// 	  filespin << " " << tot_bincontent2;
// 	}
// 	filespin << endl;
//       }
      
//       filespin.close();
      
//     }
    
//     //      continue;
//     //      hEne->Delete();
    
//     delete hEne1;
//     delete hEne2;
//     delete hJEne1;
//     delete hJEne2;
//     //hEne->Reset();
    
//   }
//   /*  TCanvas *c2 = new TCanvas("c2","c2");
//       Eex->Draw("J:Ene>>hJEne");
//       hJEne->Draw("colz");*/
  
//   if (exitIO) gApplication->Terminate();
  
// }


// void writeZavg(TString infile = "clean-U235+n_th.lmf"){

// TString rootinfile = infile+".root";


//   TFile rootfile(rootinfile);
//   TNtuple *Eex;
//   if (rootfile.IsZombie()){
//     cout << "File "<< rootinfile << " doesn't exist. Reading file " << infile << " and creating Ntuple..." << endl;
    
//     Eex = new TNtuple("Eex","Eex","Z1:Z2:A1pre:A2pre:A1post:A2post:I1pre:I2pre:I1gs:I2gs:Eexc1:Eexc2:n1:n2:TKEpre:TKEpost");
//     cout << "Ntuple created. " << endl;
//     Eex->ReadFile(infile);
//     cout << "Ntuple filled from file " << infile << ". " << endl;
//     TFile rootfile2(rootinfile,"recreate");
//     Eex->Write();
//     rootfile2.Close();
//     cout << "File "<< rootinfile << " created! :)" << endl;
//   }
//   else {
//     Eex = (TNtuple *) rootfile.Get("Eex");
//   }
  
//  Int_t Zmin = TMath::Min(Eex->GetMinimum("Z1"),Eex->GetMinimum("Z2"));
//   Int_t Zmax = TMath::Max(Eex->GetMaximum("Z1"),Eex->GetMaximum("Z2"));

  

//   TCanvas *c1 = new TCanvas("c1","c1");
//   c1->Update();
  
//   cout << "A = " << AA << " | Zmin = " << Zmin << " | Zmax = " << Zmax << endl;

//   if (avgIO == 2.) {
//     Zmax = Zmin;
//       Eex->Draw("(A1pre+A2pre)>>hAcn");
//       Double_t Acn = hAcn->GetMean();
//       Eex->Draw("(Z1+Z2)>>hZcn");
//       Double_t Zcn = hZcn->GetMean();
      
//       cout << "Compound Nucleus: Z="<<Zcn << "  - A="<< Acn << endl;
      
//   }
  
//   for (Int_t ZZ = Zmin; ZZ <= Zmax; ZZ++){
    

//     Double_t enemax = TMath::Max(Eex->GetMaximum("Eexc1"),Eex->GetMaximum("Eexc2"));
    
//     TH1F *hEne1 = new TH1F("hEne1","hEne1",50,0.,enemax);
//     TH1F *hEne2 = new TH1F("hEne2","hEne2",50,0.,enemax);
    

//     //**************************************************************
//     //                        ### #######               #   
//     //     ##   #    #  ####   #  #     #              ##   
//     //    #  #  #    # #    #  #  #     #    #####    # #   
//     //   #    # #    # #       #  #     #               #   
//     //   ###### #    # #  ###  #  #     #    #####      #   
//     //   #    #  #  #  #    #  #  #     #               #   
//     //   #    #   ##    ####  ### #######             ##### 
//     //                                                      
//     //**************************************************************
//     //
//     // IN CASE WE WANT THE AVERAGE, WE JUST WRITE ONE LINE IN THE TALYS .energy FILE:
//     if (avgIO==1){
//       cout << "We are in the new avg analysis. (avgIO = 1)" << endl;
    
//       Eex->Draw("Eexc1>>hEne1",Form("(Z1==%i)&&(A1pre==%i)",ZZ,AA));
//       Eex->Draw("Eexc2>>+hEne1",Form("(Z2==%i)&&(A2pre==%i)",ZZ,AA));
//       getchar();
//       if (hEne1->GetEntries() == 0) {
	
// 	cout << "No events found!!! Skipping A = " << AA << " Z = " << ZZ << "." << endl;
	
// 	delete hEne1;
// 	delete hEne2;
// 	getchar();
// 	continue; //if Nr. of events == 0 doesn't create the .ene/.jene files
//       }    
      
//       cout << " I found some events! I continue..." <<endl;
//       getchar();
//       Double_t eneAvg = hEne1->GetMean(1);
//       Double_t eneRMS = hEne1->GetRMS(1);
      
//     //Create energy-file
//     ofstream file;
    
//     Int_t hEne_nbins = hEne1->GetNbinsX();
    
//     if (spinIO!=2){
//       file.open (Form("geflmf_%i-%i.ene",ZZ,AA));

//       //      file << hEne_nbins << " 0 1 " << hEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;


//       file << "3 0 1" ;

//       switch (talysVersion) {
//       case 1.8:
// 	file << endl;
// 	cout << "Creating .ene input for Talys-1.8" << endl;
// 	break;
//       case 1.6:
// 	file << " " << (eneAvg + 4*eneRMS) << endl;
// 	cout << "Creating .ene input for Talys-1.6" << endl;
// 	break;
//       default:
// 	file << " " << (eneAvg + 4*eneRMS) << endl;
// 	break;
//       }

//       Int_t tot_bincontent = 0.;
//       tot_bincontent = hEne1->GetEntries();
//       file << eneAvg - 2*eneRMS << " " << 0. << endl;
//       file << eneAvg << " " << tot_bincontent << endl;
//       file << eneAvg + 2*eneRMS << " " << 0. << endl;
//       file.close();
//     }
    
//     if (spinIO!=0){

//       cerr << "SpinIO must be 0!" << endl;

//       return;
    
//     /* ofstream filespin;
      
//       Int_t hEne_nbins2 = hJEne1->GetNbinsX();
//       Int_t hJ_nbins = hJEne1->GetNbinsY();
      
//       filespin.open (Form("geflmf_%i-%i.jene",ZZ,AA));

//       // n_energy_bins n_J_bins n_parities ene_max

//       //      filespin << hEne_nbins2 << " " << hJ_nbins <<" 1 " << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;

//       //ALTERNATIVE CODE TO ACCOUNT FOR THE DIFFERENCE OF THE NEW TALYS VERSION!!!
//       filespin << hEne_nbins2 << " " << hJ_nbins <<" 1 " ;

//       switch (talysVersion) {
//       case 1.8:
// 	filespin << endl;
// 	cout << "Creating .jene input for Talys-1.8" << endl;
// 	break;
//       case 1.6:
// 	filespin << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins2) << endl;
// 	cout << "Creating .jene input for Talys-1.6" << endl;
// 	break;
//       default:
// 	filespin << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins2) << endl;
// 	break;
//       }

//       Int_t tot_bincontent2 = 0.;
//       for(Int_t i = 1; i <= hEne_nbins2; i++) {
// 	filespin << hEne1->GetBinCenter(i);
// 	for (Int_t j = 1; j <= hJ_nbins; j++){
// 	  tot_bincontent2 = hJEne1->GetBinContent(i,j) + hJEne2->GetBinContent(i,j);
// 	  filespin << " " << tot_bincontent2;
// 	}
// 	filespin << endl;
//       }
      
//       filespin.close();
//     */
//       }
    
//     //      continue;
//     //      hEne->Delete();
    
//     delete hEne1;
//     delete hEne2;

//     //hEne->Reset();



      
      
//       continue; //with this we should go to the next iteration.
//     }



//     //**************************************************************
//     //                        ### #######              #### 
//     //     ##   #    #  ####   #  #     #             #    #
//     //    #  #  #    # #    #  #  #     #    #####        # 
//     //   #    # #    # #       #  #     #                #  
//     //   ###### #    # #  ###  #  #     #    #####      #   
//     //   #    #  #  #  #    #  #  #     #              #    
//     //   #    #   ##    ####  ### #######             ######
//     //                                                      
//     //**************************************************************
//     //
//     // IN CASE WE WANT THE AVERAGE, WE JUST WRITE ONE LINE IN THE TALYS .energy FILE and only 1 Z
//     if (avgIO == 2){
//       cout << "We are in the new avg analysis. (avgIO = 2)" << endl;
    
//       Eex->Draw("Eexc1>>hEne1",Form("(A1pre==%i)",AA));
//       Eex->Draw("Eexc2>>+hEne1",Form("(A2pre==%i)",AA));
//       getchar();
//       if (hEne1->GetEntries() == 0) {
	
// 	cout << "No events found!!! Skipping A = " << AA << "." << endl;
	
// 	delete hEne1;
// 	delete hEne2;
// 	getchar();
// 	continue; //if Nr. of events == 0 doesn't create the .ene/.jene files
//       }    
      


//       cout << " I found some events! I continue..." <<endl;
//       getchar();
//       Double_t eneAvg = hEne1->GetMean(1);
//       Double_t eneRMS = hEne1->GetRMS(1);
      
//     //Create energy-file
//     ofstream file;
    
//     Int_t hEne_nbins = hEne1->GetNbinsX();

//     ZZavg = TMath::Nint(Zcn*AA/Acn);
//     cout << "Zavg = " << ZZavg << endl;

//     if (spinIO!=2){
//       file.open (Form("geflmf_%i-%i.ene",ZZavg,AA));

//       //      file << hEne_nbins << " 0 1 " << hEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;


//       file << "3 0 1" ;

//       switch (talysVersion) {
//       case 1.8:
// 	file << endl;
// 	cout << "Creating .ene input for Talys-1.8" << endl;
// 	break;
//       case 1.6:
// 	file << " " << (eneAvg + 4*eneRMS) << endl;
// 	cout << "Creating .ene input for Talys-1.6" << endl;
// 	break;
//       default:
// 	file << " " << (eneAvg + 4*eneRMS) << endl;
// 	break;
//       }

//       Int_t tot_bincontent = 0.;
//       tot_bincontent = hEne1->GetEntries();
//       file << eneAvg - 2*eneRMS << " " << 0. << endl;
//       file << eneAvg << " " << tot_bincontent << endl;
//       file << eneAvg + 2*eneRMS << " " << 0. << endl;
//       file.close();
//     }
    
//     if (spinIO!=0){

//       cerr << "SpinIO must be 0!" << endl;

//       return;
    
//     /* ofstream filespin;
      
//       Int_t hEne_nbins2 = hJEne1->GetNbinsX();
//       Int_t hJ_nbins = hJEne1->GetNbinsY();
      
//       filespin.open (Form("geflmf_%i-%i.jene",ZZ,AA));

//       // n_energy_bins n_J_bins n_parities ene_max

//       //      filespin << hEne_nbins2 << " " << hJ_nbins <<" 1 " << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;

//       //ALTERNATIVE CODE TO ACCOUNT FOR THE DIFFERENCE OF THE NEW TALYS VERSION!!!
//       filespin << hEne_nbins2 << " " << hJ_nbins <<" 1 " ;

//       switch (talysVersion) {
//       case 1.8:
// 	filespin << endl;
// 	cout << "Creating .jene input for Talys-1.8" << endl;
// 	break;
//       case 1.6:
// 	filespin << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins2) << endl;
// 	cout << "Creating .jene input for Talys-1.6" << endl;
// 	break;
//       default:
// 	filespin << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins2) << endl;
// 	break;
//       }

//       Int_t tot_bincontent2 = 0.;
//       for(Int_t i = 1; i <= hEne_nbins2; i++) {
// 	filespin << hEne1->GetBinCenter(i);
// 	for (Int_t j = 1; j <= hJ_nbins; j++){
// 	  tot_bincontent2 = hJEne1->GetBinContent(i,j) + hJEne2->GetBinContent(i,j);
// 	  filespin << " " << tot_bincontent2;
// 	}
// 	filespin << endl;
//       }
      
//       filespin.close();
//     */
//       }
    
//     //      continue;
//     //      hEne->Delete();
    
//     delete hEne1;
//     delete hEne2;

//     //hEne->Reset();



      
      
//       continue; //with this we should go to the next iteration.
//     }







//     //                        ### #######               ###   
//     //     ##   #    #  ####   #  #     #              #   #  
//     //    #  #  #    # #    #  #  #     #    #####    #     # 
//     //   #    # #    # #       #  #     #             #     # 
//     //   ###### #    # #  ###  #  #     #    #####    #     # 
//     //   #    #  #  #  #    #  #  #     #              #   #  
//     //   #    #   ##    ####  ### #######               ###   
//     //                                                        
//     cout << "We are in the default non-avg analysis." << endl;
//     Double_t spinmax = TMath::Max(Eex->GetMaximum("I1pre"),Eex->GetMaximum("I2pre"));
//     if (spinmax >= 30.) spinmax=29;
//     Int_t binspin = TMath::FloorNint(spinmax) + 1;
//     Double_t *binYspin;
//     binYspin = (Double_t *)  malloc((binspin + 1) * sizeof(Double_t));
    
//     for (Int_t ll = 0; ll < binspin+1 ; ll++) binYspin[ll] = ll - 0.25;
    
    
//     TH2F *hJEne1 = new TH2F("hJEne1","hJEne1",50,0.,enemax,binspin,binYspin);
//     TH2F *hJEne2 = new TH2F("hJEne2","hJEne2",50,0.,enemax,binspin,binYspin);
    
//     Eex->Draw("Eexc1>>hEne1",Form("(Z1==%i)&&(A1pre==%i)",ZZ,AA));
//     Eex->Draw("Eexc2>>hEne2",Form("(Z2==%i)&&(A2pre==%i)",ZZ,AA));
    
//     Eex->Draw("I1pre:Eexc1>>hJEne1",Form("(Z1==%i)&&(A1pre==%i)",ZZ,AA));
//     Eex->Draw("I2pre:Eexc2>>hJEne2",Form("(Z2==%i)&&(A2pre==%i)",ZZ,AA));
//     //Eex->Draw("Eexc1:I1pre>>hJEne1",Form("(Z1==%i)&&(A1pre==%i)",ZZ,AA));
//     //Eex->Draw("Eexc2:I2pre>>hJEne2",Form("(Z2==%i)&&(A2pre==%i)",ZZ,AA));
    
//     if ((hEne1->GetEntries() + hEne2->GetEntries()) == 0) {
      
//       cout << "No events found!!! Skipping A = " << AA << " Z = " << ZZ << "." << endl;
      
//       delete hEne1;
//       delete hEne2;
//       delete hJEne1;
//       delete hJEne2;

//       continue; //if Nr. of events == 0 doesn't create the .ene/.jene files
//     }    
    
//     hEne1->SetTitle(Form("hEne %i-%i;Energy (MeV);Nr. events",ZZ,AA));
//     hEne2->SetTitle(Form("hEne %i-%i;Energy (MeV);Nr. events",ZZ,AA));
    
//     hJEne1->SetTitle(Form("hJEne %i-%i;Energy (MeV);Spin Jpre;Nr. events",ZZ,AA));
//     hJEne2->SetTitle(Form("hJEne %i-%i;Energy (MeV);Spin Jpre;Nr. events",ZZ,AA));
    
//     if (printIO){
//     if ((hEne1->GetEntries()*hEne2->GetEntries()) != 0.){
//       TH1F *hEne1clone = (TH1F*)hEne1->Clone("hEneclone");
//       hEne1clone->SetTitle(Form("hEne %i-%i;Energy (MeV);Nr. events",ZZ,AA));
//       hEne1clone->Add(hEne2,1.);
//       hEne1clone->Draw();
//       c1->Update();
//       c1->Print(Form("plots/hEne12-%i-%i.pdf",ZZ,AA),"pdf");
//     }
//     if (hEne1->GetEntries() != 0.){
//       hEne1->Draw();
//       c1->Update();
//       if (printIO) c1->Print(Form("plots/hEne1-%i-%i.pdf",ZZ,AA),"pdf");
//       hJEne1->Draw();
//       c1->Update();
      
//       if (printIO) c1->Print(Form("plots/hJEne1-%i-%i.pdf",ZZ,AA),"pdf");
//     }
//     if (hEne2->GetEntries() != 0.){
//       hEne2->Draw();
//       c1->Update();
//       if (printIO)	c1->Print(Form("plots/hEne2-%i-%i.pdf",ZZ,AA),"pdf");
//       hJEne2->Draw();
//       c1->Update();
//       if (printIO) c1->Print(Form("plots/hJEne2-%i-%i.pdf",ZZ,AA),"pdf");
//     }}
//     //Create energy-file
//     ofstream file;
    
//     Int_t hEne_nbins = hEne1->GetNbinsX();
    
//     if (spinIO!=2){
//       file.open (Form("geflmf_%i-%i.ene",ZZ,AA));

//       //      file << hEne_nbins << " 0 1 " << hEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;

            
//       file << hEne_nbins << " 0 1 " ;

//       switch (talysVersion) {
//       case 1.8:
// 	file << endl;
// 	cout << "Creating .ene input for Talys-1.8" << endl;
// 	break;
//       case 1.6:
// 	file << hEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;
// 	cout << "Creating .ene input for Talys-1.6" << endl;
// 	break;
//       default:
// 	file << hEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;
// 	break;
//       }

      
//       Int_t tot_bincontent = 0.;
//       for(Int_t i = 1; i <= hEne_nbins; i++) {
// 	tot_bincontent = hEne1->GetBinContent(i) + hEne2->GetBinContent(i);
// 	file << hEne1->GetBinCenter(i) << " " << tot_bincontent << endl;
//       }
      
//       file.close();
//     }
    
//     if (spinIO!=0){
//       ofstream filespin;
      
//       Int_t hEne_nbins2 = hJEne1->GetNbinsX();
//       Int_t hJ_nbins = hJEne1->GetNbinsY();
      
//       filespin.open (Form("geflmf_%i-%i.jene",ZZ,AA));

//       // n_energy_bins n_J_bins n_parities ene_max

//       //      filespin << hEne_nbins2 << " " << hJ_nbins <<" 1 " << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins) << endl;

    
//       //ALTERNATIVE CODE TO ACCOUNT FOR THE DIFFERENCE OF THE NEW TALYS VERSION!!!


//       filespin << hEne_nbins2 << " " << hJ_nbins <<" 1 " ;

//       switch (talysVersion) {
//       case 1.8:
// 	filespin << endl;
// 	cout << "Creating .jene input for Talys-1.8" << endl;
// 	break;
//       case 1.6:
// 	filespin << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins2) << endl;
// 	cout << "Creating .jene input for Talys-1.6" << endl;
// 	break;
//       default:
// 	filespin << hJEne1->GetXaxis()->GetBinUpEdge(hEne_nbins2) << endl;
// 	break;
//       }
  


//       Int_t tot_bincontent2 = 0.;
//       for(Int_t i = 1; i <= hEne_nbins2; i++) {
// 	filespin << hEne1->GetBinCenter(i);
// 	for (Int_t j = 1; j <= hJ_nbins; j++){
// 	  tot_bincontent2 = hJEne1->GetBinContent(i,j) + hJEne2->GetBinContent(i,j);
// 	  filespin << " " << tot_bincontent2;
// 	}
// 	filespin << endl;
//       }
      
//       filespin.close();
      
//     }
    
//     //      continue;
//     //      hEne->Delete();
    
//     delete hEne1;
//     delete hEne2;
//     delete hJEne1;
//     delete hJEne2;
//     //hEne->Reset();
    
//   }
//   /*  TCanvas *c2 = new TCanvas("c2","c2");
//       Eex->Draw("J:Ene>>hJEne");
//       hJEne->Draw("colz");*/
  
//   if (exitIO) gApplication->Terminate();
  
// }




// void printZavg(TString infile = "clean-U235+n_th_beta.lmf"){

// TString rootinfile = infile+".root";


//   TFile rootfile(rootinfile);
//   TNtuple *Eex;
//   if (rootfile.IsZombie()){
//     cout << "File "<< rootinfile << " doesn't exist. Reading file " << infile << " and creating Ntuple..." << endl;
    
//     Eex = new TNtuple("Eex","Eex","Z1:Z2:A1pre:A2pre:A1post:A2post:I1pre:I2pre:I1gs:I2gs:Eexc1:Eexc2:n1:n2:TKEpre:TKEpost");
//     cout << "Ntuple created. " << endl;
//     Eex->ReadFile(infile);
//     cout << "Ntuple filled from file " << infile << ". " << endl;
//     TFile rootfile2(rootinfile,"recreate");
//     Eex->Write();
//     rootfile2.Close();
//     cout << "File "<< rootinfile << " created! :)" << endl;
//   }
//   else {
//     Eex = (TNtuple *) rootfile.Get("Eex");
//   }
  
//   Int_t Amin = TMath::Min(Eex->GetMinimum("A1pre"),Eex->GetMinimum("A2pre"));
//   Int_t Amax = TMath::Max(Eex->GetMaximum("A1pre"),Eex->GetMaximum("A2pre"));
//   Int_t Zmin = TMath::Min(Eex->GetMinimum("Z1"),Eex->GetMinimum("Z2"));
//   Int_t Zmax = TMath::Max(Eex->GetMaximum("Z1"),Eex->GetMaximum("Z2"));

//   Int_t NbinsZ = Zmax - Zmin + 1;

//   TH1F *hZdist = new TH1F("hZdist","",NbinsZ,Zmin,Zmax);
  
//   for (Int_t AA = Amin; AA <= Amax; AA++){
    
//     Eex->Draw("Z1>>hZdist",Form("(A1pre == %d)",AA));
//     Eex->Draw("Z2>>+hZdist",Form("(A2pre == %d)",AA));
    
//     Double_t avgZ = hZdist->GetMean(1);
//     Double_t rmsZ = hZdist->GetRMS(1);

//     cout << AA << " " << avgZ << " " << rmsZ <<endl;

//   }
  
  
  
  
  
// }
