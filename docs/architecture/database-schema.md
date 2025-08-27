# Database Schema

**本地文件存储架构：** 采用JSON + SQLite混合存储方案

## 项目配置文件 (project.json)
```json
{
  "projectId": "string",
  "name": "string", 
  "version": "semver",
  "createdAt": "timestamp",
  "modifiedAt": "timestamp",
  "settings": {
    "targetGame": "string",
    "defaultTimeout": "number",
    "executionMode": "string"
  }
}
```

## 蓝图数据文件 (blueprint.json)
```json
{
  "graphId": "string",
  "nodes": [
    {
      "id": "string",
      "type": "string",
      "position": {"x": "number", "y": "number"},
      "parameters": "object",
      "comment": "string"
    }
  ],
  "connections": [
    {
      "from": "string",
      "to": "string", 
      "fromPort": "string",
      "toPort": "string"
    }
  ]
}
```

## 执行日志数据库 (logs.sqlite)
```sql
CREATE TABLE execution_logs (
  id INTEGER PRIMARY KEY,
  session_id TEXT,
  timestamp DATETIME,
  node_id TEXT,
  event_type TEXT,
  message TEXT,
  duration_ms INTEGER
);

CREATE INDEX idx_logs_session ON execution_logs(session_id);
CREATE INDEX idx_logs_timestamp ON execution_logs(timestamp);
```
