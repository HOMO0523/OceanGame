# Ocean MVP 会话变更记录

**日期**: 2026-06-17 ~ 2026-06-18  
**分支**: codex/ocean-phase-one-workflows  
**范围**: 从 autoplay 验证到完整 MVP 游戏循环 + 存档系统 + 玩家交互

---

## 一、修复的问题

### 1. GameMode BeginPlay 不执行
- **原因**: BP_OceanMVPGameMode 蓝图覆盖了 BeginPlay 且未调用 Parent::BeginPlay
- **修复**: 删除并重建干净的 BP_OceanMVPGameMode（只继承 C++，无自定义节点）
- **验证**: `[TDD] OceanMVPGameMode: start day=1 phase=Playing total_events=21` 正常输出

### 2. UI 时间不刷新
- **原因**: `SetDayAndTime` 函数缺少 `UFUNCTION` 标记，导致 `AddDynamic` 委托绑定失败
- **修复**: 给 `SetDay`/`SetTimeOfDay`/`SetDayAndTime` 加 `UFUNCTION(BlueprintCallable)`
- **验证**: `[TDD] OceanTimePanel: SetDayAndTime called day=2 time=0` 委托回调成功

### 3. DirectionalLight 移动性
- **问题**: 昼夜系统无法修改光源参数（LightComponent 移动性为 Static/Stationary）
- **修复**: 通过 Bridge 将 `DirectionalLight_0` 的 `LightComponent0` 移动性改为 `MOVABLE`

### 4. 平台位置和碰撞
- **问题**: 平台 Z=60 太高；无可见 mesh；漂移不带角色；角色 spawn 在 Z=250
- **修复**:
  - 平台 Z 降到 10（水面+10）
  - 平台 mesh 厚度从 0.2 缩到 0.1，碰撞盒高度从 40 降到 20
  - 漂移用 `SetActorLocation(sweep=true)` 平滑移动
  - PlayerStart Z 从 250 降到 110

### 5. 角色沉到水底地形
- **问题**: 角色入水后被重力拉到 Z=-220 的 Landscape 水底
- **修复**: 入水时切换 `MOVE_Swimming`（禁用重力），Z 钳制不低于 -110

---

## 二、新增功能

### 1. 昼夜系统 UI 修正
- 时间显示从 `Afternoon` 改为 `Noon`
- 循环: Morning → Noon → Night → (Day+1) Morning
- 每 20 秒一个事件节点，7天×3事件=21个节点

### 2. 平台可见化 + 漂移逻辑
- **新增**: `PlatformMesh`（立方体占位）+ `PlatformCollision`（BoxComponent 碰撞）
- **新增**: `IsWorldLocationOverPlatform()` — 检查位置是否在平台范围内
- **漂移逻辑**: 只有角色在平台上时才向北（Y+）漂移，速度 50 u/s
- **文件**: `OceanFloatingPlatform.h/.cpp`

### 3. 游泳/潜水系统
- **游泳判定**: 角色 Z < 0（水面）即进入游泳状态，切换 `MOVE_Swimming`
- **Z 钳制**: 游泳时 Z 不低于 -110（`SwimDepthFloor`）
- **潜水（X键）**: 
  - 需要背包有 `dive_suit`
  - 潜水时落到海底地形/备用深度，动画切换 `DiveSuitDive`
  - 潜水时切换到海底 Walking 移动，摄像机弹簧臂缩短，形成真正下潜视角
  - 再按 X 浮出水面 Z=0，回到游泳状态，并恢复进入潜水前的摄像机臂长
- **Paper2D 表现层 Z**: 状态切换只改 `Paper2DVisualComponent` 相对 Z，不移动胶囊或 Gameplay Pawn：陆地/行走 `Z=-115`，水面/游泳 `Z=5`，潜水 `Z=300`
- **爬上平台（C键）**: 游泳状态 + 靠近平台 → 移到平台上方，切回 Walking
- **可配置参数**: `WaterSurfaceZ`（默认0）、`SwimDepthFloor`（默认-110）
- **文件**: `OceanCharacter.h/.cpp`

### 4. 钓鱼系统（G键）
- **前提**: 背包有 `fishing_rod`
- **条件**: 站在平台上（向下射线检测命中平台）
- **效果**: -1 体力，+15 饱食度，背包加 `fish` 物品
- **文件**: `OceanCharacter.cpp` TryFish()

### 5. 背包系统增强
- **新增**: `HasItem(FName ItemId)` — 检查背包是否有指定物品
- **初始物品**: 游戏开始时自动放入 `fishing_rod` 和 `dive_suit`（KeyItem）
- **文件**: `OceanInventoryComponent.h/.cpp`, `OceanMVPGameMode.cpp`

### 6. 资源每天刷新
- **跨天清除**: `ClearSpawnedPickups()` 销毁昨天的 pickup actors
- **重新散布**: 在玩家周围 400 单位半径散布 5 个新 pickup
- **文件**: `OceanItemScatterComponent.h/.cpp`, `OceanMVPGameMode.cpp`

### 7. 存档系统
- **存档数据结构**: `UOceanSaveGame` — 保存天数/时段/事件/体力/水分/饱食/背包/平台位置/种子/音量
- **存档管理器**: `UOceanSaveManager`（GameInstanceSubsystem）
  - 3 个存档槽位
  - Save/Load/Delete/GetSlotInfo
  - 暂停菜单只选择“下一次跨天快照槽”，不立即手动写盘
  - Night → Morning 跨天时自动写入所选槽位快照
  - 读档强制回到保存的平台安全点，清除潜水状态并恢复水面摄像机
  - 背包资源栈与物品槽位通过 `RestoreInventoryState` 恢复
  - 设置持久化（BGM/SFX 音量）
- **文件**: `OceanSaveGame.h/.cpp`, `OceanSaveManager.h/.cpp`

### 8. 主菜单 + 暂停菜单
- **主菜单**（`L_MainMenu` 独立场景）:
  - New Game → OpenLevel(L_WaterOcean)
  - Continue → 选存档槽位 → 加载
  - Settings → 音量设置
  - Quit → 退出游戏
- **暂停菜单**（U键呼出）:
  - 选择 3 个 Day Snapshot 槽位之一
  - Resume / Settings / Quit to Menu
- **文件**: `OceanMainMenuWidget.h/.cpp`, `OceanPauseMenuWidget.h/.cpp`, `OceanSaveSlotWidget.h/.cpp`, `OceanSettingsWidget.h/.cpp`

### 9. 游戏流程分层
- **GameMode `bGameStarted` 标志**: 主菜单期间不 tick，点 New Game/Continue 后才启动
- **StartGame()**: 初始化 DayNight/AutoPlay/Scatter，设 bGameStarted=true
- **InitGameSystems()**: 独立的初始化函数（避免重复初始化）
- **PlayerController BeginPlay**: 根据关卡名判断是菜单还是游戏，分别处理
- **文件**: `OceanMVPGameMode.h/.cpp`, `OceanPlayerController.h/.cpp`

### 10. AutoPlay 种子系统
- **新增**: `GetRandomSeed()`/`SetRandomSeed()` — 存档可保存/恢复种子保证事件序列一致
- **文件**: `OceanAutoPlayComponent.h`

---

## 三、按键映射

| 按键 | 功能 | 前提条件 |
|---|---|---|
| 鼠标点击 | 移动角色 | 游戏中 |
| F | 拾取物品（交互） | 靠近 pickup actor |
| G | 钓鱼 | 背包有鱼竿 + 站在平台上 |
| C | 爬上平台 | 游泳状态 + 靠近平台 |
| X | 切换潜水 | 背包有潜水服 |
| U | 暂停菜单 | 游戏中 |
| Tab | 切换背包 | 游戏中 |

---

## 四、新增文件

| 文件 | 用途 |
|---|---|
| `Source/Ocean/OceanPrototype/OceanSaveGame.h/.cpp` | 存档数据结构 |
| `Source/Ocean/OceanPrototype/OceanSaveManager.h/.cpp` | 存档管理器 |
| `Source/Ocean/OceanPrototype/UI/OceanMainMenuWidget.h/.cpp` | 主菜单 |
| `Source/Ocean/OceanPrototype/UI/OceanPauseMenuWidget.h/.cpp` | 暂停菜单 |
| `Source/Ocean/OceanPrototype/UI/OceanSaveSlotWidget.h/.cpp` | 存档槽位 |
| `Source/Ocean/OceanPrototype/UI/OceanSettingsWidget.h/.cpp` | 设置面板 |
| `Content/OceanPrototype/Maps/L_MainMenu.umap` | 菜单场景 |

---

## 五、修改的文件

| 文件 | 改动 |
|---|---|
| `OceanMVPGameMode.h/.cpp` | bGameStarted/StartGame/InitGameSystems/每天刷新资源/初始物品/AutoPlay getter |
| `OceanCharacter.h/.cpp` | 游泳/潜水/爬平台/钓鱼/Move_Swimming/Z钳制 |
| `OceanPlayerController.h/.cpp` | 主菜单显示/CreateHUD/C/X/G/U键绑定/关卡判断 |
| `OceanFloatingPlatform.h/.cpp` | mesh+碰撞/IsWorldLocationOverPlatform/条件漂移 |
| `OceanInventoryComponent.h/.cpp` | HasItem() |
| `OceanItemScatterComponent.h/.cpp` | ClearSpawnedPickups() |
| `OceanAutoPlayComponent.h` | GetRandomSeed/SetRandomSeed |
| `OceanTimePanelWidget.h/.cpp` | UFUNCTION修正/Noon显示 |
| `Config/DefaultEngine.ini` | GameDefaultMap 改为 L_MainMenu |

---

## 六、待办 / 已知问题

- [ ] PCG 系统集成（当前用 deterministic scatter 替代）
- [ ] 岛屿生成系统（每天可能刷新岛屿）
- [ ] 钓鱼动画/计时器（当前即时完成）
- [ ] 游泳动画 flipbook 资源（当前用占位）
- [ ] 音量设置实际应用（当前只保存数值）
- [ ] 氧气机制、海底采集奖励、完整潜水 UI（当前只做 X 下潜/浮上与摄像机往返）
