#include "../include/file_io.h"

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
    if (file == NULL) {
        printf("\n[i] File du lieu %s chua ton tai. He thong se tu tao khi luu.\n", filename);
        return;
    }

    ServerNode tempNode;
    while (fread(&tempNode, sizeof(ServerNode), 1, file) == 1) {
        // Tai tao lai node tren RAM
        addServer(head, tempNode.server_id, tempNode.gpu_model, tempNode.vram_total, tempNode.api_endpoint);
        
        // Phuc hoi trang thai VRAM dang su dung
        ServerNode* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->vram_used = tempNode.vram_used;
    }
    fclose(file);
    printf("\n[i] Da nap du lieu tu %s thanh cong.\n", filename);
}

void exportToCSV(ServerNode* head, const char* filename) {
    // Mo file o che do van ban "w" (Write)
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf(RED "\n[!] Loi: Khong the tao file bao cao %s\n" RESET, filename);
        return;
    }

    // 1. Ghi dong tieu de (Header) cho Excel
    // Dung dau phay de ngan cach cac cot
    fprintf(file, "ID,GPU Model,VRAM Total (GB),VRAM Used (GB),API Endpoint,Status\n");

    // 2. Duyet qua danh sach lien ket va ghi tung dong du lieu
    ServerNode* temp = head;
    int count = 0;
    while (temp != NULL) {
        fprintf(file, "%d,%s,%d,%d,%s,%s\n",
                temp->server_id,
                temp->gpu_model,
                temp->vram_total,
                temp->vram_used,
                temp->api_endpoint,
                (temp->status == 1) ? "Online" : "Offline");
        
        temp = temp->next;
        count++;
    }

    fclose(file);
    printf(GREEN "\n[+] THANH CONG: Da xuat bao cao %d may chu ra file '%s'!\n" RESET, count, filename);
    printf(CYAN "[i] Ban co the mo file nay truc tiep bang Microsoft Excel hoac Google Sheets.\n" RESET);
}