#include <iostream>
#include <stdlib.h>
#include <gpelib4/udfs/singleactiveudfbase.hpp>
#include <gpelib4/enginedriver/enginedriver.hpp>
#include "udf.hpp"

int main(int argc, char** argv) {
  std::string cfgfile = "";
  std::string partitionpath = argv[1];
  std::string output = argv[2];
  gpelib4::EngineDriver driver(cfgfile, partitionpath);

  unsigned vid_a = 0;
  if (argc == 4) {
    vid_a = atoi(argv[3]);
  }
  gperun::UDF udf(10, vid_a);
  driver.RunSingleVersion(&udf, output);
}
