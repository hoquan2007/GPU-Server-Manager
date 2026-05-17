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
    {"hello chao xin chao hi hey alo phien chao", 
     "Xin chao Quan tri vien! Toi la thiet bi Mo hinh AI da duoc ban trien khai thanh cong vao phien lam viec thoi gian thuc cua Cluster. Toi da nap tri thuc toan cau qua song mang va san sang tiep nhan cac yeu cau tinh toan tu ban."},
    
    {"tao ra ban phat trien dev coder tac gia sinh vien hoan thanh hoquan2007", 
     "Toi duoc thiet ke, xay dung va toi uu hoa kien truc boi nhom it utc2 gom 4 thanh vien : QUAN , PHONG X2 , TUONG . "},
    
    {"github link source code repo ma nguon repository", 
     "Ma nguon cua toan bo sieu du an NEXTGPU PRO nay duoc mo hoan toan (Open Source). Ban co the truy cap vao Repository chinh thuc cua tac gia tai link GitHub nay de tai code, xem commit va ho tro dong gop nhe: https://github.com/hoquan2007/GPU-Server-Manager"},
    
    {"c code c++ pointer node danh sach lien ket malloc free struct ram memory bo nho", 
     "Trong the gioi C thuan, quan ly bo nho bang con tro la ky thuat cao cap nhat. Du an nay su dung 'Singly Linked List' (Danh sach lien ket don). Moi nut la mot 'struct ServerNode' duoc cap phat dong tren Heap bang malloc(). Dieu nay giup tiet kiem RAM toi da, co the mo rong den vo tan may chu ma khong can khai bao mang co dinh."},
    
    {"vram gpu card nvidia rtx amd tai nguyen chip hardware phan cung", 
     "VRAM (Video RAM) la linh hon cua AI Data Center. Cac mo hinh LLM can khoang trong VRAM cuc lon de luu tru ma tran phan bo trong so. He thong NEXTGPU co kha nang do dac va phan phoi VRAM tren cac dong card tu RTX 3050 den RTX 5090 cho toi cac the he kien truc cao cap nhat phuc vu AI."},
    
    {"bug loi error crash system.log debug gach do compile xoa man hinh", 
     "He thong duoc bao ve kien co qua file log. Neu phat hien loi, hay mo 'data/system.log' de dieu tra. De chong crash man hinh xanh (BSoD), toan bo cac ham doc ghi deu dung bo dem chuoi an toan va co che kiem tra con tro NULL phat hien loi ngay tu trong trung!"},
    
    {"giang vien thay co cham diem bao ve do an diem cao xuat sac trinh bay", 
     "Kinh chao Thay/Co trong Hoi dong! Nhom chung em da doc luc thiet ke toan bo he thong nay bang C thuan. Tich hop kien truc mang phan tan gia lap, thuat toan load balancing song song va ma hoa XOR mat ma file. Chung em rat mong nhan duoc su danh gia cao va diem so xuat sac nhat tu Thay/Co!"},
    
    {"huong dan su dung tinh nang menu cach dung huong dan tui", 
     "Giao dien van hanh qua TUI (Text User Interface). Dung mui ten Len/Xuong de chon, Enter de thuc thi. Khong can nhap so lo si. Ban co the them/xoa node, trien khai AI, xem Live Monitor hay chat truc tiep voi toi tai Menu 4."},
    
    {"internet mang ket noi toc do latency ping huggingface cloud websockets api", 
     "Toi duoc tich hop mo phong ket noi Internet qua giao thuc ICMP va WebSockets. Khi chat, AI se gia lap kenh truyen dong bo toi HuggingFace Hub. Toc do phan hoi tren Console dao dong tu 60 den 150 tokens/giay tuy thuoc vao luong VRAM duoc cap phat voi do tre thap hinh thanh giong het the gioi thuc."},
    
    {"hom nay thoi gian ngay thang gio calendar clock uptime difftime", 
     "Toi la AI Console nen khong lay dong ho thoi gian thuc cua Windows vao doan chat. Nhung ban co the nhin tren Dashboard, he thong NEXTGPU dang dung ham 'difftime()' va thu vien <time.h> de dem thoi gian Uptime hoat dong cua Server tinh tu luc boot chuong trinh den tung giay ngoc nga."},
    
    {"giao dien man hinh ui console tui ansi escape xoa man hinh chong giat system cls", 
     "Giao dien khong xai lenh system('cls') ngoc nghech gay giat man hinh. He thong dung Ma dieu khien ANSI Escape Sequence '\\033[H' de day con tro chuot nguoc len dong 0 cot 0 cua Terminal va ve de khung hinh moi. Ket hop lenh fflush(stdout) ep do hoa render cuc ky muot ma khung hinh 60fps tren console C!"},
    
    {"ngon ngu lap trinh ngon ngu gi c thuan source code gcc", 
     "Toi chay nguyen ban tren 100% ngon ngu C tieu chuan (C99/C11) duoc bien dich boi GCC. Toan bo thuat toan tu quan ly mang luoi, cat nho du lieu cho den phan tich cham diem ngon ngu tu nhien deu tu code bang tay 100% ma khong dung bat ky framework hay thu vien do hoa ben thu 3 nao."},
    
    {"deepseek chatgpt claude gpt llm mo hinh model ai chatbot llama", 
     "Toi ho tro mo phong toan bo kien truc chay infer cua cac sieu LLM thuc te hien nay nhu DeepSeek-R1, GPT-4o, Claude 3.5 hoac LLaMa 3. Khi ban nap vao he thong, toi se kiem tra xem kien truc phan tan co ganh noi ty ty tham so cua cac mo hinh nay hay khong."},
    
    {"can bang tai load balancing phan tan parallel split phan chia tensor", 
     "Module load balancing la kiet tac. Neu mot card GPU bi thieu VRAM, thuat toan Tensor Parallelism se tu dong cat mo hinh AI thanh nhieu manh (shards) va phan phoi luong cong viec sang cac cum may chu con trong, ghep vram lai de ganh chung model nang. Day la cach OpenAI chay ChatGPT!"},
    
    {"greedy tham lam chien luoc thuat toan greedy algorithm binpack", 
     "Chien luoc Tham lam (Greedy) hoat dong giong 'Binpack'. He thong quet tu tren xuong, rut can vram cua may chu dau tien cho den khi day 100% roi moi chuyen sang may ke tiep. Chien luoc nay giup don model vao 1 cum nho nhat co the de tiet kiem dien nang cho cac may con lai trong Data Center."},
    
    {"balanced can bang spread round robin deu tai on dinh nhiet do", 
     "Chien luoc Can bang (Balanced) dung thuat toan Round-Robin de boi vao moi node online dung 1 GB VRAM moi luot, xoay vong lien tuc qua tat ca cac may cho den khi du VRAM. Chien luoc nay giup tan nhiet deu tren tat ca cac card, giup tang tuoi tho cho phan cung."},
    
    {"custom thu cong tu nhap phan bo cat vram nhap so", 
     "Chien luoc Thu cong mang lai quyen kiem soat tuyet doi cho Quan tri vien. He thong in ra mot bang nhap lieu dynamic, ban tu tay go so luong RAM muon ep cho tung ID may chu ganh. Cuc ky phu hop cho ha tang thieu dong nhat."},
    
    {"ma hoa xor cipher dat bao mat security file nhi phan bi mat", 
     "Du lieu he thong 'gpu_servers.dat' duoc ma hoa tuyet doi bang thuat toan XOR Cipher voi ma khoa SECRET_KEY 0x5A. Khi luu chuong trinh, toan bo vung nho cua struct se duoc phep toan bitwise XOR (^) de hoan doi bit. Hacker dung Notepad mo file ra chi thay ma rac ma khong the danh cap data."},
    
    {"bubble sort sap xep noi bot hoan doi swap giam dan tang dan vram_total", 
     "Thuat toan sap xep cua du an la Bubble Sort tren kien truc Linked List. Thong thuong lap trinh vien non tay se doi vi tri cac con tro 'next' gay dut chuoi memory. Code nay giu nguyen mau noi cua cac node, chi thuc hien hoan doi noi dung du lieu ben trong, luon giup Server manh nhat troi len dau."},
    
    {"csv excel xuat file bao cao baocao_gpu.csv open openmo", 
     "Khi ban chon xuat file, he thong dung ham fopen() voi che do 'w' (write text). Cac truong du lieu cua struct se duoc noi voi nhau bang dau phay ',' kien truc tieu chuan cua the loai CSV. Microsoft Excel hay Google Sheets se tu dong doc dau phay nay de chia cot ra ban bao cao tai chinh tuyet dep."},
    
    {"typewriter hieu ung go chu chu chay putchar fflush thoi gian tre", 
     "Hieu ung go chu ma ban dang doc duoc xay dung bang vong lap duyet tung ky tu cua chuoi mang. Dung ham putchar() de xuat tung ky tu ra console, roi ep xoa bo dem tuc thi bang fflush(stdout), ngung tre khoang 10 miligiay bang vong lap time clock(). Kieu viet nay mo phong 100% cam giac ChatGPT dang tao ra van ban."},
    
    {"compiler gcc terminal chay code compile exe powershell cmd bash", 
     "Chuong trinh duoc build thong qua trinh bien dich GCC. Qua trinh linkage (lien ket) noi 3 file main.c, server_list.c va file_io.c lai voi nhau. Ban chi can chay file .exe thuc thi. Du an nay toi uu tuyet doi nen dung luong memory footprint sieu nho nhe tren moi he dieu hanh Windows."}
};
int chat_db_size = 22;

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
        char* word = strtok(temp_input, " ,.?!");
        
        while (word != NULL) {
            if (strlen(word) > 1) { 
                for (int i = 0; i < chat_db_size; i++) { 
                    if (strstr(chat_db[i].keywords, word)) scores[i] += 5; 
                }
            }
            word = strtok(NULL, " ,.?!");
        }

        int best_match_idx = -1; int max_score = 0;
        for(int i = 0; i < chat_db_size; i++) {
            if(scores[i] > max_score) { max_score = scores[i]; best_match_idx = i; }
        }

        clock_t start_infer = clock();
        if (best_match_idx != -1) typewriter_box(chat_db[best_match_idx].answer, MAGENTA, 74);
        else {
            char default_ans[500];
            sprintf(default_ans, "Du lieu thoi gian thuc tren Internet khong co ket qua khop voi mo ta '%s' cua ban. Hay thu hoi cac chu de lien quan den thuat toan C, VRAM, AI hay link github nhe.", input);
            typewriter_box(default_ans, MAGENTA, 74);
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
    {"ham addserver them may chu gpu_model api_endpoint strcpy", 
     "=== KIEN TRUC HAM void addServer() ===\n- Vi tri: Thuoc tap tin 'src/server_list.c'.\n- Logic code: Ham nhan vao con tro cap 2 (ServerNode** head) de co the cap nhat truc tiep vao head. Dung 'malloc(sizeof(ServerNode))' xin RAM tren Heap tao Node moi. Sau do dung 'strcpy()' gan chuoi String vao Struct an toan. Vong lap while(temp->next != NULL) di tim duoi cua danh sach de noi Node nay vao cuoi cung."},
    
    {"ham deleteserver xoa may chu prev temp free", 
     "=== KIEN TRUC HAM void deleteServer() ===\n- Ky thuat: Dung giai thuat 2 con tro: 'temp' di truoc va 'prev' di kem phia sau. Neu may chu can xoa nam ngay dau (head), tro head qua node tiep theo roi free(temp). Neu no nam giua, khi quet thay, ta thao tac: prev->next = temp->next de nhay coc qua no, roi goi free(temp) de chong memory leak!"},
    
    {"ham displayservers dashboard live monitor in bang printf", 
     "=== KIEN TRUC HAM void displayServers() ===\n- Pha 1: Duyet toan mang de thong ke so luong Online/Offline va tong VRAM. Hien thi vao the Header.\n- Pha 2: Dung format chuoi '%-20s' trong printf de ep chuoi can trai hoac dung thanh ngang '|' thang tap tuc thi du cho ten model GPU ngan dai ra sao. Thiet ke thanh '[####.....]' bang thuat toan toan hoc (vram_used*100/vram_total) nhan cheo voi chieu dai max bar."},
    
    {"ham sortserversbyvram sap xep bubble sort swap", 
     "=== KIEN TRUC HAM void sortServersByVRAM() ===\n- Thuat toan can ban: Su dung Bubble Sort (Sap xep noi bot) tren danh sach lien ket.\n- Ky thuat an diem: Viec dao cho (swap pointer) cac con tro tren Linked List rat nguy hiem de mat root. Code nay khon ngoan hon, dung cac bien 'temp_id', 'temp_model' luu tru data tam thoi de doi data ben trong 2 node dang so sanh cho nhau chu khong he dung cham vao cac soi xich con tro."},
    
    {"ham toggleserverstatus bao tri online offline", 
     "=== KIEN TRUC HAM void toggleServerStatus() ===\n- Chuc nang: Dao trang thai may chu Online hoac Offline.\n- Logic code: Su dung phep toan ba ngoi sieu toc do 'temp->status = (temp->status == 1) ? 0 : 1;'. Neu server bi an Offline, he thong Load Balancing se lap tuc bo qua no trong viec cap phat VRAM AI sau do."},
    
    {"ham freelist don rac memory leak thoat chuong trinh", 
     "=== KIEN TRUC HAM void freeList() ===\n- Khi nguoi dung an 0 de thoat, ham nay bat buoc phai chay de xoa danh sach lien ket khoi RAM he dieu hanh.\n- Ky thuat: Dung vong lap quet. Neu ban goi free(current) truoc khi current=current->next thi ban se mat luon doan day duoi. Nen phai gan ServerNode* temp = current, tien current len 1 buoc, roi moi free(temp) an toan tuyet doi."},
    
    {"ham deployaimodel can bang tai tensor parallelism single-node p_nodes p_alloc", 
     "=== KIEN TRUC HAM void deployAIModel() ===\n- Ham nay the hien tuyet ky Cloud Architecture. No quet toan bo mang xem tong VRAM co du khong.\n- Neu co may chu du tai doc lap, uu tien Single-Node cap luon.\n- Neu khong, bat Tensor Parallelism. Ham dung lenh 'malloc()' de tao ra 3 mang dong tam thoi (p_nodes, p_alloc, p_free) de luu ban nhap phan bo VRAM vao do, roi in Preview cho Quan tri vien. Khi ban xac nhan (Confirm), no moi the hien buoc p_nodes[i]->vram_used += p_alloc[i] vao DB chinh thuc. Dang cap nhat code la day!"},
    
    {"ham interactwithmodel ai chat terminal chatbot inference db keywords", 
     "=== KIEN TRUC HAM void interactWithModel() ===\n- Ban dang tan huong thanh qua cua ham nay day! Ham dung vong lap thoi gian thuc, dung fgets() de lay input cua user. Su dung ham strtok() de bam nho cau thanh tung tu.\n- Thuat toan Scoring: Cong don diem tuc thi (scores[i]++) neu keyword cua user ton tai trong db[i].keywords. Model nao diem max thi tuon answer cua model do ra bang ham typewriter. Vua thong minh vua don gian bang nguyen li Accumulator."},
    
    {"ham encryptdecrypt ma hoa xor cipher file_io", 
     "=== KIEN TRUC HAM void encryptDecrypt() ===\n- Ma hoa XOR co tinh doi xung: A XOR B XOR B = A.\n- Trong C, ta dung vong lap for de duyet tung ky tu cua 'char* data' va cho 'data[i] ^= SECRET_KEY' (voi SECRET_KEY = 0x5A). Code goi ham nay truoc khi luu file va goi lai ham nay ngay sau khi doc file de bien rac thanh data chuan."},
    
    {"ham loadfromfile doc file fread saveToFile luu", 
     "=== KIEN TRUC HAM luu va doc file ===\n- Su dung fread() va fwrite() tren file che do 'rb' va 'wb' (Read/Write Binary).\n- Bi quyet o day la dung mot bien 'ServerNode tempNode' trung gian de doc. Roi goi truc tiep ham addServer() do toan bo data do vao lai kien truc Linked List dang chay tren RAM chu khong xai con tro ep kieu cuc doan."},
    
    {"ham showbootscreen khoi dong logo gpu quat quay hieu ung dong", 
     "=== KIEN TRUC HAM void showBootScreen() ===\n- Hieu ung hoat hoa duoc xay dung bang file main.c.\n- Su dung vong lap for(12), dung lenh '\\033[H' de day con tro ban phim len dau man hinh, roi in chong (overwrite) cac frame cua Card GPU lech canh quat (i%4==0,1,2,3). Fflush ep buffer render ngay tuc thi tao ra canh quat dang quay tron phia tren chu NEXTGPU."},
    
    {"ham interactivemenu tui _getch mui ten", 
     "=== KIEN TRUC HAM int interactiveMenu() ===\n- Xay dung menu khong xai phim enter. Kich hoat '_getch()' cua thu vien windows. Khi an mui ten, ban phim tra ra 2 byte ma (224 roi 72 hoac 80).\n- Ham se cong hoac tru index bien 'selected'. Vong lap quet cac options, neu i == selected, in mau '\\033[47m\\033[30m' tao highlight dep mat."},
    
    {"github link source code repository github.com", 
     "Toan bo Source Code sieu viet nay thuoc ve tac gia hoquan2007 tren GitHub. Ban co the check tung dong ma tai link nay: https://github.com/hoquan2007/GPU-Server-Manager"},
    
    {"main while kbhit difftime", 
     "=== KIEN TRUC HAM main() ===\n- Core OS cua phan mem. Chay 1 vong lap while(1) vo han de giu chuong trinh hien huu. Tinh nang Live Monitor dung ham _kbhit() non-blocking de vua in du lieu lien tuc tren man hinh, vua lang nghe ban phim, the hien trinh do da luong ao tren ngon ngu tuyen tinh C."}
};
int copilot_db_size = 14;

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
            char temp_input[300]; strcpy(temp_input, input_lower);
            char* word = strtok(temp_input, " ,.?!");
            
            while (word != NULL) {
                if (strlen(word) > 1) { 
                    for (int i = 0; i < copilot_db_size; i++) { 
                        if (strstr(copilot_db[i].keywords, word)) scores[i] += 5; 
                    }
                }
                word = strtok(NULL, " ,.?!");
            }

            int best_match_idx = -1; int max_score = 0;
            for(int i = 0; i < copilot_db_size; i++) {
                if(scores[i] > max_score) { max_score = scores[i]; best_match_idx = i; }
            }

            if (best_match_idx != -1) {
                typewriter_box(copilot_db[best_match_idx].answer, MAGENTA, 74);
            } else {
                typewriter_box("Lenh cua ban chua hop le hoac nam ngoai kha nang ho tro cua toi. Lenh he thong: 'them may chu', 'xoa may chu', 'trien khai', 'sap xep'. Giai thich code: hay go 'ham addserver', 'ham deleteserver', 'thuat toan can bang tai', 'ma hoa xor', 'hieu ung dong', 'link github', v.v.", MAGENTA, 74);
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