#include "Executor.h"

Executor::Executor(const char* sb3Filename) {
    zfile = new ZipFile(sb3Filename);

    u8* content = 0;;
    size_t size;
    int error = zfile->getContentForFile("project.json", &content, &size, true);

    if (error == 0) {
        targets = new Targets(json::parse((char*)content));
    }
    else targets = 0;

    if (content)
        delete[] content;
}

Executor::~Executor() {
    delete targets;
    delete zfile;
}