#include "../include/file_io.h"
#include <time.h> // Thu vien thoi gian thuc cua C

void saveToFile(ServerNode* head, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("\n[!] Loi: Khong the mo file %s de ghi du lieu.\n", filename);
        return;
    }
    ServerNode* temp = head;
    while (temp != NULL) {
        fwrite(temp, sizeof(ServerNode), 1, file);
        temp = temp->next;
    }
    fclose(file);
}

void loadFromFile(ServerNode** head, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) return;

    ServerNode tempNode;
    while (fread(&tempNode, sizeof(ServerNode), 1, file) == 1) {
        addServer(head, tempNode.server_id, tempNode.gpu_model, tempNode.vram_total, tempNode.api_endpoint);
        ServerNode* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->vram_used = tempNode.vram_used;
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
    printf("\n[+] THANH CONG: Da xuat bao cao ra file '%s'!\n", filename);
}

// === TINH NANG MOI: GHI NHAT KY HE THONG (REAL-TIME LOGGING) ===
void writeSystemLog(const char* message) {
    // Mo che do "a" (Append) de ghi noi tiep vao cuoi file
    FILE* file = fopen("data/system.log", "a");
    if (file == NULL) return;

    // Lay thoi gian thuc te tu may tinh
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    char time_str[30];
    
    // Dinh dang thoi gian theo chuan Quoc te: YYYY-MM-DD HH:MM:SS
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

    // Ghi thong diệp vao file
    fprintf(file, "[%s] %s\n", time_str, message);
    
    fclose(file);
}