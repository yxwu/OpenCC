/*
 * Open Chinese Convert
 *
 * Copyright 2010-2013 BYVoid <byvoid@byvoid.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <getopt.h>
#include <locale.h>

#include "Config.hpp"
#include "ConversionChain.hpp"

using Opencc::Optional;
using Opencc::Config;
using Opencc::FileNotFound;
using Opencc::FileNotWritable;

void ShowVersion() {
  printf(_("\n"));
  printf(_("Open Chinese Convert (OpenCC) Command Line Tool\n"));
  printf(_("Version %s\n"), VERSION);
  printf(_("\n"));
  printf(_("Author: %s\n"), "Carbo Kuo <byvoid@byvoid.com>");
  printf(_("Bug Report: %s\n"), "http://github.com/BYVoid/OpenCC/issues");
  printf(_("\n"));
}

void ShowUsage() {
  ShowVersion();
  printf(_("Usage:\n"));
  printf(_(" opencc [Options]\n"));
  printf(_("\n"));
  printf(_("Options:\n"));
  printf(_(" -i [file], --input=[file]   Read original text from [file].\n"));
  printf(_(" -o [file], --output=[file]  Write converted text to [file].\n"));
  printf(_(" -c [file], --config=[file]  Load configuration from [file].\n"));
  printf(_(" -v, --version               Print version and build information.\n"));
  printf(_(" -h, --help                  Print this help.\n"));
  printf(_("\n"));
  printf(_("With no input file, reads standard input and writes converted stream to standard output.\n"));
  printf(_("Default configuration (simplified to traditional) will be loaded if not set.\n"));
  printf(_("\n"));
}

std::istream& GetInputStream(const Optional<string>& inputFileName) {
  if (inputFileName.IsNull()) {
    return std::cin;
  } else {
    std::ifstream* stream = new std::ifstream(inputFileName.Get());
    if (!stream->is_open()) {
      throw FileNotFound(inputFileName.Get());
    }
    return *stream;
  }
}

std::ostream& GetOutputStream(const Optional<string>& outputFileName) {
  if (outputFileName.IsNull()) {
    return std::cout;
  } else {
    std::ofstream* stream = new std::ofstream(outputFileName.Get());
    if (!stream->is_open()) {
      throw FileNotWritable(outputFileName.Get());
    }
    return *stream;
  }
}

void Convert(const Optional<string>& inputFileName, const Optional<string>& outputFileName, const string& configFileName) {
  Config config;
  config.LoadFile(configFileName);
  auto conversionChain = config.GetConversionChain();
  
  std::istream& inputStream = GetInputStream(inputFileName);
  std::ostream& outputStream = GetOutputStream(outputFileName);
  
  while (!inputStream.eof()) {
    string line;
    std::getline(inputStream, line);
    string converted = conversionChain->Convert(line);
    outputStream << converted << std::endl;
    outputStream.flush();
  }
}

int main(int argc, const char * argv[]) {
#ifdef ENABLE_GETTEXT
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE_NAME, LOCALEDIR);
#endif /* ifdef ENABLE_GETTEXT */
  struct option longopts[] =
  {
    { "version", no_argument, NULL, 'v' },
    { "help", no_argument, NULL, 'h' },
    { "input", required_argument, NULL, 'i' },
    { "output", required_argument, NULL, 'o' },
    { "config", required_argument, NULL, 'c' },
    { 0, 0, 0, 0 },
  };
  int oc;
  Optional<string> inputFileName;
  Optional<string> outputFileName;
  string configFileName;
  while ((oc = getopt_long(argc, (char*const*)argv, "vh?i:o:c:", longopts, NULL)) != -1) {
    switch (oc) {
      case 'v':
        ShowVersion();
        return 0;
      case 'h':
      case '?':
        ShowUsage();
        return 0;
      case 'i':
        inputFileName = Optional<string>(optarg);
        break;
      case 'o':
        outputFileName = Optional<string>(optarg);
        break;
      case 'c':
        configFileName = optarg;
        break;
    }
  }
  if (configFileName == "") {
    // TODO
    configFileName = "";
  }
  Convert(inputFileName, outputFileName, configFileName);
  return 0;
}
