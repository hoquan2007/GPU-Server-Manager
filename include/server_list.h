#ifndef SERVER_LIST_H
#define SERVER_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- BANG MAU UX/UI (ANSI ESCAPE CODES) ---
#define RESET   "\033[0m"      // Dat lai mau mac dinh
#define BOLD    "\033[1m"      // In dam
#define RED     "\033[1;31m"   // Do (Dung cho Loi/Canh bao/Offline)
#define GREEN   "\033[1;32m"   // Xanh la (Dung cho Thanh cong/Online)
#define YELLOW  "\033[1;33m"   // Vang (Dung cho Cho phep/Bao tri)
#define CYAN    "\033[1;36m"   // Xanh duong nhat (Dung cho Tieu de/Menu)
#define MAGENTA "\033[1;35m"   // Tim (Dung cho diem nhan)

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
void sortServersByVRAM(ServerNode* head);
void toggleServerStatus(ServerNode* head, int id);

#endif // SERVER_LIST_H