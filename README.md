# CommonPlayerAttributes
**CommonPlayerAttributes** is a lightweight, modular C++ runtime plugin that provides a complete player attribute management framework (Health, Stamina, Movement Speed, XP Progression, and Weapon Overlay States) ready to attach to any `ACharacter` in Unreal Engine.


## 🌟 Features
### ❤️ Health & Damage System
- **Max & Current Health Management**: Built-in methods for health clamping, healing eligibility, and percentage normalization (0 to 1) ideal for UI progress bars.
- **Damage Handling & Feedback**: 
  - Plays hit animation montages from configurable montage pools upon receiving damage.
  - Spawns blood particle VFX (Niagara) dynamically at hit locations and rotations.
  - Triggers gamepad force-feedback/rumble vibration effects on damage.
- **Death & Health Events**: Broadcasts `FOnHealthChange` and `FOnPlayerDeath` dynamic delegates for easy HUD and gameplay blueprint integration.
---
### ⚡ Stamina & Locomotion System
- **Stamina Drain & Recovery Timers**: Automatic stamina consumption during sprinting or melee attacks, with configurable drain rates, fill rates, and delay timers.
- **Movement Speed Integration**: Smooth switching between movement speed profiles (`Slow Walk`, `Walk`, `Sprint`, `Crouch`).
- **Master Sprint Controls**: Ability to toggle sprint capabilities per level area or stamina state.
- **Stamina Delegates**: Broadcasts `FOnStaminaChange` and `FOnToggleSprint` events.
---
### ⭐ XP, Leveling & Token Economy
- **Level Progression**: Automatically calculates level-up thresholds based on accumulated XP (`Player_XP` vs `Player_Max_XP`).
- **Progress Normalized Getters**: Easy single-function calls (`GetPlayerProgress()`) for level progress bars.
- **Token Currency System**: Built-in support for reward tokens (`Player_Tokens`) used for unlockables, shops, or skill trees.
- **Progression Delegates**: Broadcasts `FOnPlayerLevelUp` and `FOnTokensAdded` dynamically.
---
### ⚔️ Weapon Overlay & Animation Blueprint Integration
- **Overlay States Enum (`EOverlayState`)**: Supports `Unarmed`, `Melee`, `Pistol`, `Shotgun`, and `Rifle` overlay states.
- **AnimBP Sync**: Seamlessly drives Animation Blueprint layer switching and posture transitions via the `FOnOverlayStateChange` delegate.
---
## 🚀 Quick Setup Guide
1. Add `UPlayerAttributeComponent` to your player character in C++ or Blueprint:
   ```cpp
   // In your Character's constructor:
   AttributeComponent = CreateDefaultSubobject<UPlayerAttributeComponent>(TEXT("PlayerAttributeComponent"));

Bind to dynamic delegates in your HUD Widget or Controller:
   AttributeComponent->OnHealthChange.AddDynamic(this, &UYourHUDWidget::UpdateHealthBar);
   AttributeComponent->OnPlayerLevelUp.AddDynamic(this, &UYourHUDWidget::ShowLevelUpBanner);

   🛠️ Technical Details
Module Type: Runtime
Primary Component: UPlayerAttributeComponent
Dependencies: Niagara
Supported Platforms: Win64, Mac, Linux, iOS, Android, Consoles
