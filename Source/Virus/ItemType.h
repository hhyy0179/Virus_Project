#pragma once

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Weapon UMETA(DisplayName = "Weapon"),
	EIT_AttackItem UMETA(DisplayName = "AttackItem"),
	EIT_CCTVDefenseItem UMETA(DisplayName = "CCTVDefenseItem"),
	EIT_AttackDefenseItem UMETA(DisplayName = "AttackDefenseItem"),
	EIT_DoubleJump UMETA(DisplayName = "DoubleJump"),
	EIT_SpeedItem UMETA(DisplayName = "SpeedItem"),
	EIT_Key UMETA(DisplayName = "Key"),

	EIT_MAX UMETA(Displayname = "DefaultMAX")
};