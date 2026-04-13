# TopDown55 — Leap Slam Ability

## Overview

**`UTDWAbility_LeapSlam`** is a Gameplay Ability (GAS) that launches the owning character toward a cursor-targeted location, then detonates a radial slam on landing. It is part of the **TheDarkWestTest** plugin.

**Class hierarchy:** `UTDWAbility_LeapSlam` → `UTDWAbility_Attack` → `UTDWAbility` → `UGameplayAbility`

**Source files:**
| File | Path |
|------|------|
| Header | `Plugins/TheDarkWestTest/Source/TheDarkWestTest/Public/GameplayAbility/Abilities/TDWAbility_LeapSlam.h` |
| Implementation | `Plugins/TheDarkWestTest/Source/TheDarkWestTest/Private/GameplayAbility/Abilities/TDWAbility_LeapSlam.cpp` |

---

## Ability Flow

```
ActivateAbility()
  │
  ├─ CommitAbility()              ← checks/deducts costs & cooldown
  │
  ├─ Spawns TDWTargetActor_CursorTrace
  │    └─ Player clicks to confirm target location
  │
  ├─ OnTargetDataReceived()
  │    ├─ LaunchCharacter() — arc jump toward target
  │    │
  │    ├─ [SlamMontage assigned]
  │    │    ├─ PlayMontageAndWait   → OnMontageCompleted / OnMontageCancelled
  │    │    └─ WaitGameplayEvent(ImpactEventTag) → OnImpactEventReceived()
  │    │
  │    └─ [No SlamMontage]
  │         └─ WaitMovementModeChange(Walking) → OnLanded()
  │
  └─ ApplyLandingImpact()
       ├─ Spawns ImpactVFX particle
       ├─ GetActorsInRadius(ScaledRadius) — sphere overlap
       ├─ ApplyDamageToTarget() on each actor
       ├─ ApplyGameplayEffect(SecondaryEffectClass) on each actor
       └─ LaunchCharacter() knockback on each ACharacter in range
```

---

## Configuration Properties

### Core Leap Slam

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `SlamMontage` | `UAnimMontage*` | `null` | Animation played during the leap. The impact is triggered by `ImpactEventTag` sent from a montage notify. If `null`, the impact fires when the character lands. |
| `ImpactEventTag` | `FGameplayTag` | — | Gameplay tag sent as a montage notify to trigger `ApplyLandingImpact`. Only used when `SlamMontage` is set. |
| `SlamRadius` | `float` | `300` | Base radial impact radius (cm). Scaled by `RadiusScaleCurve` at runtime. |
| `ImpactVFX` | `UParticleSystem*` | `null` | Legacy particle effect spawned at the impact location. |
| `KnockbackStrength` | `float` | `500` | Base impulse applied outward to characters caught in the blast. Scaled by `KnockbackScaleCurve`. |
| `SecondaryEffectClass` | `TSubclassOf<UTDWGameplayEffect>` | `null` | Optional Gameplay Effect applied to every actor hit (e.g. slow, stun, bleed). |

### Inherited from `UTDWAbility_Attack`

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `DamageEffectClass` | `TSubclassOf<UTDWGameplayEffect>` | `null` | Gameplay Effect used to deal damage. |
| `BaseDamage` | `float` | `10` | Flat damage value. Overridden by `GetDamageAmount()` when a `DamageScaleCurve` is assigned. |

---


## Blueprint Setup

1. **Create a Blueprint child class** of `UTDWAbility_LeapSlam`.
2. Assign a **Slam Montage** (optional but recommended).
   - Add an **AnimNotify** at the impact frame and send `ImpactEventTag` via `SendGameplayEventToActor` or a custom notify.
   - If no montage is set, the impact fires when the character transitions back to `Walking` movement mode.
3. Set **DamageEffectClass** and **BaseDamage** (inherited from `UTDWAbility_Attack`).
4. Set **SlamRadius** and **KnockbackStrength** as needed.
5. *(Optional)* Assign **SecondaryEffectClass** for a status effect on impact.
6. **Grant the ability** to the character's `UAbilitySystemComponent` via `GiveAbility()` or the ability set.

---

## Internal Notes

- **Attack speed** is read from `UTDWCombatAttributeSet::GetAttackSpeedAttribute()` on the owning ASC at the moment of activation. It drives both the physics launch velocity (hang-time) and the montage play rate so both stay in sync.
- The launch velocity formula is: `ZVelocity = (AttackSpeed × |GravityZ|) / 2`, which guarantees that the character reaches the target in roughly `AttackSpeed` seconds regardless of gravity.
- **Knockback** is applied as a 2D outward impulse from `ImpactLocation`, preserving vertical momentum of the target.