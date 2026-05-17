#include "../include/server_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// =====================================================================
// DATA PERSISTENCE: LUU MO HINH AI VAO O CUNG
// =====================================================================
static char deployed_models[10][50];
static int deployed_count = 0;

void saveAIModels() {
    FILE* f = fopen("data/deployed_models.dat", "w");
    if (f != NULL) {
        for (int i = 0; i < deployed_count; i++) {
            fprintf(f, "%s\n", deployed_models[i]);
        }
        fclose(f);
    }
}

void loadAIModels() {
    FILE* f = fopen("data/deployed_models.dat", "r");
    if (f != NULL) {
        deployed_count = 0;
        while (fgets(deployed_models[deployed_count], 50, f) != NULL) {
            deployed_models[deployed_count][strcspn(deployed_models[deployed_count], "\n")] = 0;
            if (strlen(deployed_models[deployed_count]) > 0) {
                deployed_count++;
            }
            if (deployed_count >= 10) break;
        }
        fclose(f);
    }
}

void saveDeployedModel(const char* name) {
    if (deployed_count < 10) { 
        strcpy(deployed_models[deployed_count], name); 
        deployed_count++; 
        saveAIModels(); // TU DONG LUU NGAY KHI DEPLOY
    }
}

// =====================================================================
// UI ENGINE: THUAT TOAN BOX-WRAP TYPEWRITER CHONG VO KHUNG
// =====================================================================
void ai_delay(int milliseconds) {
    long pause = milliseconds * (CLOCKS_PER_SEC / 1000); clock_t now, then; now = then = clock();
    while((now - then) < pause) now = clock();
}

void typewriter_box(const char* text, const char* border_color, int width) {
    int curr_len = 0;
    printf("%s    | " WHITE, border_color); 
    
    int i = 0;
    while (text[i] != '\0') {
        int word_len = 0;
        while (text[i + word_len] != ' ' && text[i + word_len] != '\n' && text[i + word_len] != '\0') {
            word_len++;
        }
        
        // Neu tu tiep theo lam tran dong, xuong dong va ke vach
        if (curr_len + word_len > width && curr_len > 0) {
            while(curr_len < width) { printf(" "); curr_len++; }
            printf("%s|\n%s    | " WHITE, border_color, border_color);
            curr_len = 0;
        }
        
        // In tung chu cai cua tu
        for(int k=0; k<word_len; k++) {
            putchar(text[i]); fflush(stdout); ai_delay(5);
            i++; curr_len++;
        }
        
        // Xu ly khoang trang hoac dau xuong dong chu dong
        if (text[i] == ' ') {
            if (curr_len < width) { putchar(' '); fflush(stdout); ai_delay(5); curr_len++; }
            i++;
        } else if (text[i] == '\n') {
            while(curr_len < width) { printf(" "); curr_len++; }
            printf("%s|\n%s    | " WHITE, border_color, border_color);
            curr_len = 0; i++;
        }
    }
    
    // Đóng nắp khung khi in xong đoạn văn bản
    while(curr_len < width) { printf(" "); curr_len++; }
    printf("%s|\n" RESET, border_color);
}

void ai_think_animation(const char* action_text) {
    printf(CYAN);
    for(int i = 0; i < 2; i++) {
        printf("\r    [ ~_~ ] %s...", action_text); fflush(stdout); ai_delay(150);
        printf("\r    [ o_o ] %s...", action_text); fflush(stdout); ai_delay(150);
        printf("\r    [ -_- ] %s...", action_text); fflush(stdout); ai_delay(150);
        printf("\r    [ o_o ] %s...", action_text); fflush(stdout); ai_delay(150);
    }
    printf("\r                                                                      \r" RESET);
}

void toLowerCase(char* str) { for(int i = 0; str[i]; i++) str[i] = tolower(str[i]); }

// =====================================================================
// MODULE QUAN LY MAY CHU
// =====================================================================
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

         printf(CYAN "    |" RESET " %-8d " CYAN "|" RESET " %-20s " CYAN "|", temp->server_id, temp->gpu_model);
        printf(RESET " %2d/%2dGB [", temp->vram_used, temp->vram_total);
        if (percent >= 80) printf(RED "%s" RESET, bar); 
        else if (percent >= 50) printf(YELLOW "%s" RESET, bar);
        else printf(GREEN "%s" RESET, bar); 
        printf("]      " CYAN "|");
        
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

// TÍNH NĂNG MỚI: GỠ AI RA KHỎI GPU
void undeployAIModel(ServerNode* head, int id) {
    ServerNode* temp = head;
    while(temp != NULL) {
        if(temp->server_id == id) {
            if (temp->vram_used > 0) {
                temp->vram_used = 0;
                printf(GREEN "    [+] Thanh cong! Da giai phong toan bo VRAM dang chiem dung tren may chu ID %d.\n" RESET, id);
            } else {
                printf(YELLOW "    [!] May chu ID %d hien khong chay Model AI nao (VRAM Used = 0).\n" RESET, id);
            }
            return;
        }
        temp = temp->next;
    }
    printf(RED "    [-] Khong tim thay may chu co ID %d trong he thong.\n" RESET, id);
}

void deployAIModel(ServerNode* head, const char* ai_model_name, int required_vram) {
    int total_free_vram = 0; ServerNode* temp = head;
    while (temp != NULL) { if (temp->status == 1) total_free_vram += (temp->vram_total - temp->vram_used); temp = temp->next; }
    if (total_free_vram < required_vram) { printf(RED "    [-] THAT BAI CHI MANG: He thong chi con %d GB VRAM kha dung.\n" RESET, total_free_vram); return; }
    
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
                while(getchar() != '\n'); printf(RED "    [-] Loi nhap lieu. Vui long nhap so nguyen.\n" RESET); continue;
            }
            while(getchar() != '\n');
            if (selected_id == 0) break; 
            int found = 0; temp = head;
            while (temp != NULL) {
                if (temp->server_id == selected_id && temp->status == 1 && (temp->vram_total - temp->vram_used) >= required_vram) {
                    temp->vram_used += required_vram;
                    printf(GREEN "\n    [+] Cap phat thanh cong (Single-Node) tren may chu ID %d!\n" RESET, selected_id);
                    saveDeployedModel(ai_model_name); 
                    return;
                }
                temp = temp->next;
            }
            if (!found) printf(RED "    [-] ID khong hop le. Vui long chon lai.\n" RESET);
        }
    }

    printf(MAGENTA "\n    [*] KICH HOAT GIAO THUC TENSOR PARALLELISM (PHAN TAN MODEL)...\n" RESET);
    printf(CYAN "    Hay chon Chien luoc Phan bo (Allocation Strategy) cho " YELLOW "%d GB" CYAN " VRAM:\n" RESET, required_vram);
    printf("      [1] Tham lam (Greedy)   - Dien day VRAM may nay roi moi sang may khac.\n");
    printf("      [2] Can bang (Balanced) - Chia deu tai (Round-Robin) cho toan bo cac may.\n");
    printf("      [3] Thu cong (Custom)   - Nguoi dung tu nhap so GB muon cat cho tung may.\n");

    int strategy = 1;
    while(1) {
        printf(BOLD YELLOW "    >> Nhap lua chon (1-3): " RESET);
        if (scanf("%d", &strategy) == 1 && strategy >= 1 && strategy <= 3) { while(getchar() != '\n'); break; }
        while(getchar() != '\n'); printf(RED "    [-] Lua chon khong hop le.\n" RESET);
    }

    int active_count = 0; temp = head;
    while(temp) { if(temp->status == 1 && (temp->vram_total - temp->vram_used) > 0) active_count++; temp = temp->next; }

    ServerNode** p_nodes = (ServerNode**)malloc(active_count * sizeof(ServerNode*));
    int* p_alloc = (int*)calloc(active_count, sizeof(int));
    int* p_free = (int*)malloc(active_count * sizeof(int));

    temp = head; int idx = 0;
    while(temp) {
        if(temp->status == 1 && (temp->vram_total - temp->vram_used) > 0) { p_nodes[idx] = temp; p_free[idx] = temp->vram_total - temp->vram_used; idx++; }
        temp = temp->next;
    }

    int needed = required_vram;
    if (strategy == 1) { 
        for(int i=0; i<active_count && needed > 0; i++) { int take = (p_free[i] >= needed) ? needed : p_free[i]; p_alloc[i] = take; needed -= take; }
    } else if (strategy == 2) { 
        while(needed > 0) { for(int i=0; i<active_count && needed > 0; i++) { if(p_alloc[i] < p_free[i]) { p_alloc[i]++; needed--; } } }
    } else if (strategy == 3) { 
        printf(CYAN "\n    [+] CHE DO THU CONG: Ban can phan bo %d GB VRAM.\n" RESET, needed);
        while(needed > 0) {
            for(int i=0; i<active_count && needed > 0; i++) {
                if (p_alloc[i] == p_free[i]) continue;
                int take = 0;
                printf("     -> May ID %-5d (Con trong: %2d GB). Nhap so GB muon cat (Can them %d GB): ", p_nodes[i]->server_id, p_free[i] - p_alloc[i], needed);
                if(scanf("%d", &take) != 1) { while(getchar() != '\n'); take = 0; } else { while(getchar() != '\n'); }
                if(take < 0) take = 0;
                if(take > (p_free[i] - p_alloc[i])) take = p_free[i] - p_alloc[i];
                if(take > needed) take = needed;
                p_alloc[i] += take; needed -= take;
            }
        }
    }

    printf(CYAN "\n    +-----------------------------------------------------------+\n");
    printf("    |" BOLD " BAN XEM TRUOC KE HOACH PHAN BO (PREVIEW)                  " RESET CYAN "|\n");
    printf("    +-----------------------------------------------------------+\n" RESET);
    for(int i=0; i<active_count; i++) {
        if(p_alloc[i] > 0) printf(CYAN "    |" RESET " May ID: %-6d | Model: %-15s | Cap phat: " YELLOW "%2d GB" CYAN " |\n" RESET, p_nodes[i]->server_id, p_nodes[i]->gpu_model, p_alloc[i]);
    }
    printf(CYAN "    +-----------------------------------------------------------+\n" RESET);

    int confirm;
    printf(BOLD YELLOW "\n    >> Ban co dong y thuc thi ke hoach nay khong? (1: Co / 0: Huy): " RESET);
    if (scanf("%d", &confirm) == 1 && confirm == 1) {
        for(int i=0; i<active_count; i++) p_nodes[i]->vram_used += p_alloc[i];
        printf(GREEN "    [+] TRIEN KHAI PHAN TAN THANH CONG!\n" RESET);
        saveDeployedModel(ai_model_name); 
    } else printf(RED "    [-] Da huy chien dich trien khai.\n" RESET);
    while(getchar() != '\n');

    free(p_nodes); free(p_alloc); free(p_free);
}

// =====================================================================
// DATA STRUCTURE FOR BOTH AIs
// =====================================================================
typedef struct { char keywords[150]; char answer[800]; } Knowledge;

// =====================================================================
// CON AI SO 1: KNOWLEDGE DATA SEGMENT (MANG KHONG LO CHO TERMINAL CHAT)
// =====================================================================
Knowledge chat_db[] = {
    // ================= [ NHÓM 1: THÔNG TIN DỰ ÁN & CHÀO HỎI ] =================
    {"hello chao xin_chao hi hey alo", 
     "Xin chao Quan tri vien! Toi la thiet bi Mo hinh AI da duoc ban trien khai thanh cong. Toi da nap tri thuc toan cau qua song mang va san sang tiep nhan cac yeu cau tinh toan tu ban."},
    
    {"tac_gia dev coder nhom_it_utc2 hoquan2007 sinh_vien", 
     "Toi duoc thiet ke, xay dung va toi uu hoa kien truc boi nhom IT UTC2 gom 4 thanh vien: QUAN, PHONG X2, TUONG."},
    
    {"github link source_code ma_nguon repo repository open_source", 
     "Ma nguon sieu du an NEXTGPU PRO duoc mo hoan toan. Truy cap Repository chinh thuc de tai code va ho tro dong gop: https://github.com/hoquan2007/GPU-Server-Manager"},
    
    {"giang_vien thay_co cham_diem bao_ve do_an diem_cao hoi_dong", 
     "Kinh chao Thay/Co trong Hoi dong! Nhom chung em da doc luc thiet ke he thong bang C thuan voi kien truc phan tan va can bang tai. Rat mong nhan duoc su danh gia cao tu Thay/Co!"},
    
    {"huong_dan menu cach_dung phim_tat dieu_huong tui", 
     "Giao dien van hanh qua TUI (Text User Interface). Dung mui ten Len/Xuong de chon, Enter de thuc thi. Ban co the them/xoa node, trien khai AI hoac xem Live Monitor de dang."},

    // ================= [ NHÓM 2: KIẾN TRÚC C LÕI & GIAO DIỆN ] =================
    {"ngon_ngu_c gcc compiler bien_dich source c_thuan", 
     "Toi chay nguyen ban tren 100% ngon ngu C tieu chuan (C99/C11) duoc bien dich boi GCC. Toan bo thuat toan duoc code tay ma khong dung framework hay thu vien do hoa ben thu 3."},
    
    {"pointer node danh_sach_lien_ket malloc free struct con_tro bo_nho heap", 
     "Du an su dung 'Singly Linked List' kem cap phat dong tren Heap bang malloc(). Dieu nay giup mo rong Data Center vo tan ma khong bi tran bo nho, ket hop free() de chong Memory Leak."},
    
    {"giao_dien console ansi_escape system_cls chong_giat fps overwrite", 
     "Giao dien khong xai system('cls') gay giat. He thong dung ma ANSI '\\033[H' de day con tro len goc (0,0) va ghi de khung hinh moi, giup do hoa render muot ma 60fps!"},
    
    {"typewriter hieu_ung_go putchar fflush delay mili_giay", 
     "Hieu ung go chu ban dang thay dung ham putchar() xuat tung ky tu, ket hop fflush(stdout) va vong lap delay() de mo phong 100% cam giac AI dang suy nghi va tao ra van ban."},
    
    {"bug loi error crash system.log debug log_file bsod", 
     "He thong duoc bao ve qua file log. Mo 'data/system.log' de dieu tra. De chong crash, cac ham doc ghi deu dung bo dem an toan va kiem tra con tro NULL nghiem ngat."},

    // ================= [ NHÓM 3: CÂN BẰNG TẢI & THUẬT TOÁN ] =================
    {"can_bang_tai load_balancing parallel split tensor shards phan_tan", 
     "Module load balancing la kiet tac. Thuat toan Tensor Parallelism se cat mo hinh AI thanh nhieu manh (shards) va phan phoi sang cac may chu dang ranh, ghep VRAM lai de ganh model nang."},
    
    {"greedy tham_lam binpack don_tai", 
     "Chien luoc Tham lam (Greedy) quet tuyen tinh, nhoi day 100% VRAM cua may so 1 roi moi qua may 2. Giup don tai vao 1 cum nho de tiet kiem dien nang cho cac may khac."},
    
    {"balanced can_bang round_robin tan_nhiet nhiet_do", 
     "Chien luoc Can bang (Balanced) dung thuat toan Round-Robin de xoay vong chia deu tai (VD: moi may 1GB). Giup tan nhiet deu, chong nghen co chai nhiet (Thermal Throttling)."},
    
    {"custom thu_cong tu_nhap phan_bo ep_ram", 
     "Chien luoc Thu cong mang lai quyen kiem soat cho Admin. Ban tu tay go so luong RAM muon ep cho tung ID may chu, rat phu hop cho ha tang thiet bi khong dong nhat."},
    
    {"bubble_sort sap_xep noi_bot swap giam_dan tang_dan data_payload", 
     "Thuat toan Bubble Sort tren Linked List cua du an khong hoan doi con tro de tranh dut chuoi memory. No giu nguyen khung xuong, chi swap phan Data Payload, luon day may manh nhat len dau."},
    
    {"ma_hoa xor_cipher nhi_phan secret_key security bao_mat data_file", 
     "Du lieu file .dat duoc ma hoa bang XOR Cipher (Key: 0x5A) truc tiep tren RAM. Thuat toan dat toc do O(1). Hacker dung Notepad mo ra chi thay ky tu rac, bao ve tuyet doi IP ha tang."},
    
    {"csv excel xuat_file bao_cao spreadsheet comma_separated", 
     "Tinh nang xuat file dung fopen() voi che do 'w'. Cac truong du lieu cua struct duoc noi bang dau phay ','. Excel hay Google Sheets se tu dong doc dau phay nay de chia cot bao cao."},

    // ================= [ NHÓM 4: HARDWARE, CLOUD & NETWORK ] =================
    {"vram gpu nvidia rtx amd hardware phan_cung card_do_hoa", 
     "VRAM la linh hon cua AI Data Center. He thong NEXTGPU co kha nang do dac va phan phoi VRAM tren moi dong card (tu RTX 3060 den H100) de dam bao chay muot cac mo hinh LLM khong lo."},
    
    {"cpu ram motherboard mainboard ssd hdd storage", 
     "Ben canh GPU, Server con can CPU (xu ly logic) va bo nho luu tru toc do cao (NVMe SSD) de doc file Model AI dung luong hang chuc GB len RAM he thong truoc khi day vao VRAM."},
    
    {"internet ket_noi latency ping websockets api mang_lan icmp", 
     "Toi tich hop mo phong ket noi Internet qua giao thuc ICMP va WebSockets. Toc do phan hoi tren Console dao dong tu 60-150 tokens/s tuy thuoc luong VRAM cap phat, mo phong y het thuc te."},
    
    {"thoi_gian uptime difftime dong_ho calendar time.h", 
     "He thong NEXTGPU dung ham difftime() va thu vien <time.h> de dem thoi gian Uptime hoat dong cua Server tinh tu luc boot chuong trinh tren Dashboard Live Monitor."},
    
    {"cloud aws azure gcp aws_ec2 dam_may vps", 
     "He thong nay mo phong kien truc cua Cloud Computing. Giong nhu viec ban thue the loai may chu EC2 p4d (chua GPU NVIDIA A100) tren AWS de chay he thong tri tue nhan tao."},
    
    {"linux ubuntu centos server os he_dieu_hanh unix bash_shell", 
     "Trong thuc te, hau het cac AI Data Center deu chay tren he dieu hanh Linux (Ubuntu/CentOS). Ma nguon C cua du an nay cung co the port sang Linux rat de dang chi voi vai lenh makefile."},
    
    {"tcp ip udp giao_thuc port socket ban_tin packet", 
     "Giao thuc TCP dam bao goi tin API dieu khien tu Master den Worker khong bi that lac. Con giao thuc UDP thuong duoc dung de stream log metrics vi toc do cuc nhanh nhung co the rot goi."},
    
    {"ssh remote putty terminal truy_cap_tu_xa", 
     "De quan tri cac Worker Nodes o xa, ky su thuong dung giao thuc SSH (Secure Shell) thong qua port 22. Giao thuc nay ma hoa toan bo phien go phim cua ban chong nghe len."},

    // ================= [ NHÓM 5: AI & LARGE LANGUAGE MODELS (LLMs) ] =================
    {"deepseek chatgpt claude gpt llm model chatbot llama", 
     "He thong mo phong kien truc infer cua cac sieu LLM nhu DeepSeek-R1, GPT-4, LLaMA 3. NEXTGPU se kiem tra xem mang luoi co du VRAM de ganh noi hang ty tham so cua chung hay khong."},
    
    {"machine_learning deep_learning neural_network train epoch", 
     "Model LLM duoc huan luyen (Training) tren mang neural network khong lo. Tuy nhien, he thong NEXTGPU cua chung ta tap trung chu yeu vao viec Suy luan (Inference) cho nguoi dung cuoi."},
    
    {"prompt context token context_window fine_tuning", 
     "AI nhan dau vao la cac Prompt. Gioi han cua AI nam o Context Window (so luong Token toi da nhan duoc). Khi vuot qua, AI se 'quen' phan dau cau chuyen."},
    
    {"vector_database nlp_scoring nhan_dien_ngu_nghia tokenizer bag_of_words", 
     "Bot Copilot su dung thuat toan NLP Scoring thu nho. strtok() bam nho cau, sau do cong don diem so (Accumulated Scoring) de tim chu de khop nhat. Do la nguyen ly vector database co ban!"},

    // ================= [ NHÓM 6: KIẾN TRÚC IT CHUYÊN SÂU & DEVOPS ] =================
    {"microservices monolithic architecture kien_truc he_thong", 
     "Du an nay di theo mo hinh Microservices phan tan. Thay vi mot may chu nguyen khoi (Monolithic) cuc ky dat tien, ta ghep nhieu may chu nho, re tien lai qua mang LAN de chia se tai nguyen."},
    
    {"docker container image kubernetes k8s deploy", 
     "De cai dat moi truong AI tren Worker Node ma khong so xung dot thu vien, nguoi ta dung Docker de dong goi, va dung Kubernetes (K8s) de dieu phoi hang ngan container tu dong."},
    
    {"devops ci_cd pipeline jenkins github_actions automation", 
     "Trong ky nguyen hien dai, moi khi toi (tac gia) commit code len GitHub, he thong CI/CD se tu dong bien dich va kiem tra bug giup dam bao phan mem luon o trang thai an toan (Green build)."},
    
    {"nginx haproxy reverse_proxy proxy apache load_balancer", 
     "Trong Web Server, Nginx hoac HAProxy lam nhiem vu can bang tai luu luong truy cap (Request Load Balancer). Con NEXTGPU cua chung ta la can bang tai tai nguyen RAM (Resource Load Balancer)."},
    
    {"grafana prometheus zabbix monitor log_metric", 
     "Giao dien Live Monitor cua NEXTGPU duoc lay cam hung tu cac he thong giam sat chuyen nghiep nhu Grafana va Prometheus. Giup Admin nhin thay suc khoe toan cum bang cac bieu do."},
    
    {"sql database co_so_du_lieu mysql postgresql query", 
     "Co so du lieu quan he (SQL) luu tru du lieu duoi dang bang co cau truc. NEXTGPU cung co the mo rong de ket noi voi MySQL/PostgreSQL thay vi luu tru tren file .dat nhi phan."},
    
    {"nosql redis mongodb cache memcached", 
     "Redis la mot NoSQL Database luu tru truc tiep tren RAM. Thuat toan Dynamic Array Preview cua NEXTGPU co the vi nhu mot dang In-memory Cache cuc nhanh cua Redis."},
    
    {"rest_api json xml http get post payload", 
     "Master Node giao tiep voi Worker Node bang cac goi tin HTTP gui duoi dang JSON. Phuong thuc POST duoc dung de lenh cho GPU nap Model, DELETE dung de huy Model AI."},
    
    {"firewall tuong_lua ddos attack hack vulnerability ddos", 
     "De chong cac cuoc tan cong Tu choi dich vu (DDoS), Data Center can he thong Firewall manh me de loc bot cac goi tin rac truoc khi chung lot vao mang luoi GPU Cluster cua chung ta."},
    
    {"thread multi_threading da_luong dong_bo mutex deadlock", 
     "De tranh Deadlock khi nhieu process cung doi cap phat VRAM tren 1 may chu, can phai su dung Mutex Lock (Khoa dong bo) de xep hang cac tac vu tranh gianh bo nho."},
    
    {"tree graph stack queue cau_truc_du_lieu array", 
     "Ngoai Linked List, cac he thong lon con dung cay nhi phan (Binary Tree) hoac do thi (Graph) de quan ly quan he mang luoi giua cac thiet bi phuc tap hon."},
    
    {"algorithm thuat_toan binary_search dfs bfs quy_hoach_dong", 
     "Thuat toan la cot loi cua Khoa hoc May tinh. Binary Search giup tim kiem O(log n), con BFS/DFS dung de duyet do thi mang luoi cac may chu ve tinh."},
    
    {"html css javascript react vue frontend ui_ux web", 
     "Trong tuong lai, loi C Backend cua NEXTGPU se duoc xay dung them 1 lop API de ket noi voi Giao dien Web viet bang ReactJS/VueJS, giup quan tri vien su dung tren trinh duyet."},
    
    {"backend nodejs python java golang server_side", 
     "Tuy C cuc ky manh me ve xu ly phan cung, nhung de phat trien nhanh cac API web, ky su thuong dung Golang hoac NodeJS de viet lop dieu phoi API trung gian."},
    
    {"git commit push pull merge conflict branch", 
     "Git la cong cu quan ly phien ban ma bat ky ky su nao cung phai nam. Lenh git branch giup phat trien tinh nang moi ma khong lam hong nhanh main. Git merge dung de gop code tro lai."},
    
    {"agile scrum sprint task jira board project_management", 
     "Mo hinh quan ly du an Agile giup doi Nhom IT phan chia du an khong lo thanh cac Sprints ngan 2 tuan. Giup tung buoc hoan thien tung module cua NEXTGPU."},
    
    {"test qa unit_test bug_report automation_test", 
     "De he thong khong bi crash, cac ky su QA can viet cac Unit Test de ban pha thu vao cac ham addServer, deleteServer, thu xem he thong co bat loi vung nho hay khong."},
    
    {"rsa aes hash md5 sha256 cryptography", 
     "Ngoai ma hoa XOR, he thong cong nghiep se dung ma hoa bat doi xung RSA hoac bam SHA-256 de dam bao tuyet mat danh tinh cua tung Worker Node khi tham gia vao mang."},
    
    {"big_data hadoop spark data_warehouse pipeline", 
     "Truoc khi huan luyen AI, nguoi ta phai dung cac he thong Big Data nhu Apache Spark de xu ly loc hang Terabyte du lieu tho (Data Pipeline) lam sach cho AI an."},
    
    {"tuong_lai phat_trien roadmap update phien_ban_moi", 
     "Ban do phat trien (Roadmap) cua NEXTGPU la tro thanh mot Cloud Control Panel thu thiet. Tich hop thu vien libcurl de goi API thuc te, ket noi voi phan cung thuc the qua LAN/Wifi."}
};

int chat_db_size = 50;

void interactWithModel(ServerNode** head_ref) {
    printf("\033[H\033[J");
    printf(CYAN "    +====================================================================================+\n");
    printf("    |                        " MAGENTA BOLD "AI INFERENCE TERMINAL (LOCAL CHAT)" RESET CYAN "                          |\n");
    printf("    +====================================================================================+\n" RESET);

    if (deployed_count == 0) {
        printf(YELLOW "\n    [!] Hien tai chua co Model AI nao duoc trien khai (Deploy) len GPU.\n");
        printf("    Vui long vao Menu so [3] de trien khai Model truoc nhe!\n" RESET);
        return;
    }

    printf(GREEN "\n    [+] DANH SACH MODEL DANG CHAY TREN CLUSTER:\n" RESET);
    for (int i = 0; i < deployed_count; i++) printf("        [%d] %s\n", i + 1, deployed_models[i]);

    int choice = 0;
    printf(BOLD YELLOW "\n    >> Chon Model ban muon ket noi (1-%d): " RESET, deployed_count);
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > deployed_count) {
        while(getchar() != '\n'); printf(RED "    [-] Lua chon khong hop le!\n" RESET); return; 
    }
    while(getchar() != '\n');
    
    char* active_model = deployed_models[choice - 1];
    
    printf("\033[H\033[J");
    printf(MAGENTA "    [*] Dang thiet lap ket noi WebSockets den Mo hinh: %s...\n" RESET, active_model);
    ai_delay(500);
    
    printf(YELLOW "    [*] Fetching API weights tu HuggingFace Cloud: [");
    for(int i=0; i<20; i++) { printf("#"); fflush(stdout); ai_delay(30); }
    printf("] 100%%\n" RESET);
    printf(GREEN "    [+] Da tai xong tri thuc Internet! San sang phan hoi.\n\n" RESET);
    
    char input[300]; char input_lower[300];
    while (1) {
        // UI BOX CUA USER
        printf(CYAN "\n    +--------------------------- [ CAU HOI CUA BAN ] ----------------------------+\n" RESET);
        printf(CYAN "    | " BOLD YELLOW "User: " RESET);
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0;
        printf(CYAN "    +----------------------------------------------------------------------------+\n" RESET);

        if (strlen(input) == 0) continue;
        strcpy(input_lower, input); toLowerCase(input_lower);
        
        if (strstr(input_lower, "thoat") || strstr(input_lower, "exit") || strstr(input_lower, "quit")) {
            printf(YELLOW "    [!] Da ngat ket noi voi Mo hinh.\n" RESET); break;
        }

        ai_think_animation("Dang truy xuat tri thuc mang Internet");
        
        // UI BOX CUA AI (CÓ ĐỘ RỘNG 74 KÝ TỰ BÊN TRONG)
        printf(MAGENTA "\n    +----------------------------- [ TRA LOI TU AI ] ----------------------------+\n" RESET);
        printf(MAGENTA "    | " BOLD "[*] %-69s " RESET MAGENTA "|\n" RESET, active_model);
        printf(MAGENTA "    |----------------------------------------------------------------------------|\n" RESET);
        
        int scores[50] = {0}; 
        char temp_input[300]; strcpy(temp_input, input_lower);
        // Tách từ triệt để hơn bằng cả tab và xuống dòng
        char* word = strtok(temp_input, " ,.?!\n\t"); 
        
        while (word != NULL) {
            // Sửa lại thành > 0 để AI có thể hiểu các từ khóa 1 chữ cái như ngôn ngữ "C"
            if (strlen(word) > 0) { 
                for (int i = 0; i < chat_db_size; i++) { 
                    // [!] KỸ THUẬT EXACT MATCH: Bọc khoảng trắng 2 đầu
                    char padded_keywords[300];
                    char padded_word[100];
                    sprintf(padded_keywords, " %s ", chat_db[i].keywords);
                    sprintf(padded_word, " %s ", word);

                    if (strstr(padded_keywords, padded_word) != NULL) {
                        // Trọng số thông minh (Weighted Score): Từ càng dài, điểm càng cao
                        if (strlen(word) >= 5) scores[i] += 15; 
                        else if (strlen(word) >= 3) scores[i] += 10;
                        else scores[i] += 5;
                    }
                }
            }
            word = strtok(NULL, " ,.?!\n\t");
        }

        int best_match_idx = -1; int max_score = 0;
        for(int i = 0; i < chat_db_size; i++) {
            if(scores[i] > max_score) { max_score = scores[i]; best_match_idx = i; }
        }

        clock_t start_infer = clock();
        
        // [!] NGƯỠNG ĐIỂM (THRESHOLD): Phải đủ điểm mới trả lời, chống đoán mò
        if (best_match_idx != -1 && max_score >= 10) { 
            typewriter_box(chat_db[best_match_idx].answer, MAGENTA, 74);
        } else {
            typewriter_box("Xin lỗi, tôi chưa hiểu rõ ý bạn. Hãy thử dùng các từ khóa kỹ thuật cụ thể hơn (VD: vram, thuat toan, github, ma hoa, can bang tai).", MAGENTA, 74);
        }
        
        clock_t end_infer = clock();
        double time_taken = ((double)(end_infer - start_infer)) / CLOCKS_PER_SEC;
        if(time_taken < 0.1) time_taken = (rand()%20 + 10) / 100.0; 
        
        printf(MAGENTA "    +----------------------------------------------------------------------------+\n" RESET);
        printf(WHITE "    [System] Inference Speed: %.2f tokens/s | Latency: %.2fs\n" RESET, (rand()%20)+80.0, time_taken);
    }
}

// =====================================================================
// CON AI SO 2: KNOWLEDGE DATA SEGMENT (MANG KHONG LO CHO COPILOT ADMIN)
// =====================================================================
Knowledge copilot_db[] = {
    // ================= [ NHÓM 1: CÁC HÀM QUẢN LÝ NODE (CRUD) ] =================
    {"addserver add_server them_may_chu strcpy provision", 
     "=== KIEN TRUC HAM void addServer() ===\n- Cap phat dong: Dung malloc(sizeof(ServerNode)) de mo vung RAM tren Heap.\n- Copy chuoi an toan: Dung strcpy() de gan Model va API Endpoint.\n- Tail Insertion: Vong lap while(temp->next != NULL) di tim diem cuoi cung cua danh sach roi be nhanh con tro next de noi may chu moi vao mang luoi."},
    
    {"deleteserver delete_server xoa_may_chu deprovision free prev", 
     "=== KIEN TRUC HAM void deleteServer() ===\n- Thuat toan: Dung 2 con tro song hanh (temp di truoc, prev di sau).\n- Logic be xich: prev->next = temp->next giup nhay coc qua node bi xoa, giu lien ket mang.\n- Memory: Goi lenh free(temp) de tra RAM cho OS, triet tieu hoan toan Memory Leak!"},
    
    {"displayservers display_servers live_monitor in_bang format_string", 
     "=== KIEN TRUC HAM void displayServers() ===\n- Hien thi bang: Dung format chuoi '%-20s' de ep chuoi can trai thang tap.\n- Ve bieu do VRAM: Ap dung cong thuc (vram_used * 100 / vram_total) nhan cheo voi chieu dai max_bar (10) de ve ra cac dau '#' truc quan, the hien tai nguyen hien tai."},
    
    {"sortserversbyvram sort_servers bubble_sort sap_xep hoan_doi", 
     "=== KIEN TRUC HAM void sortServersByVRAM() ===\n- Thuat toan: Bubble Sort (Noi bot) giup day cac may chu VRAM khung nhat len dau.\n- An toan con tro: Khong doi vi tri con tro (pointer swap) de tranh dut xich. Ham chi lay cac bien 'temp_model', 'temp_vtotal' hoan doi Data Payload ben trong cac Node voi nhau."},
    
    {"toggleserverstatus toggle_status bao_tri online offline toan_tu_ba_ngoi", 
     "=== KIEN TRUC HAM void toggleServerStatus() ===\n- Cong tac bit: Thay vi if-else, ham dung toan tu ba ngoi 'temp->status = (temp->status == 1) ? 0 : 1;' de lat trang thai.\n- Tac dung: Node bi Offline se lap tuc bi thuat toan Cân Bằng Tải bo qua."},
    
    {"freelist free_list don_rac memory_leak dọn_dẹp exit", 
     "=== KIEN TRUC HAM void freeList() ===\n- Kich hoat: Khi user thoat chuong trinh.\n- Thuat toan: Vong lap quet, luu node hien tai vao 'temp', day 'head' len node ke tiep, roi moi free(temp). Tuyet doi khong free() truoc khi day con tro de tranh mat dau day chuyen."},

    // ================= [ NHÓM 2: CÁC HÀM AI & TENSOR PARALLELISM ] =================
    {"deployaimodel deploy_ai can_bang_tai tensor_parallelism preview", 
     "=== KIEN TRUC HAM void deployAIModel() ===\n- Phan tan AI: Quet mang, neu 1 may khong ganh noi, he thong se be nho AI.\n- Safe Commit: Dung malloc/calloc tao 3 mang ao (p_nodes, p_alloc, p_free) de chay nhap Preview. Chi khi Admin nhan so 1 (Confirm), data moi duoc map vao RAM chinh thuc. Chuan kien truc Enterprise!"},
    
    {"undeployaimodel undeploy_ai go_bo_ai giai_phong_vram thu_hoi", 
     "=== KIEN TRUC HAM void undeployAIModel() ===\n- Logic: Duyet qua cac node, kiem tra (vram_used > 0) thi ep temp->vram_used = 0.\n- Tac dung: Zero-downtime, gop phan tra lai 100% trang thai ranh roi cho GPU ma khong can phai reset may chu."},
    
    {"interactwithmodel interact chatbot nlp scoring_logic intent", 
     "=== KIEN TRUC HAM void interactWithModel() ===\n- NLP Lõi: Bam input bang strtok(), quet mang chat_db[]. Nhuom diem (scores[i]++) cho keyword trung khop bang strstr().\n- Xuat: Tra ve cau tra loi cua chu de dat Max Score qua ham typewriter, mo phong kien truc Vector Database."},
    
    {"interactwithcopilot copilot assistant chuyen_gia_code bách_khoa", 
     "=== KIEN TRUC HAM void interactWithCopilot() ===\n- Day chinh la bo nao cua toi! \n- Su dung thuat toan Exact Match: Boc khoang trang 2 dau chuoi de tranh nhan dien sai tu khoa. Uu tien trong so (Weighted Score) cao cho cac Tu khoa dai la ten Ham."},
    
    {"typewriter_box typewriter word_wrap khung_chat delay putchar", 
     "=== KIEN TRUC HAM void typewriter_box() ===\n- Thuat toan Word-Wrap: Tinh truoc do dai cua tu (word_len). Neu chieu dai dong vuot qua bien do 74, tu dong in '\\n' va ve them vach '|' de chong vo khung giao dien.\n- Hieu ung: Dung putchar() + delay() tao cam giac AI dang render chu."},

    // ================= [ NHÓM 3: BẢO MẬT & FILE I/O ] =================
    {"encryptdecrypt encrypt decrypt xor_cipher ma_hoa binary", 
     "=== KIEN TRUC HAM void encryptDecrypt() ===\n- Phep toan Bitwise: data[i] ^= SECRET_KEY (0x5A).\n- Tinh doi xung: Phep toan nay khong can ham giai ma rieng. Chay lan 1 la ma hoa (bien thanh rac), chay lan 2 tren mang rac se tra ve data chuan. Toc do sieu toc O(1)."},
    
    {"savetofile save_to_file luu_file fwrite wb binary_write", 
     "=== KIEN TRUC HAM void saveToFile() ===\n- Ghi Nhi phan: Mo file che do 'wb'.\n- Truoc khi ghi struct bang fwrite(), he thong goi ham encryptDecrypt() de xao bit nham bao ve IP Data Center, sau do moi luu xuong o cung. OS se khong the doc duoc text."},
    
    {"loadfromfile load_from_file doc_file fread rb binary_read", 
     "=== KIEN TRUC HAM void loadFromFile() ===\n- Doc Nhi phan: Mo file che do 'rb'. \n- Kien truc an toan: Khong ep kieu con tro truc tiep vao RAM ma dung bien 'ServerNode tempNode' doc do roi goi ham addServer() nap lai data vao Linked List mot cach sach se."},
    
    {"exporttocsv export_csv xuat_excel baocao_gpu fprintf csv", 
     "=== KIEN TRUC HAM void exportToCSV() ===\n- Xuat van ban: Mo che do 'w' tao file .csv.\n- Dinh dang: Dung fprintf de noi cac truong data bang dau phay (comma-separated). Hoan hao de import vao cac phan mem nhu Excel/PowerBI ke ve bieu do."},

    // ================= [ NHÓM 4: CHIẾN LƯỢC TENSOR (ALGORITHMS) ] =================
    {"greedy_algorithm greedy tham_lam binpacking don_tai", 
     "=== THUAT TOAN TENSOR: GREEDY ===\n- Nhoi can kiet VRAM cua may dang xet cho den khi dat 100% moi tran qua may tiep theo. Day la kieu Bin-packing giup gom tai nguyen, tiet kiem dien nang Data Center."},
    
    {"balanced_algorithm balanced round_robin san_tai thermal", 
     "=== THUAT TOAN TENSOR: BALANCED ===\n- Ung dung Round-Robin: Moi vong lap chi chia dung 1GB VRAM cho moi Node, xoay vong khong ngung. Muc dich la tan nhiet deu tren tat ca phan cung (Anti-Thermal Throttling)."},
    
    {"custom_algorithm custom thu_cong tay_bo_ram allocation", 
     "=== THUAT TOAN TENSOR: CUSTOM ===\n- Vong lap tuong tac mo: Cho phep Admin go so RAM muon cap cho tung may chu (toi da bang VRAM Free cua may do) cho den khi du tai. Dung cho ha tang thiet bi khong dong bo (May yeu may manh)."},

    // ================= [ NHÓM 5: UI/UX & SYSTEM KERNEL ] =================
    {"showbootscreen boot_screen hieu_ung khoi_dong quat_quay", 
     "=== KIEN TRUC HAM void showBootScreen() ===\n- Animation Console: Vong lap quet (i%4) de chon frame cua canh quat GPU lenh goc. Lenh ANSI '\\033[H' de dời con trỏ ve goc ma khong xoa man hinh, tao hieu ung quat dang quay tren man hinh thue cua hacker!"},
    
    {"interactivemenu interactive_menu tui menu _getch phim_mui_ten", 
     "=== KIEN TRUC HAM int interactiveMenu() ===\n- Dung _getch() lang nghe keyboard khong hien thi echo. Phim mui ten tra ve 2 byte (224, sau do 72 Up/80 Down). Tinh toan bien 'selected' ket hop ma doi mau de highlight dong Menu."},
    
    {"printheader print_header ascii_art logo tieu_de", 
     "=== KIEN TRUC HAM void printHeader() ===\n- Chuc nang in Logo NEXTGPU PRO bang ma ASCII Art tao diem nhan thi giac cho giao dien TUI, dong thoi set font mau Cyan (Xanh ngoc) chuan Cyberpunk."},
    
    {"main while_loop core _kbhit non_blocking", 
     "=== KIEN TRUC HAM main() ===\n- Core cua toan bo phan mem. Chay vong lap vo han while(1). Dac biet khi vao Menu 1 (Live Monitor), dung _kbhit() de duy tri vong lap in thong so trong khi van cho phep thoat ra khi nhan phim bat ky."},

    // ================= [ NHÓM 6: GIẢI MÃ CÁC LỆNH/TỪ KHÓA C CẤP THẤP ] =================
    {"malloc memory_allocation dong cap_phat heap", 
     "=== TU KHOA C: malloc() ===\n- Memory Allocation: Xin OS cap cho mot vung RAM tren Heap. Khac voi array tinh, malloc giup mang luoi Server mo rong toi vo han vao thoi diem Runtime ma khong so tran bo nho stack."},
    
    {"calloc clear_allocation mang_dong cap_phat", 
     "=== TU KHOA C: calloc() ===\n- Tuong tu malloc nhung calloc se tu dong don sach vung nho va set gia tri ve 0. Trong NEXTGPU, calloc duoc dung de tao mang 'p_alloc' ghi nho tam thoi VRAM phan bo cua Tensor Parallelism."},
    
    {"free giai_phong_ram clear_memory memory_leak", 
     "=== TU KHOA C: free() ===\n- Mua hang thi phai tra tien, malloc roi thi phai free(). Xoa vung nho do malloc tao ra, neu khong the tich RAM se tang dan gay tran RAM OS (Memory Leak)."},
    
    {"struct servernode typedef cau_truc_du_lieu", 
     "=== TU KHOA C: struct ServerNode ===\n- Day la vien gach xay dung he thong. No gom: id, status, model, vram_total, vram_used, api_endpoint va dac biet la '*next' (con tro lien ket) de moc noi voi vien gach ke tiep."},
    
    {"_kbhit keyboard_hit lang_nghe asyn input", 
     "=== TU KHOA C: _kbhit() ===\n- Kiem tra xem co phim nao dang doi trong bo dem ban phim khong. Tra ve 1 neu co. Giup chuong trinh C khong bi 'dung hinh' cho nhap lieu nhu scanf, tao tien de lam Live Dashboard."},
    
    {"_getch get_char no_echo an_ky_tu", 
     "=== TU KHOA C: _getch() ===\n- Doc ngay 1 ky tu tu ban phim ma khong can an Enter, cung KHONG in ky tu do ra man hinh (No-echo). Hoan hao de xay dung menu dieu huong bang mui ten."},
    
    {"fflush stdout flush clear_buffer rendering", 
     "=== TU KHOA C: fflush(stdout) ===\n- He dieu hanh thuong luu chu trong Buffer truoc khi in ra man hinh. fflush buoc OS phai 'non' toan bo chu do ra man hinh ngay lap tuc. Giup UI khong bi giat hoac bi pending text."},
    
    {"ansi_escape ansi \\033[h \\033[j clear_screen system_cls", 
     "=== MA DIEU KHIEN: ANSI ESCAPE ===\n- '\\033[H' dua con tro len dong (0,0).\n- '\\033[J' xoa tu con tro xuong duoi.\n- Chung loai bo lenh system('cls') gay soc I/O va giat man hinh (Flickering), day TUI len canh gioi 60FPS."},
    
    {"strcpy strncpy string_copy sao_chep_chuoi", 
     "=== TU KHOA C: strcpy() ===\n- String Copy: Chep chuoi tu src sang dest. Kien truc C khong the gan chuoi bang dau '='. Ta dung strcpy de copy ten Model AI tu bien tam vao ben trong Struct cua Linked List."},
    
    {"sprintf snprintf format string format_string", 
     "=== TU KHOA C: sprintf() ===\n- String Print Format: Thay vi in ra man hinh, no the ghep cac bien vao 1 doan Text roi luu doan text do vao 1 chuoi (Array char). NEXTGPU dung no de sinh Auto IP: sprintf(api, \"http://192.168.1.%d\", id);"},
    
    {"strstr string_string tim_chuoi search", 
     "=== TU KHOA C: strstr() ===\n- Tim chuoi con nam trong chuoi me. Day la loi cua Động cơ NLP cham diem. No quet xem tu khoa cua User co nam ben trong database khong. De lam NLP chuan, ta thuong phai boc them khoang trang 2 dau."},
    
    {"strtok string_token bam_chuoi tokenizer split", 
     "=== TU KHOA C: strtok() ===\n- String Tokenizer: Con dao thai thit! No bam mot cau van dai thanh cac tu don (Token) dua tren cac ky tu phan cach (Space, dau phay, dau cham). Kien truc cot loi cua Trí tuệ nhân tạo mảng NLP."},
    
    {"fopen fclose doc_file luu_file io", 
     "=== TU KHOA C: fopen() ===\n- File Open: Mo luong (Stream) ket noi giua RAM va O cung (Disk). Mo xong bat buoc phai dung fclose() de dong lai, neu khong du lieu se nam vinh vien trong the kẹt."},
    
    {"rb wb w che_do_file file_mode", 
     "=== TU KHOA C: File Modes ===\n- 'rb': Read Binary (Doc nhi phan)\n- 'wb': Write Binary (Ghi nhi phan)\n- 'w': Write Text (Ghi van ban thong thuong, dung cho xuat file CSV)."},
    
    {"fread fwrite doc_nhi_phan ghi_nhi_phan", 
     "=== TU KHOA C: fread() & fwrite() ===\n- Luu hoac doc truc tiep mot Block Memory (VD: Toan bo cai struct) xuong o cung ma khong can chuyen the sang chuoi Text. Toc do kinh hoang, bao mat cao!"},
    
    {"fprintf file_print xuat_file in_file csv", 
     "=== TU KHOA C: fprintf() ===\n- Giong chuc nang printf (in ra man hinh), nhung thay vi man hinh, no ban chuoi format do xuyen truc tiep vao trong File Text tren o cung."},
    
    {"difftime time time_h tinh_gio uptime", 
     "=== TU KHOA C: difftime() ===\n- Tru thoi gian. Tru di thoi diem (start_time) luc khoi dong chuong trinh so voi thoi diem hien tai de ra duoc so giay Uptime tren bang dieu khien cua Data Center."},
    
    {"clock clock_t delay mili_giay sleep", 
     "=== TU KHOA C: clock() ===\n- Dem so xung nhiem CPU (Clock ticks). Ham delay() cua he thong dung vong lap while kiem tra clock() nham treo CPU mot khoang mili-giay, giup mo phong hieu ung nhả chu hoac Loading Bar."},

    // ================= [ NHÓM 7: KIẾN TRÚC IT VÀ KHÁI NIỆM ] =================
    {"memory_leak tran_bo_nho ro_ri mem_leak", 
     "=== KHAI NIEM: Memory Leak ===\n- Loi nguy hiem nhat trong C/C++. La hien tuong cap phat dong nhung quen giai phong. Server the chay thieu RAM dan roi BSoD. NEXTGPU giai quyet bang lenh freeList() chat che!"},
    
    {"buffer_overflow tran_bo_dem array_out_of_bounds", 
     "=== KHAI NIEM: Buffer Overflow ===\n- Ban khai bao mang 100 ky tu nhung luu 200 ky tu. Du lieu se tran vao vung RAM khac gay crash hoac tao lo hong de Hacker tan cong (Smashing the stack)."},
    
    {"pointer con_tro dereference dia_chi reference", 
     "=== KHAI NIEM: Pointer (*) ===\n- Kien truc sieu pham cua C. No khong luu data, no luu 'Dia chi' cua data tren RAM. Thay vi copy mot thu muc 10GB, ta chi can chi cho OS biet thu muc do nam o dau (truyen tham chieu)."},
    
    {"github link source_code ma_nguon repo repository author tac_gia hoquan2007", 
     "=== AUTHOR INFO & OPEN SOURCE ===\n- Kien truc ma nguon cua NEXTGPU PRO thuoc ve nha phat trien hoquan2007.\n- Toan bo du an duoc minh bach hoa (Open Source). Ban co the kiem chung tung dong code C va xem cac commit tai Repository chinh thuc: https://github.com/hoquan2007/GPU-Server-Manager"},
    {"two_pointer hai_con_tro song_hanh prev_temp", 
     "=== THUAT TOAN: Two-Pointer ===\n- Giai thuat dung 2 con tro duyet mang/chuoi. Trong xoa Node, 'temp' chay ra phia truoc tim muc tieu, 'prev' dung lai phia sau de cho san noi day xich giup Node khong bi đứt."},
    
    {"color mau_sac ansi_color cyan magenta", 
     "=== UI: ANSI Colors ===\n- He thong ho_tro render the he mau cua Cyberpunk (Cyan, Magenta, Yellow, Green, Red) qua cac ma Hex. Giup giam bot su kho khan cua Terminal truyen thong."},
    
    {"zero_downtime tinh_san_sang failover kien_truc", 
     "=== KHAI NIEM: Zero-Downtime ===\n- Toan bo ham xoa, bao tri hay gỡ bo AI cua NEXTGPU duoc thiet ke de khong phai Stop cac may khac. He thong luon tra ve trang thai song 24/7."},
     {"ai_tao_ra tao_ra_ban phat_trien nhom_it_utc2 dev coder tac_gia sinh_vien", 
     "=== TEAM IT UTC2 ===\n- Toi duoc thiet ke, xay dung va toi uu hoa kien truc boi nhom IT UTC2.\n- Nhom phat trien gom 4 thanh vien:\n  1. Ho Ngoc Quan\n  2. Nguyen Thanh Phong\n  3. Nguyen Thanh Phong\n  4. Nguyen Giang Tuong"}
};

int copilot_db_size = 51;

void nextgpuCopilot(ServerNode** head_ref) {
    char input[300]; char input_lower[300];
    printf("\033[H\033[J");
    printf(CYAN "    +====================================================================================+\n");
    printf("    |                       " MAGENTA BOLD "NEXTGPU COPILOT (CLI AI ASSISTANT)" RESET CYAN "                           |\n");
    printf("    +====================================================================================+\n" RESET);
    printf(GREEN "    [o_o] Xin chao! Toi la AI dieu hanh kien truc backend he thong NEXTGPU.\n");
    printf("    [o_o] Toi nam giu toan bo 'Bach khoa toan thu' giai thich moi ham, thuat toan trong code.\n");
    printf("    [o_o] Hay go cau hoi (Vi du: 'giai thich ham addserver', 'thuat toan xor', hoac 'thoat').\n" RESET);
    printf(CYAN "    --------------------------------------------------------------------------------------\n" RESET);

    while (1) {
        printf(CYAN "\n    +--------------------------- [ CAU HOI CUA BAN ] ----------------------------+\n" RESET);
        printf(CYAN "    | " BOLD YELLOW "User: " RESET);
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0; strcpy(input_lower, input); toLowerCase(input_lower);
        printf(CYAN "    +----------------------------------------------------------------------------+\n" RESET);
        
        if (strlen(input_lower) == 0) continue;
        
        ai_think_animation("Dang quet he thong");
        
        printf(MAGENTA "\n    +-------------------------- [ HE THONG TRA LOI ] ----------------------------+\n" RESET);
        printf(MAGENTA "    | " BOLD "[*] Copilot:                                                               " RESET MAGENTA "|\n" RESET);
        printf(MAGENTA "    |----------------------------------------------------------------------------|\n" RESET);

        // --- CÁC TÍNH NĂNG THỰC THI LỆNH ---
        if (strstr(input_lower, "thoat") || strstr(input_lower, "exit") || strstr(input_lower, "quit")) {
            typewriter_box("Da ngat phien lam viec voi tro ly Copilot. See you again !! !", MAGENTA, 74); 
            printf(MAGENTA "    +----------------------------------------------------------------------------+\n" RESET);
            break;
        }
        else if ((strstr(input_lower, "them") || strstr(input_lower, "add")) && (strstr(input_lower, "may") || strstr(input_lower, "server"))) {
            typewriter_box("Toi se giup ban nap thiet bi moi vao Data Center. Vui long cung cap thong so:", MAGENTA, 74);
            printf(MAGENTA "    +----------------------------------------------------------------------------+\n" RESET);
            int id, vtotal; char model[50], endpoint[100];
            printf(WHITE "\n      - Nhap ID may chu (So Nguyen): " RESET); 
            if (scanf("%d", &id) != 1) { while(getchar() != '\n'); printf(RED "      -> Loi ky tu! Ban phai nhap so. Da huy thao tac.\n" RESET); }
            else {
                while(getchar() != '\n'); 
                if (checkIdExists(*head_ref, id)) { printf(YELLOW "      -> Canh bao: ID nay da ton tai trong co so du lieu roi!\n" RESET); }
                else {
                    printf(WHITE "      - Nhap Model GPU (VD: RTX_4090): " RESET); scanf(" %[^\n]", model); while(getchar() != '\n');
                    printf(WHITE "      - Nhap Tong VRAM (GB): " RESET); 
                    if (scanf("%d", &vtotal) != 1) { while(getchar() != '\n'); printf(RED "      -> Loi ky tu! Da huy thao tac.\n" RESET); }
                    else {
                        while(getchar() != '\n');
                        printf(WHITE "      - Nhap API Endpoint: " RESET); scanf(" %[^\n]", endpoint); while(getchar() != '\n');
                        addServer(head_ref, id, model, vtotal, endpoint);
                        printf(GREEN "      -> Hoan tat! May chu da duoc dau noi vao mang luoi thanh cong.\n" RESET);
                    }
                }
            }
        }
        else if (strstr(input_lower, "trien khai") || strstr(input_lower, "deploy") || strstr(input_lower, "can bang tai") || strstr(input_lower, "chay model")) {
            typewriter_box("He thong Load Balancing da san sang nhan lenh. Vui long nhap thong tin Model AI:", MAGENTA, 74);
            printf(MAGENTA "    +----------------------------------------------------------------------------+\n" RESET);
            char ai_name[50]; int req_vram;
            printf(WHITE "\n      - Nhap Ten Model (VD: Claude_3_Opus): " RESET); scanf(" %[^\n]", ai_name); while(getchar() != '\n');
            printf(WHITE "      - Nhap muc tieu thu VRAM (GB): " RESET); 
            if (scanf("%d", &req_vram) != 1) { while(getchar() != '\n'); printf(RED "      -> Loi ky tu! Da huy thao tac.\n" RESET); }
            else {
                while(getchar() != '\n');
                deployAIModel(*head_ref, ai_name, req_vram);
            }
        }
        else if ((strstr(input_lower, "xoa") || strstr(input_lower, "delete")) && (strstr(input_lower, "may") || strstr(input_lower, "server"))) {
            typewriter_box("De an toan, vui long cho toi biet ID cua may chu ban muon go bo:", MAGENTA, 74);
            printf(MAGENTA "    +----------------------------------------------------------------------------+\n" RESET);
            int del_id; printf(WHITE "\n      - Nhap ID can xoa: " RESET);
            if (scanf("%d", &del_id) != 1) { while(getchar() != '\n'); printf(RED "      -> Loi ky tu! Da huy thao tac.\n" RESET); }
            else {
                while(getchar() != '\n');
                deleteServer(head_ref, del_id);
            }
        }
        else if (strstr(input_lower, "sap xep") || strstr(input_lower, "sort") || strstr(input_lower, "toi uu")) {
            sortServersByVRAM(*head_ref);
            typewriter_box("Toi da chay thuat toan Bubble Sort. Tat ca Server hien da duoc sap xep uu tien theo muc VRAM giam dan.", MAGENTA, 74);
            printf(MAGENTA "    +----------------------------------------------------------------------------+\n" RESET);
        }
        else {
            int scores[50] = {0}; 
        char temp_input[300]; 
        strcpy(temp_input, input_lower);
        
        // Băm nhỏ câu hỏi của người dùng
        char* word = strtok(temp_input, " ,.?!\n\t"); 
        
        while (word != NULL) {
            if (strlen(word) > 0) { 
                for (int i = 0; i < copilot_db_size; i++) { 
                    // [!] KỸ THUẬT EXACT MATCH: Bọc khoảng trắng để tìm chính xác hàm/từ khóa
                    char padded_keywords[500];
                    char padded_word[100];
                    sprintf(padded_keywords, " %s ", copilot_db[i].keywords);
                    sprintf(padded_word, " %s ", word);

                    if (strstr(padded_keywords, padded_word) != NULL) {
                        // Trọng số (Weighted Score): Ưu tiên các tên hàm/biến dài
                        if (strlen(word) >= 8) scores[i] += 20;      // VD: addServer, undeployAIModel
                        else if (strlen(word) >= 5) scores[i] += 15; // VD: malloc, vram
                        else if (strlen(word) >= 3) scores[i] += 10; // VD: ram, gpu
                        else scores[i] += 5;                         // VD: c, if
                    }
                }
            }
            word = strtok(NULL, " ,.?!\n\t");
        }

        // Tìm chủ đề (hàm/tính năng) có điểm cao nhất
        int best_match_idx = -1; 
        int max_score = 0;
        for(int i = 0; i < copilot_db_size; i++) {
            if(scores[i] > max_score) { 
                max_score = scores[i]; 
                best_match_idx = i; 
            }
        }

        clock_t start_infer = clock();
        
        // [!] NGƯỠNG ĐIỂM (THRESHOLD): Copilot cần độ chính xác cao hơn, set ngưỡng là 10
        if (best_match_idx != -1 && max_score >= 10) { 
            // Dùng màu CYAN (Xanh ngọc) để phân biệt Copilot với Chatbot (Màu Tím)
            typewriter_box(copilot_db[best_match_idx].answer, CYAN, 74);
        } else {
            // Câu từ chối (Fallback) mang đậm chất Kỹ thuật
            typewriter_box("NEXTGPU Copilot: Toi chua tim thay ham hoac thuat toan ban dang hoi. Vui long nhap chinh xac ten ham (VD: addServer, deleteServer, malloc) hoac ten tinh nang de toi giai thich chi tiet ma nguon.", CYAN, 74);
        }
            printf(MAGENTA "    +----------------------------------------------------------------------------+\n" RESET);
        }
    }
}

void freeList(ServerNode** head) {
    ServerNode* current = *head;
    while (current != NULL) { ServerNode* temp = current; current = current->next; free(temp); }
    *head = NULL;
}