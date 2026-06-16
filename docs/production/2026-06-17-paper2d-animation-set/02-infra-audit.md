---
unit_id: 2026-06-17-paper2d-animation-set
status: audited
owner: asset-pipeline
updated_at: 2026-06-17T02:16:00
source_commit: 24a3c4f
depends_on: [2026-06-16-mvp-survival-loop]
parallel_lock: Ocean.Paper2DAnimationSet
---

# Paper2D / HD2D 角色动作集 — 基础设施审计

## 审计结论

| 需求 | 状态 | 证据 / 说明 |
|---|---|---|
| 参考图可读 | supported | `D:\UE5 demo\Paper2d\153F5D01C26C6D6BEEE2EEE1C4A39F9E.jpg` 已作为角色外观参考 |
| AI Game Workbench 可用 | supported | `D:\UE5 demo\Paper2d` 已包含解压后的工具包和导出目录 |
| 生图默认输出可追踪 | supported | Codex 生图输出保存于 `C:\Users\shxuw\.codex\generated_images\019ecf40-22e1-7f61-b85e-4055c76d68a9` |
| 绿幕去除 | supported | 使用 `C:\Users\shxuw\.codex\skills\.system\imagegen\scripts\remove_chroma_key.py` |
| 单动作 `8x4` 生成 | supported | `D:\UE5 demo\Paper2d\Export\OceanSurvivor\regen_blocks_8x4` |
| 总 atlas 合成 | supported | `D:\UE5 demo\Paper2d\Export\OceanSurvivor\atlas_final_8x20` |
| UE 内正式 Paper2D 导入 | partial | 当前只产出外部 PNG/帧序列；尚未通过 UE 编辑器导入 Texture/Sprite/Flipbook |
| Gameplay 状态机接入 | missing | 后续需要接入 `BP_OceanSurvivorCharacter` 的表现层，而不替换 Pawn 职责 |

## 禁止回退

- 不把单次 `8x20` 直生稿作为最终导入源。
- 不用四帧 `idle` 冒充最终动作集；最终候选必须统一每方向 8 帧。
- 不在本轮修改或提交 `BP_OceanSurvivorCharacter.uasset` 的现有未提交改动。

