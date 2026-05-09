#ifndef FILE_IO_H
#define FILE_IO_H

#include "server_list.h"

// Khai bao cac ham doc/ghi file
void saveToFile(ServerNode* head, const char* filename);
void loadFromFile(ServerNode** head, const char* filename);
// Xuat du lieu ra dinh dang CSV de mo bang Excel
void exportToCSV(ServerNode* head, const char* filename);
// Ghi nhat ky hoat dong vao file log
void writeSystemLog(const char* message);

#endif // FILE_IO_H