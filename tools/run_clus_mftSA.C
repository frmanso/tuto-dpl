#if !defined(__CLING__) || defined(__ROOTCLING__)
#include <TStopwatch.h>
#include "DetectorsBase/GeometryManager.h"
#include "MFTReconstruction/ClustererTask.h"
#include "ITSMFTReconstruction/Clusterer.h"
#include "FairLogger.h"
#endif

// Clusterization avoiding FairRunAna management.
// Works both with MC digits and with "raw" data (in this case the last argument must be
// set to true). The raw data should be prepared beforeahand from the MC digits using e.g.
// o2::ITSMFT::RawPixelReader<o2::ITSMFT::ChipMappingMFT> reader;
// reader.convertDigits2Raw("dig.raw","o2dig.root","o2sim","MFTDigit");
//
// Use for MC mode:
// root -b -q run_clus_mftSA.C+\(\"o2clus.root\",\"mftdigits.root\"\) 2>&1 | tee clusSA.log
//
// Use for RAW mode:
// root -b -q run_clus_mftSA.C+\(\"o2clus.root\",\"dig.raw\"\) 2>&1 | tee clusSARAW.log
//
// Use of topology dictionary: flag withDicitonary -> true
// A dictionary must be generated with the macro CheckTopologies.C

void run_clus_mftSA(std::string outputfile = "mftclusters.root", std::string inputfile = "mftdigits.root", bool raw = false, bool withDictionary = false, std::string dictionaryfile = "complete_dictionary.bin")
{
  // Initialize logger
  FairLogger* logger = FairLogger::GetLogger();
  logger->SetLogVerbosityLevel("LOW");
  logger->SetLogScreenLevel("INFO");

  TStopwatch timer;
  o2::Base::GeometryManager::loadGeometry(); // needed provisionary, only to write full clusters

  // Setup clusterizer
  Bool_t useMCTruth = kTRUE;  // kFALSE if no comparison with MC needed
  Bool_t entryPerROF = kTRUE; // write single tree entry for every ROF. If false, just 1 entry will be saved
  o2::MFT::ClustererTask* clus = new o2::MFT::ClustererTask(useMCTruth, raw);
  if (withDictionary) {
    clus->loadDictionary(dictionaryfile.c_str());
    clus->getClusterer().setWantCompactClusters(true); // require compact clusters with patternID
  }
  clus->getClusterer().setMaskOverflowPixels(true);  // set this to false to switch off masking
  clus->getClusterer().setWantFullClusters(true);    // require clusters with coordinates and full pattern

  clus->run(inputfile, outputfile, entryPerROF);

  timer.Stop();
  timer.Print();
}
