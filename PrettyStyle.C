#include <iostream>

#include "PrettyStyle.h"

#include "TROOT.h"

void SetPrettyStyle ()
{
  static TStyle* PrettyStyle = 0;
  std::cout << "\nApplying Pretty style settings...\n" << std::endl ;
  if ( PrettyStyle==0 ) PrettyStyle = PrettyStyle();
  gROOT->SetStyle("Pretty");
  gROOT->ForceStyle();
  gStyle->SetPalette(1);
}

TStyle* PrettyStyle() 
{
  TStyle *PrettyStyle = new TStyle("Pretty","Pretty style");

  // use plain black on white colors
  Int_t icol=0; // WHITE
  PrettyStyle->SetFrameBorderMode(icol);
  PrettyStyle->SetFrameFillColor(icol);
  PrettyStyle->SetCanvasBorderMode(icol);
  PrettyStyle->SetCanvasColor(icol);
  PrettyStyle->SetPadBorderMode(icol);
  PrettyStyle->SetPadColor(icol);
  PrettyStyle->SetStatColor(icol);
  //PrettyStyle->SetFillColor(icol); // don't use: white fill color for *all* objects

  // set the paper & margin sizes
  PrettyStyle->SetPaperSize(20,26);

  // set margin sizes
  PrettyStyle->SetPadTopMargin(0.05);
  PrettyStyle->SetPadRightMargin(0.05); // 0.05
  PrettyStyle->SetPadBottomMargin(0.16);
  PrettyStyle->SetPadLeftMargin(0.12); // 0.16

  // set title offsets (for axis label)
  PrettyStyle->SetTitleXOffset(1.2); // 1.4
  PrettyStyle->SetTitleYOffset(1.25); // 1.4

  // use large fonts
  //Int_t font=72; // Helvetica italics
  Int_t font=42; // Helvetica
  Double_t tsize=0.05;
  PrettyStyle->SetTextFont(font);

  PrettyStyle->SetTextSize(tsize);
  PrettyStyle->SetLabelFont(font,"x");
  PrettyStyle->SetTitleFont(font,"x");
  PrettyStyle->SetLabelFont(font,"y");
  PrettyStyle->SetTitleFont(font,"y");
  PrettyStyle->SetLabelFont(font,"z");
  PrettyStyle->SetTitleFont(font,"z");
  
  PrettyStyle->SetLabelSize(tsize,"x");
  PrettyStyle->SetTitleSize(tsize,"x");
  PrettyStyle->SetLabelSize(tsize,"y");
  PrettyStyle->SetTitleSize(tsize,"y");
  PrettyStyle->SetLabelSize(tsize,"z");
  PrettyStyle->SetTitleSize(tsize,"z");

  // use bold lines and markers
  PrettyStyle->SetMarkerStyle(20);
  PrettyStyle->SetMarkerSize(1.2);
  PrettyStyle->SetHistLineWidth(2.);
  PrettyStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars 
  //PrettyStyle->SetErrorX(0.001);
  // get rid of error bar caps
  PrettyStyle->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
  PrettyStyle->SetOptTitle(0);
  //PrettyStyle->SetOptStat(1111);
  PrettyStyle->SetOptStat(0);
  //PrettyStyle->SetOptFit(1111);
  PrettyStyle->SetOptFit(0);

  // put tick marks on top and RHS of plots
  PrettyStyle->SetPadTickX(1);
  PrettyStyle->SetPadTickY(1);

  return PrettyStyle;

}

