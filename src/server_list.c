#include "../include/server_list.h"

ServerNode* createServerNode(int id, const char* model, int vtotal, const char* endpoint) {
    ServerNode* newNode = (ServerNode*)malloc(sizeof(ServerNode));
    if (newNode == NULL) {
        printf("Loi: Khong the cap phat bo nho cho Server!\n");
        return NULL;
    }
    newNode->server_id = id;
    strcpy(newNode->gpu_model, model);
    newNode->vram_total = vtotal;
    newNode->vram_used = 0; 
    strcpy(newNode->api_endpoint, endpoint);
    newNode->status = 1;
    newNode->next = NULL;
    return newNode;
}

void addServer(ServerNode** head, int id, const char* model, int vtotal, const char* endpoint) {
    ServerNode* newNode = createServerNode(id, model, vtotal, endpoint);
    if (newNode == NULL) return;

    if (*head == NULL) {
        *head = newNode;
        return;
    }

    ServerNode* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
}

void displayServers(ServerNode* head) {
    if (head == NULL) {
        printf("\n[!] Danh sach may chu hien dang trong.\n");
        return;
    }
    
    printf("\n========================================== DASHBOARD HA TANG GPU ==========================================\n");
    printf("%-5s | %-15s | %-20s | %-25s | %-10s\n", 
           "ID", "GPU Model", "Tai nguyen VRAM", "API Endpoint", "Status");
    printf("-----------------------------------------------------------------------------------------------------------\n");
    
    ServerNode* temp = head;
    while (temp != NULL) {
        int percent = (temp->vram_total == 0) ? 0 : (temp->vram_used * 100) / temp->vram_total;
        int bar_length = 10;
        int filled = (percent * bar_length) / 100;
        
        char bar[20] = "[";
        for (int i = 0; i < bar_length; i++) {
            if (i < filled) strcat(bar, "#");
            else strcat(bar, ".");
        }
        strcat(bar, "]");

        char vram_info[50];
        sprintf(vram_info, "%d/%dGB %s", temp->vram_used, temp->vram_total, bar);

        printf("%-5d | %-15s | %-20s | %-25s | %-10s\n",
               temp->server_id, temp->gpu_model, vram_info,
               temp->api_endpoint, temp->status == 1 ? "Online" : "Offline");
               
        temp = temp->next;
    }
    printf("===========================================================================================================\n");
}

void deployAIModel(ServerNode* head, const char* ai_model_name, int required_vram) {
    printf("\n>>> YEU CAU TRIEN KHAI MODEL: %s (Can %d GB VRAM)\n", ai_model_name, required_vram);
    printf("Dang quet ha tang mang...\n");

    ServerNode* temp = head;
    int found = 0;

    while (temp != NULL) {
        int free_vram = temp->vram_total - temp->vram_used;
        if (temp->status == 1 && free_vram >= required_vram) {
            printf("[+] Phat hien Server ID %d (%s) con %d GB VRAM trong.\n", 
                   temp->server_id, temp->gpu_model, free_vram);
            
            temp->vram_used += required_vram; // Cap nhat data node
            printf("[+] TRIEN KHAI THANH CONG! He thong da cap phat tai nguyen.\n");
            found = 1;
            break;
        }
        temp = temp->next;
    }

    if (!found) {
        printf("[-] THAT BAI: Toan bo he thong da het VRAM hoac khong co Server nao du dap ung %d GB!\n", required_vram);
    }
}

void freeList(ServerNode** head) {
    ServerNode* current = *head;
    ServerNode* nextNode;
    while (current != NULL) {
        nextNode = current->next;
        free(current);
        current = nextNode;
    }
    *head = NULL;
}