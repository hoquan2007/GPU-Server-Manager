#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/server_list.h"
#include "../include/file_io.h"

#ifdef _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
    int _getch(void) {
        struct termios oldt, newt; int ch;
        tcgetattr(STDIN_FILENO, &oldt); newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); return ch;
    }
    int _kbhit(void) {
        struct termios oldt, newt; int ch, oldf;
        tcgetattr(STDIN_FILENO, &oldt); newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); fcntl(STDIN_FILENO, F_SETFL, oldf);
        if(ch != EOF) { ungetc(ch, stdin); return 1; }
        return 0;
    }
#endif

void delay(int milliseconds) {
    long pause = milliseconds * (CLOCKS_PER_SEC / 1000);
    clock_t now, then; now = then = clock();
    while( (now - then) < pause ) now = clock();
}

void pauseAndClear() {
    printf(CYAN "\n    ... Nhan phim bat ky de tro ve Menu chinh \033[5m_\033[0m" RESET);
    _getch(); 
}

void printHeader() {
    printf("\033[H\033[J"); 
    // Giam do dai cac duong ke xuong con 68 ky tu de chong loi tran vien (Wrapping)
    printf(CYAN "    +====================================================================+\n");
    printf("    |              " BOLD WHITE "NEXTGPU PRO - DATA CENTER CONTROL PANEL" RESET CYAN "               |\n");
    printf("    +====================================================================+\n" RESET);
}

void showBootScreen() {
    printf("\033[H\033[J"); 
    for (int i = 0; i < 12; i++) {
        printf("\033[H"); 
        if (i % 4 == 0) {
            printf(GREEN "                        _________________\n");
            printf(GREEN "                       [  " CYAN "  \\ | /  " GREEN "  ]\n");
            printf(GREEN "                       [  " CYAN "  - " YELLOW "O" CYAN " -  " GREEN "  ]\n");
            printf(GREEN "                       [  " CYAN "  / | \\  " GREEN "  ]\n");
            printf(GREEN "                       [_________________]\n");
            printf(GREEN "                         |||       |||    \n" RESET);
        } else if (i % 4 == 1) {
            printf(GREEN "                        _________________\n");
            printf(GREEN "                       [  " CYAN "   \\|/   " GREEN "  ]\n");
            printf(GREEN "                       [  " CYAN "  --" YELLOW "O" CYAN "--  " GREEN "  ]\n");
            printf(GREEN "                       [  " CYAN "   /|\\   " GREEN "  ]\n");
            printf(GREEN "                       [_________________]\n");
            printf(GREEN "                         |||       |||    \n" RESET);
        } else if (i % 4 == 2) {
            printf(GREEN "                        _________________\n");
            printf(GREEN "                       [  " CYAN "  / | \\  " GREEN "  ]\n");
            printf(GREEN "                       [  " CYAN "  - " YELLOW "O" CYAN " -  " GREEN "  ]\n");
            printf(GREEN "                       [  " CYAN "  \\ | /  " GREEN "  ]\n");
            printf(GREEN "                       [_________________]\n");
            printf(GREEN "                         |||       |||    \n" RESET);
        } else {
            printf(GREEN "                        _________________\n");
            printf(GREEN "                       [  " CYAN "   /|\\   " GREEN "  ]\n");
            printf(GREEN "                       [  " CYAN "  --" YELLOW "O" CYAN "--  " GREEN "  ]\n");
            printf(GREEN "                       [  " CYAN "   \\|/   " GREEN "  ]\n");
            printf(GREEN "                       [_________________]\n");
            printf(GREEN "                         |||       |||    \n" RESET);
        }
        
        printf(CYAN "\n");
        printf("         _   _  _____ __   __ _____  _____  ____  _   _ \n");
        printf("        | \\ | ||  ___|\\ \\ / /|_   _||  __ \\|  _ \\| | | |\n");
        printf("        |  \\| || |__   \\ V /   | |  | |  \\/| |_) | | | |\n");
        printf("        | . ` ||  __|  /   \\   | |  | | __ |  __/| | | |\n");
        printf("        | |\\  || |___ / /^\\ \\  | |  | |_\\ \\| |   | |_| |\n");
        printf("        \\_| \\_/\\____/ \\/   \\/  \\_/   \\____/\\_|    \\___/ \n");
        
        printf(WHITE "\n");
        printf("         +-- " BOLD "NEXTGPU PRO - DATA CENTER CONTROL PANEL" RESET WHITE " --+\n\n" RESET);
        
        fflush(stdout); delay(120);     
    }

    printf(CYAN "    [*] Kiem tra he thong: Intel Core i7, 16GB RAM... " GREEN "[OK]\n" RESET); delay(300);
    printf(CYAN "    [*] Khoi tao NEXTGPU Kernel (v8.0.0-core)...\n" RESET); delay(200);
    printf(CYAN "    [*] Dang nap module bao mat RSA & XOR Cipher... " GREEN "[OK]\n" RESET); delay(300);
    printf(CYAN "    [*] Quet mang luoi phan tan (Distributed Node)...\n" RESET); delay(400);
    printf(YELLOW "    [!] Phat hien 1 thiet bi offline. Da bo qua.\n" RESET); delay(300);

    printf(WHITE "\n    [ Dang thiet lap giao dien Web UI ]\n" RESET);
    printf("    [");
    for(int i = 0; i < 50; i++) {
        // Su dung dau bang (=) tieu chuan chong loi font hien thi
        printf(CYAN "=" RESET); fflush(stdout); delay(15);
    }
    printf("] 100%%\n"); delay(400);
}

int interactiveMenu() {
    const char* options[] = {
        "[1]  Hien thi Dashboard ha tang (Live Monitor)",
        "[2]  Them may chu GPU moi vao he thong",
        "[3]  Trien khai Model AI (Can bang tai phan tan)",
        "[4]  Go bo Model AI khoi may chu (Giai phong VRAM)", 
        "[5]  Go bo may chu (Thanh ly phan cung)",
        "[6]  Sap xep may chu theo VRAM (Toi uu hoa)",
        "[7]  Bao tri may chu (Chuyen trang thai On/Off)",
        "[8]  Xuat bao cao he thong (Dinh dang Excel/CSV)",
        "[9]  Tuong tac voi AI Model (Terminal Chat)",      
        "[10] NEXTGPU Copilot (AI dieu hanh & Giai thich)",
        "[0]  Thoat chuong trinh & Luu tru du lieu an toan"
    };
    int num_options = 11; int selected = 0; int key;

    while(1) {
        printHeader(); printf("\n");
        for(int i = 0; i < num_options; i++) {
            if(i == selected) {
                // Background Cyan, text den, dai 60 ky tu giup vua khit man hinh
                printf("\033[46m\033[30m" BOLD "      >> %-60s \033[0m\n", options[i]);
            } else {
                // Hai khoi AI [9] va [10] da duoc gom chung mau Tim cuc net
                if (i == 10) printf(RED "         %-60s \n" RESET, options[i]);                      
                else if (i == 8 || i == 9) printf(MAGENTA "         %-60s \n" RESET, options[i]);    
                else if (i == 3 || i == 4) printf(YELLOW "         %-60s \n" RESET, options[i]);     
                else if (i == 5 || i == 6) printf(GREEN "         %-60s \n" RESET, options[i]);      
                else if (i == 7) printf(WHITE "         %-60s \n" RESET, options[i]);                
                else printf(CYAN "         %-60s \n" RESET, options[i]);                              
            }
        }
        
        printf(CYAN "\n    ----------------------------------------------------------------------\n" RESET);
        printf(YELLOW "      [DUNG MUI TEN DE DI CHUYEN - PHIM ENTER DE CHON] \n" RESET);

        key = _getch();
        if (key == 224 || key == 27) { 
            key = _getch(); if (key == 91) key = _getch(); 
            if (key == 72 || key == 65) { selected--; if (selected < 0) selected = num_options - 1; } 
            else if (key == 80 || key == 66) { selected++; if (selected >= num_options) selected = 0; }
        } else if (key == 13 || key == 10) { 
            if (selected == 10) return 0; 
            return selected + 1;         
        }
    }
}

int main() {
    srand(time(NULL)); 
    time_t start_time = time(NULL); 
    ServerNode* server_list = NULL;
    int choice;
    char log_buffer[256]; 
    
    showBootScreen();
    
    loadFromFile(&server_list, "data/gpu_servers.dat");
    loadAIModels(); 
    
    writeSystemLog("SYSTEM: Nguoi dung khoi dong he thong NEXTGPU PRO.");

    while (1) {
        choice = interactiveMenu();
        printHeader(); 

        switch (choice) {
            case 1: {
                int blink = 1;
                while(!_kbhit()) {
                    printf("\033[H\033[J"); 
                    printHeader();          
                    int uptime = (int)difftime(time(NULL), start_time);
                    displayServers(server_list, 1, uptime, blink); 
                    printf(YELLOW "\n    ... Nhan phim bat ky de thoat che do Live Monitor ..." RESET);
                    blink = !blink; 
                    delay(1000); 
                }
                _getch(); 
                break;
            }
            case 2: {
                int id, vtotal; char model[50], endpoint[100]; char next_char;
                printf(CYAN "\n    +--------------------------------------------------------+\n");
                printf("    |                 " GREEN "[+] THEM MAY CHU MOI" RESET CYAN "                   |\n");
                printf("    +--------------------------------------------------------+\n");
                printf("    |  ID May Chu (So)   : [                               ] |\n");
                printf("    |  Model GPU         : [                               ] |\n");
                printf("    |  Tong VRAM (GB)    : [                               ] |\n");
                printf("    |  API Endpoint      : [ " YELLOW "Go '0' de tao Auto IP" RESET CYAN "          ] |\n"); 
                printf("    +--------------------------------------------------------+\n" RESET);

                while (1) {
                    printf("\033[8;29H                               \033[8;29H"); 
                    if (scanf("%d%c", &id, &next_char) != 2 || next_char != '\n') { 
                        if (next_char != '\n') while(getchar() != '\n'); 
                        continue;
                    }
                    if (!checkIdExists(server_list, id) && id > 0) break; 
                }

                printf("\033[9;29H"); scanf(" %[^\n]", model); while(getchar() != '\n');
                printf("\033[10;29H"); scanf("%d", &vtotal); while(getchar() != '\n');
                
                printf("\033[11;29H"); 
                scanf(" %[^\n]", endpoint); while(getchar() != '\n');
                
                if (strcmp(endpoint, "0") == 0) {
                    sprintf(endpoint, "http://192.168.1.%d:11434/api", rand() % 200 + 10);
                    printf("\033[11;29H%-31s", endpoint); 
                    fflush(stdout);
                    delay(400); 
                }

                printf("\033[14;1H\033[J"); 
                printf(CYAN "    [*] Dang thiet lap ket noi ICMP den %s...\n" RESET, endpoint); delay(600);
                printf(WHITE "    Pinging %s with 32 bytes of data:\n" RESET, endpoint);
                for(int i = 1; i <= 3; i++) {
                    delay(700); int ping_time = rand() % 20 + 5; 
                    printf(GREEN "    Reply from %s: bytes=32 time=%dms TTL=112\n" RESET, endpoint, ping_time);
                }
                delay(600);
                addServer(&server_list, id, model, vtotal, endpoint);
                printf(GREEN "\n    [+] KET NOI THANH CONG! Da them may chu %s vao mang luoi.\n" RESET, model);
                
                sprintf(log_buffer, "INFO: Thiet lap may chu moi ID %d (Model: %s, VRAM: %dGB).", id, model, vtotal);
                writeSystemLog(log_buffer);

                pauseAndClear(); 
                break;
            }
            case 3: {
                char ai_name[50]; int req_vram;
                printf(CYAN "\n    +--------------------------------------------------------+\n");
                printf("    |         " MAGENTA "[>] TRIEN KHAI MODEL AI (LOAD BALANCING)" RESET CYAN "       |\n");
                printf("    +--------------------------------------------------------+\n");
                printf("    |  Ten Model AI      : [                               ] |\n");
                printf("    |  VRAM Yeu cau (GB) : [                               ] |\n");
                printf("    +--------------------------------------------------------+\n" RESET);
                printf("\033[8;29H"); scanf(" %[^\n]", ai_name); while(getchar() != '\n');
                printf("\033[9;29H"); scanf("%d", &req_vram); while(getchar() != '\n');
                printf("\033[12;1H"); 
                
                deployAIModel(server_list, ai_name, req_vram); 
                
                sprintf(log_buffer, "ACTION: Thuc thi lenh Trien khai Model %s (Yeu cau %d GB VRAM).", ai_name, req_vram);
                writeSystemLog(log_buffer);

                pauseAndClear(); 
                break;
            }
            case 4: { 
                int free_id;
                printf(CYAN "\n    +--------------------------------------------------------+\n");
                printf("    |            " YELLOW "[x] GO BO MODEL AI (FREE VRAM)" RESET CYAN "              |\n");
                printf("    +--------------------------------------------------------+\n");
                printf("    |  Nhap ID may chu   : [                               ] |\n");
                printf("    +--------------------------------------------------------+\n" RESET);
                printf("\033[8;29H"); 
                if (scanf("%d", &free_id) == 1) { 
                    while(getchar() != '\n'); printf("\033[11;1H"); 
                    undeployAIModel(server_list, free_id); 
                    
                    sprintf(log_buffer, "ACTION: Giai phong VRAM AI tren may chu ID %d.", free_id);
                    writeSystemLog(log_buffer);
                } 
                else { while(getchar() != '\n'); }
                pauseAndClear(); 
                break;
            }
            case 5: { 
                int del_id;
                printf(CYAN "\n    +--------------------------------------------------------+\n");
                printf("    |                " RED "[-] THANH LY MAY CHU GPU" RESET CYAN "                |\n");
                printf("    +--------------------------------------------------------+\n");
                printf("    |  Nhap ID can xoa   : [                               ] |\n");
                printf("    +--------------------------------------------------------+\n" RESET);
                printf("\033[8;29H"); 
                if (scanf("%d", &del_id) == 1) { 
                    while(getchar() != '\n'); printf("\033[11;1H"); 
                    deleteServer(&server_list, del_id); 
                    
                    sprintf(log_buffer, "WARNING: Thanh ly go bo may chu ID %d khoi he thong.", del_id);
                    writeSystemLog(log_buffer);
                } 
                else { while(getchar() != '\n'); }
                pauseAndClear(); 
                break;
            }
            case 6: {
                printf("\n"); 
                sortServersByVRAM(server_list); 
                writeSystemLog("SYSTEM: Quan tri vien thuc thi thuat toan sap xep danh sach may chu.");
                pauseAndClear(); 
                break;
            }
            case 7: {
                int toggle_id;
                printf(CYAN "\n    +--------------------------------------------------------+\n");
                printf("    |                " YELLOW "[*] BAO TRI MAY CHU GPU" RESET CYAN "                 |\n");
                printf("    +--------------------------------------------------------+\n");
                printf("    |  Nhap ID bao tri   : [                               ] |\n");
                printf("    +--------------------------------------------------------+\n" RESET);
                printf("\033[8;29H"); 
                if (scanf("%d", &toggle_id) == 1) { 
                    while(getchar() != '\n'); printf("\033[11;1H"); 
                    toggleServerStatus(server_list, toggle_id); 
                    
                    sprintf(log_buffer, "WARNING: Thay doi trang thai hoat dong may chu ID %d.", toggle_id);
                    writeSystemLog(log_buffer);
                } 
                else { while(getchar() != '\n'); }
                pauseAndClear(); 
                break;
            }
            case 8: {
                printf("\n"); 
                exportToCSV(server_list, "data/baocao_gpu.csv"); 
                writeSystemLog("SYSTEM: Da xuat bao cao tai chinh ra file CSV.");
                pauseAndClear(); 
                break;
            }
            case 9: { 
                interactWithModel(&server_list);
                pauseAndClear(); 
                break;
            }
            case 10: {
                writeSystemLog("SYSTEM: Quan tri vien truy cap NEXTGPU Copilot AI.");
                nextgpuCopilot(&server_list); 
                pauseAndClear(); 
                break;
            }
            case 0: {
                printf(GREEN "\n    Dang luu tru du lieu ma hoa xuong o cung...\n" RESET);
                saveToFile(server_list, "data/gpu_servers.dat"); freeList(&server_list);
                printf(GREEN "    He thong tat an toan. Tam biet!\n\n" RESET);
                return 0;
            }
        } 
    } 
    return 0;
}