#include <iostream>

#include "ZipFile.h"

#include "json.hpp"
using json = nlohmann::json;

int main(int argc, char* argv[])
{
    ZipFile *zfile = new ZipFile("Test.sb3");

    u8* content = 0;;
    size_t size;
    int error = zfile->getContentForFile("project.json", &content, &size, true);

    if (error == 0) {

        json data = json::parse((char*)content);
        std::cout << data["targets"] << std::endl;
    }

    if(content)
        delete[] content;

    return 0;
}