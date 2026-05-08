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