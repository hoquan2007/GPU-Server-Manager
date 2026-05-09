#include <stdio.h>
#include <stdlib.h>
#include "../include/server_list.h"
#include "../include/file_io.h"

// Ham ho tro: Cho nguoi dung doc ket qua roi moi xoa man hinh
void pauseAndClear() {
    printf("\n... Nhan ENTER de tro ve Menu chinh ...");
    while(getchar() != '\n'); // Cho nguoi dung bam Enter
    system("cls");            // Xoa sach man hinh (Dung cho Windows)
}

void printMenu() {
    printf("\n");
    printf(CYAN "========================================================\n" RESET);
    printf(BOLD CYAN "        HE THONG QUAN LY HA TANG AI (NEXTGPU PRO)       \n" RESET);
    printf(CYAN "========================================================\n" RESET);
    printf("  1. Hien thi Dashboard ha tang (VRAM)\n");
    printf("  2. Them may chu moi vao he thong\n");
    printf("  3. Trien khai Model AI (Can bang tai)\n");
    printf("  4. Go bo may chu (Thanh ly)\n");
    printf("  5. Sap xep may chu theo VRAM (Giam dan)\n");
    printf("  6. Bao tri may chu (Chuyen trang thai Online/Offline)\n");
    printf("  7. Xuat bao cao he thong (Dinh dang Excel/CSV)\n"); 
    printf(YELLOW "  0. Thoat chuong trinh & Luu du lieu an toan\n" RESET);
    printf(CYAN "--------------------------------------------------------\n" RESET);
    printf(BOLD " Nhap lua chon cua ban: " RESET);
}

int main() {
    ServerNode* server_list = NULL;
    int choice;

    // Lam sach man hinh luc moi bat len
    system("cls"); 
    loadFromFile(&server_list, "data/gpu_servers.dat");
    pauseAndClear();

    while (1) {
        printMenu();
        
        char next_char;
        if (scanf("%d%c", &choice, &next_char) != 2 || next_char != '\n') {
            printf(RED "\n[!] Loi: Vui long chi nhap so nguyen!\n" RESET);
            if (next_char != '\n') while(getchar() != '\n'); 
            pauseAndClear();
            continue;
        }

        switch (choice) {
            case 1:
                displayServers(server_list);
                pauseAndClear();
                break;
            case 2: {
                int id, vtotal;
                char model[50], endpoint[100];
                printf("\n--- THEM MAY CHU MOI ---\n");
                
                while (1) {
                    printf("Nhap ID may chu (So nguyen): ");
                    if (scanf("%d%c", &id, &next_char) != 2 || next_char != '\n') { 
                        printf(RED "[!] Loi: ID khong hop le!\n" RESET);
                        if (next_char != '\n') while(getchar() != '\n'); 
                        continue;
                    }
                    if (checkIdExists(server_list, id)) {
                        printf("[!] Loi: Server ID %d da ton tai!\n", id);
                    } else if (id <= 0) {
                        printf(RED "[!] Loi: ID phai lon hon 0!\n"RESET);
                    } else break; 
                }

                printf("Nhap Model GPU (VD: RTX_5060): ");
                scanf(" %[^\n]", model);   
                while(getchar() != '\n');

                printf("Nhap Tong VRAM (GB): ");
                scanf("%d", &vtotal);
                while(getchar() != '\n');

                printf("Nhap API Endpoint (VD: http://nextgpu.vn/api/node): ");
                scanf(" %[^\n]", endpoint);
                while(getchar() != '\n');

                addServer(&server_list, id, model, vtotal, endpoint);
                printf("\n[+] Da them may chu %s thanh cong!\n", model);
                pauseAndClear();
                break;
            }
            case 3: {
                char ai_name[50];
                int req_vram;
                printf("\n--- TRIEN KHAI MODEL AI ---\n");
                printf("Nhap ten Model (VD: Qwen-31B): ");
                scanf(" %[^\n]", ai_name);
                while(getchar() != '\n');
                
                printf("Nhap dung luong VRAM yeu cau (GB): ");
                scanf("%d", &req_vram);
                while(getchar() != '\n');
                
                deployAIModel(server_list, ai_name, req_vram);
                pauseAndClear();
                break;
            }
            case 4: {
                int del_id;
                printf("\n--- GO BO MAY CHU ---\n");
                printf("Nhap ID may chu can thanh ly: ");
                if (scanf("%d", &del_id) == 1) {
                    while(getchar() != '\n');
                    deleteServer(&server_list, del_id);
                } else {
                    printf("[!] Loi nhap lieu!\n");
                    while(getchar() != '\n'); 
                }
                pauseAndClear();
                break;
            }
            case 5:
                sortServersByVRAM(server_list);
                pauseAndClear();
                break;
            case 6: {
                int toggle_id;
                printf("\n--- BAO TRI MAY CHU ---\n");
                printf("Nhap ID may chu muon bat/tat trang thai: ");
                if (scanf("%d", &toggle_id) == 1) {
                    while(getchar() != '\n');
                    toggleServerStatus(server_list, toggle_id);
                } else {
                    printf("[!] Loi nhap lieu!\n");
                    while(getchar() != '\n'); 
                }
                pauseAndClear();
                break;
            }
            case 7:
                printf("\n--- XUAT BAO CAO HE THONG ---\n");
                exportToCSV(server_list, "data/baocao_gpu.csv");
                pauseAndClear();
                break;
            case 0:
                printf("\nDang luu du lieu...\n");
                saveToFile(server_list, "data/gpu_servers.dat");
                printf("Dang don dep bo nho...\n");
                freeList(&server_list);
                printf("He thong tat an toan. Tam biet!\n");
                return 0;
            default:
                printf("\n[!] Lua chon khong hop le.\n");
                pauseAndClear();
        }
    }
    return 0;
}