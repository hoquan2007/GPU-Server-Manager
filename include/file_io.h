#ifndef FILE_IO_H
#define FILE_IO_H

#include "server_list.h"

// Khai bao cac ham doc/ghi file
void saveToFile(ServerNode* head, const char* filename);
void loadFromFile(ServerNode** head, const char* filename);

#endif // FILE_IO_H