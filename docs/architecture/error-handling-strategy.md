# Error Handling Strategy

## General Approach

- **Error Model:** 异常分层处理 - 系统级异常/业务逻辑异常/用户操作异常
- **Exception Hierarchy:** std::exception为基类的自定义异常体系
- **Error Propagation:** 底层模块抛出异常，上层模块捕获并转换为用户友好信息

## Logging Standards

- **Library:** spdlog 1.12+
- **Format:** 结构化JSON格式，支持多输出目标
- **Levels:** TRACE/DEBUG/INFO/WARN/ERROR/CRITICAL
- **Required Context:**
  - Correlation ID: UUID格式会话标识
  - Service Context: 模块名称和版本信息
  - User Context: 匿名用户ID和操作上下文

## Error Handling Patterns

### External API Errors
- **Retry Policy:** 指数退避重试，最大3次
- **Circuit Breaker:** 连续失败5次后断路30秒
- **Timeout Configuration:** 网络操作15秒，文件操作30秒，AI推理5秒
- **Error Translation:** 系统错误码映射为用户可理解的提示信息

### Business Logic Errors
- **Custom Exceptions:** BlueprintValidationError、NodeExecutionError、PathfindingError
- **User-Facing Errors:** 多语言错误提示，包含解决建议和帮助链接
- **Error Codes:** BPE-001格式的错误编码系统

### Data Consistency
- **Transaction Strategy:** 文件操作使用原子写入，内存数据使用RAII管理
- **Compensation Logic:** 操作失败时自动恢复到上一个稳定状态
- **Idempotency:** 所有用户操作支持安全重试
