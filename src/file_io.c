#include "../include/file_io.h"

void saveToFile(ServerNode* head, const char* filename) {
    // Mo file o che do "wb" (Write Binary - Ghi nhi phan)
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("\n[!] Loi: Khong the mo file %s de ghi du lieu.\n", filename);
        return;
    }

    ServerNode* temp = head;
    while (temp != NULL) {
        // Ghi truc tiep tung vung nho cua struct xuong file
        fwrite(temp, sizeof(ServerNode), 1, file);
        temp = temp->next;
    }

    fclose(file); // Bat buoc phai dong file sau khi dung
    printf("\n[i] Da luu toan bo du lieu may chu xuong file %s an toan.\n", filename);
}

void loadFromFile(ServerNode** head, const char* filename) {
    // Mo file o che do "rb" (Read Binary - Doc nhi phan)
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("\n[i] File du lieu %s chua ton tai. He thong se tu dong tao moi khi luu.\n", filename);
        return;
    }

    ServerNode tempNode;
    // Doc tung khoi du lieu bang dung kich thuoc cua mot ServerNode
    while (fread(&tempNode, sizeof(ServerNode), 1, file) == 1) {
        // Tai tao lai danh sach lien ket tren RAM bang cach goi ham addServer
        addServer(head, tempNode.server_id, tempNode.gpu_model, tempNode.vram_total, tempNode.api_endpoint);
    }

    fclose(file);
    printf("\n[i] Da nap du lieu tu %s thanh cong.\n", filename);
}