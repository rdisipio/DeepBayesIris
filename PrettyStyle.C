#include <iostream>

#include "PrettyStyle.h"

#include "TROOT.h"

void SetPrettyStyle()
{
  static TStyle* prettyStyle = 0;
  std::cout << "\nApplying Pretty style settings...\n" << std::endl ;
  if ( prettyStyle==0 ) prettyStyle = PrettyStyle();
  gROOT->SetStyle("Pretty");
  gROOT->ForceStyle();
  gStyle->SetPalette(1);
}

TStyle* PrettyStyle() 
{
  TStyle *prettyStyle = new TStyle("Pretty","Pretty style");

  // use plain black on white colors
  Int_t icol=0; // WHITE
  prettyStyle->SetFrameBorderMode(icol);
  prettyStyle->SetFrameFillColor(icol);
  prettyStyle->SetCanvasBorderMode(icol);
  prettyStyle->SetCanvasColor(icol);
  prettyStyle->SetPadBorderMode(icol);
  prettyStyle->SetPadColor(icol);
  prettyStyle->SetStatColor(icol);
  //prettyStyle->SetFillColor(icol); // don't use: white fill color for *all* objects

  // set the paper & margin sizes
  prettyStyle->SetPaperSize(20,26);

  // set margin sizes
  prettyStyle->SetPadTopMargin(0.05);
  prettyStyle->SetPadRightMargin(0.05); // 0.05
  prettyStyle->SetPadBottomMargin(0.16);
  prettyStyle->SetPadLeftMargin(0.12); // 0.16

  // set title offsets (for axis label)
  prettyStyle->SetTitleXOffset(1.2); // 1.4
  prettyStyle->SetTitleYOffset(1.25); // 1.4

  // use large fonts
  //Int_t font=72; // Helvetica italics
  Int_t font=42; // Helvetica
  Double_t tsize=0.05;
  prettyStyle->SetTextFont(font);

  prettyStyle->SetTextSize(tsize);
  prettyStyle->SetLabelFont(font,"x");
  prettyStyle->SetTitleFont(font,"x");
  prettyStyle->SetLabelFont(font,"y");
  prettyStyle->SetTitleFont(font,"y");
  prettyStyle->SetLabelFont(font,"z");
  prettyStyle->SetTitleFont(font,"z");
  
  prettyStyle->SetLabelSize(tsize,"x");
  prettyStyle->SetTitleSize(tsize,"x");
  prettyStyle->SetLabelSize(tsize,"y");
  prettyStyle->SetTitleSize(tsize,"y");
  prettyStyle->SetLabelSize(tsize,"z");
  prettyStyle->SetTitleSize(tsize,"z");

  // use bold lines and markers
  prettyStyle->SetMarkerStyle(20);
  prettyStyle->SetMarkerSize(1.2);
  prettyStyle->SetHistLineWidth(2.);
  prettyStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars 
  //prettyStyle->SetErrorX(0.001);
  // get rid of error bar caps
  prettyStyle->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
  prettyStyle->SetOptTitle(0);
  //prettyStyle->SetOptStat(1111);
  prettyStyle->SetOptStat(0);
  //prettyStyle->SetOptFit(1111);
  prettyStyle->SetOptFit(0);

  // put tick marks on top and RHS of plots
  prettyStyle->SetPadTickX(1);
  prettyStyle->SetPadTickY(1);

  return prettyStyle;

}

