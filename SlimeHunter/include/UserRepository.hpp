#pragma once
// UserRepository.hpp - Lưu tài khoản và tiến trình người chơi bằng file CSV (thư mục data/).
//
// Module này KHÔNG phụ thuộc SFML nên có thể kiểm thử độc lập (tests/csv_test.cpp).
//
// ---- DỮ LIỆU LÂU DÀI (lưu CSV) và DỮ LIỆU TRONG PHIÊN (không lưu) ----
//  Lâu dài : tài khoản; màn đang chơi; màn cao nhất đã mở khóa; máu tối đa; sát thương;
//            tổng số quái đã hạ; tổng điểm.
//  Trong phiên (KHÔNG lưu): vị trí, máu hiện tại, cấp kinh nghiệm của lượt chơi, hồi chiêu, quái đang sống,
//            hiệu ứng, thời gian sống sót của lượt chơi.
//
// ---- QUY TẮC CSV ----
//  * Dòng đầu là tiêu đề; mỗi dòng sau là một bản ghi; các trường ngăn bằng dấu phẩy.
//  * Trường chứa dấu phẩy / ngoặc kép được bọc trong "..." và " được viết thành "" (như chuẩn RFC 4180),
//    nhưng KHÔNG hỗ trợ xuống dòng trong trường. Ngoài ra username/mật khẩu bị GIỚI HẠN ký tự (xem bên dưới)
//    nên các ký tự đặc biệt không bao giờ được ghi vào file.
//
// ---- BẢO MẬT (đọc kỹ) ----
//  Mật khẩu hiện được lưu DẠNG VĂN BẢN THUẦN để dễ quan sát khi học xử lý file. Đây chỉ phù hợp với dữ liệu thử nghiệm
//  cục bộ; TUYỆT ĐỐI không dùng mật khẩu thật. Việc lưu và kiểm tra mật khẩu nằm gọn trong hai hàm
//  encodePassword() / verifyPassword(): sau này đổi sang băm (hash) chỉ cần sửa hai hàm này.
//  Module không bao giờ in mật khẩu ra console hay log.

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

struct UserRecord {
    int userId = 0;
    std::string username;
    std::string password;  // giá trị đang lưu trong file (hiện là văn bản thuần); sẽ bị xóa khỏi bản trả về của login()
    std::string createdAt;
};

struct ProgressRecord {
    int userId = 0;
    int currentLevel = 1;       // màn đang chơi / màn chơi gần nhất
    int maxLevelUnlocked = 1;   // màn cao nhất đã mở khóa
    int playerMaxHp = 100;      // máu tối đa lâu dài (đã gồm các lần nâng cấp khi qua màn)
    int playerDamage = 20;      // sát thương cơ bản lâu dài
    int totalKills = 0;
    int totalScore = 0;
    std::string updatedAt;
};

enum class AuthResult { Ok, InvalidUsername, InvalidPassword, UsernameTaken, WrongCredentials, StorageError };

namespace csv {
std::string escapeField(const std::string& field);                       // thêm "..." nếu cần
std::vector<std::string> splitLine(const std::string& line, bool* wellFormed = nullptr);
bool parseInt(const std::string& text, int& out);                        // chỉ nhận số nguyên thuần, không khoảng trắng
}  // namespace csv

class UserRepository {
public:
    // Tạo thư mục và file (kèm dòng tiêu đề) nếu chưa có. Nếu không ghi được: isUsable() = false (game vẫn chạy ở chế độ khách).
    explicit UserRepository(std::filesystem::path dataDir);

    // Tìm thư mục data/ theo thứ tự: ./data, ../data, ../../data, đường dẫn nguồn do CMake ghi lại, cuối cùng tạo ./data.
    static std::filesystem::path findDataDirectory();

    bool isUsable() const { return m_usable; }
    const std::filesystem::path& getDataDir() const { return m_dataDir; }
    const std::string& getLastError() const { return m_lastError; }
    const std::vector<std::string>& getWarnings() const { return m_warnings; }  // dòng dữ liệu hỏng đã bị bỏ qua (không chứa mật khẩu)

    // ---- Kiểm tra định dạng đầu vào ----
    static bool isValidUsername(const std::string& username);  // 3-20 ký tự: chữ không dấu, số, '_' hoặc '-'
    static bool isValidPassword(const std::string& password);  // 4-32 ký tự in được, không khoảng trắng, không có ',' và '"'

    // ---- Tài khoản ----
    AuthResult registerUser(const std::string& username, const std::string& password, UserRecord* created = nullptr);
    AuthResult login(const std::string& username, const std::string& password, UserRecord& userOut);
    std::optional<UserRecord> findByUsername(const std::string& username) const;
    bool usernameExists(const std::string& username) const;
    std::vector<UserRecord> readAllUsers() const;

    // ---- Tiến trình ----
    static ProgressRecord defaultProgress(int userId);
    static bool isValidProgress(const ProgressRecord& progress);
    ProgressRecord loadProgress(int userId) const;  // thiếu / hỏng / ngoài giới hạn -> trả về mặc định, không bao giờ crash
    bool saveProgress(const ProgressRecord& progress);  // chỉ thay đúng dòng của user này; các dòng khác giữ nguyên

    // ---- Mật khẩu: TÁCH RIÊNG để sau này đổi sang hash mà không sửa phần còn lại ----
    static std::string encodePassword(const std::string& plain);                      // hiện: giữ nguyên
    static bool verifyPassword(const std::string& plain, const std::string& stored);  // hiện: so sánh bằng

private:
    std::filesystem::path usersPath() const { return m_dataDir / "users.csv"; }
    std::filesystem::path progressPath() const { return m_dataDir / "player_progress.csv"; }
    bool ensureFile(const std::filesystem::path& path, const std::string& header);
    bool readLines(const std::filesystem::path& path, std::vector<std::string>& lines) const;
    bool writeLinesAtomically(const std::filesystem::path& path, const std::vector<std::string>& lines);
    void fail(const std::string& message) const;

    std::filesystem::path m_dataDir;
    bool m_usable = false;
    mutable std::string m_lastError;
    mutable std::vector<std::string> m_warnings;
};
