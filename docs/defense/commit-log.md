# 《洋流》开发提交记录

> 用途：面向毕设答辩记录每次 Git 提交“做了什么、证明了什么、答辩能讲什么”。后续每个阶段提交后继续追加。

## 项目基础

### `06358b0` — Initial Ocean UE prototype

- 做了什么：建立 UE5.7 Ocean 原型项目基础。
- 证明了什么：项目可以作为独立 UE 工程继续迭代。
- 答辩价值：说明毕设从标准 UE 工程出发，后续功能均在该工程内逐步构建。

### `56492c2` — docs: add Ocean workflows and phase plan

- 做了什么：加入 Ocean 工作流和第一阶段规划。
- 证明了什么：项目开始有可追踪的开发流程、阶段边界和实现顺序。
- 答辩价值：可展示开发管理意识，不是只堆功能，而是按阶段推进。

### `425e713` — feat: add Ocean phase one foundations

- 做了什么：加入建造格子、模块定义、浮力占位 Actor、资源节点、资源场和 PCG/fallback 基础。
- 证明了什么：第一阶段核心技术基础已经存在，模块不是单格物品，而是可占用多个格子的建造单元。
- 答辩价值：可讲 UE 水体、浮力组件、PCG 资源散播、稳定逻辑格子之间的技术取舍。

## MVP 冻结与自动化

### `065269f` — docs: freeze Ocean MVP design and defense summary

- 做了什么：冻结“小船生存主循环 + 轻建造扩展”MVP；明确 WASD + F、保留左键点地、钓鱼/潜水后移。
- 证明了什么：项目范围从开放想象收敛为可实现、可测试、可答辩的第一阶段闭环。
- 答辩价值：可直接展示范围边界、技术取舍和后续计划。

### `5195d31` — docs: plan Ocean MVP survival loop

- 做了什么：将 MVP 拆成输入、生存背包、交互拾取、建造、HUD、地图资产、文档验收七步。
- 证明了什么：实现不是临时堆代码，而是按生产单元推进。
- 答辩价值：可展示任务分解、测试计划和阶段性验收方法。

### `650bdb0` — chore: add Ocean automation and WaterOcean baseline

- 做了什么：迁移 UnrealBridge、无 Live Coding 冷编译链路、生产单元校验和 WaterOcean 基线地图。
- 证明了什么：项目有保存、关闭编辑器、冷编译、重开项目、Bridge 验证的自动化链路。
- 答辩价值：可展示 UE 项目工程化与防热编译污染意识。

### `f388ebf` / `1349319` — pipeline hardening

- 做了什么：修复并加固 `--no-launch` 构建路径和检查逻辑。
- 证明了什么：自动化链路能区分“只编译”和“启动 PIE 验证”。
- 答辩价值：可解释为什么 C++ 变更必须冷编译，不依赖 Live Coding。

## 可玩主循环

### `5ffea35` — feat: add Ocean WASD input path

- 做了什么：新增 WASD 相机相对移动，同时保留左键点地移动。
- 证明了什么：双移动语义可以共存，WASD 不会删除模板 TopDown 操作。
- 答辩价值：可讲休闲玩家与键盘微操之间的 UX 取舍。

### `bdbdea4` — feat: add Ocean survival and inventory components

- 做了什么：新增生存状态与堆叠背包组件。
- 证明了什么：三条状态和资源消耗有独立可测的运行时基础。
- 答辩价值：可展示生存循环的数据基础，而不是只做场景。

### `6078aee` / `b5397b4` — interaction pickup and priority tests

- 做了什么：新增 F 交互组件、资源拾取流程和最近有效交互优先级测试。
- 证明了什么：左键移动与 F 交互语义分离，资源拾取有失败反馈和距离判断。
- 答辩价值：可讲输入冲突如何拆解，以及交互边界如何测试。

### `680a6c8` — feat: add Ocean deck build component

- 做了什么：新增建造模式、模块旋转、资源消耗、格子占用、浮力模块生成。
- 证明了什么：1x1 地板只是模块数据的一种，系统支持格子组、邻接和占用规则。
- 答辩价值：可展示建造系统不是摆模型，而是有逻辑网格和资源闭环。

### `dbc9da9` — feat: add Ocean MVP debug HUD

- 做了什么：新增 HUD 显示生存状态、背包、建造模式和交互提示。
- 证明了什么：MVP 可以被玩家和答辩观众直接理解当前状态。
- 答辩价值：可展示调试 UI 如何服务原型验证。

## 地图资产与验收

### `0c5b5ca` — content: set up Ocean MVP map assets

- 做了什么：生成 `L_WaterOcean` 的输入资产、角色/控制器/GameMode 蓝图、1x1 Deck 数据资产、starter 平台、资源场和 16 个资源节点。
- 证明了什么：MVP 不只在 C++ 层存在，已经落成可打开、可检查的 UE 内容资产。
- 答辩价值：可展示地图、蓝图、数据资产和脚本自动生成链路。

### `2067839` — fix: harden Ocean MVP setup verification

- 做了什么：补强 verify，检查 9 个输入映射、控制器绑定、GameMode 默认类、Deck runtime 数据和精确 16 个资源节点。
- 证明了什么：验证不只看“资产存在”，还检查资产是否能真的参与运行。
- 答辩价值：可讲二进制 UE 资产也能通过脚本做结构化验收。

### `3cfc434` — fix: auto bind Ocean build target platform

- 做了什么：让建造组件在未显式设置目标平台时自动找到场景中的浮动平台，并让角色 BP 默认选中 1x1 Deck 模块。
- 证明了什么：玩家进入建造模式后不会卡在“未选择模块/没有目标平台”的不可玩状态。
- 答辩价值：可展示从代码评审发现“可玩性断点”，再用红绿测试修复的过程。

### `a243627` — docs: refresh Ocean MVP verification metadata

- 做了什么：同步生产单元文档中的 source commit、验证结果和 active unit 数量。
- 证明了什么：开发记录和实际提交保持一致，方便答辩追溯。
- 答辩价值：可展示完整的工程留痕与阶段验收记录。

### `docs: define Ocean minimal loop workflow` — 本次文档规划提交

- 做了什么：阅读《洋流》DOCX 与当前 Ocean MVP，整理 `BP_OceanSurvivorCharacter` / `BP_TopDownCharacter` 的 Pawn 边界风险，并冻结下一步最小循环文档。
- 证明了什么：项目进入代码前先明确饥饿/脱水回复、背包、合成、拖拽、事件、放置区域、船体漂流和终点通关的边界。
- 答辩价值：可展示“GPT 负责 Spec/TDD 策略，用户确认后 DS 编码”的自动化协作流程。

### `docs: add WASD and Paper2D gates` — 本次补充规划提交

- 做了什么：补充用户实测的 WASD 全部朝右问题，定位为优先调查 `IMC_OceanMVP` Axis2D 映射；同时明确角色后续用 Paper2D / HD2D 表现。
- 证明了什么：输入 bug 不靠猜测修，先验证 W/S/A/D 的实际向量；Paper2D 只替换表现层，不替换 `BP_OceanSurvivorCharacter` 的玩法职责。
- 答辩价值：可展示美术风格方向与工程边界分离，且保留测试先行的调试流程。

### `docs: document Paper2D animation atlas workflow` — 本次资产流程提交

- 做了什么：围绕白发幸存者参考图生成 Paper2D / HD2D 占位动作集，覆盖待机、水面游泳、上岸/爬船、跳跃、潜水服潜水，每个动作四方向八帧。
- 证明了什么：单次 `8x20` 大图直生会出现行序错位和动作混入，改用“每动作 `8x4` 单独生成 → 脚本合成 `8x20` atlas”的流程后，可得到 160 帧且网格对齐的导入候选。
- 答辩价值：可展示 AI 美术资产并不是直接拿图用，而是经过规格冻结、失败方案对比、脚本合成和审计验证后进入 UE 资产管线。

### `docs: correct Paper2D atlas slicing ratio` — 本次切图修正提交

- 做了什么：重新测量五张绿幕源图，确认源尺寸均为 `1774x887`，自然单元是 `221.75x221.75`；最终采用 `222x222` 源定位格 + `288x288` 安全导入格。
- 证明了什么：`256x256` 不是这批绿幕图的真实切图比例，直接切会造成头部、鞋子或脚蹼贴边 / 裁切；安全版 160 帧审计问题数为 0。
- 答辩价值：可讲 AI 生成图进入引擎前必须做像素级资产验收，发现比例错误后通过脚本重新导出安全 atlas。

### `docs: align Paper2D import cell spec` — 本次口径清理提交

- 做了什么：清理语义文档中残留的 `178x222` / `256x256` 旧导入目标，统一为 `222x222` 源定位格与 `288x288` UE 安全导入格。
- 证明了什么：文档中的验收目标、测试结论和答辩 HTML 口径一致，不会误导后续 Paper2D 导入。
- 答辩价值：可展示工程文档不只是记录结果，也会反查并消除规格冲突。

### `fix: harden Ocean MVP validation workflow` — 本次验证硬化提交

- 做了什么：修复资源节点、背包 WBP、StatusPanel 自动化红测；把 MVP 验证脚本同步到 13 个输入映射、`AOceanMVPGameMode` 父类和 `PlayerStart_WaterOcean`；新增外部 HUD PIE 探针。
- 证明了什么：MVP 不再只靠编译或手动打开地图判断，而是可以通过项目测试前缀、Bridge 资产验证、真实 PIE HUD 日志形成闭环。
- 答辩价值：可展示“发现红测 → 定位根因 → 修测试/脚本 → 重跑验证 → 写入答辩记录”的工程化过程，并解释为什么不能用裸 `Automation RunTests Ocean` 作为验收命令。
