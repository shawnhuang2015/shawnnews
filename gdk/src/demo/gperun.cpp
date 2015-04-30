#include <iostream>
#include <stdlib.h>
#include <gpelib4/udfs/singleactiveudfbase.hpp>
#include <gpelib4/enginedriver/enginedriver.hpp>
#include "sp.hpp"

int main(int argc, char** argv) {
  std::string cfgfile = "";
  std::string partitionpath = argv[1];
  std::string output = argv[2];
  gpelib4::EngineDriver driver(cfgfile, partitionpath);

  unsigned vid_a = 0, vid_b = 1;
  if (argc == 5) {
    vid_a = atoi(argv[3]);
    vid_b = atoi(argv[4]);
  }
  gperun::SPUDF udf(10, vid_a, vid_b);
  driver.RunSingleVersion(&udf, output);
}
