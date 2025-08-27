# Security

## Input Validation
- **Validation Library:** 自定义验证框架基于Qt Validator
- **Validation Location:** UI输入层和业务逻辑层双重验证
- **Required Rules:**
  - 所有用户输入必须经过白名单验证
  - 文件路径输入防止目录遍历攻击
  - 蓝图节点参数类型和范围验证

## Authentication & Authorization
- **Auth Method:** 本地桌面应用无需网络认证
- **Session Management:** 本地会话管理，自动保存用户设置
- **Required Patterns:**
  - 敏感配置文件权限控制
  - 临时文件安全清理

## Secrets Management
- **Development:** 开发环境无敏感密钥
- **Production:** 用户本地配置加密存储
- **Code Requirements:**
  - 不在代码中硬编码任何密钥或配置
  - 使用Windows CryptAPI加密敏感配置
  - 日志和错误信息不包含敏感数据

## API Security
- **Rate Limiting:** 本地应用无需API限流
- **CORS Policy:** 不适用于桌面应用
- **Security Headers:** 不适用于桌面应用
- **HTTPS Enforcement:** 不适用于桌面应用

## Data Protection
- **Encryption at Rest:** 用户项目文件可选AES加密
- **Encryption in Transit:** 不适用于本地应用
- **PII Handling:** 不收集用户个人信息，匿名使用统计
- **Logging Restrictions:** 禁止记录文件路径、用户输入内容

## Dependency Security
- **Scanning Tool:** GitHub Dependabot监控依赖漏洞
- **Update Policy:** 每月检查依赖更新，及时修复安全漏洞
- **Approval Process:** 新增第三方库需要安全评估

## Security Testing
- **SAST Tool:** 集成clang静态分析和PVS-Studio
- **DAST Tool:** 不适用于桌面应用
- **Penetration Testing:** 每6个月进行安全代码审计
