#pragma once

#include "CoreMinimal.h"
#include "GASMPTypes.generated.h"

class ABaseItemActor;
class UBaseGameplayAbility;

USTRUCT(BlueprintType)
struct FCharacterData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
    TArray<TSubclassOf<class UGameplayEffect>> Effects;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
    TArray<TSubclassOf<class UBaseGameplayAbility>> Abilities;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
    class UCharacterAnimDataAsset* CharacterAnimDataAsset;

};

UENUM(BlueprintType)
enum class EFoot : uint8
{
    Left UMETA(DisplayName = "Left"),
    Right UMETA(DisplayName = "Right")
};

USTRUCT(BlueprintType)
struct FCharacterAnimationData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly)
    class UBlendSpace* UnEquippedMovementBlendSpace = nullptr;

    UPROPERTY(EditDefaultsOnly)
    class UAnimSequenceBase* UnEquippedIdleAnimation = nullptr;
};

UCLASS(BlueprintType, Blueprintable)
class UItemStaticData : public UObject
{
    GENERATED_BODY()
    
public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName Name;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<ABaseItemActor> ItemActorClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName AttachmentSocket = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bCanBeEquipped = false;

protected:

};

UENUM(BlueprintType)
enum class EItemState : uint8
{
    None    UMETA(DisplayName="None"),
    Equipped    UMETA(DisplayName="Equipped"),
    Dropped UMETA(DisplayName="Dropped"),

};