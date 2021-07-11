#include <math.h>
#include <iostream>

#include <App/Header.h>

#include <Translators/MTT/Reader.h>
#include <Translators/MTT/Writer.h>
#include <Translators/SLMSOL/Reader.h>
#include <Translators/SLMSOL/Writer.h>
#include <Translators/Realizer/Reader.h>
#include <Translators/Realizer/Writer.h>

int main(int argc, char *argv[])
{

    if(argc < 2) {
        std::cerr << "Number of argument must be one ";
    }

    std::string path = argv[0];
    std::string file = argv[1];

    std::string filePath = file;

    std::cout << "Reading file" << filePath << std::endl;

    int mode = 2;
    
    if(mode == 1) {
        slm::MTT::Reader reader(filePath);

        std::cout << "Parsing File" << std::endl;
        reader.parse();

        slm::MTT::Writer writer("out.mtt");

        slm::Header header;
        header.zUnit = reader.getZUnit();
        header.vMajor = 1;
        header.vMinor = 6;
        header.fileName = "MTT Layerfile";

        writer.write(header, reader.getModels(), reader.getLayers());

    } else if(mode == 2) {

        slm::realizer::Reader reader(filePath);

        std::cout << "Parsing File" << std::endl;
        reader.parse();

        slm::realizer::Writer writer("out.rea");

        slm::Header header;

        header.zUnit = 1000;
        header.vMajor = 4;
        header.vMinor = 7;
        header.fileName = "out.rea";

        std::string headerStr = reader.getHeader();
        writer.setHeaderText(headerStr);

        writer.write(header, reader.getModels(), reader.getLayers());
    
    }
    
}
