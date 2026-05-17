#ifndef FILE_IO_H
#define FILE_IO_H

#include "server_list.h"

void saveToFile(ServerNode* head, const char* filename);
void loadFromFile(ServerNode** head, const char* filename);
void exportToCSV(ServerNode* head, const char* filename);
void writeSystemLog(const char* message);

#endif