---
name: ue-tdd-livecoding
description: UE5 no-LiveCoding TDD workflow —— 严格 TDD 五阶段流程：规划测试日志 → 打桩 UE_LOG → 实现功能 → bridge 保存 → 关闭编辑器 → 冷编译 → PIE 捕获日志 → 分析修复。触发：修改 C++ 后需要测试或 TDD 验证。禁止 Live Coding/Hot Reload。
---

# UE TDD No-LiveCoding Skill

## 强制规则 (NEVER SKIP)

**写任何功能代码之前，必须先写好测试日志桩。** 这条规则没有例外。如果你跳过打桩直接写实现代码，流程就是错误的。

## 五阶段流程

### Phase 1 — 规划 (Plan)

在写任何代码之前：
1. 明确要测试的变量或行为
2. 定义 `UE_LOG` 输出格式：
   - Category: 使用相关 LogCategory（如 `LogOcean`）
   - Verbosity: `Log` 或 `Warning`
   - 前缀: `[TDD]` （必须，用于 pipeline 过滤）
   - 格式: `[TDD] CheckName: actual=%d expected=%d` 或 `[TDD] CheckName: result=%s`
3. 列出所有测试检查点及其预期值

示例规划（在修改代码前写在对话中）：
```
Test point 1: [TDD] BuildFootprintCellCount: actual=%d expected=6
  — 在建造 footprint 生成后打印占用格子数量

Test point 2: [TDD] FloatingActorHasBuoyancy: result=%s
  — 创建漂浮模块或资源 Actor 后验证 BuoyancyComponent 存在
```

### Phase 2 — 打桩 (Instrument)

**在实现代码中插入 UE_LOG 语句。此阶段不修改任何功能逻辑。**

```cpp
// 示例：在 Ocean placement 或 resource spawn 检查点添加
UE_LOG(LogOcean, Log, TEXT("[TDD] BuildFootprintCellCount: actual=%d expected=6"), Footprint.Num());
UE_LOG(LogOcean, Log, TEXT("[TDD] FloatingActorHasBuoyancy: result=%s"), HasBuoyancy ? TEXT("PASS") : TEXT("FAIL"));
```

规则：
- 只添加 `UE_LOG`，不改功能代码
- 每条日志必须有 `[TDD]` 前缀
- 用 `actual=` 记录实际值，`expected=` 记录预期值
- 对非数值断言用 `result=PASS` 或 `result=FAIL`

### Phase 3 — 实现 (Implement)

打桩完成后，编写功能代码。保留所有 `UE_LOG([TDD]...)` 语句——它们作为运行时断言。

### Phase 4 — 构建运行 (Build & Run)

**禁止 Live Coding / Hot Reload。** 如果编辑器已运行，先通过 UnrealBridge 保存 dirty packages，保存成功后关闭编辑器，再冷编译。不要用 `LiveCoding.Compile`，不要在编辑器开着时用 `--no-launch` 当作编译验证。

调用 pipeline：
```bash
python scripts/ue_tdd_pipeline.py --pie-duration 5
```

流水线自动执行：
1. 若编辑器正在运行，使用 UnrealBridge 执行 `unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)` 并确认保存结果
2. 保存成功后关闭编辑器
3. `Build.bat OceanEditor Win64 Development -NoHotReload` — 冷编译
4. 编译失败 → 报告编译错误，修复后重新执行 step 3
5. `Start-Process UnrealEditor.exe` — 后台启动编辑器
6. 等待 UnrealBridge TCP 连接就绪（最多 120s）
7. `Editor.clear_log_buffer()` — 清空日志缓冲
8. `Editor.write_log_message("[TDD] Pipeline start")` — 写入起始标记
9. `Editor.start_pie()` — 启动 Play-in-Editor
10. 等待 `--pie-duration` 秒（默认 5s）
11. `Editor.get_recent_log_lines(num_lines=300)` — 捕获日志
12. `Editor.stop_pie()` — 停止 PIE

对于日志快速检查（编辑器已运行）：
```bash
python scripts/ue_tdd_pipeline.py --check-only
```
`--check-only` 只允许读取日志，不算 C++ 编译验证。

### Phase 5 — 分析 (Analyze)

Pipeline 自动分析捕获的 `[TDD]` 行：
- 解析 `actual=%d expected=%d` 模式
- actual == expected → PASS
- actual != expected → FAIL（报告差异）
- 含 PASS/OK → PASS
- 含 FAIL/ERROR → FAIL

输出报告示例：
```
=== TDD Report ===
Total [TDD] lines: 3
Passed:            2
Failed:            1

--- Details ---
[PASS] [TDD] BuildFootprintCellCount: actual=6 expected=6
[PASS] [TDD] FloatingActorHasBuoyancy: result=PASS
[FAIL] [TDD] ResourceSpawnCount: actual=3 expected=5
        expected=5  actual=3
```

**有失败** → 定位 bug → 修复 → 回到 Phase 4（不需要重新打桩，因为日志已经在了）

**全部通过** → 完成

## 编辑器管理

### 编辑器卡死时
```powershell
taskkill /f /im UnrealEditor.exe
```

### 编辑器启动后检查状态
```python
# via BridgeClient
client.get_editor_state()
client.is_in_pie()
```

### 禁止热编译

不要使用 Live Coding、Hot Reload、`LiveCoding.Compile` 或打开编辑器状态下的 `--no-launch` 编译。需要重新编译时，先 bridge 保存并关闭编辑器，再运行完整冷编译流水线。

## 关键约束

- **不要在 Phase 2 之前写功能代码**——先打桩
- **保持 `[TDD]` 日志在代码中**——它们是运行时断言，不是临时 debug 代码
- **不要使用 Live Coding / Hot Reload**——C++ 验证必须冷编译
- **编译前保存并关闭编辑器**——通过 UnrealBridge 保存 dirty packages 后再关
- **每次都清空日志缓冲**——`clear_log_buffer()` 在 `start_pie()` 之前
- **等待足够的 PIE 时间**——给物理模拟足够时间运行（至少 3-5 秒）
- **UnrealBridge 端口是动态的**——bridge client 通过 netstat 自动发现

## 故障排除

| 问题 | 解决方案 |
|---|---|
| Bridge 连接超时 | 编辑器加载慢，等待更久（最多 120s）或检查编辑器是否崩溃 |
| PIE 启动失败 | 确保 level 已加载，上一次 PIE 已正确关闭 |
| `[TDD]` 日志未出现 | 检查 UE_LOG category 和 verbosity；确认 processor 确实执行了 |
| 编译失败 | 修复编译错误后重新运行 pipeline（不指定 `--no-build`） |
| 编辑器卡死 | `taskkill /f /im UnrealEditor.exe`，等 2s 后重新启动 |

