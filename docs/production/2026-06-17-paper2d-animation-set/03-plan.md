---
unit_id: 2026-06-17-paper2d-animation-set
status: planned
owner: asset-pipeline
updated_at: 2026-06-17T02:16:00
source_commit: 24a3c4f
depends_on: [2026-06-16-mvp-survival-loop]
parallel_lock: Ocean.Paper2DAnimationSet
---

# Paper2D / HD2D 角色动作集 — 制作计划

## 资产制作顺序

1. 读取参考图，固定角色识别特征：白发、灰绿外套、黑裙、单侧深色长袜、靴子、工具包。
2. 以 `#00ff00` 纯色绿幕背景生成每个动作的四方向 sprite sheet。
3. 使用本地 chroma-key 工具转为透明 PNG。
4. 将每个 sheet 归一到 `178x222` 单元，并拆成单帧目录。
5. 写入 manifest，记录帧数、方向、推荐 FPS、UE 导入参数。
6. 在 UE 中导入 PNG，按 Grid 切分 Paper2D sprites，再创建 flipbook。

## UE 接入策略

- 先保持 `BP_TopDownCharacter` / `BP_OceanSurvivorCharacter` 的当前控制链路，不因贴图工作重写 Pawn。
- 角色朝向使用四方向枚举：`South`、`West`、`East`、`North`。
- 动作状态使用最小状态机：`Grounded`、`WaterSurface`、`Climbing`、`Jumping`、`DivingSuit`。
- 输入与动画解耦：`WASD` 只产出移动向量，动画由“移动向量 + 当前运动介质 + 过渡状态”决定。

## 待后续 UE 验证

- `WASD` 当前“都朝右走”的 bug 需要单独排查输入轴、Controller Rotation、Movement Component 和相机坐标转换。
- Paper2D 资源导入后需要检查 pivot，建议陆地类为 Bottom Center，游泳 / 潜水类可用 Center 或 capsule 驱动位置。
- `climb` 需要和船体 / 岛岸交互点绑定，不能仅靠动画本身决定落点。

