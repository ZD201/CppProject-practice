# C++命令行参数解析器详细实践蓝图

## 项目概述

这是一个专门为提高C++水平设计的实践项目，涵盖从项目初始化到完整交付的全流程。通过实现一个功能完善的命令行参数解析库，你将掌握现代C++开发的最佳实践。

## 开发环境搭建（第0天）

### 必需工具清单

```bash
# 编译器选择（推荐顺序）
1. GCC 11+ (Linux/WSL推荐)
2. Clang 13+ (macOS推荐)  
3. MSVC 2019+ (Windows推荐)

# 构建工具
- CMake 3.20+
- Make/Ninja

# 版本控制
- Git 2.30+

# 推荐IDE/编辑器
- CLion (最佳C++体验，商业)
- VSCode + C++ Extension Pack (免费，配置复杂)
- Visual Studio 2022 (Windows用户)
```

### 环境验证脚本

```bash
# 创建验证脚本 check_env.sh
#!/bin/bash
echo "=== C++ 开发环境检查 ==="

# 检查编译器
echo "检查 G++ 版本:"
g++ --version | head -1

echo -e "\n检查 CMake 版本:"
cmake --version | head -1

echo -e "\n检查 Git 版本:"
git --version

# 测试编译环境
echo -e "\n测试 C++17 编译:"
echo '#include <iostream>
#include <string_view>
int main() { 
    std::string_view sv = "Hello C++17!";
    std::cout << sv << std::endl; 
    return 0; 
}' > test.cpp

g++ -std=c++17 test.cpp -o test && ./test && rm -f test test.cpp
echo "✅ 环境检查完成"
```

## 项目初始化流程

### Git仓库设置

```bash
# 1. 创建项目目录
mkdir cmdline-parser && cd cmdline-parser

# 2. 初始化Git仓库
git init
git config user.name "Your Name"
git config user.email "your.email@example.com"

# 3. 创建 .gitignore
cat > .gitignore << 'EOF'
# 构建目录
build/
cmake-build-*/

# 编译产物
*.o
*.obj
*.exe
*.dll
*.so
*.dylib
*.a
*.lib

# IDE文件
.vscode/
.idea/
*.vcxproj*
*.sln

# 系统文件
.DS_Store
Thumbs.db

# 测试覆盖率
*.gcov
*.gcda
*.gcno
coverage/

# 调试文件
*.pdb
core
EOF

# 4. 创建初始commit
git add .gitignore
git commit -m "Initial commit with gitignore"
```

### 目录结构搭建

```bash
# 按照以下结构创建目录
mkdir -p {src,include,tests,examples,docs,scripts,third_party}

# 创建基础文件
touch src/cmdline_parser.cpp
touch include/cmdline_parser.h
touch tests/test_main.cpp
touch examples/basic_example.cpp
touch README.md
touch CMakeLists.txt
touch docs/design.md

# 提交初始结构
git add .
git commit -m "Add initial project structure"
```

## 详细开发计划

### 阶段0：项目基础设施（第1天上午）

#### CMake配置文件

```cmake
# 根目录 CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(CmdLineParser
    VERSION 1.0.0
    DESCRIPTION "Modern C++ Command Line Parser"
    LANGUAGES CXX
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译器特定设置
if(MSVC)
    add_compile_options(/W4 /WX)  # 高警告级别，警告视为错误
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
    add_compile_options(-Wno-unused-parameter)  # 暂时忽略未使用参数警告
endif()

# Debug模式下添加调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_BUILD)
    if(NOT MSVC)
        add_compile_options(-g -O0)
    endif()
endif()

# 包含目录
include_directories(include)

# 主库
add_subdirectory(src)

# 测试（可选编译）
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 示例（可选编译）
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
```

#### 构建脚本

```bash
# scripts/build.sh - 统一构建脚本
#!/bin/bash
set -e  # 遇到错误立即退出

BUILD_TYPE=${1:-Debug}  # 默认Debug模式
BUILD_DIR="build_${BUILD_TYPE,,}"

echo "=== 构建配置 ==="
echo "构建类型: $BUILD_TYPE"
echo "构建目录: $BUILD_DIR"

# 清理并创建构建目录
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# 配置项目
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=ON

# 编译
cmake --build . --parallel $(nproc 2>/dev/null || echo 4)

echo "✅ 构建完成"

# 运行测试
if [ "$BUILD_TYPE" = "Debug" ]; then
    echo "=== 运行测试 ==="
    ctest --output-on-failure
fi
```

#### 代码格式化配置

```yaml
# .clang-format
---
Language: Cpp
BasedOnStyle: Google
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 100
BreakBeforeBraces: Attach
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
AllowShortFunctionsOnASingleLine: Empty
IncludeBlocks: Regroup
SortIncludes: true
```

**阶段0检查点**：

- [ ] 项目能够成功构建
- [ ] Git历史清晰，每次提交有意义
- [ ] 构建脚本可以正常运行
- [ ] 代码格式化工具配置完成

### 阶段1：基础类设计（第1天下午 - 第2天）

#### 设计原则

```cpp
// include/cmdline_parser.h
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace cmdline {

// 前向声明 - 减少编译依赖
class Option;
class OptionGroup;

/**
 * @brief 参数值的类型定义
 * 
 * 使用 std::variant 而不是 void* 来保证类型安全
 * 这是现代C++的推荐做法
 */
using OptionValue = std::variant<bool, int, double, std::string>;

/**
 * @brief 主要的命令行解析器类
 * 
 * 设计原则：
 * 1. RAII - 资源获取即初始化
 * 2. 单一职责 - 只负责参数解析
 * 3. 易于使用 - 链式调用API
 * 4. 异常安全 - 使用RAII和智能指针
 */
class CmdLineParser {
public:
    // 删除拷贝构造和拷贝赋值 - 避免浅拷贝问题
    CmdLineParser(const CmdLineParser&) = delete;
    CmdLineParser& operator=(const CmdLineParser&) = delete;
    
    // 默认移动构造和移动赋值 - 支持高效传递
    CmdLineParser(CmdLineParser&&) = default;
    CmdLineParser& operator=(CmdLineParser&&) = default;
    
    // 构造函数使用委托构造 - 减少代码重复
    explicit CmdLineParser(std::string program_name);
    CmdLineParser(std::string program_name, std::string description);
    
    // 虚析构函数 - 支持继承（虽然目前不需要）
    virtual ~CmdLineParser() = default;
    
    // 链式调用API设计
    CmdLineParser& add_flag(const std::string& short_name,
                           const std::string& long_name,
                           const std::string& description);
                           
    CmdLineParser& add_option(const std::string& short_name,
                             const std::string& long_name,
                             const std::string& description,
                             const OptionValue& default_value = std::string{});
    
private:
    // 使用pimpl模式隐藏实现细节
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace cmdline
```

#### 错误处理设计

```cpp
// include/cmdline_exceptions.h
#pragma once

#include <stdexcept>
#include <string>

namespace cmdline {

/**
 * @brief 命令行解析异常基类
 * 
 * 继承自 std::runtime_error 而不是 std::exception
 * 这样可以携带错误信息
 */
class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& message)
        : std::runtime_error("Parse error: " + message) {}
};

/**
 * @brief 未知选项异常
 */
class UnknownOptionError : public ParseError {
public:
    explicit UnknownOptionError(const std::string& option)
        : ParseError("Unknown option: " + option) {}
};

/**
 * @brief 缺少必需参数异常
 */
class MissingRequiredError : public ParseError {
public:
    explicit MissingRequiredError(const std::string& option)
        : ParseError("Missing required option: " + option) {}
};

/**
 * @brief 参数值类型错误异常
 */
class InvalidValueError : public ParseError {
public:
    InvalidValueError(const std::string& option, const std::string& value)
        : ParseError("Invalid value '" + value + "' for option: " + option) {}
};

} // namespace cmdline
```

#### 渐进式实现策略

```cpp
// src/cmdline_parser.cpp - 第一版本实现
#include "cmdline_parser.h"
#include "cmdline_exceptions.h"

#include <iostream>
#include <sstream>

namespace cmdline {

// Pimpl实现 - 隐藏内部实现细节
class CmdLineParser::Impl {
public:
    std::string program_name;
    std::string description;
    std::vector<std::unique_ptr<Option>> options;
    std::unordered_map<std::string, Option*> short_map;
    std::unordered_map<std::string, Option*> long_map;
    
    // 添加选项的内部实现
    Option& add_option_impl(std::unique_ptr<Option> option);
    
    // 查找选项
    Option* find_option(const std::string& name) const;
};

CmdLineParser::CmdLineParser(std::string program_name)
    : CmdLineParser(std::move(program_name), "") {}

CmdLineParser::CmdLineParser(std::string program_name, std::string description)
    : pimpl_(std::make_unique<Impl>()) {
    pimpl_->program_name = std::move(program_name);
    pimpl_->description = std::move(description);
}

// TODO: 在下一阶段实现具体功能

} // namespace cmdline
```

**阶段1检查点**：

- [ ] 头文件结构清晰，使用了合适的前向声明
- [ ] 异常类层次设计合理
- [ ] 使用了现代C++特性（智能指针、移动语义）
- [ ] 代码编译通过，无警告
- [ ] 提交信息描述了设计决策

**常见错误和避免方法**：

1. **内存管理错误**

   ```cpp
   // ❌ 错误：使用裸指针
   std::vector<Option*> options;
   
   // ✅ 正确：使用智能指针
   std::vector<std::unique_ptr<Option>> options;
   ```

2. **异常安全性错误**

   ```cpp
   // ❌ 错误：可能泄露资源
   void parse(int argc, char* argv[]) {
       Option* opt = new Option();  // 可能抛异常
       options.push_back(opt);      // 如果这里抛异常，opt泄露
   }
   
   // ✅ 正确：使用RAII
   void parse(int argc, char* argv[]) {
       auto opt = std::make_unique<Option>();
       options.push_back(std::move(opt));
   }
   ```

3. **拷贝语义错误**

   ```cpp
   // ❌ 错误：意外的深拷贝
   CmdLineParser create_parser() {
       CmdLineParser parser("test");
       // ... 配置
       return parser;  // 可能触发昂贵的拷贝
   }
   
   // ✅ 正确：显式移动或删除拷贝
   CmdLineParser create_parser() {
       CmdLineParser parser("test");
       // ... 配置
       return std::move(parser);  // 明确移动
   }
   ```

### 阶段2：Option类实现（第3天）

#### Option类详细设计

```cpp
// include/option.h
#pragma once

#include "cmdline_exceptions.h"
#include <functional>
#include <optional>
#include <string>
#include <variant>

namespace cmdline {

class Option {
public:
    enum class Type {
        BOOL,    // 布尔型标志
        STRING,  // 字符串参数
        INT,     // 整数参数
        DOUBLE   // 浮点数参数
    };

    // 值验证器类型
    using Validator = std::function<bool(const OptionValue&)>;
    
    // 构造函数 - 使用成员初始化列表
    Option(std::string short_name, std::string long_name, std::string description, Type type);
    
    // 链式调用方法
    Option& set_required(bool required = true);
    Option& set_default_value(const OptionValue& value);
    Option& set_validator(Validator validator);
    Option& set_help_text(const std::string& help);
    
    // 访问器方法 - const正确性
    const std::string& get_short_name() const noexcept { return short_name_; }
    const std::string& get_long_name() const noexcept { return long_name_; }
    const std::string& get_description() const noexcept { return description_; }
    Type get_type() const noexcept { return type_; }
    bool is_required() const noexcept { return required_; }
    bool is_set() const noexcept { return is_set_; }
    
    // 值设置和获取
    void set_value(const std::string& str_value);
    void set_flag(); // 用于布尔标志
    
    template<typename T>
    T get_value() const;
    
    // 验证方法
    bool validate() const;
    
private:
    std::string short_name_;
    std::string long_name_;
    std::string description_;
    Type type_;
    bool required_ = false;
    bool is_set_ = false;
    
    OptionValue value_;
    std::optional<OptionValue> default_value_;
    Validator validator_;
    
    // 私有辅助方法
    OptionValue parse_value(const std::string& str_value) const;
    void validate_value(const OptionValue& value) const;
};

// 模板特化声明
template<> bool Option::get_value<bool>() const;
template<> int Option::get_value<int>() const;
template<> double Option::get_value<double>() const;
template<> std::string Option::get_value<std::string>() const;

} // namespace cmdline
```

#### 实现细节与最佳实践

```cpp
// src/option.cpp
#include "option.h"
#include <charconv>  // C++17的高性能字符串转换
#include <stdexcept>

namespace cmdline {

Option::Option(std::string short_name, std::string long_name, 
               std::string description, Type type)
    : short_name_(std::move(short_name))
    , long_name_(std::move(long_name))
    , description_(std::move(description))
    , type_(type) {
    
    // 参数验证 - 构造时就检查
    if (short_name_.empty() && long_name_.empty()) {
        throw std::invalid_argument("Option must have at least short or long name");
    }
    
    // 为布尔类型设置默认值
    if (type_ == Type::BOOL) {
        value_ = false;
    }
}

void Option::set_value(const std::string& str_value) {
    try {
        value_ = parse_value(str_value);
        validate_value(value_);
        is_set_ = true;
    } catch (const std::exception& e) {
        throw InvalidValueError(long_name_.empty() ? short_name_ : long_name_, str_value);
    }
}

OptionValue Option::parse_value(const std::string& str_value) const {
    switch (type_) {
        case Type::BOOL:
            // 布尔值解析 - 支持多种格式
            if (str_value == "true" || str_value == "1" || str_value == "yes" || str_value == "on") {
                return true;
            } else if (str_value == "false" || str_value == "0" || str_value == "no" || str_value == "off") {
                return false;
            } else {
                throw std::invalid_argument("Invalid boolean value");
            }
            
        case Type::INT: {
            // 使用 std::from_chars 进行高性能转换
            int result;
            auto [ptr, ec] = std::from_chars(str_value.data(), 
                                           str_value.data() + str_value.size(), result);
            if (ec != std::errc{} || ptr != str_value.data() + str_value.size()) {
                throw std::invalid_argument("Invalid integer value");
            }
            return result;
        }
        
        case Type::DOUBLE: {
            // 注意：std::from_chars 对 double 的支持在某些编译器中可能不完整
            // 为了兼容性，这里使用 strtod
            char* end;
            double result = std::strtod(str_value.c_str(), &end);
            if (end == str_value.c_str() || *end != '\0') {
                throw std::invalid_argument("Invalid double value");
            }
            return result;
        }
        
        case Type::STRING:
            return str_value;
            
        default:
            throw std::logic_error("Unknown option type");
    }
}

// 模板特化实现 - 类型安全的值获取
template<>
bool Option::get_value<bool>() const {
    if (!is_set_ && default_value_) {
        return std::get<bool>(*default_value_);
    }
    return std::get<bool>(value_);
}

template<>
int Option::get_value<int>() const {
    if (!is_set_ && default_value_) {
        return std::get<int>(*default_value_);
    }
    return std::get<int>(value_);
}

// ... 其他特化实现

} // namespace cmdline
```

**阶段2检查点**：

- [ ] Option类接口设计合理
- [ ] 错误处理完善
- [ ] 使用了现代C++特性（std::optional, std::variant）
- [ ] 模板特化正确实现
- [ ] 单元测试覆盖核心功能

### 阶段3：解析器核心逻辑（第4天）

#### 解析状态机设计

```cpp
// src/cmdline_parser.cpp - 核心解析逻辑
namespace cmdline {

class CmdLineParser::Impl {
public:
    enum class ParseState {
        NORMAL,           // 正常解析状态
        EXPECTING_VALUE,  // 等待选项值
        POSITIONAL_ONLY   // 只处理位置参数（遇到 -- 后）
    };
    
    struct ParseContext {
        int argc;
        char** argv;
        int current_index = 1;  // 跳过程序名
        ParseState state = ParseState::NORMAL;
        Option* pending_option = nullptr;  // 等待值的选项
        
        bool has_more() const { return current_index < argc; }
        std::string current_arg() const { return argv[current_index]; }
        void advance() { ++current_index; }
    };
    
    void parse_impl(ParseContext& ctx);
    void parse_long_option(ParseContext& ctx);
    void parse_short_options(ParseContext& ctx);
    void handle_option_value(ParseContext& ctx, Option* option);
    void finalize_parsing();
};

void CmdLineParser::parse(int argc, char* argv[]) {
    if (argc < 1) {
        throw std::invalid_argument("Invalid argument count");
    }
    
    Impl::ParseContext ctx{argc, argv};
    
    try {
        pimpl_->parse_impl(ctx);
        pimpl_->finalize_parsing();
    } catch (const std::exception& e) {
        // 重新抛出，添加上下文信息
        throw ParseError(std::string(e.what()) + " at argument " + std::to_string(ctx.current_index));
    }
}

void CmdLineParser::Impl::parse_impl(ParseContext& ctx) {
    while (ctx.has_more()) {
        std::string arg = ctx.current_arg();
        
        // 处理特殊情况
        if (arg == "--") {
            ctx.state = ParseState::POSITIONAL_ONLY;
            ctx.advance();
            continue;
        }
        
        // 根据当前状态处理
        switch (ctx.state) {
            case ParseState::EXPECTING_VALUE:
                handle_option_value(ctx, ctx.pending_option);
                break;
                
            case ParseState::POSITIONAL_ONLY:
                positional_args.push_back(arg);
                ctx.advance();
                break;
                
            case ParseState::NORMAL:
                if (arg.starts_with("--")) {
                    parse_long_option(ctx);
                } else if (arg.starts_with("-") && arg.length() > 1) {
                    parse_short_options(ctx);
                } else {
                    positional_args.push_back(arg);
                    ctx.advance();
                }
                break;
        }
    }
}

void CmdLineParser::Impl::parse_long_option(ParseContext& ctx) {
    std::string arg = ctx.current_arg();
    ctx.advance();
    
    // 移除 "--" 前缀
    std::string option_part = arg.substr(2);
    
    // 检查是否有 = 分隔的值
    size_t eq_pos = option_part.find('=');
    std::string option_name, option_value;
    bool has_embedded_value = false;
    
    if (eq_pos != std::string::npos) {
        option_name = option_part.substr(0, eq_pos);
        option_value = option_part.substr(eq_pos + 1);
        has_embedded_value = true;
    } else {
        option_name = option_part;
    }
    
    // 查找选项
    auto it = long_map.find(option_name);
    if (it == long_map.end()) {
        throw UnknownOptionError("--" + option_name);
    }
    
    Option* option = it->second;
    
    // 处理不同类型的选项
    if (option->get_type() == Option::Type::BOOL) {
        if (has_embedded_value) {
            option->set_value(option_value);
        } else {
            option->set_flag();
        }
    } else {
        if (has_embedded_value) {
            option->set_value(option_value);
        } else {
            // 下一个参数作为值
            ctx.state = ParseState::EXPECTING_VALUE;
            ctx.pending_option = option;
        }
    }
}

} // namespace cmdline
```

**阶段3检查点**：

- [ ] 解析逻辑正确处理各种情况
- [ ] 状态机设计清晰
- [ ] 错误处理完善
- [ ] 支持POSIX和GNU风格的选项格式

### 阶段4：测试驱动开发（第5天）

#### 测试框架搭建

```cpp
// tests/test_framework.h - 简单的测试框架
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>

namespace test {

class TestRunner {
public:
    struct TestCase {
        std::string name;
        std::function<void()> test_func;
    };
    
    static TestRunner& instance() {
        static TestRunner runner;
        return runner;
    }
    
    void add_test(const std::string& name, std::function<void()> test_func) {
        tests_.push_back({name, std::move(test_func)});
    }
    
    int run_all() {
        int passed = 0;
        int failed = 0;
        
        std::cout << "Running " << tests_.size() << " tests...\n\n";
        
        for (const auto& test : tests_) {
            try {
                std::cout << "Running: " << test.name << "... ";
                test.test_func();
                std::cout << "PASSED\n";
                ++passed;
            } catch (const std::exception& e) {
                std::cout << "FAILED: " << e.what() << "\n";
                ++failed;
            }
        }
        
        std::cout << "\nResults: " << passed << " passed, " << failed << " failed\n";
        return failed;
    }
    
private:
    std::vector<TestCase> tests_;
};

// 便利宏
#define TEST(name) \
    void test_##name(); \
    static int dummy_##name = (test::TestRunner::instance().add_test(#name, test_##name), 0); \
    void test_##name()

#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            throw std::runtime_error("Expected: " + std::to_string(expected) + \
                                   ", Actual: " + std::to_string(actual)); \
        } \
    } while (0)

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            throw std::runtime_error("Assertion failed: " #condition); \
        } \
    } while (0)

#define ASSERT_THROWS(expression, exception_type) \
    do { \
        bool caught = false; \
        try { \
            (expression); \
        } catch (const exception_type&) { \
            caught = true; \
        } \
        if (!caught) { \
            throw std::runtime_error("Expected exception not thrown: " #exception_type); \
        } \
    } while (0)

} // namespace test
```

#### 完整的测试用例

```cpp
// tests/test_option.cpp
#include "test_framework.h"
#include "option.h"
#include "cmdline_exceptions.h"

using namespace cmdline;
using namespace test;

TEST(option_construction) {
    Option opt("h", "help", "Show help", Option::Type::BOOL);
    ASSERT_EQ("h", opt.get_short_name());
    ASSERT_EQ("help", opt.get_long_name());
    ASSERT_EQ("Show help", opt.get_description());
    ASSERT_EQ(Option::Type::BOOL, opt.get_type());
    ASSERT_EQ(false, opt.is_required());
    ASSERT_EQ(false, opt.is_set());
}

TEST(boolean_option_flag) {
    Option opt("v", "verbose", "Verbose output", Option::Type::BOOL);
    opt.set_flag();
    ASSERT_TRUE(opt.is_set());
    ASSERT_TRUE(opt.get_value<bool>());
}

TEST(string_option_value) {
    Option opt("f", "file", "Input file", Option::Type::STRING);
    opt.set_value("test.txt");
    ASSERT_TRUE(opt.is_set());
    ASSERT_EQ("test.txt", opt.get_value<std::string>());
}

TEST(integer_option_value) {
    Option opt("n", "number", "A number", Option::Type::INT);
    opt.set_value("42");
    ASSERT_TRUE(opt.is_set());
    ASSERT_EQ(42, opt.get_value<int>());
}

TEST(invalid_integer_value) {
    Option opt("n", "number", "A number", Option::Type::INT);
    ASSERT_THROWS(opt.set_value("not_a_number"), InvalidValueError);
}

TEST(required_option_validation) {
    Option opt("r", "required", "Required option", Option::Type::STRING);
    opt.set_required(true);
    ASSERT_TRUE(opt.is_required());
    // 注意：验证逻辑应该在parser层面实现
}

TEST(default_value_behavior) {
    Option opt("d", "default", "Option with default", Option::Type::INT);
    opt.set_default_value(100);
    ASSERT_EQ(100, opt.get_value<int>());  // 未设置时返回默认值
    
    opt.set_value("200");
    ASSERT_EQ(200, opt.get_value<int>());  // 设置后返回设置的值
}

// 高级测试 - 值验证器
TEST(value_validator) {
    Option opt("p", "port", "Port number", Option::Type::INT);
    opt.set_validator([](const OptionValue& value) {
        int port = std::get<int>(value);
        return port >= 1 && port <= 65535;
    });
    
    opt.set_value("8080");  // 有效端口
    ASSERT_TRUE(opt.validate());
    
    ASSERT_THROWS(opt.set_value("70000"), InvalidValueError);  // 无效端口
}
```

#### 集成测试

```cpp
// tests/test_integration.cpp
#include "test_framework.h"
#include "cmdline_parser.h"

using namespace cmdline;
using namespace test;

// 测试辅助函数
std::vector<char*> make_argv(const std::vector<std::string>& args) {
    std::vector<char*> argv;
    for (const auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    return argv;
}

TEST(basic_flag_parsing) {
    CmdLineParser parser("test");
    parser.add_flag("h", "help", "Show help");
    parser.add_flag("v", "verbose", "Verbose output");
    
    auto args = std::vector<std::string>{"test", "-h", "--verbose"};
    auto argv = make_argv(args);
    
    parser.parse(args.size(), argv.data());
    
    ASSERT_TRUE(parser.get_value<bool>("help"));
    ASSERT_TRUE(parser.get_value<bool>("verbose"));
}

TEST(option_with_value_parsing) {
    CmdLineParser parser("test");
    parser.add_option("f", "file", "Input file", std::string{});
    parser.add_option("n", "number", "A number", 0);
    
    auto args = std::vector<std::string>{"test", "-f", "input.txt", "--number=42"};
    auto argv = make_argv(args);
    
    parser.parse(args.size(), argv.data());
    
    ASSERT_EQ("input.txt", parser.get_value<std::string>("file"));
    ASSERT_EQ(42, parser.get_value<int>("number"));
}

TEST(positional_arguments) {
    CmdLineParser parser("test");
    parser.add_flag("v", "verbose", "Verbose output");
    
    auto args = std::vector<std::string>{"test", "-v", "file1.txt", "file2.txt"};
    auto argv = make_argv(args);
    
    parser.parse(args.size(), argv.data());
    
    ASSERT_TRUE(parser.get_value<bool>("verbose"));
    auto positional = parser.get_positional_args();
    ASSERT_EQ(2, positional.size());
    ASSERT_EQ("file1.txt", positional[0]);
    ASSERT_EQ("file2.txt", positional[1]);
}

TEST(error_handling) {
    CmdLineParser parser("test");
    parser.add_option("r", "required", "Required option", std::string{}).set_required(true);
    
    auto args = std::vector<std::string>{"test"};
    auto argv = make_argv(args);
    
    ASSERT_THROWS(parser.parse(args.size(), argv.data()), MissingRequiredError);
}
```

**阶段4检查点**：

- [ ] 测试框架搭建完成
- [ ] 单元测试覆盖率 > 80%
- [ ] 集成测试覆盖主要使用场景
- [ ] 所有测试都能通过
- [ ] 使用CI/CD自动运行测试

### 阶段5：高级功能实现（第6天）

#### 帮助信息生成

```cpp
// include/help_formatter.h
#pragma once

#include <string>
#include <vector>

namespace cmdline {

class CmdLineParser;

class HelpFormatter {
public:
    struct Style {
        int max_width = 80;           // 最大行宽
        int option_indent = 2;        // 选项缩进
        int description_indent = 24;  // 描述缩进
        std::string section_separator = "\n";
        bool color_support = false;   // 是否支持颜色输出
    };
    
    explicit HelpFormatter(Style style = Style{});
    
    std::string format_help(const CmdLineParser& parser) const;
    std::string format_usage(const CmdLineParser& parser) const;
    std::string format_options(const CmdLineParser& parser) const;
    
private:
    Style style_;
    
    std::string wrap_text(const std::string& text, int width, int indent = 0) const;
    std::string colorize(const std::string& text, const std::string& color) const;
};

} // namespace cmdline
```

#### 参数组和验证

```cpp
// include/option_group.h
#pragma once

#include <memory>
#include <string>
#include <vector>

namespace cmdline {

class Option;

/**
 * @brief 选项组 - 用于组织相关选项
 */
class OptionGroup {
public:
    enum class Type {
        NORMAL,        // 普通组
        MUTUALLY_EXCLUSIVE,  // 互斥组
        REQUIRED_GROUP  // 必需组（至少选择一个）
    };
    
    OptionGroup(std::string name, std::string description, Type type = Type::NORMAL);
    
    void add_option(Option* option);
    void validate() const;
    
    const std::string& get_name() const { return name_; }
    const std::string& get_description() const { return description_; }
    Type get_type() const { return type_; }
    const std::vector<Option*>& get_options() const { return options_; }
    
private:
    std::string name_;
    std::string description_;
    Type type_;
    std::vector<Option*> options_;
};

} // namespace cmdline
```

#### 性能优化考虑

```cpp
// 字符串处理优化
namespace cmdline {
namespace utils {

// 避免不必要的字符串拷贝
class StringRef {
public:
    StringRef(const char* data, size_t size) : data_(data), size_(size) {}
    StringRef(const std::string& str) : data_(str.data()), size_(str.size()) {}
    
    bool starts_with(const StringRef& prefix) const {
        return size_ >= prefix.size_ && 
               std::memcmp(data_, prefix.data_, prefix.size_) == 0;
    }
    
    StringRef substr(size_t pos, size_t len = std::string::npos) const {
        size_t actual_len = (len == std::string::npos) ? size_ - pos : std::min(len, size_ - pos);
        return StringRef(data_ + pos, actual_len);
    }
    
    std::string to_string() const { return std::string(data_, size_); }
    
private:
    const char* data_;
    size_t size_;
};

// 高效的字符串分割
std::vector<StringRef> split(StringRef str, char delimiter) {
    std::vector<StringRef> result;
    size_t start = 0;
    
    for (size_t i = 0; i <= str.size_; ++i) {
        if (i == str.size_ || str.data_[i] == delimiter) {
            if (i > start) {
                result.emplace_back(str.data_ + start, i - start);
            }
            start = i + 1;
        }
    }
    
    return result;
}

} // namespace utils
} // namespace cmdline
```

**阶段5检查点**：

- [ ] 帮助信息格式美观，信息完整
- [ ] 参数组功能正常工作
- [ ] 性能测试通过（处理大量参数时）
- [ ] 内存使用合理（valgrind检查）

### 阶段6：文档和示例（第7天）

#### API文档编写

```cpp
/**
 * @file cmdline_parser.h
 * @brief Modern C++ Command Line Parser Library
 * @author Your Name
 * @date 2025
 * @version 1.0.0
 * 
 * This library provides a modern, type-safe command line argument parser
 * for C++ applications. It supports both POSIX and GNU style options,
 * automatic help generation, and extensive validation capabilities.
 * 
 * @example Basic Usage
 * @code
 * #include "cmdline_parser.h"
 * 
 * int main(int argc, char* argv[]) {
 *     cmdline::CmdLineParser parser("myapp", "My Application");
 *     
 *     parser.add_flag("h", "help", "Show help message");
 *     parser.add_option("f", "file", "Input file", std::string{})
 *           .set_required(true);
 *     
 *     try {
 *         parser.parse(argc, argv);
 *         
 *         if (parser.get_value<bool>("help")) {
 *             std::cout << parser.generate_help() << std::endl;
 *             return 0;
 *         }
 *         
 *         std::string filename = parser.get_value<std::string>("file");
 *         // ... process file
 *         
 *     } catch (const cmdline::ParseError& e) {
 *         std::cerr << e.what() << std::endl;
 *         return 1;
 *     }
 *     
 *     return 0;
 * }
 * @endcode
 */
```

#### 完整示例程序

```cpp
// examples/file_processor.cpp
/**
 * @brief 文件处理工具示例
 * 
 * 这个示例展示了如何使用cmdline parser创建一个
 * 功能完整的命令行工具，包括：
 * - 多种类型的选项
 * - 参数验证
 * - 互斥选项组
 * - 详细的帮助信息
 */

#include "cmdline_parser.h"
#include <fstream>
#include <iostream>

class FileProcessor {
public:
    struct Config {
        std::string input_file;
        std::string output_file;
        bool verbose = false;
        bool dry_run = false;
        int threads = 1;
        std::string format = "text";
        std::vector<std::string> filters;
    };
    
    static Config parse_arguments(int argc, char* argv[]) {
        cmdline::CmdLineParser parser("file-processor", 
            "A powerful file processing utility with support for multiple formats and filters.");
        
        // 基本选项
        parser.add_flag("h", "help", "Show this help message and exit");
        parser.add_flag("v", "verbose", "Enable verbose output");
        parser.add_flag("n", "dry-run", "Show what would be done without actually doing it");
        
        // 文件选项
        parser.add_option("i", "input", "Input file path", std::string{})
              .set_required(true)
              .set_validator([](const cmdline::OptionValue& value) {
                  std::string path = std::get<std::string>(value);
                  std::ifstream file(path);
                  return file.good();
              });
        
        parser.add_option("o", "output", "Output file path", std::string{});
        
        // 处理选项
        parser.add_option("j", "threads", "Number of processing threads", 1)
              .set_validator([](const cmdline::OptionValue& value) {
                  int threads = std::get<int>(value);
                  return threads >= 1 && threads <= 32;
              });
        
        parser.add_option("f", "format", "Output format", std::string{"text"})
              .set_validator([](const cmdline::OptionValue& value) {
                  std::string format = std::get<std::string>(value);
                  return format == "text" || format == "json" || format == "xml";
              });
        
        // 创建互斥组
        auto& mode_group = parser.add_group("mode", "Processing mode", 
                                          cmdline::OptionGroup::Type::MUTUALLY_EXCLUSIVE);
        
        auto& compress_opt = parser.add_flag("c", "compress", "Compress output");
        auto& decompress_opt = parser.add_flag("d", "decompress", "Decompress input");
        
        mode_group.add_option(&compress_opt);
        mode_group.add_option(&decompress_opt);
        
        try {
            parser.parse(argc, argv);
            
            if (parser.get_value<bool>("help")) {
                std::cout << parser.generate_help() << std::endl;
                std::exit(0);
            }
            
            Config config;
            config.input_file = parser.get_value<std::string>("input");
            config.output_file = parser.get_value<std::string>("output");
            config.verbose = parser.get_value<bool>("verbose");
            config.dry_run = parser.get_value<bool>("dry-run");
            config.threads = parser.get_value<int>("threads");
            config.format = parser.get_value<std::string>("format");
            
            // 获取位置参数作为过滤器
            auto positional = parser.get_positional_args();
            config.filters.assign(positional.begin(), positional.end());
            
            return config;
            
        } catch (const cmdline::ParseError& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::cerr << "\nUse --help for usage information." << std::endl;
            std::exit(1);
        }
    }
    
    void process(const Config& config) {
        if (config.verbose) {
            std::cout << "Processing configuration:\n";
            std::cout << "  Input: " << config.input_file << "\n";
            std::cout << "  Output: " << (config.output_file.empty() ? "<stdout>" : config.output_file) << "\n";
            std::cout << "  Format: " << config.format << "\n";
            std::cout << "  Threads: " << config.threads << "\n";
            std::cout << "  Filters: ";
            for (const auto& filter : config.filters) {
                std::cout << filter << " ";
            }
            std::cout << "\n";
        }
        
        if (config.dry_run) {
            std::cout << "Dry run mode - no changes will be made.\n";
            return;
        }
        
        // 实际的处理逻辑...
        std::cout << "Processing complete.\n";
    }
};

int main(int argc, char* argv[]) {
    FileProcessor processor;
    auto config = FileProcessor::parse_arguments(argc, argv);
    processor.process(config);
    return 0;
}
```

## 项目质量保证

### 代码审查清单

**设计质量**

- [ ] 类的职责单一且明确
- [ ] 接口设计符合最少惊讶原则
- [ ] 使用了适当的设计模式
- [ ] 异常安全性得到保证

**代码质量**

- [ ] 遵循一致的命名规范
- [ ] 函数长度合理（< 50行）
- [ ] 避免了代码重复
- [ ] 注释清晰且有意义

**现代C++实践**

- [ ] 优先使用栈分配而非堆分配
- [ ] 正确使用智能指针
- [ ] 利用移动语义避免不必要拷贝
- [ ] 使用const正确性

**性能考虑**

- [ ] 避免了不必要的内存分配
- [ ] 字符串处理高效
- [ ] 算法复杂度合理

### 常见陷阱和解决方案

#### 1. 内存安全问题

```cpp
// ❌ 危险：悬空指针
class BadParser {
    std::vector<Option*> options;  // 指向可能被销毁的对象
public:
    void add_option(Option& opt) {
        options.push_back(&opt);  // 如果opt是临时对象就危险了
    }
};

// ✅ 安全：拥有所有权
class GoodParser {
    std::vector<std::unique_ptr<Option>> options;
public:
    Option& add_option(/* 参数 */) {
        auto opt = std::make_unique<Option>(/* 参数 */);
        Option& ref = *opt;
        options.push_back(std::move(opt));
        return ref;  // 返回引用支持链式调用
    }
};
```

#### 2. 异常安全问题

```cpp
// ❌ 不安全：可能泄露资源
void parse(int argc, char* argv[]) {
    auto opt = new Option();  // 可能抛异常
    process(opt);             // 如果这里抛异常，opt泄露
    delete opt;
}

// ✅ 异常安全：RAII
void parse(int argc, char* argv[]) {
    auto opt = std::make_unique<Option>();
    process(*opt);  // 即使抛异常，opt也会自动销毁
}
```

#### 3. 性能陷阱

```cpp
// ❌ 低效：重复查找
for (const auto& arg : args) {
    if (short_map.find(arg) != short_map.end()) {
        auto option = short_map[arg];  // 又查找了一次！
        option->set_flag();
    }
}

// ✅ 高效：缓存查找结果
for (const auto& arg : args) {
    auto it = short_map.find(arg);
    if (it != short_map.end()) {
        it->second->set_flag();
    }
}
```

### 调试和诊断

#### 调试构建配置

```cmake
# Debug构建时添加调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # 启用所有警告
    if(GCC OR CLANG)
        add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    
    # 启用sanitizer
    add_compile_options(-fsanitize=address -fsanitize=undefined)
    add_link_options(-fsanitize=address -fsanitize=undefined)
    
    # 启用调试符号
    add_compile_options(-g3 -O0)
endif()
```

#### 日志系统

```cpp
// include/debug_utils.h
#pragma once

#ifdef DEBUG_BUILD
#include <iostream>
#define DEBUG_LOG(msg) \
    std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " " << msg << std::endl
#else
#define DEBUG_LOG(msg) ((void)0)
#endif

#define TRACE_FUNCTION() \
    DEBUG_LOG("Entering function: " << __FUNCTION__)
```

### 持续集成配置

#### GitHub Actions示例

```yaml
# .github/workflows/ci.yml
name: CI

on: [push, pull_request]

jobs:
  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        compiler: [gcc, clang]
        exclude:
          - os: windows-latest
            compiler: clang
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup C++
      uses: aminya/setup-cpp@v1
      with:
        compiler: ${{ matrix.compiler }}
        cmake: true
        ninja: true
    
    - name: Build
      run: |
        mkdir build
        cd build
        cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
        ninja
    
    - name: Test
      run: |
        cd build
        ctest --output-on-failure
    
    - name: Run examples
      run: |
        cd build
        ./examples/file_processor --help
```

## 项目交付

### 发布清单

- [ ] 所有功能按预期工作
- [ ] 测试覆盖率 ≥ 90%
- [ ] 文档完整（README、API文档、示例）
- [ ] 性能基准测试通过
- [ ] 内存泄露检查通过（valgrind）
- [ ] 多平台编译测试通过
- [ ] 代码审查完成

### 最终目录结构

```
cmdline-parser/
├── README.md                 # 项目概述和快速开始
├── CMakeLists.txt           # 根构建文件
├── .gitignore               # Git忽略规则
├── .clang-format           # 代码格式化配置
├── LICENSE                  # 开源协议
├── include/                 # 公共头文件
│   ├── cmdline_parser.h
│   ├── option.h
│   ├── cmdline_exceptions.h
│   └── help_formatter.h
├── src/                     # 实现文件
│   ├── CMakeLists.txt
│   ├── cmdline_parser.cpp
│   ├── option.cpp
│   └── help_formatter.cpp
├── tests/                   # 测试文件
│   ├── CMakeLists.txt
│   ├── test_main.cpp
│   ├── test_option.cpp
│   ├── test_parser.cpp
│   └── test_integration.cpp
├── examples/                # 示例程序
│   ├── CMakeLists.txt
│   ├── basic_example.cpp
│   ├── advanced_example.cpp
│   └── file_processor.cpp
├── docs/                    # 文档
│   ├── api_reference.md
│   ├── design_decisions.md
│   └── performance_notes.md
├── scripts/                 # 构建和开发脚本
│   ├── build.sh
│   ├── format.sh
│   └── check_env.sh
└── .github/workflows/       # CI/CD配置
    └── ci.yml
```

通过这个详尽的蓝图，你将不仅完成一个功能完善的命令行解析器，更重要的是养成优秀的C++开发习惯，为后续的项目打下坚实基础。记住，重要的不是完成项目，而是在过程中学到的知识和技能！