#ifndef SERVER_LIST_H
#define SERVER_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Dinh nghia cau truc may chu (Node)
typedef struct ServerNode {
    int server_id;
    char gpu_model[50];
    int vram_total;
    int vram_used;
    char api_endpoint[100];
    int status;
    struct ServerNode* next; // Cau truc tu tro
} ServerNode;

// Khai bao cac ham xu ly
ServerNode* createServerNode(int id, const char* model, int vtotal, const char* endpoint);
void addServer(ServerNode** head, int id, const char* model, int vtotal, const char* endpoint);
void displayServers(ServerNode* head);
void freeList(ServerNode** head);
void deployAIModel(ServerNode* head, const char* ai_model_name, int required_vram);
int checkIdExists(ServerNode* head, int id);
void deleteServer(ServerNode** head, int id);

#endif // SERVER_LIST_H