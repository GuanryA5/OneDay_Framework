# Test Strategy and Standards

## Testing Philosophy
- **Approach:** 测试驱动开发(TDD)，优先编写测试用例
- **Coverage Goals:** 单元测试80%代码覆盖率，关键路径100%覆盖
- **Test Pyramid:** 70%单元测试 + 20%集成测试 + 10%端到端测试

## Test Types and Organization

### Unit Tests
- **Framework:** Google Test 1.14+
- **File Convention:** `*_test.cpp`文件命名
- **Location:** `tests/unit/`目录
- **Mocking Library:** Google Mock进行依赖隔离
- **Coverage Requirement:** 核心业务逻辑80%覆盖率

**AI Agent Requirements:**
- 为所有public方法生成测试用例
- 覆盖边界条件和异常情况
- 遵循AAA模式（Arrange, Act, Assert）
- Mock所有外部依赖（文件系统、网络、GPU）

### Integration Tests
- **Scope:** 模块间接口测试，数据流验证
- **Location:** `tests/integration/`
- **Test Infrastructure:**
  - **Database:** SQLite内存数据库进行测试
  - **File System:** 临时目录隔离测试环境
  - **GPU/AI Models:** Mock推理结果进行测试

### End-to-End Tests
- **Framework:** Qt Test Framework
- **Scope:** 完整用户操作流程验证
- **Environment:** 虚拟化Windows测试环境
- **Test Data:** 预设游戏场景和蓝图模板

## Test Data Management
- **Strategy:** 工厂模式生成测试数据，避免硬编码
- **Fixtures:** `tests/fixtures/`目录存储测试资源
- **Factories:** TestDataFactory类生成各类测试对象
- **Cleanup:** RAII自动清理临时文件和内存

## Continuous Testing
- **CI Integration:** GitHub Actions自动运行全套测试
- **Performance Tests:** 基准测试监控性能回归
- **Security Tests:** 静态分析检查内存安全和数据泄露
