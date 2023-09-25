# Inject_Windows_Service
Inject and start a simple Windows service in the remote desktop environment, using Win32 API in C++.
---------------------------------------------------------------------------------------------------
Các yêu cầu tại remote và local pc:
- Kết nối với remotename: \\\\x.x.x.x\\admin$ để có thể truy cập thư mục share với quyền 
admin
- Remote computer: thêm và thay đổi giá trị LocalAccountTokenFilterPolicy, login bằng local account

Kết nối và tạo service trên remote pc:
- Kết nối thông qua SMB bằng hàm: WNetAddConnection2
- Cài đặt service trên máy remote bằng cách mở SC_Manager bằng hàm OpenSCManager với 
flag SC_MANAGER_CREATE_SERVICE
- Sử dụng hàm CreateService & StartService để khởi tạo và chạy service trên remote.

Kết quả:
![image](https://github.com/thenguyenltv/Inject_Windows_Service/assets/95334187/45319944-cf44-465e-b93c-ce4919be34cd)

![image](https://github.com/thenguyenltv/Inject_Windows_Service/assets/95334187/ddf4fdc4-2e93-4ede-bc65-90180e79d23b)


