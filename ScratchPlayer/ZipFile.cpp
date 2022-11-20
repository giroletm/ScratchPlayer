#include <sys/stat.h>
#include <string>
#include <fstream>

#include "ZipFile.h"



inline bool fileExists(const char* name) {
	struct stat buffer;
	return (stat(name, &buffer) == 0);
}


ZipFile::ZipFile(const char* filename) {
	int errorp = ZIP_ER_OK;
	this->archive = zip_open(filename, 0, &errorp);

    if (errorp != ZIP_ER_OK) {
        printf("Couldn't open ZIP file \"%s\": Error %d\n", filename, errorp);

        this->archive = 0;
    }

    #ifdef _DEBUG
    else printf("Opened ZIP \"%s\" successfully.\n", filename);
    #endif
}


ZipFile::~ZipFile() {
	zip_close(archive);
}

int ZipFile::getContentForFileByIndex(int index, u8** outBuffer, size_t* size, bool terminate) { // https://www.geeksforgeeks.org/c-program-to-read-and-print-all-files-from-a-zip-file/
    if (this->archive == 0) return -2;

    struct zip_stat* finfo = new (struct zip_stat);
    zip_stat_init(finfo);

    if (outBuffer != NULL)
        *outBuffer = NULL;

    if (size != NULL)
        *size = 0;

    int exists = -1;
    if ((zip_stat_index(archive, index, 0, finfo)) == 0) {
        if (size != NULL)
            *size = finfo->size;

        if (outBuffer != NULL) {
            if (terminate) {
                *outBuffer = new u8[finfo->size + 1];
                (*outBuffer)[finfo->size] = 0;
            }
            else *outBuffer = new u8[finfo->size];

            zip_file_t* fd = zip_fopen_index(archive, index, 0);
            zip_fread(fd, *outBuffer, finfo->size);
        }

        exists = 0;
    }
    else {
        printf("Couldn't find file ID \"%d\" in ZIP file.\n", index);
    }

    delete finfo;

    return exists;
}

int ZipFile::getContentForFile(const char* path, u8** outBuffer, size_t *size, bool terminate) {
    if (this->archive == 0) return -2;

    struct zip_stat* finfo = new (struct zip_stat);
    zip_stat_init(finfo);

    if(outBuffer != NULL)
        *outBuffer = NULL;

    if (size != NULL)
        *size = 0;

    int exists = -1;
    if ((zip_stat(archive, path, 0, finfo)) == 0) {
        if(size != NULL)
            *size = finfo->size;

        if (outBuffer != NULL) {
            if (terminate) {
                *outBuffer = new u8[finfo->size + 1];
                    (*outBuffer)[finfo->size] = 0;
            }
            else *outBuffer = new u8[finfo->size];

            zip_file_t* fd = zip_fopen(archive, path, 0);
            zip_fread(fd, *outBuffer, finfo->size);
        }

        exists = 0;
    }
    else {
        printf("Couldn't find file \"%s\" in ZIP file.\n", path);
    }

    delete finfo;

    return exists;
}