// csv_test.cpp - Kiểm thử module CSV (UserRepository) độc lập với game/SFML.
// Chạy: ./build/csv_test   (hoặc `ctest` trong thư mục build). Trả về 0 nếu mọi kiểm tra đạt.

#include "UserRepository.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

namespace {
int g_failures = 0;
int g_checks = 0;

void check(bool ok, const std::string& what) {
    ++g_checks;
    std::cout << (ok ? "[ OK ] " : "[FAIL] ") << what << '\n';
    if (!ok) ++g_failures;
}

std::string slurp(const fs::path& p) {
    std::ifstream in(p, std::ios::binary);
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

void spit(const fs::path& p, const std::string& text) {
    std::ofstream out(p, std::ios::binary | std::ios::trunc);
    out << text;
}

fs::path makeTempDir(const std::string& tag) {
    fs::path dir = fs::temp_directory_path() / ("slimehunter_csv_test_" + tag + "_" + std::to_string(std::rand()));
    fs::remove_all(dir);
    return dir;
}
}  // namespace

int main() {
    std::srand(12345);

    // ---------------------------------------------------------------- 1. Thư mục/file chưa tồn tại
    const fs::path dirA = makeTempDir("a");  // chưa tồn tại
    {
        UserRepository repo(dirA);
        check(repo.isUsable(), "Thu muc va file chua ton tai: tu tao, isUsable()");
        check(fs::exists(dirA / "users.csv") && fs::exists(dirA / "player_progress.csv"), "Hai file CSV duoc tao");
        check(slurp(dirA / "users.csv").rfind("user_id,username,password,created_at", 0) == 0, "users.csv co dong tieu de");
        check(repo.readAllUsers().empty(), "File chi co tieu de -> khong co tai khoan");
    }

    // ---------------------------------------------------------------- 2. Đăng ký / trùng / định dạng
    {
        UserRepository repo(dirA);
        UserRecord alice, bob;
        check(repo.registerUser("alice", "pass1234", &alice) == AuthResult::Ok && alice.userId == 1, "Tao tai khoan alice (user_id = 1)");
        check(repo.registerUser("bob_2", "secret99", &bob) == AuthResult::Ok && bob.userId == 2, "Tao tai khoan bob_2 (user_id = 2)");
        check(repo.registerUser("alice", "other123") == AuthResult::UsernameTaken, "Username trung bi tu choi");
        check(repo.registerUser("ALICE", "other123") == AuthResult::UsernameTaken, "Username trung (khac hoa/thuong) bi tu choi");
        check(repo.readAllUsers().size() == 2, "Van chi co 2 tai khoan sau khi bi tu choi");
        check(repo.registerUser("ab", "pass1234") == AuthResult::InvalidUsername, "Username qua ngan bi tu choi");
        check(repo.registerUser("a,b,c", "pass1234") == AuthResult::InvalidUsername, "Username co dau phay bi tu choi");
        check(repo.registerUser("a\"b\"c", "pass1234") == AuthResult::InvalidUsername, "Username co ngoac kep bi tu choi");
        check(repo.registerUser("a b c", "pass1234") == AuthResult::InvalidUsername, "Username co khoang trang bi tu choi");
        check(repo.registerUser("nguyễnvăn", "pass1234") == AuthResult::InvalidUsername, "Username co dau tieng Viet bi tu choi");
        check(repo.registerUser(std::string(21, 'x'), "pass1234") == AuthResult::InvalidUsername, "Username qua dai bi tu choi");
        check(repo.registerUser("carol", "ab") == AuthResult::InvalidPassword, "Mat khau qua ngan bi tu choi");
        check(repo.registerUser("carol", "abc,defg") == AuthResult::InvalidPassword, "Mat khau co dau phay bi tu choi");
        check(repo.registerUser("carol", "abc\"defg") == AuthResult::InvalidPassword, "Mat khau co ngoac kep bi tu choi");
        check(repo.readAllUsers().size() == 2, "Dau vao sai khong lam hong file");
    }

    // ---------------------------------------------------------------- 3. Đăng nhập
    {
        UserRepository repo(dirA);
        UserRecord u;
        check(repo.login("alice", "pass1234", u) == AuthResult::Ok && u.userId == 1, "Dang nhap dung");
        check(u.password.empty(), "login() khong tra ve mat khau trong ban ghi");
        check(repo.login("alice", "WRONG", u) == AuthResult::WrongCredentials, "Sai mat khau bi tu choi");
        check(repo.login("nobody", "pass1234", u) == AuthResult::WrongCredentials, "Tai khoan khong ton tai bi tu choi");
        check(repo.login("", "", u) == AuthResult::WrongCredentials, "Dau vao rong bi tu choi");
        check(UserRepository::verifyPassword("x1y2", UserRepository::encodePassword("x1y2")), "encode/verify mat khau nhat quan");
    }

    // ---------------------------------------------------------------- 4. Tiến trình: mặc định, lưu, không mất dữ liệu người khác
    {
        UserRepository repo(dirA);
        ProgressRecord p1 = repo.loadProgress(1);
        check(p1.currentLevel == 1 && p1.maxLevelUnlocked == 1 && p1.playerMaxHp == 100 && p1.playerDamage == 20, "Tien trinh mac dinh cua tai khoan moi");
        ProgressRecord p2 = repo.loadProgress(2);
        p2.currentLevel = 3; p2.maxLevelUnlocked = 3; p2.playerMaxHp = 132; p2.playerDamage = 26; p2.totalKills = 40; p2.totalScore = 900;
        check(repo.saveProgress(p2), "Luu tien trinh bob");
        const std::string before = slurp(dirA / "player_progress.csv");
        ProgressRecord p1b = repo.loadProgress(1);
        p1b.totalKills = 7; p1b.totalScore = 70;
        check(repo.saveProgress(p1b), "Luu tien trinh alice");
        ProgressRecord again2 = repo.loadProgress(2);
        check(again2.currentLevel == 3 && again2.maxLevelUnlocked == 3 && again2.playerMaxHp == 132 && again2.playerDamage == 26 &&
                  again2.totalKills == 40 && again2.totalScore == 900,
              "Cap nhat alice KHONG lam mat du lieu cua bob");
        check(repo.loadProgress(1).totalKills == 7, "Doc lai dung tien trinh cua alice");
        std::istringstream a(before), b(slurp(dirA / "player_progress.csv"));
        std::string line;
        int bobLinesBefore = 0, bobLinesAfter = 0;
        while (std::getline(a, line)) if (line.rfind("2,", 0) == 0) ++bobLinesBefore;
        while (std::getline(b, line)) if (line.rfind("2,", 0) == 0) ++bobLinesAfter;
        check(bobLinesBefore == 1 && bobLinesAfter == 1, "Dong cua bob chi xuat hien dung 1 lan (khong nhan ban)");
        repo.saveProgress(p1b); repo.saveProgress(p1b); repo.saveProgress(p1b);
        std::istringstream c(slurp(dirA / "player_progress.csv"));
        int aliceLines = 0;
        while (std::getline(c, line)) if (line.rfind("1,", 0) == 0) ++aliceLines;
        check(aliceLines == 1, "Luu nhieu lan van chi co 1 dong cho moi nguoi");
        check(!fs::exists(dirA / "player_progress.csv.tmp"), "Khong de lai file tam");
        ProgressRecord bad = p1b; bad.currentLevel = 50; bad.maxLevelUnlocked = 2; bad.playerMaxHp = -5; bad.totalKills = -1;
        check(repo.saveProgress(bad) && UserRepository::isValidProgress(repo.loadProgress(1)), "Gia tri sai duoc ep ve gioi han hop le khi luu");
    }

    // ---------------------------------------------------------------- 5. File rỗng / thiếu / hỏng
    {
        const fs::path dirB = makeTempDir("b");
        fs::create_directories(dirB);
        spit(dirB / "users.csv", "");              // file rỗng 0 byte
        spit(dirB / "player_progress.csv", "");
        UserRepository repo(dirB);
        check(repo.isUsable() && repo.readAllUsers().empty(), "File rong (0 byte): tu them tieu de, khong crash");
        check(repo.registerUser("dave", "pass1234") == AuthResult::Ok, "Dang ky duoc tren file vua duoc khoi tao lai");
        check(repo.loadProgress(999).maxLevelUnlocked == 1, "Nguoi choi chua co tien trinh -> mac dinh");

        // file hỏng nặng: rác, thiếu trường, chữ thay vì số, số vượt giới hạn, ngoặc kép không đóng, giá trị âm
        spit(dirB / "player_progress.csv",
             "user_id,current_level,max_level_unlocked,player_max_hp,player_damage,total_kills,total_score,updated_at\n"
             "ra rac khong phai csv\n"
             "10,abc,2,100,20,0,0,x\n"
             "11,1,1\n"
             "12,1,1,99999999999999,20,0,0,x\n"
             "13,1,1,100,20,-4,0,x\n"
             "14,\"1,1,100,20,0,0,x\n"
             "15,2,3,140,25,10,200,2026-01-01 00:00:00\n"
             "16,5,2,100,20,0,0,x\n");
        UserRepository repo2(dirB);
        const std::vector<int> bad = {10, 11, 12, 13, 14, 16};
        bool allDefault = true;
        for (int id : bad) {
            ProgressRecord p = repo2.loadProgress(id);
            if (p.currentLevel != 1 || p.maxLevelUnlocked != 1 || p.playerMaxHp != 100) allDefault = false;
        }
        check(allDefault, "Dong hong / sai kieu / ngoai gioi han -> dung du lieu mac dinh, khong crash");
        ProgressRecord ok15 = repo2.loadProgress(15);
        check(ok15.currentLevel == 2 && ok15.maxLevelUnlocked == 3 && ok15.playerMaxHp == 140, "Dong hop le nam giua cac dong hong van doc dung");
        ProgressRecord p20 = UserRepository::defaultProgress(20);
        p20.totalKills = 5;
        check(repo2.saveProgress(p20), "Luu tien trinh khi file dang co dong hong");
        const std::string after = slurp(dirB / "player_progress.csv");
        check(after.find("ra rac khong phai csv") != std::string::npos && after.find("14,\"1,1,100,20,0,0,x") != std::string::npos &&
                  after.find("15,2,3,140,25,10,200") != std::string::npos,
              "Dong cua nguoi khac (ke ca dong hong) duoc giu nguyen van khi luu");

        // users.csv: dòng hỏng bị bỏ qua, BOM + CRLF được chấp nhận, dòng cuối không có xuống dòng
        spit(dirB / "users.csv",
             "\xEF\xBB\xBFuser_id,username,password,created_at\r\n"
             "1,erin,pw1234,2026-01-01 00:00:00\r\n"
             "khong,phai,dong,hop,le,\r\n"
             "x,frank,pw1234,2026-01-01\r\n"
             "3,\"gr,ace\",pw1234,2026-01-01\r\n"
             "4,heidi,pw1234,2026-01-01 00:00:00");
        UserRepository repo3(dirB);
        const std::vector<UserRecord> users = repo3.readAllUsers();
        check(users.size() == 2 && users[0].username == "erin" && users[1].username == "heidi", "users.csv: BOM/CRLF OK, dong hong (id sai, username khong hop le) bi bo qua");
        check(!repo3.getWarnings().empty(), "Co canh bao cho dong bi bo qua");
        bool leaked = false;
        for (const std::string& w : repo3.getWarnings()) if (w.find("pw1234") != std::string::npos) leaked = true;
        check(!leaked, "Canh bao khong lo mat khau");
        check(repo3.registerUser("ivan", "pass1234") == AuthResult::Ok, "Dang ky khi dong cuoi khong co xuong dong");
        const std::vector<UserRecord> users2 = repo3.readAllUsers();
        check(users2.size() == 3 && users2[2].username == "ivan" && users2[2].userId == 5, "Dong moi khong dinh vao dong cuoi cu; user_id = max + 1");
        fs::remove_all(dirB);
    }

    // ---------------------------------------------------------------- 6. Tiện ích CSV (dấu phẩy / ngoặc kép)
    {
        check(csv::escapeField("a,b") == "\"a,b\"", "escapeField: boc dau phay");
        check(csv::escapeField("say \"hi\"") == "\"say \"\"hi\"\"\"", "escapeField: nhan doi ngoac kep");
        check(csv::escapeField("plain") == "plain", "escapeField: chuoi thuong giu nguyen");
        bool ok = false;
        std::vector<std::string> f = csv::splitLine("1,\"a,b\",\"say \"\"hi\"\"\",x", &ok);
        check(ok && f.size() == 4 && f[1] == "a,b" && f[2] == "say \"hi\"", "splitLine: doc dung truong co dau phay va ngoac kep");
        csv::splitLine("1,\"chua dong", &ok);
        check(!ok, "splitLine: ngoac kep khong dong -> bao sai dinh dang");
        csv::splitLine("1,ab\"cd,3", &ok);
        check(!ok, "splitLine: ngoac kep giua truong khong boc -> bao sai dinh dang");
        int v = 0;
        check(csv::parseInt("42", v) && v == 42 && !csv::parseInt("4 2", v) && !csv::parseInt("", v) && !csv::parseInt("12abc", v) &&
                  !csv::parseInt("99999999999999999999", v),
              "parseInt: chi nhan so nguyen thuan, bat tran so");
    }

    // ---------------------------------------------------------------- 7. Thư mục không ghi được
    {
        const fs::path blocker = fs::temp_directory_path() / ("slimehunter_csv_block_" + std::to_string(std::rand()));
        spit(blocker, "toi la mot FILE chu khong phai thu muc");
        UserRepository repo(blocker / "data");  // không thể tạo thư mục bên trong một file
        UserRecord u;
        check(!repo.isUsable(), "Khong tao duoc thu muc data -> isUsable() = false");
        check(repo.registerUser("zed", "pass1234") == AuthResult::StorageError, "Dang ky khi kho luu tru loi -> StorageError (khong crash)");
        check(repo.login("zed", "pass1234", u) == AuthResult::WrongCredentials, "Dang nhap khi kho luu tru loi -> bi tu choi (khong crash)");
        check(repo.loadProgress(1).maxLevelUnlocked == 1 && !repo.saveProgress(UserRepository::defaultProgress(1)), "Tien trinh: doc mac dinh, ghi that bai an toan");
        fs::remove(blocker);
    }

    fs::remove_all(dirA);
    std::cout << "\n" << (g_checks - g_failures) << "/" << g_checks << " kiem tra dat\n";
    std::cout << (g_failures == 0 ? "CSV TEST: DAT\n" : "CSV TEST: KHONG DAT\n");
    return g_failures == 0 ? 0 : 1;
}
