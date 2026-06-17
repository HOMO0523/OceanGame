# 《洋流》HUD 与右侧背包抽屉 UI/UX 设计冻结

## 1. 设计状态

- 状态：已获得用户认可，进入正式 spec。
- 范围：常驻 HUD、右侧背包抽屉、物品拖拽入口、放置预览入口。
- 当前阶段：只冻结 UI/UX、WBP 层级、数据边界和测试策略；不在本 spec 内直接实现 UMG 资产或 C++ 改动。

## 2. 设计来源

用户提供的交互草图定义了两种界面状态：

1. 背包关闭时：中央视野保持干净，只保留左上状态栏、上方时间、右上背包/方向信息、左下菜单。
2. 背包打开时：右侧滑出背包抽屉，玩家仍能看到小船和海面，不进入全屏菜单。

这套布局服务小船生存主循环：玩家在海面小船上观察状态压力、管理背包、拖拽放置物、继续拾取资源和扩展船体。

## 3. 目标

- 用低遮挡 HUD 支持固定 45–60 度俯视玩法。
- 让玩家不打开编辑器细节面板也能理解状态、时间、背包、方向、放置失败原因。
- 建立可扩展 WBP 层级，后续替换图片、图标、音效和美术资源时不需要重写交互结构。
- 将背包、物品使用、拖拽、建造放置通过清晰接口连接到现有生存、背包、建造格子系统。
- 为答辩提供可解释的 UI/UX 取舍：不是复杂 RPG 背包，而是服务小船生存闭环的轻量抽屉式背包。

## 4. 非目标

- 不做最终美术完成度。
- 不做全屏 RPG 装备栏。
- 不做复杂拆分堆叠、装备穿戴、快捷栏战斗技能。
- 不做钓鱼、完整潜水、上岛探索 UI。
- 不让 UI 直接修改格子占用或生成 Actor；UI 只发起命令，规则判断由 gameplay component 完成。

## 5. 常驻 HUD 布局

### 5.1 背包关闭状态

屏幕默认状态保持干净，避免遮挡小船、角色和漂浮物。

| 区域 | 内容 | 说明 |
|---|---|---|
| 左上 | 状态栏 | 体力、水分、饱食度。 |
| 上中 | 时间面板 | 显示第几天、早/中/晚、事件点进度。 |
| 右上 | 背包按钮、方向/月相信息 | 背包入口和漂流方向提示。 |
| 中央 | 小船与角色 | 不放大型 UI，保证可读性。 |
| 左下 | 菜单按钮 | 暂停、设置、返回标题等低频操作。 |

### 5.2 背包打开状态

右侧背包抽屉滑出，占据屏幕右侧约 28%–35% 宽度。中央小船区域仍可见，方便拖拽到场景。

背包打开时：

- 显示鼠标。
- 输入模式切换为 `Game and UI`。
- WASD 和左键移动暂停或弱化，避免拖拽时误移动。
- `Esc`、右键空白处、再次按背包键都可以关闭抽屉。

### 5.3 放置预览状态

当玩家从背包拖拽可放置物到场景，或从建造面板选择模块时，进入放置预览状态。

放置预览状态显示：

- 吸附到格子的预览模型。
- footprint 覆盖格高亮。
- 绿色表示可放置，红色表示不可放置。
- 不可放置时显示具体原因，例如材料不足、占用、未邻接、区域不允许、超出范围。

## 6. 输入规则

| 输入 | 行为 | 决策 |
|---|---|---|
| `Tab` / `I` | 打开或关闭背包抽屉。 | 推荐新增背包键，避免抢占当前 `B` 建造键。 |
| 点击右上背包图标 | 打开或关闭背包抽屉。 | 支持鼠标玩家。 |
| `B` | 保留当前建造切换。 | 当前项目已绑定建造，暂不改为背包。 |
| `F` | 场景交互。 | 拾取、事件、可交互物仍走统一交互。 |
| `R` | 放置预览时旋转模块。 | 沿用建造系统已有习惯。 |
| 左键 | 背包关闭时点地移动；放置预览时确认。 | 根据 UI 状态分流。 |
| 右键 / `Esc` | 取消拖拽、取消放置、关闭背包或关闭弹窗。 | 取消优先级从最内层 UI 向外层冒泡。 |

## 7. WBP 层级

### 7.1 根节点

`WBP_OceanHUDRoot`

- `CanvasRoot`
  - `Layer_Status`
    - `WBP_StatusPanel`
  - `Layer_Time`
    - `WBP_TimePanel`
  - `Layer_TopRight`
    - `WBP_TopRightPanel`
  - `Layer_Backpack`
    - `WBP_BackpackDrawer`
  - `Layer_Build`
    - `WBP_BuildPalette`
  - `Layer_Placement`
    - `WBP_PlacementOverlay`
  - `Layer_Modal`
    - `WBP_ConfirmModal`
  - `Layer_Toast`
    - `WBP_ToastStack`
  - `Layer_Menu`
    - `WBP_PauseMenuButton`

`WBP_OceanHUDRoot` 只负责层级管理、可见性和输入模式切换，不直接执行业务规则。

### 7.2 状态面板

`WBP_StatusPanel`

- `StaminaBar`
- `HydrationBar`
- `SatietyBar`
- `CriticalFlash`

状态条第一版可用纯色块或进度条。美术方向为高饱和、简化、2.5D 平面风格。

### 7.3 时间面板

`WBP_TimePanel`

- `DayText`
- `PeriodText`
- `EventPointText`
- `OptionalMoonOrWeatherIcon`

时间面板用于承接 7 天 21 事件点结构。第一版可以只显示文本，不做复杂日月动画。

### 7.4 右上信息面板

`WBP_TopRightPanel`

- `BackpackButton`
- `BackpackFullBadge`
- `DirectionIndicator`
- `OptionalMoonPhase`

方向信息用于表达漂流方向或当前事件方向。第一版允许用占位图标。

### 7.5 背包抽屉

`WBP_BackpackDrawer`

- `DrawerPanel`
  - `Header`
    - `TitleText`
    - `CloseButton`
  - `InventoryGrid`
    - 多个 `WBP_InventorySlot`
  - `ItemDetailPanel`
    - `ItemName`
    - `ItemDescription`
    - `PrimaryActionButton`
    - `SecondaryActionButton`
  - `TrashDropZone`
  - `FooterHints`

背包抽屉只显示和发出请求：

- 请求使用物品。
- 请求拖拽交换槽位。
- 请求进入放置预览。
- 请求丢弃物品。

背包抽屉不直接扣除物品、不直接生成 Actor。

### 7.6 背包格子

`WBP_InventorySlot`

- `SlotBackground`
- `IconImage`
- `QuantityText`
- `SelectedOutline`
- `DragHighlight`
- `UsableMarker`
- `PlaceableMarker`
- `LockedMarker`

槽位支持四种视觉状态：

1. 空槽。
2. 普通物品。
3. 可使用物品。
4. 可放置物品。

关键道具可以显示锁定标记，防止玩家误丢。

### 7.7 拖拽视觉

`WBP_ItemDragVisual`

- 跟随鼠标显示图标。
- 显示数量。
- 如果拖到无效区域，鼠标释放后回到原槽。
- 如果拖到世界区域且物品可放置，转入 `WBP_PlacementOverlay`。

### 7.8 放置覆盖层

`WBP_PlacementOverlay`

- `PreviewValidityText`
- `CostText`
- `RotateHint`
- `ConfirmHint`
- `CancelHint`

场景里的实际预览模型由 gameplay/build component 管理，UMG 只显示文字、按键和失败原因。

## 8. 数据模型边界

### 8.1 物品定义

第一版需要从当前资源枚举升级到物品定义层。建议新增数据结构或 DataAsset，至少包含：

- `ItemId`
- `DisplayName`
- `Description`
- `Icon`
- `Category`
- `MaxStack`
- `bUsable`
- `bPlaceable`
- `bKeyItem`
- `UseEffect`
- `PlacementDefinition`

推荐物品分类：

| 分类 | 示例 | UI 行为 |
|---|---|---|
| 消耗品 | 罐头、淡水、椰子、汽水 | 点击后弹确认框，确认后恢复状态。 |
| 资源 | 木材、碎片、绳子 | 可堆叠，用于合成和建造。 |
| 关键道具 | 潜水装备、手电、信号镜、信号弹、钩子 | 显示锁定，默认不可丢弃。 |
| 可放置物 | 地板、储物箱、集水器 | 可拖到场景，进入吸附放置。 |

### 8.2 背包槽位

第一版背包采用固定槽位，而不是只显示资源总数。

槽位数据建议包含：

- `SlotIndex`
- `ItemId`
- `Quantity`
- `bLocked`

拖拽规则：

- 空槽接收物品。
- 同类物品尽量合并到堆叠上限。
- 不同物品交换位置。
- 无效拖拽回滚，不丢物。
- 丢弃关键道具必须被拒绝并显示提示。

### 8.3 与现有资源栈的兼容

当前项目已有 `UOceanInventoryComponent` 和资源栈，可继续服务建造资源消耗。UI 背包需要在设计上兼容两层：

1. 槽位物品用于玩家可见背包。
2. 资源栈用于建造成本、测试和已有资源节点。

第一版可以通过适配层把拾取到的资源同时表现为槽位物品，并保留资源消耗接口，避免一次性推翻已有建造系统。

## 9. 物品使用流程

消耗品使用流程：

1. 玩家打开背包。
2. 点击可使用物品。
3. `WBP_ConfirmModal` 显示确认文案。
4. 玩家确认。
5. 背包组件原子化执行：检查物品存在、应用效果、扣除数量、广播 UI 刷新。
6. `WBP_ToastStack` 显示结果。

失败情况：

- 物品不存在：刷新背包并提示“物品不存在”。
- 状态已满：允许使用但提示收益溢出，或拒绝并提示“当前不需要”。
- 关键道具不可使用：显示说明，不扣除。

第一版建议：食物和水可以溢出裁剪到上限，减少玩家困惑。

## 10. 拖拽与场景放置流程

可放置物拖拽流程：

1. 玩家从背包槽拖出可放置物。
2. 鼠标离开背包抽屉并进入世界区域。
3. HUD 请求进入放置预览。
4. Build/Placement component 根据鼠标世界位置计算 grid anchor。
5. Placement query 返回结果。
6. 预览层显示绿色或红色。
7. 玩家左键确认或松开确认。
8. 成功时消耗物品或资源、生成 Actor、占用格子。
9. 失败时不消耗物品，显示原因。

可放置失败原因必须可枚举，避免只显示“无法放置在此处”：

- `OccupiedCell`
- `DetachedFromPlatform`
- `BlockedCell`
- `OutsideBuildRadius`
- `WrongZone`
- `InsufficientResources`
- `MissingItem`
- `InvalidWorldHit`

## 11. 建造格子与区域规划

现有格子系统已有 footprint、占用和邻接检查。为了支持背包拖拽放置，需要扩展出“查询而非立即放置”的边界。

推荐新增查询结果：

```text
FOceanPlacementQueryResult
- bCanPlace
- FailureReason
- AnchorCell
- FootprintCells
- SnappedWorldLocation
- RotationQuarterTurns
```

推荐格子区域：

| 区域 | 用途 |
|---|---|
| Core | 小船初始核心，不建议被替换或拆除。 |
| Deck | 普通甲板，可放地板、储物箱、集水器。 |
| Edge | 船体边缘，可作为钓鱼/潜水/上岸事件入口的后续挂点。 |
| WaterAdjacent | 邻水格，支持后续潜水入口、打捞入口。 |
| Blocked | 玩家出生点、重要交互点、不可占用区域。 |
| EventDock | 后续事件入口占位区。 |

第一版 WBP 不直接显示所有区域，但放置失败原因必须能从区域规则中得到。

## 12. UI 状态机

HUD 根节点维护轻量 UI 状态：

| 状态 | 可见 UI | 输入模式 |
|---|---|---|
| `Normal` | 常驻 HUD | Game Only 或 Game and UI hidden cursor |
| `BackpackOpen` | 常驻 HUD + 背包抽屉 | Game and UI，鼠标显示 |
| `DraggingItem` | 背包抽屉 + 拖拽视觉 | Game and UI，鼠标显示 |
| `PlacementPreview` | 放置覆盖层 + 预览 Actor | Game and UI，鼠标显示 |
| `ModalOpen` | 弹窗 | UI 优先，底层输入暂停 |
| `Paused` | 暂停菜单 | UI Only |

取消优先级：

1. 关闭弹窗。
2. 取消放置预览。
3. 取消拖拽。
4. 关闭背包。
5. 打开暂停菜单。

## 13. 视觉风格

- 高饱和、平面化、读数清晰。
- UI 可先使用纯色块、描边、简单图标。
- 背包抽屉建议使用绿色或荧光浅色描边，呼应草图。
- 状态栏可使用红/蓝/黄或红/青/橙区分，避免只靠文字。
- 所有图标后续可替换，WBP 结构不依赖最终图片尺寸。

## 14. 错误反馈

必须提供玩家可见反馈：

| 场景 | 反馈 |
|---|---|
| 背包满 | Toast：“背包栏已满”。 |
| 使用物品成功 | Toast：“恢复水分 +20”。 |
| 使用物品失败 | Modal 或 Toast 说明原因。 |
| 拖拽无效 | 槽位闪红，物品回到原位。 |
| 放置失败 | 红色预览 + 失败原因文本。 |
| 材料不足 | 建造面板成本文字变红。 |
| 关键道具丢弃 | Toast：“关键道具不能丢弃”。 |

## 15. 实现切片建议

本 spec 后续可拆成四个实现切片：

1. HUD Root 与常驻 UI：状态、时间、右上信息、菜单按钮。
2. 背包抽屉：槽位显示、点击选择、基础详情。
3. 物品使用：确认弹窗、恢复状态、Toast。
4. 拖拽放置入口：拖拽视觉、placement query、红绿预览、确认放置。

切片 1 和 2 可以先使用假图标与占位数据，但 WBP 命名、层级和事件接口必须按最终结构建立。

## 16. 测试与验证策略

### 16.1 C++ / Gameplay 测试

- `Ocean.MVP.Inventory.UseItem`：使用物品时原子化扣除并恢复状态。
- `Ocean.MVP.Inventory.DragDropModel`：交换、合并、无效拖拽不丢物。
- `Ocean.MVP.Build.AllowedArea`：放置查询拒绝占用、未邻接、阻挡、错误区域。
- `Ocean.MVP.Survival.RecoveryItems`：食物/水恢复并 clamp 到上限。

### 16.2 UnrealBridge / PIE 验证

- 打开 `L_WaterOcean` 后自动创建并显示 `WBP_OceanHUDRoot`。
- 常驻 HUD 能读到三条状态。
- 背包键打开右侧抽屉，关闭后恢复视野。
- 背包打开时鼠标显示，角色移动暂停或弱化。
- 拖拽可放置物进入场景时显示 placement overlay。

### 16.3 文档与答辩验证

- 答辩 HTML 需要同步说明：HUD 采用低遮挡常驻信息 + 右侧抽屉背包。
- “技术取舍”章节说明：背包 UI 不直接生成 Actor，而是通过 placement query 接入格子系统。

## 17. 与现有项目的接口

本 UI/UX 设计应接入现有系统，而不是重做它们：

- `UOceanSurvivalComponent`：提供体力、水分、饱食度。
- `UOceanInventoryComponent`：继续提供资源栈与容量基础，后续增加槽位/物品适配。
- `UOceanBuildGridComponent`：提供格子、footprint、占用和区域查询。
- `UOceanBuildComponent`：提供建造模式、旋转和最终放置执行。
- `AOceanPlayerController`：负责背包键、输入模式、取消优先级。

## 18. 验收标准

- 玩家不打开背包时，只看到轻量 HUD，不遮挡小船。
- 玩家可以通过键盘或右上按钮打开右侧背包抽屉。
- 背包抽屉具有清晰槽位、物品详情、关闭按钮和拖拽视觉入口。
- 消耗品、资源、关键道具、可放置物在 UI 上有不同表现。
- 拖拽可放置物到场景时进入格子吸附预览。
- 放置失败原因明确显示。
- UI 层级、命名和职责允许后续替换图片，不需要改业务逻辑。

## 19. 答辩表达

本设计可以这样说明：

> 《洋流》的背包不是独立菜单，而是海上生存 HUD 的一部分。玩家默认看到状态、时间和方向；需要管理物资时，背包从右侧滑出，不遮挡小船。可放置物从背包拖向船体后，会进入格子吸附预览，由建造系统判断能否放置。这样既保留休闲玩家的直观操作，又保证背包、建造和船体扩展共用同一套规则。

技术取舍：

- 先做抽屉式背包，不做复杂 RPG 背包。
- UI 只发起请求，规则判断在 gameplay component。
- `B` 保留建造，背包使用 `Tab/I`，减少输入冲突。
- 拖拽到场景只开放给可放置物，消耗品走点击确认。
- 第一版用占位图标和纯色 UI，优先保证交互闭环可测。
