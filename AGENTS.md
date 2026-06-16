# Ocean — AI Agent Constraints

## Hard Constraints

### Rule Authority
- `AGENTS.md` is the canonical project rule file for agent behavior.
- If a mirrored workflow or skill conflicts with this file, update `AGENTS.md` first, then sync the mirror.
- Read `docs/design/agent-operating-guide.md` after this file when joining the project with low context.

### Editor Automation Rule

碰到 UE 编辑器操作（编译、PIE、创建资产、修改蓝图、运行测试）时，优先通过 UnrealBridge 自动执行，不要求用户手动点编辑器。

- 编辑器没启动 → 用 `python scripts/ue_tdd_pipeline.py` 自动启动。
- 编辑器已运行 → 用 `scripts/ue_tdd_bridge.py` 连接 UnrealBridge 执行 Python。
- 组播发现失败 → 从编辑器日志找 `LogUnrealBridge: Listening on 127.0.0.1:<port>`，或让 bridge client 走 netstat fallback。
- 编辑器可能有未保存改动且 bridge 连不上 → 不得直接 kill；先报告保存门阻塞。

### Compile Rule — No Live Coding

UE C++ 编译验证禁止走 Live Coding / Hot Reload。

- 编译前如果编辑器正在运行，必须先通过 UnrealBridge / Editor Python 保存 dirty packages。
- 保存成功后关闭编辑器，再运行 `Build.bat` 或 `python scripts/ue_tdd_pipeline.py` 的完整冷启动编译流程。
- `--check-only` 只能用于读取已运行编辑器日志；它不是 C++ 编译验证。

### Editor Save Gate

凡是通过 UE 编辑器或自动化脚本创建/修改关卡、蓝图、材质、PCG 图、项目设置后，关闭、重启、kill 编辑器或报告完成前必须先执行保存检查。

- 优先执行 `unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)`。
- 保存后验证关键资产/关卡存在且可加载。
- 测试报告中记录保存状态：`saved`、`save_failed` 或 `not_checked`。

### Ocean Prototype Rules

- 缺模型时先用立方体占位，后续可替换 Actor 资产。
- 漂浮模块、资源节点、水上道具 Actor 必须保留浮力路径；C++ 用 `UBuoyancyComponent`，蓝图继承时不能移除该组件。
- 建造系统采用“模块是格子组”：单个模块占用一个或多个 grid cells。
- 逻辑建造网格保持稳定；视觉浮动可以 bob，但不能让格子坐标跟浪漂。
- 水体必须启用 `Water` 插件，并在 `Config/DefaultEngine.ini` 保留 `WaterBodyCollision` profile。
- 资源散播优先走 `PCG` 插件；没有 PCG graph 资产时允许使用 deterministic fallback spawn 作为测试替代。

### TDD Discipline

写 UE C++ 功能前先定义可验证证据。

1. **Plan** — 定义要测试的变量、行为和预期 `[TDD]` 输出。
2. **Instrument** — 先加入 `[TDD]` 日志或 C++ automation test。
3. **Implement** — 编写功能代码。
4. **Build & Run** — no-LiveCoding 冷启动流程：bridge 保存 → 关闭编辑器 → Build.bat 编译 → 启动编辑器 → PIE → 捕获日志。
5. **Analyze** — 过滤 `[TDD]` 日志或 automation test 输出，对比实际值与预期值。

### Documentation Update Rules

| Event | Update Target |
|---|---|
| 约束/偏好/禁止项变更 | `AGENTS.md` |
| 子系统/数据流/模块关系变更 | `memory-bank/architecture.md` |
| 实现进度、验证证据、阻塞 | `memory-bank/progress.md` |
| 技术选型、插件、自动化脚本变更 | `memory-bank/tech-stack.md` |
| 多步骤实现计划 | `docs/superpowers/plans/*` |
| 冻结玩法/工具语义 | `docs/production/{date}-{unit}/01-semantics.md` |

### Git Upload Workflow

Before pushing:

1. Run verification appropriate to changed files.
2. Run `python scripts/doc_sync_hook.py --phase pre-commit --apply-memory --history --quiet` when committing or staging project-level work.
3. Run `git diff --check`.
4. Inspect `git status --short`; avoid staging unrelated junk.
5. Commit with a message that names the gameplay/tooling/docs scope.
6. Push the active branch to `origin`.

### Navigation

| Resource | Location |
|---|---|
| 低上下文 agent 接手指南 | `docs/design/agent-operating-guide.md` |
| 子系统架构 + 数据流 | `memory-bank/architecture.md` |
| 实现进度 | `memory-bank/progress.md` |
| 技术栈 + 依赖 | `memory-bank/tech-stack.md` |
| 项目工作流总览 | `docs/workflows/ocean-agent-workflows.md` |

### Available Project Skills

| Skill | Purpose |
|---|---|
| `unreal-bridge` | UE5 编辑器 TCP 桥接：执行 Python、控制 PIE、捕获日志、保存资产 |
| `ue-tdd-livecoding` | 历史名；实际是 no-LiveCoding TDD 流程 |
| `ocean-ai-production-hardness` | Ocean 原型生产闭环：冻结语义、审计基础设施、验证水体/建造/PCG/浮力 |
| `superpowers:*` | brainstorming、TDD、writing-plans、executing-plans、systematic-debugging、verification 等 |

### Pipeline Scripts

| Script | Purpose |
|---|---|
| `scripts/ue_tdd_bridge.py` | Python bridge client：端口发现、PIE 控制、日志捕获 |
| `scripts/ue_tdd_pipeline.py` | 全自动流水线：保存 → kill/close → build → launch → PIE → capture → analyze |
| `scripts/harness_state_validator.py` | 校验 `docs/production` 状态文件和 `parallel_lock` |
| `scripts/doc_sync_hook.py` | 显式/提交前文档漂移检测和 `progress.md` 有界快照 |
