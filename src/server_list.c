#include "../include/server_list.h"

// 1. Hàm cấp phát bộ nhớ động cho một Node mới
ServerNode* createServerNode(int id, const char* model, int vtotal, const char* endpoint) {
    // Sử dụng malloc để xin hệ điều hành cấp phát một vùng nhớ vừa bằng kích thước của struct ServerNode
    ServerNode* newNode = (ServerNode*)malloc(sizeof(ServerNode));
    
    // Luôn phải kiểm tra xem malloc có thành công không (tránh lỗi crash chương trình)
    if (newNode == NULL) {
        printf("Loi: Khong the cap phat bo nho cho Server %d!\n", id);
        return NULL;
    }
    
    // Gán các giá trị khởi tạo
    newNode->server_id = id;
    strcpy(newNode->gpu_model, model); // Phải dùng strcpy cho chuỗi ký tự trong C
    newNode->vram_total = vtotal;
    newNode->vram_used = 0;            // Mặc định server mới thêm vào chưa chạy model nào
    strcpy(newNode->api_endpoint, endpoint);
    newNode->status = 1;               // Mặc định là Online (1)
    
    newNode->next = NULL;              // Node mới tạo luôn trỏ vào NULL (đứng cuối)
    return newNode;
}

// 2. Hàm thêm một máy chủ vào cuối danh sách liên kết
void addServer(ServerNode** head, int id, const char* model, int vtotal, const char* endpoint) {
    ServerNode* newNode = createServerNode(id, model, vtotal, endpoint);
    if (newNode == NULL) return;

    // Trường hợp 1: Danh sách đang rỗng, node mới chính là Node đầu tiên (head)
    if (*head == NULL) {
        *head = newNode;
        return;
    }

    // Trường hợp 2: Danh sách đã có phần tử, duyệt tìm Node cuối cùng
    ServerNode* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    // Trỏ Node cuối cùng vào Node vừa mới tạo
    temp->next = newNode;
}

// 3. Hàm in danh sách máy chủ ra màn hình console
void displayServers(ServerNode* head) {
    if (head == NULL) {
        printf("\n[!] Danh sach may chu hien dang trong.\n");
        return;
    }
    
    printf("\n================================= DANH SACH MAY CHU GPU =================================\n");
    printf("%-5s | %-15s | %-10s | %-10s | %-25s | %-10s\n", 
           "ID", "GPU Model", "VRAM Total", "VRAM Used", "API Endpoint", "Status");
    printf("-----------------------------------------------------------------------------------------\n");
    
    ServerNode* temp = head;
    while (temp != NULL) {
        printf("%-5d | %-15s | %-7d GB | %-7d GB | %-25s | %-10s\n",
               temp->server_id, temp->gpu_model, temp->vram_total, temp->vram_used,
               temp->api_endpoint, temp->status == 1 ? "Online" : "Offline");
        temp = temp->next; // Dịch chuyển con trỏ sang node tiếp theo
    }
    printf("=========================================================================================\n");
}

// 4. Hàm giải phóng TOÀN BỘ bộ nhớ (Chống Memory Leak)
void freeList(ServerNode** head) {
    ServerNode* current = *head;
    ServerNode* nextNode;

    // Duyệt qua từng node và dùng lệnh free() để trả lại bộ nhớ cho hệ điều hành
    while (current != NULL) {
        nextNode = current->next; // Giữ lại địa chỉ node tiếp theo trước khi xóa node hiện tại
        free(current);            // Giải phóng node hiện tại
        current = nextNode;       // Chuyển sang node tiếp theo
    }
    *head = NULL; // Đặt lại head về NULL để đảm bảo an toàn
}