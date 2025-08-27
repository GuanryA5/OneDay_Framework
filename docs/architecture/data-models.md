# Data Models

## BlueprintNode

**Purpose:** 蓝图编辑器中的基础节点类型，承载脚本逻辑的最小单元

**Key Attributes:**
- id: string - 唯一标识符
- type: NodeType - 节点类型枚举
- position: Point2D - 画布坐标位置
- inputs: vector<NodePort> - 输入端口列表
- outputs: vector<NodePort> - 输出端口列表
- parameters: map<string, Variant> - 节点参数配置
- comment: string - 用户备注信息
- state: NodeState - 执行状态

**Relationships:**
- 通过NodeConnection连接到其他节点
- 属于特定的BlueprintGraph
- 可包含子节点（复合节点）

## BlueprintGraph

**Purpose:** 完整的蓝图脚本结构，包含所有节点和连接关系

**Key Attributes:**
- id: string - 图表唯一标识
- name: string - 用户定义名称
- nodes: vector<BlueprintNode> - 节点列表
- connections: vector<NodeConnection> - 连接关系
- variables: map<string, Variable> - 图表变量
- metadata: GraphMetadata - 版本、创建时间等元信息

**Relationships:**
- 包含多个BlueprintNode
- 属于特定的Project
- 可以被ExecutionEngine执行

## ImageRecognitionModel

**Purpose:** AI图像识别模型的配置和状态管理

**Key Attributes:**
- modelPath: string - ONNX模型文件路径
- modelType: RecognitionType - 识别类型（模板匹配/对象检测等）
- inputSize: Size2D - 模型输入尺寸
- confidence: float - 置信度阈值
- preprocessConfig: PreprocessConfig - 预处理配置

**Relationships:**
- 被ImageProcessingNode引用
- 关联多个RecognitionResult

## PathfindingMap

**Purpose:** 寻路系统的地图数据结构

**Key Attributes:**
- mapId: string - 地图唯一标识
- dimensions: Size2D - 地图尺寸
- obstacles: vector<Polygon> - 障碍物区域
- safeZones: vector<Polygon> - 安全区域
- waypoints: vector<Point2D> - 路径点
- gridResolution: float - 网格精度

**Relationships:**
- 被PathfindingNode使用
- 关联多个PathfindingResult
