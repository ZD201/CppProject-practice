    std::optional<std::string> value_;      // 【重要】
                                            // std::optional 是 C++17 引入的，
                                            // 表示一个“可能存在，也可能不存在”的值。
                                            // 如果 has_value_ 为 true 且用户提供了值，
                                            // 那么 value_ 会存储那个字符串值。
                                            // 如果没有值，它就是空的 (empty)，非常安全。
                                            // 比使用指针或特殊字符串（如空字符串）来表示“无值”更清晰和安全。