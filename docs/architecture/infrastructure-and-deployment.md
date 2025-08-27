# Infrastructure and Deployment

## Infrastructure as Code

- **Tool:** 无需IaC工具，本地桌面应用
- **Location:** `scripts/build/`
- **Approach:** CMake + vcpkg自动化构建

## Deployment Strategy

- **Strategy:** 桌面应用程序安装包部署
- **CI/CD Platform:** GitHub Actions
- **Pipeline Configuration:** `.github/workflows/build.yml`

## Environments

- **Development:** 开发者本地环境 - Windows 10/11 + Visual Studio 2022
- **Testing:** 自动化测试环境 - GitHub Actions Windows Runner
- **Production:** 用户桌面环境 - Windows 10/11发布版本

## Environment Promotion Flow

```
Development → Pull Request → Automated Testing → Release Build → Distribution
```

## Rollback Strategy

- **Primary Method:** 版本回退 + 用户数据备份恢复
- **Trigger Conditions:** 严重Bug报告、崩溃率>1%、性能下降>50%
- **Recovery Time Objective:** 24小时内发布修复版本
