#ifndef SERVER_LIST_H
#define SERVER_LIST_H

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"
#define MAGENTA "\033[1;35m"
#ifndef WHITE
#define WHITE   "\033[37m"
#endif

typedef struct ServerNode {
    int server_id;
    char gpu_model[50];
    int vram_total;
    int vram_used;
    char api_endpoint[100];
    int status; 
    struct ServerNode* next;
} ServerNode;

void addServer(ServerNode** head, int id, const char* model, int vtotal, const char* endpoint);
void displayServers(ServerNode* head, int is_live, int uptime_seconds, int blink);
void freeList(ServerNode** head);
void deployAIModel(ServerNode* head, const char* ai_model_name, int required_vram);
int checkIdExists(ServerNode* head, int id);
void deleteServer(ServerNode** head, int id);
void sortServersByVRAM(ServerNode* head);
void toggleServerStatus(ServerNode* head, int id);

// --- TÍNH NĂNG MỚI ĐƯỢC THÊM ---
void undeployAIModel(ServerNode* head, int id);
void loadAIModels();
void saveAIModels();

// --- 2 CON SIÊU AI ---
void interactWithModel(ServerNode** head_ref);
void nextgpuCopilot(ServerNode** head_ref);

#endif