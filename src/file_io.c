#include "../include/file_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SECRET_KEY 0x5A 

void encryptDecrypt(char* data, int size) {
    for (int i = 0; i < size; i++) data[i] ^= SECRET_KEY;
}

void saveToFile(ServerNode* head, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) return;
    ServerNode* temp = head;
    while (temp != NULL) {
        ServerNode tempNode = *temp; 
        tempNode.next = NULL;        
        encryptDecrypt((char*)&tempNode, sizeof(ServerNode)); 
        fwrite(&tempNode, sizeof(ServerNode), 1, file);
        temp = temp->next;
    }
    fclose(file);
}

void loadFromFile(ServerNode** head, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) return;
    ServerNode tempNode;
    while (fread(&tempNode, sizeof(ServerNode), 1, file) == 1) {
        encryptDecrypt((char*)&tempNode, sizeof(ServerNode)); 
        addServer(head, tempNode.server_id, tempNode.gpu_model, tempNode.vram_total, tempNode.api_endpoint);
        ServerNode* current = *head;
        while (current->next != NULL) current = current->next;
        current->vram_used = tempNode.vram_used;
        current->status = tempNode.status; 
    }
    fclose(file);
}

void exportToCSV(ServerNode* head, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) return;
    fprintf(file, "ID,GPU Model,VRAM Total (GB),VRAM Used (GB),API Endpoint,Status\n");
    ServerNode* temp = head;
    while (temp != NULL) {
        fprintf(file, "%d,%s,%d,%d,%s,%s\n",
                temp->server_id, temp->gpu_model, temp->vram_total,
                temp->vram_used, temp->api_endpoint,
                (temp->status == 1) ? "Online" : "Offline");
        temp = temp->next;
    }
    fclose(file);
    printf(GREEN "\n    [+] THANH CONG: Da xuat bao cao ra file '%s'!\n" RESET, filename);
}

void writeSystemLog(const char* message) {
    FILE* file = fopen("data/system.log", "a");
    if (file == NULL) return;
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    char time_str[30];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    fprintf(file, "[%s] %s\n", time_str, message);
    fclose(file);
}