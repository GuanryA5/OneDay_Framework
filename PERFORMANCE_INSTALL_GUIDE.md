# 高性能依赖安装指南

## 🚀 优化后的安装脚本功能

`install_dependencies.ps1` 脚本已经过性能优化，支持以下高性能特性：

### 🔧 新增参数

| 参数 | 说明 | 示例 |
|------|------|------|
| `-MaxJobs <数量>` | 指定并行编译任务数 | `-MaxJobs 8` |
| `-UseCache` | 启用二进制缓存加速 | `-UseCache` |
| `-MaxPerformance` | 最大性能模式 | `-MaxPerformance` |

### 🎯 性能优化特性

1. **自动CPU检测**: 自动检测CPU核心数并设置最优并行任务数
2. **内存优化**: 根据系统内存自动启用高级特性
3. **二进制缓存**: 重复安装时大幅提升速度
4. **网络优化**: 增加下载超时时间，减少网络错误
5. **磁盘空间检查**: 安装前检查可用空间
6. **性能统计**: 显示详细的安装时间和速度统计

## 🚀 使用方法

### 方案1：最大性能模式（推荐）
```powershell
# 使用所有CPU核心，启用所有优化
.\install_dependencies.ps1 -MaxPerformance -Fast
```

### 方案2：自定义并行任务数
```powershell
# 指定使用8个并行任务
.\install_dependencies.ps1 -MaxJobs 8 -UseCache -Fast
```

### 方案3：标准优化模式
```powershell
# 使用缓存，保留1个CPU核心给系统
.\install_dependencies.ps1 -UseCache -Fast
```

### 方案4：跳过大型包的快速安装
```powershell
# 跳过Qt和OpenCV，只安装核心依赖
.\install_dependencies.ps1 -SkipQt -SkipOpenCV -MaxPerformance -Fast
```

## 📊 性能对比

| 模式 | CPU使用率 | 预估时间 | 适用场景 |
|------|-----------|----------|----------|
| 标准模式 | ~75% | 40-80分钟 | 稳定安装，系统响应优先 |
| 高性能模式 | ~90% | 20-40分钟 | 快速安装，性能优先 |
| 最大性能模式 | ~100% | 15-30分钟 | 最快安装，专用编译机器 |

## 🔧 系统要求建议

### 最佳性能配置
- **CPU**: 8核心以上
- **内存**: 16GB以上
- **存储**: SSD硬盘，至少15GB可用空间
- **网络**: 稳定的宽带连接

### 最小配置
- **CPU**: 4核心
- **内存**: 8GB
- **存储**: 至少15GB可用空间

## 🎯 实际使用示例

### 示例1：开发机器快速安装
```powershell
# 适合开发机器，平衡性能和系统响应
.\install_dependencies.ps1 -MaxJobs 6 -UseCache -Fast
```

### 示例2：CI/CD服务器最大性能
```powershell
# 适合专用构建服务器，追求最快速度
.\install_dependencies.ps1 -MaxPerformance -Fast
```

### 示例3：低配置机器稳定安装
```powershell
# 适合配置较低的机器，确保稳定性
.\install_dependencies.ps1 -MaxJobs 2 -UseCache
```

## 📈 性能监控

脚本会自动显示以下性能信息：

### 安装前检查
- CPU核心数检测
- 内存容量检测
- 磁盘空间检查
- 优化参数配置

### 安装过程中
- 每个包的安装时间
- 实时性能统计
- 错误和警告信息

### 安装完成后
- 总安装时间
- 平均每包时间
- CPU利用率统计
- 性能优化建议

## 🛠️ 故障排除

### 常见问题

1. **内存不足**
   ```powershell
   # 减少并行任务数
   .\install_dependencies.ps1 -MaxJobs 2
   ```

2. **磁盘空间不足**
   ```powershell
   # 清理vcpkg缓存
   vcpkg remove --outdated
   ```

3. **网络超时**
   ```powershell
   # 脚本已自动增加超时时间到10分钟
   # 如仍有问题，检查网络连接或代理设置
   ```

4. **编译错误**
   ```powershell
   # 重新安装失败的包
   vcpkg install <package-name>:x64-windows --recurse
   ```

### 性能调优建议

1. **关闭不必要的程序**: 释放CPU和内存资源
2. **使用SSD**: 大幅提升I/O性能
3. **稳定网络**: 确保下载不中断
4. **充足电源**: 笔记本电脑建议插电使用

## 🎉 预期效果

使用优化后的脚本，你应该能看到：

- ✅ **安装时间减少50-70%**
- ✅ **CPU利用率提升到80-100%**
- ✅ **自动错误恢复和重试**
- ✅ **详细的性能统计报告**
- ✅ **智能的系统资源管理**

## 🚀 立即开始

选择适合你系统的命令并执行：

```powershell
# 推荐：最大性能模式
.\install_dependencies.ps1 -MaxPerformance -Fast

# 或者：保守的高性能模式  
.\install_dependencies.ps1 -MaxJobs 6 -UseCache -Fast
```

安装完成后，你就可以享受高性能的C++开发环境了！
