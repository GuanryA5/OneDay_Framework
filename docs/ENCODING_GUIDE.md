# C++项目编码问题解决指南

## 📋 常见编码问题

### 1. **中文字符显示问题**
```cpp
// ❌ 错误：可能出现乱码
std::cout << "你好世界" << std::endl;

// ✅ 正确：使用UTF-8字符串
#include "core/common/encoding_utils.h"
using namespace oneday::core;

Utf8String message("你好世界");
std::cout << message.str() << std::endl;
```

### 2. **文件路径中文问题**
```cpp
// ❌ 错误：中文路径可能无法访问
std::ifstream file("C:/用户/文档/config.json");

// ✅ 正确：使用宽字符API
std::wstring widePath = EncodingUtils::utf8ToWide("C:/用户/文档/config.json");
std::ifstream file(widePath);
```

### 3. **字符串长度计算错误**
```cpp
std::string chinese = "你好世界";

// ❌ 错误：返回字节数(12)，不是字符数(4)
size_t byteCount = chinese.length();

// ✅ 正确：返回实际字符数(4)
size_t charCount = EncodingUtils::utf8Length(chinese);
```

## 🛠️ 解决方案

### 1. **编译器配置**
项目已配置以下编译选项：
```cmake
# CMakeLists.txt
if(MSVC)
    add_compile_options(/utf-8)  # 源文件和执行字符集都使用UTF-8
    add_compile_definitions(
        UNICODE          # 启用Unicode API
        _UNICODE         # 启用Unicode CRT
        NOMINMAX         # 避免Windows.h定义min/max宏
        WIN32_LEAN_AND_MEAN  # 减少Windows.h包含内容
        _CRT_SECURE_NO_WARNINGS  # 禁用CRT安全警告
    )
endif()
```

### 2. **使用编码工具类**

#### 基本转换
```cpp
#include "core/common/encoding_utils.h"
using namespace oneday::core;

// UTF-8 ↔ 宽字符转换
std::string utf8Str = "你好世界";
std::wstring wideStr = EncodingUtils::utf8ToWide(utf8Str);
std::string backToUtf8 = EncodingUtils::wideToUtf8(wideStr);

// 本地编码 ↔ UTF-8转换
std::string localStr = "本地编码字符串";  // 可能是GBK
std::string utf8Str = EncodingUtils::localToUtf8(localStr);
```

#### 安全字符串类
```cpp
// 自动处理编码转换
Utf8String str1("你好");           // 从C字符串
Utf8String str2(std::string("世界")); // 从std::string
Utf8String str3(L"Hello");         // 从宽字符串

// 字符串操作
size_t length = str1.length();     // 字符数，不是字节数
Utf8String substr = str1.substr(0, 2);  // 按字符截取
Utf8String combined = str1 + str2;      // 拼接

// 转换
std::string stdStr = str1.str();   // 转为std::string
std::wstring wideStr = str1.wstr(); // 转为std::wstring
```

### 3. **文件操作**
```cpp
// 读取可能包含中文的配置文件
bool loadConfig(const std::string& filename) {
    // 转换为宽字符路径
    std::wstring widePath = EncodingUtils::utf8ToWide(filename);
    std::ifstream file(widePath);
    
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    
    // 确保内容是有效的UTF-8
    if (!EncodingUtils::isValidUtf8(content)) {
        content = EncodingUtils::localToUtf8(content);
    }
    
    // 处理UTF-8内容...
    return true;
}
```

### 4. **日志输出**
```cpp
// 在main函数开始时设置控制台编码
int main(int argc, char* argv[]) {
    // 设置控制台UTF-8编码（Windows下必需）
    EncodingUtils::setConsoleUtf8();
    
    // 现在可以正确输出中文
    Logger::info("程序启动成功");
    Logger::error("配置文件加载失败：找不到文件");
    
    return 0;
}
```

### 5. **Qt界面中文显示**
```cpp
// Qt会自动处理UTF-8，但确保字符串是正确编码
class MainWindow : public QMainWindow {
public:
    MainWindow() {
        // 使用UTF-8字符串
        setWindowTitle(QString::fromUtf8("OneDay框架"));
        
        // 或使用编码工具类
        Utf8String title("OneDay框架");
        setWindowTitle(QString::fromStdString(title.str()));
    }
};
```

## 🔧 最佳实践

### 1. **源文件编码**
- 所有源文件保存为UTF-8编码（无BOM）
- VSCode已配置自动检测和转换编码

### 2. **字符串字面量**
```cpp
// ✅ 推荐：使用UTF-8字符串字面量
const char* message = u8"你好世界";

// ✅ 或使用宏简化
#define U8(str) reinterpret_cast<const char*>(u8##str)
const char* message = U8("你好世界");

// ✅ 或使用Utf8String类
Utf8String message("你好世界");
```

### 3. **跨平台兼容性**
```cpp
// 跨平台文件路径处理
std::string getConfigPath() {
#ifdef _WIN32
    // Windows下使用UTF-8路径
    return EncodingUtils::wideToUtf8(L"C:/Users/用户/Documents/config.json");
#else
    // Linux下直接使用UTF-8
    return "/home/用户/Documents/config.json";
#endif
}
```

### 4. **错误处理**
```cpp
bool processText(const std::string& input) {
    // 检查输入是否为有效UTF-8
    if (!EncodingUtils::isValidUtf8(input)) {
        Logger::warn("输入不是有效的UTF-8，尝试转换");
        
        // 尝试从本地编码转换
        std::string converted = EncodingUtils::localToUtf8(input);
        if (EncodingUtils::isValidUtf8(converted)) {
            return processText(converted);
        } else {
            Logger::error("无法转换输入文本编码");
            return false;
        }
    }
    
    // 处理有效的UTF-8文本...
    return true;
}
```

## 🐛 常见问题排查

### 1. **控制台输出乱码**
```cpp
// 确保在main函数开始时调用
EncodingUtils::setConsoleUtf8();
```

### 2. **文件读写乱码**
```cpp
// 使用宽字符API打开文件
std::wstring widePath = EncodingUtils::utf8ToWide(filename);
std::ifstream file(widePath);
```

### 3. **字符串比较失败**
```cpp
// 确保两个字符串都是相同编码
std::string str1 = EncodingUtils::localToUtf8(input1);
std::string str2 = EncodingUtils::localToUtf8(input2);
bool equal = (str1 == str2);
```

### 4. **JSON配置文件中文问题**
```cpp
// nlohmann/json自动处理UTF-8
nlohmann::json config;
config["title"] = Utf8String("应用标题").str();
config["description"] = Utf8String("应用描述").str();
```

## 📝 编码检查清单

- [ ] 所有源文件保存为UTF-8编码
- [ ] main函数调用`EncodingUtils::setConsoleUtf8()`
- [ ] 文件路径使用宽字符API或UTF-8转换
- [ ] 字符串长度使用`utf8Length()`而不是`length()`
- [ ] 字符串截取使用`utf8Substr()`
- [ ] 日志输出使用UTF-8编码
- [ ] 配置文件读写处理编码转换
- [ ] Qt界面使用`QString::fromUtf8()`

## 🎯 性能优化

### 1. **避免频繁转换**
```cpp
// ❌ 低效：每次都转换
for (const auto& item : items) {
    std::wstring wide = EncodingUtils::utf8ToWide(item);
    // 处理...
}

// ✅ 高效：批量转换或缓存结果
std::vector<std::wstring> wideItems;
for (const auto& item : items) {
    wideItems.push_back(EncodingUtils::utf8ToWide(item));
}
```

### 2. **使用移动语义**
```cpp
// 避免不必要的字符串拷贝
Utf8String createMessage() {
    return Utf8String("消息内容");  // 返回值优化
}
```

---

*遵循这些指南可以有效解决C++项目中的编码问题，确保中文字符正确显示和处理。*
