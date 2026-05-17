#include "../include/server_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

void addServer(ServerNode** head, int id, const char* model, int vtotal, const char* endpoint) {
    ServerNode* newNode = (ServerNode*)malloc(sizeof(ServerNode));
    newNode->server_id = id; strcpy(newNode->gpu_model, model);
    newNode->vram_total = vtotal; newNode->vram_used = 0;
    strcpy(newNode->api_endpoint, endpoint); newNode->status = 1; newNode->next = NULL;
    if (*head == NULL) { *head = newNode; return; }
    ServerNode* temp = *head;
    while (temp->next != NULL) temp = temp->next;
    temp->next = newNode;
}

int checkIdExists(ServerNode* head, int id) {
    ServerNode* temp = head;
    while (temp != NULL) { if (temp->server_id == id) return 1; temp = temp->next; }
    return 0;
}

void displayServers(ServerNode* head, int is_live, int uptime_seconds, int blink) {
    if (head == NULL) { printf(YELLOW "\n    [i] Khong co may chu nao hoat dong trong he thong. Vui long them moi!\n" RESET); return; }
    int total_nodes = 0, online_nodes = 0, offline_nodes = 0; int global_vram_total = 0, global_vram_used = 0;
    ServerNode* temp = head;
    while (temp != NULL) {
        total_nodes++; if (temp->status == 1) online_nodes++; else offline_nodes++;
        global_vram_total += temp->vram_total; global_vram_used += temp->vram_used; temp = temp->next;
    }
    printf(CYAN "\n    +----------------------------------------------------------------------------------------------------------------+\n");
    if (is_live) {
        int h = uptime_seconds / 3600; int m = (uptime_seconds % 3600) / 60; int s = uptime_seconds % 60; char live_badge[50];
        if (blink) sprintf(live_badge, "\033[31m[* LIVE *]\033[36m"); else sprintf(live_badge, "\033[37m[* LIVE *]\033[36m"); 
        printf("    | %s" BOLD " SYSTEM RESOURCE MONITOR (GLOBAL)                   " RESET YELLOW "Uptime: %02d:%02d:%02d" RESET CYAN "                            |\n", live_badge, h, m, s);
    } else {
        printf("    |" BOLD " SYSTEM RESOURCE MONITOR (GLOBAL)                                                                               " RESET CYAN "|\n");
    }
    printf("    +----------------------------------------------------------------------------------------------------------------+\n");
    printf("    | " RESET "Total Nodes: %-4d | " GREEN "Online: %-4d" RESET " | " RED "Offline: %-4d" RESET CYAN "                                                                |\n", total_nodes, online_nodes, offline_nodes);

    int global_percent = (global_vram_total == 0) ? 0 : (global_vram_used * 100) / global_vram_total;
    int global_bar_length = 50; int global_filled = (global_percent * global_bar_length) / 100;
    printf("    | " RESET "VRAM Usage : [%4d / %4d GB] ", global_vram_used, global_vram_total);
    printf("[");
    for (int i = 0; i < global_bar_length; i++) {
        if (i < global_filled) {
            if (global_percent >= 85) printf(RED "#" RESET); else if (global_percent >= 60) printf(YELLOW "#" RESET); else printf(GREEN "#" RESET);
        } else printf("."); 
    }
    printf(CYAN "] %3d%%                 |\n", global_percent);
    printf("    +----------------------------------------------------------------------------------------------------------------+\n\n" RESET);

    printf(CYAN "    +----------+----------------------+---------------------------+-------------------------------------+-----------+\n");
    printf("    |" BOLD " ID       " RESET CYAN "|" BOLD " GPU Model            " RESET CYAN "|" BOLD " Tai nguyen VRAM           " RESET CYAN "|" BOLD " API Endpoint                        " RESET CYAN "|" BOLD " Status    " RESET CYAN "|\n");
    printf("    +----------+----------------------+---------------------------+-------------------------------------+-----------+\n" RESET);
    
    temp = head;
    while (temp != NULL) {
        int percent = (temp->vram_total == 0) ? 0 : (temp->vram_used * 100) / temp->vram_total;
        int bar_length = 10; int filled = (percent * bar_length) / 100; char bar[100] = ""; 
        for (int i = 0; i < bar_length; i++) { if (i < filled) strcat(bar, "#"); else strcat(bar, "."); }

       // Cột 1 & 2: ID (8 chars) & Model (20 chars)
        printf(CYAN "    |" RESET " %-8d " CYAN "|" RESET " %-20s " CYAN "|", temp->server_id, temp->gpu_model);
        
        // Cột 3: VRAM (FIXED: Chinh xac 27 chars)
        // 10 chars (thong so) + 10 chars (bar) + 7 chars (duoi) = 27
        printf(RESET " %2d/%2dGB [", temp->vram_used, temp->vram_total);
        if (percent >= 80) printf(RED "%s" RESET, bar); 
        else if (percent >= 50) printf(YELLOW "%s" RESET, bar);
        else printf(GREEN "%s" RESET, bar); 
        printf("]      " CYAN "|"); // Dùng đúng 6 khoảng trắng ở đây
        
        // Cột 4: Endpoint (35 chars)
        printf(RESET " %-35s " CYAN "|", temp->api_endpoint);
        if (temp->status == 1) printf(GREEN " %-9s " CYAN "|\n" RESET, "Online"); else printf(RED " %-9s " CYAN "|\n" RESET, "Offline");
        
        temp = temp->next;
        if (temp != NULL) printf(CYAN "    +----------+----------------------+---------------------------+-------------------------------------+-----------+\n" RESET);
    }
    printf(CYAN "    +----------+----------------------+---------------------------+-------------------------------------+-----------+\n" RESET);
}

void deleteServer(ServerNode** head, int id) {
    ServerNode *temp = *head, *prev = NULL;
    if (temp != NULL && temp->server_id == id) {
        *head = temp->next; free(temp); printf(GREEN "    [+] Thuc thi thanh cong: Da go bo may chu khoi he thong!\n" RESET); return;
    }
    while (temp != NULL && temp->server_id != id) { prev = temp; temp = temp->next; }
    if (temp == NULL) { printf(RED "    [-] That bai: Khong tim thay may chu co ID %d trong CSDL.\n" RESET, id); return; }
    prev->next = temp->next; free(temp); printf(GREEN "    [+] Thuc thi thanh cong: Da go bo may chu khoi he thong!\n" RESET);
}

void sortServersByVRAM(ServerNode* head) {
    if (head == NULL) return;
    int swapped; ServerNode* ptr1; ServerNode* lptr = NULL;
    do {
        swapped = 0; ptr1 = head;
        while (ptr1->next != lptr) {
            if (ptr1->vram_total < ptr1->next->vram_total) {
                int temp_id = ptr1->server_id; ptr1->server_id = ptr1->next->server_id; ptr1->next->server_id = temp_id;
                char temp_model[50]; strcpy(temp_model, ptr1->gpu_model); strcpy(ptr1->gpu_model, ptr1->next->gpu_model); strcpy(ptr1->next->gpu_model, temp_model);
                int temp_vtotal = ptr1->vram_total; ptr1->vram_total = ptr1->next->vram_total; ptr1->next->vram_total = temp_vtotal;
                int temp_vused = ptr1->vram_used; ptr1->vram_used = ptr1->next->vram_used; ptr1->next->vram_used = temp_vused;
                char temp_ep[100]; strcpy(temp_ep, ptr1->api_endpoint); strcpy(ptr1->api_endpoint, ptr1->next->api_endpoint); strcpy(ptr1->next->api_endpoint, temp_ep);
                int temp_st = ptr1->status; ptr1->status = ptr1->next->status; ptr1->next->status = temp_st;
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

void toggleServerStatus(ServerNode* head, int id) {
    ServerNode* temp = head;
    while (temp != NULL) {
        if (temp->server_id == id) { temp->status = (temp->status == 1) ? 0 : 1; printf(GREEN "    [+] Da cap nhat trang thai bao tri cho ID %d!\n" RESET, id); return; }
        temp = temp->next;
    }
    printf(RED "    [-] That bai: Khong tim thay may chu co ID %d.\n" RESET, id);
}

void deployAIModel(ServerNode* head, const char* ai_model_name, int required_vram) {
    int total_free_vram = 0; ServerNode* temp = head;

    // 1. Kiem tra tong VRAM toan mang luoi
    while (temp != NULL) {
        if (temp->status == 1) total_free_vram += (temp->vram_total - temp->vram_used);
        temp = temp->next;
    }

    if (total_free_vram < required_vram) {
        printf(RED "\n    [-] THAT BAI CHI MANG: He thong chi con %d GB VRAM kha dung.\n" RESET, total_free_vram);
        return;
    }

    // 2. Kiem tra kha nang chay doc lap (Single-Node)
    temp = head; int capable_nodes_count = 0;
    while (temp != NULL) {
        if (temp->status == 1 && (temp->vram_total - temp->vram_used) >= required_vram) capable_nodes_count++;
        temp = temp->next;
    }

    if (capable_nodes_count > 0) {
        printf(GREEN "\n    [+] TIM THAY %d MAY CHU DU TAI (Single-Node Capable):\n" RESET, capable_nodes_count);
        printf(CYAN "    +----------+----------------------+---------------------------+\n");
        printf("    |" BOLD " ID       " RESET CYAN "|" BOLD " GPU Model            " RESET CYAN "|" BOLD " VRAM Kha dung             " RESET CYAN "|\n");
        printf("    +----------+----------------------+---------------------------+\n" RESET);

        temp = head;
        while (temp != NULL) {
            int free_vram = temp->vram_total - temp->vram_used;
            if (temp->status == 1 && free_vram >= required_vram) {
                printf(CYAN "    |" RESET " %-8d " CYAN "|" RESET " %-20s " CYAN "|" RESET " %2d GB (Trong)             " CYAN "|\n" RESET, temp->server_id, temp->gpu_model, free_vram);
            }
            temp = temp->next;
        }
        printf(CYAN "    +----------+----------------------+---------------------------+\n" RESET);

        int selected_id = -1;
        while (1) {
            printf(BOLD YELLOW "\n    >> Nhap ID may chu de trien khai (Hoac nhap 0 de mo che do Phan tan): " RESET);
            if (scanf("%d", &selected_id) != 1) {
                while(getchar() != '\n');
                printf(RED "    [-] Loi nhap lieu. Vui long nhap so nguyen.\n" RESET);
                continue;
            }
            while(getchar() != '\n');

            if (selected_id == 0) break; // Nguoi dung tu choi Single-Node, muon cat nho ra

            int found = 0; temp = head;
            while (temp != NULL) {
                if (temp->server_id == selected_id && temp->status == 1 && (temp->vram_total - temp->vram_used) >= required_vram) {
                    temp->vram_used += required_vram;
                    printf(GREEN "\n    [+] Cap phat thanh cong (Single-Node) tren may chu ID %d!\n" RESET, selected_id);
                    return;
                }
                temp = temp->next;
            }
            if (!found) printf(RED "    [-] ID khong hop le hoac may chu khong du RAM. Vui long chon lai.\n" RESET);
        }
    }

    // 3. KICH HOAT TENSOR PARALLELISM & CHO CHỌN CHIẾN LƯỢC
    printf(MAGENTA "\n    [*] KICH HOAT GIAO THUC TENSOR PARALLELISM (PHAN TAN MODEL)...\n" RESET);
    printf(CYAN "    Hay chon Chien luoc Phan bo (Allocation Strategy) cho " YELLOW "%d GB" CYAN " VRAM:\n" RESET, required_vram);
    printf("      [1] Tham lam (Greedy)   - Dien day VRAM may nay roi moi sang may khac.\n");
    printf("      [2] Can bang (Balanced) - Chia deu tai (Round-Robin) cho toan bo cac may.\n");
    printf("      [3] Thu cong (Custom)   - Nguoi dung tu nhap so GB muon cat cho tung may.\n");

    int strategy = 1;
    while(1) {
        printf(BOLD YELLOW "    >> Nhap lua chon (1-3): " RESET);
        if (scanf("%d", &strategy) == 1 && strategy >= 1 && strategy <= 3) {
            while(getchar() != '\n'); break;
        }
        while(getchar() != '\n');
        printf(RED "    [-] Lua chon khong hop le.\n" RESET);
    }

    // Tao cac mang dong de luu "Bản nháp" ke hoach
    int active_count = 0; temp = head;
    while(temp) { if(temp->status == 1 && (temp->vram_total - temp->vram_used) > 0) active_count++; temp = temp->next; }

    ServerNode** p_nodes = (ServerNode**)malloc(active_count * sizeof(ServerNode*));
    int* p_alloc = (int*)calloc(active_count, sizeof(int));
    int* p_free = (int*)malloc(active_count * sizeof(int));

    temp = head; int idx = 0;
    while(temp) {
        if(temp->status == 1 && (temp->vram_total - temp->vram_used) > 0) {
            p_nodes[idx] = temp;
            p_free[idx] = temp->vram_total - temp->vram_used;
            idx++;
        }
        temp = temp->next;
    }

    int needed = required_vram;

    // Xu ly chien luoc
    if (strategy == 1) { 
        // Greedy: Lam day may dau tien, thieu thi lay may tiep theo
        for(int i=0; i<active_count && needed > 0; i++) {
            int take = (p_free[i] >= needed) ? needed : p_free[i];
            p_alloc[i] = take; needed -= take;
        }
    }
    else if (strategy == 2) { 
        // Balanced: Chia deu 1GB moi vong lap vao cac may cho den khi du
        while(needed > 0) {
            for(int i=0; i<active_count && needed > 0; i++) {
                if(p_alloc[i] < p_free[i]) { p_alloc[i]++; needed--; }
            }
        }
    }
    else if (strategy == 3) { 
        // Custom: Hoi tung may mot
        printf(CYAN "\n    [+] CHE DO THU CONG: Ban can phan bo %d GB VRAM.\n" RESET, needed);
        while(needed > 0) {
            for(int i=0; i<active_count && needed > 0; i++) {
                if (p_alloc[i] == p_free[i]) continue; // May nay da day
                int take = 0;
                printf("     -> May ID %-5d (Con trong: %2d GB). Nhap so GB muon cat (Can them %d GB): ", p_nodes[i]->server_id, p_free[i] - p_alloc[i], needed);
                if(scanf("%d", &take) != 1) { while(getchar() != '\n'); take = 0; }
                else { while(getchar() != '\n'); }

                if(take < 0) take = 0;
                if(take > (p_free[i] - p_alloc[i])) {
                    printf(YELLOW "        [!] Vuot qua VRAM trong. Chi lay toi da %d GB.\n" RESET, p_free[i] - p_alloc[i]);
                    take = p_free[i] - p_alloc[i];
                }
                if(take > needed) take = needed;

                p_alloc[i] += take; needed -= take;
            }
        }
    }

    // Hien thi "Bản nháp" Ke hoach va Hoi xac nhan
    printf(CYAN "\n    +-----------------------------------------------------------+\n");
    printf("    |" BOLD " BAN XEM TRUOC KE HOACH PHAN BO (PREVIEW)                  " RESET CYAN "|\n");
    printf("    +-----------------------------------------------------------+\n" RESET);
    for(int i=0; i<active_count; i++) {
        if(p_alloc[i] > 0) {
            printf(CYAN "    |" RESET " May ID: %-6d | Model: %-15s | Cap phat: " YELLOW "%2d GB" CYAN " |\n" RESET, p_nodes[i]->server_id, p_nodes[i]->gpu_model, p_alloc[i]);
        }
    }
    printf(CYAN "    +-----------------------------------------------------------+\n" RESET);

    int confirm;
    printf(BOLD YELLOW "\n    >> Ban co dong y thuc thi ke hoach nay khong? (1: Co / 0: Huy): " RESET);
    if (scanf("%d", &confirm) == 1 && confirm == 1) {
        for(int i=0; i<active_count; i++) {
            p_nodes[i]->vram_used += p_alloc[i]; // THUC SU TRU VRAM O DAY
        }
        printf(GREEN "    [+] TRIEN KHAI PHAN TAN THANH CONG!\n" RESET);
    } else {
        printf(RED "    [-] Da huy chien dich trien khai.\n" RESET);
    }
    while(getchar() != '\n');

    // Giai phong bo nho tam
    free(p_nodes); free(p_alloc); free(p_free);
}

void freeList(ServerNode** head) {
    ServerNode* current = *head;
    while (current != NULL) { ServerNode* temp = current; current = current->next; free(temp); }
    *head = NULL;
}

// =====================================================================
// SIEU TRI TUE NHAN TAO: NEXTGPU COPILOT (CO HIEU UNG ROBOT)
// =====================================================================
void ai_delay(int milliseconds) {
    long pause = milliseconds * (CLOCKS_PER_SEC / 1000); clock_t now, then; now = then = clock();
    while((now - then) < pause) now = clock();
}

void typewriter(const char* text) {
    printf(WHITE "    "); 
    for(int i = 0; text[i] != '\0'; i++) {
        putchar(text[i]); fflush(stdout); ai_delay(10);   
    }
    printf("\n" RESET);
}

void ai_think_animation() {
    printf(CYAN);
    for(int i = 0; i < 2; i++) {
        printf("\r    [ ~_~ ] Dang xu ly du lieu... "); fflush(stdout); ai_delay(200);
        printf("\r    [ o_o ] Dang xu ly du lieu... "); fflush(stdout); ai_delay(200);
        printf("\r    [ -_- ] Dang xu ly du lieu... "); fflush(stdout); ai_delay(200);
        printf("\r    [ o_o ] Dang xu ly du lieu... "); fflush(stdout); ai_delay(200);
    }
    printf("\r                                        \r" RESET);
}

void toLowerCase(char* str) { for(int i = 0; str[i]; i++) str[i] = tolower(str[i]); }

void nextgpuCopilot(ServerNode** head_ref) {
    char input[300]; char input_lower[300];

    printf("\033[H\033[J");
    printf(CYAN "    +====================================================================================+\n");
    printf("    |                       " MAGENTA BOLD "NEXTGPU COPILOT (CLI AI ASSISTANT)" RESET CYAN "                           |\n");
    printf("    +====================================================================================+\n" RESET);
    printf(GREEN "    [o_o] Xin chao! Toi la AI dieu hanh he thong NEXTGPU Data Center.\n");
    printf("    [o_o] Toi co the tu tay thuc thi lenh (Them/Xoa/Trien khai AI) va giai thich Code.\n");
    printf("    [o_o] Hay nhap lenh cua ban (Vi du: 'them may chu', 'giai thich', hoac 'thoat').\n" RESET);
    printf(CYAN "    --------------------------------------------------------------------------------------\n" RESET);

    while (1) {
        printf(BOLD YELLOW "\n    > User: " RESET);
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0; strcpy(input_lower, input); toLowerCase(input_lower);
        if (strlen(input_lower) == 0) continue;
        
        ai_think_animation();
        printf(MAGENTA "    [Copilot]\n" RESET);

        // --- 1. THUC THI LENH: THOÁT ---
        if (strstr(input_lower, "thoat") || strstr(input_lower, "exit") || strstr(input_lower, "quit")) {
            typewriter("Da ro. Tam biet Quan tri vien. Chuc nhom ban bao ve do an dat ket qua Xuat sac!"); break;
        }
        
        // --- 2. THUC THI LENH: THÊM MÁY CHỦ TRỰC TIẾP ---
        else if ((strstr(input_lower, "them") || strstr(input_lower, "add")) && (strstr(input_lower, "may") || strstr(input_lower, "server"))) {
            typewriter("Toi se giup ban nap thiet bi moi vao Data Center. Vui long cung cap thong so:");
            int id, vtotal; char model[50], endpoint[100];
            printf(CYAN "    - Nhap ID may chu (So Nguyen): " RESET); 
            if (scanf("%d", &id) != 1) { while(getchar() != '\n'); typewriter("Loi ky tu! Ban phai nhap so. Da huy thao tac."); continue; }
            while(getchar() != '\n'); // clear buffer
            
            if (checkIdExists(*head_ref, id)) { typewriter("Canh bao: ID nay da ton tai trong co so du lieu roi!"); continue; }
            
            printf(CYAN "    - Nhap Model GPU (VD: RTX_4090): " RESET); scanf(" %[^\n]", model); while(getchar() != '\n');
            printf(CYAN "    - Nhap Tong VRAM (GB): " RESET); 
            if (scanf("%d", &vtotal) != 1) { while(getchar() != '\n'); typewriter("Loi ky tu! Da huy thao tac."); continue; }
            while(getchar() != '\n');
            
            printf(CYAN "    - Nhap API Endpoint: " RESET); scanf(" %[^\n]", endpoint); while(getchar() != '\n');
            
            addServer(head_ref, id, model, vtotal, endpoint);
            typewriter("Hoan tat! May chu da duoc dau noi vao mang luoi thanh cong. Ban co the kiem tra lai o Dashboard.");
        }

        // --- 3. THUC THI LENH: TRIỂN KHAI MODEL AI TRỰC TIẾP ---
        else if (strstr(input_lower, "trien khai") || strstr(input_lower, "deploy") || strstr(input_lower, "can bang tai") || strstr(input_lower, "chay model")) {
            typewriter("He thong Load Balancing da san sang nhan lenh. Vui long nhap thong tin Model AI:");
            char ai_name[50]; int req_vram;
            printf(CYAN "    - Nhap Ten Model (VD: Claude_3_Opus): " RESET); scanf(" %[^\n]", ai_name); while(getchar() != '\n');
            printf(CYAN "    - Nhap muc tieu thu VRAM (GB): " RESET); 
            if (scanf("%d", &req_vram) != 1) { while(getchar() != '\n'); typewriter("Loi ky tu! Da huy thao tac."); continue; }
            while(getchar() != '\n');
            
            deployAIModel(*head_ref, ai_name, req_vram);
        }

        // --- 4. THUC THI LENH: XÓA MÁY CHỦ ---
        else if ((strstr(input_lower, "xoa") || strstr(input_lower, "delete")) && (strstr(input_lower, "may") || strstr(input_lower, "server"))) {
            typewriter("De an toan, vui long cho toi biet ID cua may chu ban muon gỡ bỏ:");
            int del_id;
            printf(CYAN "    - Nhap ID can xoa: " RESET);
            if (scanf("%d", &del_id) != 1) { while(getchar() != '\n'); typewriter("Loi ky tu! Da huy thao tac."); continue; }
            while(getchar() != '\n');
            deleteServer(head_ref, del_id);
        }

        // --- 5. THUC THI LENH: SẮP XẾP ---
        else if (strstr(input_lower, "sap xep") || strstr(input_lower, "sort") || strstr(input_lower, "toi uu")) {
            ai_think_animation();
            sortServersByVRAM(*head_ref);
            typewriter("Toi da chay thuat toan Bubble Sort. Tat ca Server hien da duoc sap xep uu tien theo muc VRAM giam dan.");
        }

        // --- 6. HỖ TRỢ LÝ THUYẾT: GIỚI THIỆU DỰ ÁN ---
        else if (strstr(input_lower, "gioi thieu") || strstr(input_lower, "project") || strstr(input_lower, "la gi")) {
            typewriter("Chao ban, du an NEXTGPU PRO la mot He thong Quan tri va Can bang tai Ha tang may chu AI (AI Data Center Control Panel).");
            typewriter("Trong thuc te, cac mo hinh AI khong lo (nhu ChatGPT) can tieu thu luong VRAM cuc lon. He thong nay sinh ra de giam sat RAM cua toan bo Data Center.");
            typewriter("No duoc phat trien hoan toan bang ngon ngu C, tich hop cac kien thuc nhu: Danh sach lien ket, Ma hoa XOR, Thuat toan Greedy, va Giao dien Console thoi gian thuc (TUI).");
        }

        // --- 7. HỖ TRỢ LÝ THUYẾT: GITHUB ---
        else if (strstr(input_lower, "github") || strstr(input_lower, "link") || strstr(input_lower, "source")) {
            typewriter("Du an NEXTGPU PRO duoc quan ly phien ban chat che bang Git va GitHub.");
            typewriter("Chung toi lam viec nhom bang cach chia nhanh (branch), dong gop code (commit) de giang vien danh gia minh bach.");
            typewriter("Truy cap ma nguon tai: https://github.com/hoquan2007/GPU-Server-Manager");
        }

        // --- 8. HỖ TRỢ LÝ THUYẾT: THUẬT TOÁN (SIÊU CHI TIẾT) ---
        else if (strstr(input_lower, "danh sach lien ket") || strstr(input_lower, "node") || strstr(input_lower, "code nhu the nao")) {
            typewriter("Ve mat To chuc Du lieu, du an nay KHONG dung Mang (Array). Toi dung Danh Sach Lien Ket Don (Singly Linked List).");
            typewriter("Cach hoat dong: Moi may chu la 1 'Struct ServerNode' duoc cap phat bo nho dong bang ham malloc(). Trong struct nay co 1 con tro 'next' de moc vao may chu tiep theo.");
            typewriter("Uu diem tuyet doi: He thong co the mo rong den vo han ma khong ton truoc RAM. Khi goi ham deleteServer, toi dung 2 con tro 'temp' va 'prev' de nối lại danh sach roi dung lenh free() de giai phong ram.");
        }
        else if (strstr(input_lower, "ma hoa") || strstr(input_lower, "file") || strstr(input_lower, "luu")) {
            typewriter("He thong bao mat du lieu bang ky thuat Ma Hoa XOR (XOR Cipher) tren file nhi phan (.dat).");
            typewriter("Cach code: Trong file file_io.c, toi tao 1 ham encryptDecrypt voi SECRET_KEY = 0x5A. Truoc khi dung lenh fwrite() ghi xuong o cung, toi dung vong lap for XOR (^) tung byte cua Struct. Khi dung lenh fread() doc len, toi lai XOR mot lan nua de ra nguyen ban.");
        }
        else if (strstr(input_lower, "thuat toan") || strstr(input_lower, "hoat dong ra sao")) {
            typewriter("Thuat toan dinh cao nhat cua du an la Tensor Parallelism (Can Bang Tai AI). Duoc viet trong ham deployAIModel.");
            typewriter("1. Dau tien, toi dung vong lap while quet toan bo Node de tinh tong VRAM. Neu khong du, toi se huy lenh luon.");
            typewriter("2. Sau do, toi tim 1 may chu manh nhat. Neu may do du RAM de gan tat ca (Single-Node), toi cap phat luon cho may do de toi uu toc do bang thong.");
            typewriter("3. Neu khong co may don nao du RAM, toi kich hoat Thuat toan Tham lam (Greedy). Toi se duyet tung Node, rut can VRAM cua no (vram_used += allocated) cho den khi du so RAM ma Mo hinh AI yeu cau.");
        }
        else if (strstr(input_lower, "can bang tai") || strstr(input_lower, "ai") || strstr(input_lower, "tensor") || strstr(input_lower, "phan bo")) {
            typewriter("Chuc nang Cân Bằng Tải AI là module phức tạp và mang đậm tính Cloud Architecture của dự án.");
            typewriter("Hệ thống sẽ liệt kê các máy đủ tải trước. Nếu quản trị viên muốn chia nhỏ Model ra, hệ thống cung cấp 3 Chiến lược (Allocation Strategy):");
            typewriter("1. Tham lam (Greedy/Binpack): Nhồi VRAM làm đầy máy thứ nhất rồi mới sang máy thứ 2, giúp tiết kiệm số lượng máy phải chạy.");
            typewriter("2. Cân bằng (Balanced/Spread): Chia đều bằng thuật toán Round-Robin, cấp từng 1GB xoay vòng cho các máy để tránh thắt nút cổ chai nhiệt độ.");
            typewriter("3. Thủ công (Custom): Quản trị viên tự gõ số GB muốn gánh cho từng máy.");
            typewriter("Hệ thống dùng malloc tạo ra một mảng 'Bản nháp' để in Preview cho người dùng duyệt trước khi thực sự cấp phát VRAM! Rất chuyên nghiệp!");
        }
        // --- 9. MẶC ĐỊNH KHÔNG HIỂU ---
        else {
            typewriter("Lenh cua ban chua hop le hoac nam ngoai kha nang ho tro cua toi.");
            typewriter("Huong dan thao tac: Ban co the ra lenh 'them may chu', 'xoa may chu', 'trien khai', 'sap xep'.");
            typewriter("Hoac dat cau hoi: 'gioi thieu du an', 'xin link github', 'danh sach lien ket la gi', 'ma hoa file', 'thuat toan'.");
        }
    }
}