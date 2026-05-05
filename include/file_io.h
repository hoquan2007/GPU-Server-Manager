#ifndef FILE_IO_H
#define FILE_IO_H

#include "server_list.h"

// Ghi toan bo danh sach lien ket xuong file nhi phan
void saveToFile(ServerNode* head, const char* filename);

// Doc du lieu tu file nhi phan va tai len danh sach lien ket
void loadFromFile(ServerNode** head, const char* filename);

#endif // FILE_IO_H