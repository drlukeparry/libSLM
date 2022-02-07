#include <math.h>
#include <iostream>

#include <App/Header.h>

#include <Translators/EOS/Reader.h>
#include <Translators/EOS/Writer.h>

#include <Translators/MTT/Reader.h>
#include <Translators/MTT/Writer.h>

#include <Translators/Realizer/Reader.h>
#include <Translators/Realizer/Writer.h>

#include <Translators/SLMSOL/Reader.h>
#include <Translators/SLMSOL/Writer.h>

int main(int argc, char *argv[])
{

    if(argc < 2) {
        std::cerr << "Number of argument must be one ";
    }

    std::string path = argv[0];
    std::string mode = argv[1];
    std::string inputFilename = argv[2];
    std::string outputFilename = argv[3];

    std::cout << "Reading file " << inputFilename << std::endl;

    if(mode.find("mtt") != std::string::npos) {
        slm::MTT::Reader reader(inputFilename);

        std::cout << "Parsing File" << std::endl;
        reader.parse();

        slm::MTT::Writer writer(outputFilename);

        slm::Header header;
        header.zUnit = reader.getZUnit();
        header.vMajor = 1;
        header.vMinor = 6;
        header.fileName = "MTT Layerfile";

        writer.write(header, reader.getModels(), reader.getLayers());

    } else if(mode.find("realizer") != std::string::npos) {

        slm::realizer::Reader reader(inputFilename);

        std::cout << "Parsing File" << std::endl;
        reader.parse();

        slm::realizer::Writer writer(outputFilename);

        slm::Header header;

        header.zUnit = 1000;
        header.vMajor = 4;
        header.vMinor = 7;
        header.fileName = outputFilename;

        std::string headerStr = reader.getHeader();
        writer.setHeaderText(headerStr);

        writer.write(header, reader.getModels(), reader.getLayers());
    
    } else if(mode.find("eos") != std::string::npos) {

        slm::eos::Reader reader(inputFilename);

        std::cout << "Parsing File" << std::endl;
        reader.parse();

        slm::eos::Writer writer(outputFilename);

        slm::Header header;

        header.zUnit = 1000;
        header.vMajor = 4;
        header.vMinor = 7;
        header.fileName = outputFilename;

        float sf = reader.getScaleFactor();
        writer.setScaleFactor(sf);

        writer.write(header, reader.getModels(), reader.getLayers());

    } else {
        std::cerr << "Error: Invalid option given";
    }
    
}
