#include <iostream>

#include "json.hpp"
using json = nlohmann::json;

#include "ZipFile.h"
#include "Sprites.h"

int main(int argc, char* argv[])
{
    ZipFile *zfile = new ZipFile("Test3.sb3");

    u8* content = 0;;
    size_t size;
    int error = zfile->getContentForFile("project.json", &content, &size, true);

    if (error == 0) {

        Targets *targets = new Targets(json::parse((char*)content));
        //std::cout << data["targets"] << std::endl;

        delete targets;
    }

    if(content)
        delete[] content;

    return 0;
}