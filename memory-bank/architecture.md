# Architecture — Ocean

## 1. Project Shape

Ocean is a UE 5.7 single-player prototype focused on a floating ocean survival/building loop.

Current runtime module:

```text
Ocean
└── OceanPrototype
    ├── Build grid and module placement
    ├── Floating platform core
    ├── Floating resource nodes
    └── PCG/fallback resource field
```

## 2. Core Systems

| System | File/Area | Responsibility |
|---|---|---|
| Build grid | `Source/Ocean/OceanPrototype/OceanBuildGridComponent.*` | Stable logical cells, footprint rotation, overlap and adjacency checks. |
| Module definition | `Source/Ocean/OceanPrototype/OceanBuildModuleDefinition.*` | Data-asset contract for footprint, cost, preview mesh, and actor class. |
| Module actor | `Source/Ocean/OceanPrototype/OceanBuildModuleActor.*` | Cube-placeholder build module with inherited buoyancy path. |
| Floating platform | `Source/Ocean/OceanPrototype/OceanFloatingPlatform.*` | Owns stable grid and visual bobbing root. |
| Resource node | `Source/Ocean/OceanPrototype/OceanResourceNode.*` | Collectable floating resource actor with cube placeholder and buoyancy. |
| Resource field | `Source/Ocean/OceanPrototype/OceanResourceField.*` | Owns PCG component and deterministic fallback spawn positions. |

## 3. Data Flow

### Build Placement

1. Player chooses a `UOceanBuildModuleDefinition`.
2. Placement converts world position to grid anchor with `UOceanBuildGridComponent::WorldToCell`.
3. Footprint cells are generated from module size and rotation.
4. `CanPlaceFootprint` rejects overlaps and optionally enforces adjacency.
5. `ReserveFootprint` records occupied cells by module id.
6. Spawned module actor keeps buoyancy and placeholder mesh until user replaces the visual asset.

### Resource Spawn

1. PCG is the preferred authoring path for scatter rules.
2. `AOceanResourceField` provides deterministic fallback locations for tests and early maps.
3. Spawned `AOceanResourceNode` actors float on the water and return `FOceanResourceStack` when collected.

## 4. Plugin Boundaries

| Plugin | Role | Boundary |
|---|---|---|
| Water | Ocean surface, water collision profile, buoyancy sampling. | Required for water actors and `UBuoyancyComponent`. |
| PCG | Resource scattering and future island/loot generation. | Runtime actors can provide deterministic fallback until PCG graph assets exist. |
| UnrealBridge | Editor automation, Python execution, PIE/log capture, save gate. | Editor-only; never required by packaged runtime. |

## 5. Prototype Rules

- Module is a group of grid cells, not necessarily one cell.
- Logical grid is stable even when visuals bob on waves.
- Missing models use cube placeholders first.
- Floating actor assets must retain buoyancy components.
- Water setup is incomplete unless `WaterBodyCollision` exists in `Config/DefaultEngine.ini`.
