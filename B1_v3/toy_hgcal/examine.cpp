#include "Hits.C"

int main(int argc, char *argv[])
{
    if(argc<=1) { printf(">>> Please specify an input root file\n");  return 1; }
    if(argc<=2) { printf(">>> Please specify an output directory\n"); return 1; }

    TString input = argv[1];
    TString output = argv[2];
    printf(">>> input: %s\n", input.Data());
    printf(">>> output: %s\n", output.Data());

    TTree *tree = 0;
    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(input);
    if (!f || !f->IsOpen()) { f = new TFile(input); }

    f->GetObject("Hits",tree);
    Hits mytree(tree, input);

    mytree.Loop();
    mytree.MakePlots(output);
    mytree.Register();

    return 0;
}
