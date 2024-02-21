#pragma once

UENUM(BlueprintType)
enum class EPickupType : uint8
{
	EPT_Key UMETA(DisplayName = "Key"),
	EPT_Item UMETA(DisplayName = "Item"),

	EPT_MAX UMETA(DisplayName = "DefaultMAX")
};