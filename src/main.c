#include <stdio.h>
#include <stdlib.h>
#include "../include/server_list.h"
#include "../include/file_io.h"

void printMenu() {
    printf("\n=========================================\n");
    printf("   HE THONG QUAN LY HA TANG AI (GPU)\n");
    printf("=========================================\n");
    printf("1. Hien thi Dashboard ha tang (VRAM)\n");
    printf("2. Them may chu moi (Tu ban phim)\n");
    printf("3. Trien khai Model AI (Can bang tai)\n");
    printf("0. Thoat chuong trinh & Luu du lieu\n");
    printf("=========================================\n");
    printf("Nhap lua chon cua ban: ");
}

int main() {
    ServerNode* server_list = NULL;
    int choice;

    loadFromFile(&server_list, "data/gpu_servers.dat");

    while (1) {
        printMenu();
        
        if (scanf("%d", &choice) != 1) {
            printf("\n[!] Loi: Vui long nhap mot so nguyen!\n");
            while(getchar() != '\n'); 
            continue;
        }

        switch (choice) {
            case 1:
                displayServers(server_list);
                break;
            case 2: {
                int id, vtotal;
                char model[50], endpoint[100];

                printf("\n--- THEM MAY CHU MOI ---\n");
                
                // Vong lap bat nhap ID den khi nao KHONG TRUNG thi thoi
                // Vong lap bat nhap ID den khi nao KHONG TRUNG va CHI CHUA SO
                while (1) {
                    printf("Nhap ID may chu (So nguyen): ");
                    
                    char next_char;
                    // Kiem tra xem co doc duoc 1 so va 1 ky tu (Enter) hay khong
                    if (scanf("%d%c", &id, &next_char) != 2 || next_char != '\n') { 
                        printf("[!] Loi: ID khong hop le! Vui long chi nhap so nguyen.\n");
                        // Don dep bo nho dem ban phim neu chuoi bi sai
                        if (next_char != '\n') {
                            while(getchar() != '\n'); 
                        }
                        continue;
                    }

                    // Kiem tra trung lap hoac so am
                    if (checkIdExists(server_list, id)) {
                        printf("[!] Loi: Server ID %d da ton tai! Vui long chon ID khac.\n", id);
                    } else if (id <= 0) {
                        printf("[!] Loi: ID phai lon hon 0!\n");
                    } else {
                        break; // ID hoan toan hop le -> thoat vong lap
                    }
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
                printf("\n[+] Da them may chu %s thanh cong vao he thong!\n", model);
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
                break;
            }
            case 0:
                printf("\nDang luu du lieu...\n");
                saveToFile(server_list, "data/gpu_servers.dat");
                printf("Dang don dep bo nho...\n");
                freeList(&server_list);
                printf("He thong tat an toan. Tam biet!\n");
                return 0;
            default:
                printf("\n[!] Lua chon khong hop le. Vui long chon tu 0-3.\n");
        }
    }

    return 0;
}