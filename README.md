# 🚀 NEXTGPU PRO - AI Data Center Control Panel

![C](https://img.shields.io/badge/Language-C-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

**NEXTGPU PRO** là một hệ thống phần mềm quản trị tài nguyên máy chủ GPU mô phỏng theo kiến trúc Data Center thực tế, được phát triển hoàn toàn 100% bằng ngôn ngữ lập trình C thuần (C99/C11). 

Dự án ra đời nhằm giải quyết bài toán quản lý tài nguyên VRAM đắt đỏ khi triển khai các Mô hình Ngôn ngữ Lớn (LLMs) như ChatGPT, Claude hay DeepSeek, tích hợp các thuật toán cân bằng tải phân tán và Siêu trợ lý AI (Copilot) chạy trực tiếp trên giao diện Console TUI (Text User Interface).

---

## 🔗 Liên kết Dự án
* **GitHub Repository:** [https://github.com/hoquan2007/GPU-Server-Manager](https://github.com/hoquan2007/GPU-Server-Manager)
* **Tác giả:** hoquan2007 (Sinh viên Kỹ thuật Phần mềm)

---

## ✨ Tính năng Nổi bật (Core Features)

1. **🖥️ Live Monitor TUI (Giao diện Không giật lag):** * Giám sát thông số tổng quan toàn bộ cụm Server theo thời gian thực (Uptime, VRAM Usage, Online/Offline).
   * Sử dụng mã điều khiển ANSI (`\033[H`) để vẽ lại màn hình siêu mượt ở tốc độ 60fps mà không dùng lệnh `system("cls")`.
2. **⚖️ AI Load Balancing (Cân bằng tải AI - Tensor Parallelism):** * Hỗ trợ 3 chiến lược phân bổ VRAM thông minh khi triển khai Model khổng lồ: 
     * *Tham lam (Greedy/Binpack)*: Dồn đầy RAM máy này rồi mới sang máy khác.
     * *Cân bằng (Round-Robin)*: Chia đều tải tản nhiệt cho toàn cụm.
     * *Thủ công (Custom)*: Quản trị viên tự thiết lập cấu hình.
3. **🤖 Tích hợp Trợ lý AI (NEXTGPU Copilot & Inference Terminal):** * **Inference Terminal:** Giả lập kết nối WebSockets tới HuggingFace, cho phép chat trực tiếp với các Model đã Deploy kèm tính năng đo tốc độ *Inference Speed (tokens/s)*.
   * **Admin Copilot:** Trợ lý ảo điều hành bằng ngôn ngữ tự nhiên. Có khả năng tự động giải thích thuật toán, đọc hiểu mã nguồn và tự động thực thi các lệnh Thêm/Xóa/Deploy máy chủ.
4. **🔐 Bảo mật Dữ liệu (XOR Cipher):** * Toàn bộ cơ sở dữ liệu hệ thống (`gpu_servers.dat`) được mã hóa bitwise cấp thấp bằng thuật toán XOR Cipher, chống đọc trộm dữ liệu bằng các Editor thông thường.
5. **📊 Quản trị Toàn diện:** * Thêm, xóa, bảo trì máy chủ, sắp xếp danh sách ưu tiên theo chuẩn thuật toán, xuất báo cáo tài chính định dạng Excel (`.csv`) và tự động ghi nhật ký hệ thống (`system.log`).

---

## 🧠 Nền tảng Kiến thức (Technical Concepts)

Dự án áp dụng sâu sắc các cấu trúc dữ liệu và giải thuật của môn Kỹ thuật Lập trình:
* **Singly Linked List (Danh sách liên kết đơn):** Quản lý bộ nhớ động (`malloc`/`free`) để lưu trữ vô hạn số lượng máy chủ mà không bị tràn bộ đệm (Buffer Overflow) như dùng Mảng (Array).
* **Pointer Manipulation (Xử lý con trỏ):** Áp dụng kỹ thuật hoán đổi dữ liệu (Swap Data) trong thuật toán Bubble Sort thay vì đổi mối nối, giúp cấu trúc danh sách cực kỳ an toàn.
* **Non-blocking I/O:** Sử dụng `_getch()` và `_kbhit()` để bắt phím mũi tên thời gian thực, điều khiển Menu TUI không cần nhấn Enter.
* **File Processing:** Thao tác đọc/ghi file nhị phân (`rb`, `wb`) và chuỗi (`a`, `w`).

---

## ⚙️ Hướng dẫn Cài đặt & Sử dụng (Installation & Usage)

### 1. Tải dự án về máy (Clone)
Mở Terminal/Git Bash và chạy lệnh sau để tải toàn bộ mã nguồn về máy:
```bash
git clone [https://github.com/hoquan2007/GPU-Server-Manager.git](https://github.com/hoquan2007/GPU-Server-Manager.git)
cd GPU-Server-Manager

2. Biên dịch mã nguồn (Compile)
Đảm bảo máy tính của bạn đã cài đặt trình biên dịch GCC (MinGW trên Windows hoặc GCC trên Linux). Chạy lệnh sau để liên kết các module:

Bash

gcc src/main.c src/server_list.c src/file_io.c -o server_manager.exe

3. Khởi chạy Hệ thống
Chạy file thực thi vừa được tạo ra:

Bash

./server_manager.exe


(Lưu ý: Mở rộng toàn màn hình Terminal để có trải nghiệm đồ họa ASCII và Live Monitor tốt nhất).

