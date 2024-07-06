#pragma once

#include "CoreMinimal.h"
#include "GASMPTypes.generated.h"

class ABaseItemActor;

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