#include "../include/server_list.h"
// Ham kiem tra ID co ton tai trong danh sach chua (Tra ve 1 neu trung, 0 neu chua)
int checkIdExists(ServerNode* head, int id) {
    ServerNode* temp = head;
    while (temp != NULL) {
        if (temp->server_id == id) {
            return 1; // Da ton tai
        }
        temp = temp->next;
    }
    return 0; // Hop le (Chua ton tai)
}

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
        // Dung mau Vang cho canh bao nhe nhang
        printf(YELLOW "\n[i] Danh sach may chu hien dang trong. Vui long them may chu!\n" RESET);
        return;
    }
    
    // Dung mau Cyan (Xanh lam) lam chu dao cho phan vien de tao cam giac cong nghe
    printf(CYAN "\n============================================================= DASHBOARD HA TANG GPU =============================================================\n" RESET);
    printf(BOLD " %-8s | %-18s | %-26s | %-35s | %-10s \n" RESET, 
           "ID", "GPU Model", "Tai nguyen VRAM", "API Endpoint", "Status");
    printf(CYAN "----------+--------------------+----------------------------+-------------------------------------+------------\n" RESET);
    
    ServerNode* temp = head;
    while (temp != NULL) {
        int percent = (temp->vram_total == 0) ? 0 : (temp->vram_used * 100) / temp->vram_total;
        int bar_length = 10;
        int filled = (percent * bar_length) / 100;
        
        // Tao thanh bar thua nguyen ban (Khong chua ma mau)
        char bar[20] = "[";
        for (int i = 0; i < bar_length; i++) {
            if (i < filled) strcat(bar, "#");
            else strcat(bar, ".");
        }
        strcat(bar, "]");

        // 1. In ID va Model (Mau trang mac dinh)
        printf(" %-8d | %-18s | ", temp->server_id, temp->gpu_model);
        
        // 2. In muc tieu thu VRAM (8 ky tu)
        printf("%2d/%2dGB ", temp->vram_used, temp->vram_total);
        
        // 3. In Progress Bar voi mau sac thay doi theo do tai (18 ky tu)
        if (percent >= 80) {
            printf(RED "%-18s" RESET, bar); // Qua tai -> Do
        } else {
            printf(GREEN "%-18s" RESET, bar); // Binh thuong -> Xanh la
        }
        
        // 4. In Endpoint 
        printf("| %-35s | ", temp->api_endpoint);
        
        // 5. In Status
        if (temp->status == 1) {
            printf(GREEN "%-10s" RESET "\n", "Online");
        } else {
            printf(RED "%-10s" RESET "\n", "Offline");
        }
        
        printf(CYAN "----------+--------------------+----------------------------+-------------------------------------+------------\n" RESET);
               
        temp = temp->next;
    }
}

// 5. Thuật toán Cân bằng tải: Liệt kê Server trống và cho phép người dùng chọn
void deployAIModel(ServerNode* head, const char* ai_model_name, int required_vram) {
    printf("\n>>> YEU CAU TRIEN KHAI MODEL: %s (Can %d GB VRAM)\n", ai_model_name, required_vram);
    printf("Dang quet ha tang mang...\n");

    ServerNode* temp = head;
    int count = 0;

    // Buoc 1: Quet va hien thi danh sach cac server DU DIEU KIEN
    printf("\n--- DANH SACH SERVER DU KHA NANG DEPLOY ---\n");
    while (temp != NULL) {
        int free_vram = temp->vram_total - temp->vram_used;
        // Kiem tra server co dang Online va du RAM hay khong
        if (temp->status == 1 && free_vram >= required_vram) {
            printf("[+] ID: %-5d | GPU: %-15s | VRAM trong: %d GB\n", 
                   temp->server_id, temp->gpu_model, free_vram);
            count++;
        }
        temp = temp->next;
    }

    // Neu khong co server nao du dap ung
    if (count == 0) {
        printf("[-] THAT BAI: Khong co Server nao trong he thong du %d GB VRAM de chay model nay!\n", required_vram);
        return; 
    }

    // Buoc 2: Cho nguoi dung chon ID may chu de trien khai
    int selected_id;
    printf("\n>> Co %d server phu hop. Nhap ID may chu ban muon chon (Hoac nhap 0 de huy): ", count);
    if (scanf("%d", &selected_id) != 1) {
        printf("[!] Loi: Nhap lieu khong hop le. Da huy thao tac.\n");
        while(getchar() != '\n'); // Xoa bo dem
        return;
    }
    while(getchar() != '\n'); // Xoa bo dem

    if (selected_id == 0) {
        printf("[i] Da huy yeu cau trien khai.\n");
        return;
    }

    // Buoc 3: Tim dung node co ID da chon va cap nhat VRAM
    temp = head;
    int success = 0;
    while (temp != NULL) {
        if (temp->server_id == selected_id) {
            // Kiem tra lai mot lan nua cho an toan
            int free_vram = temp->vram_total - temp->vram_used;
            if (temp->status == 1 && free_vram >= required_vram) {
                temp->vram_used += required_vram; // Cap nhat du lieu node
                printf("\n[+] TRIEN KHAI THANH CONG model %s len Server ID %d!\n", ai_model_name, selected_id);
                success = 1;
            } else {
                printf("\n[-] Loi: Server ID %d khong du dieu kien (co the do loi bat ngo).\n", selected_id);
            }
            break; // Tim thay va xu ly xong thi thoat vong lap
        }
        temp = temp->next;
    }

    if (success == 0) {
        printf("\n[-] THAT BAI: Khong tim thay Server ID %d trong danh sach du dieu kien. Vui long nhap dung ID!\n", selected_id);
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

// 6. Ham xoa may chu khoi he thong (Bao tri/Huy bo)
void deleteServer(ServerNode** head, int id) {
    if (*head == NULL) {
        printf("\n[-] THAT BAI: He thong hien chua co may chu nao!\n");
        return;
    }

    ServerNode* temp = *head;
    ServerNode* prev = NULL;

    // Truong hop 1: May chu can xoa la node DAU TIEN (Head)
    if (temp != NULL && temp->server_id == id) {
        *head = temp->next; // Chuyen head sang node thu 2
        free(temp);         // Giai phong node dau tien
        printf("\n[+] Da go bo thanh cong Server ID %d ra khoi he thong.\n", id);
        return;
    }

    // Truong hop 2: May chu can xoa o doan GIUA hoac CUOI
    // Dung 2 con tro (prev va temp) de duyet tim
    while (temp != NULL && temp->server_id != id) {
        prev = temp;
        temp = temp->next;
    }

    // Neu duyet het danh sach ma van khong tim thay ID
    if (temp == NULL) {
        printf("\n[-] THAT BAI: Khong tim thay Server ID %d trong he thong!\n", id);
        return;
    }

    // Phat hien thay ID -> Ngat ket noi node hien tai va noi 'prev' voi node tiep theo
    prev->next = temp->next;
    
    // Thu hoi bo nho (Bat buoc phai co de khong bi tru diem)
    free(temp); 
    printf("\n[+] Da go bo thanh cong Server ID %d ra khoi he thong.\n", id);
}

// 7. Tinh nang VIP: Sap xep server theo VRAM (Giam dan) bang Bubble Sort
void sortServersByVRAM(ServerNode* head) {
    if (head == NULL || head->next == NULL) return;
    
    int swapped;
    ServerNode* ptr1;
    ServerNode* lptr = NULL;

    printf("\nDang toi uu hoa va sap xep lai he thong...\n");
    do {
        swapped = 0;
        ptr1 = head;

        while (ptr1->next != lptr) {
            // Neu VRAM may hien tai NHO HON may tiep theo -> Doi cho
            if (ptr1->vram_total < ptr1->next->vram_total) { 
                // Doi ID
                int t_id = ptr1->server_id; ptr1->server_id = ptr1->next->server_id; ptr1->next->server_id = t_id;
                // Doi VRAM Total & Used
                int t_vt = ptr1->vram_total; ptr1->vram_total = ptr1->next->vram_total; ptr1->next->vram_total = t_vt;
                int t_vu = ptr1->vram_used; ptr1->vram_used = ptr1->next->vram_used; ptr1->next->vram_used = t_vu;
                // Doi Status
                int t_st = ptr1->status; ptr1->status = ptr1->next->status; ptr1->next->status = t_st;
                // Doi Chuoi (Model & Endpoint)
                char t_mod[50]; strcpy(t_mod, ptr1->gpu_model); strcpy(ptr1->gpu_model, ptr1->next->gpu_model); strcpy(ptr1->next->gpu_model, t_mod);
                char t_api[100]; strcpy(t_api, ptr1->api_endpoint); strcpy(ptr1->api_endpoint, ptr1->next->api_endpoint); strcpy(ptr1->next->api_endpoint, t_api);

                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
    printf("[+] Da sap xep xong! Cac GPU manh nhat da duoc dua len top.\n");
}

// 8. Tinh nang VIP: Thay doi trang thai (Online <-> Offline) de bao tri
void toggleServerStatus(ServerNode* head, int id) {
    ServerNode* temp = head;
    while (temp != NULL) {
        if (temp->server_id == id) {
            // Dao nguoc trang thai: Dang 1 thi thanh 0, dang 0 thi thanh 1
            temp->status = (temp->status == 1) ? 0 : 1; 
            printf("\n[+] Thanh cong! Server ID %d da chuyen sang trang thai: %s\n", 
                   id, temp->status == 1 ? "ONLINE" : "OFFLINE (Bao tri)");
            return;
        }
        temp = temp->next;
    }
    printf("\n[-] THAT BAI: Khong tim thay Server ID %d.\n", id);
}