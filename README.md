# Hệ Thống Máy Bơm Thông Minh IoT (Smart Pump System) - Nhóm 08

Đồ án môn học Hệ Điều Hành Nhúng. Hệ thống giám sát nhiệt độ và tự động điều khiển máy bơm, tích hợp tính năng Điện toán biên (Edge Computing) và giám sát từ xa qua Web Dashboard (Giao thức MQTT).

## Tính Năng Nổi Bật

| Tính năng | Mô tả chi tiết |
| :--- | :--- |
| **Real-time Linux** | Tối ưu hóa lập lịch với chính sách `SCHED_FIFO` (Priority 99 cho Nút nhấn, 98 cho Cảm biến), đảm bảo hệ thống phản hồi sự kiện tức thời, triệt tiêu độ trễ. |
| **Edge Computing** | Kiến trúc đa luồng độc lập. Thiết bị có khả năng tự ra quyết định điều khiển máy bơm tại chỗ (Offline) ngay cả khi mất kết nối Internet. |
| **Auto-Recovery** | Tích hợp Linux Daemon qua `systemd`. Hệ thống tự động khởi chạy khi cấp nguồn và tự động hồi sinh (Watchdog) nếu tiến trình bị kill/treo. |
| **User-space I/O** | Giao tiếp phần cứng trực tiếp qua thanh ghi vật lý (`mmap`) với tốc độ cao, quản lý tài nguyên an toàn bằng **POSIX Mutex** chống Race Condition. |
| **IoT Web Dashboard** | Điều khiển và giám sát từ xa theo thời gian thực thông qua giao thức MQTT over WebSockets (Broker: HiveMQ). |
| **Smart Reconnect** | Thuật toán tự động chờ và nạp lại cấu hình mạng khi mất kết nối, đảm bảo uptime hệ thống liên tục. |

## Kiến Trúc Phần Cứng (BeagleBone Black)

| Thiết bị / Module | Giao tiếp | Chân kết nối (Header P9 & P8) |
| :--- | :--- | :--- |
| **Cảm biến nhiệt độ BMP280** | I2C (I2C2) | VCC: 3.3V, GND: Mass, SCL: P9_22, SDA: P9_21 |
| **Máy bơm / Relay (LED demo)** | GPIO (Output) | Tương ứng với cấu hình trong mã nguồn |
| **Nút nhấn (Chuyển chế độ)** | GPIO (Input) | Tương ứng với cấu hình trong mã nguồn |
| **UART Debugging** | UART0 | Cụm 6 chân Debug Header (Giao tiếp qua cáp USB-to-TTL) |

## Cấu Trúc Thư Mục

```text
Embedded_Operating_System_Project/
├── app/                  # Ứng dụng User-space lõi
│   ├── main.c            # Điểm neo hệ thống, quản lý luồng SCHED_FIFO
│   ├── bmp280.c/.h       # Driver giao tiếp I2C trực tiếp qua thanh ghi
│   ├── led.c/.h          # Driver điều khiển Relay/LED máy bơm
│   ├── button.c/.h       # Driver xử lý ngắt và logic nút nhấn
│   ├── uart.c/.h         # Giao tiếp nối tiếp ghi log hệ thống
│   └── mqtt_client.c/.h  # Tiến trình Gateway đẩy dữ liệu lên HiveMQ
├── web/                  # Giao diện người dùng
│   └── dashboard.html    # Web Dashboard điều khiển từ xa (JS + Paho MQTT)
├── service/              # Cấu hình dịch vụ hệ thống
│   └── smart_pump.service # File đăng ký Daemon cho systemd
└── README.md             # Tài liệu dự án
