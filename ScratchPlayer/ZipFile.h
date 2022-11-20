#pragma once

#include "common.h"

#include <zip.h>

class ZipFile {
public:
	ZipFile(const char* filename);
	~ZipFile();

	int getContentForFileByIndex(int index, u8** outBuffer, size_t* size, bool terminate=false);
	int getContentForFile(const char* path, u8** outBuffer, size_t* size, bool terminate=false);

private:
	zip_t* archive = NULL;
};