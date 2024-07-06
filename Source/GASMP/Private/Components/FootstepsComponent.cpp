// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/FootstepsComponent.h"
#include "PhysicalMaterials/BasePhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/BaseGASCharacter.h"
#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<int32> CVarShowFootSteps(
	TEXT("ShowDebugFootsteps"),
	0,
	TEXT("Draws Debug info About Footsteps")
	TEXT("	0:off/n")
	TEXT("	1:on/n"),
	ECVF_Cheat
);

UFootstepsComponent::UFootstepsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UFootstepsComponent::HandleFootstep(EFoot Foot)
{
	if(ABaseGASCharacter* GASCharacter = Cast<ABaseGASCharacter>(GetOwner()))
	{
		const int32 DebugShowFootsteps = CVarShowFootSteps.GetValueOnAnyThread();
		if(USkeletalMeshComponent* SkeletalMesh = GASCharacter->GetMesh())
		{
			FHitResult HitResult;
			const FVector SocketLocation = SkeletalMesh->GetSocketLocation(Foot == EFoot::Left ? LeftFootSocketName : RightFootSocketName);
			const FVector Location = SocketLocation + FVector::UpVector * 20;

			FCollisionQueryParams QueryParams;
			QueryParams.bReturnPhysicalMaterial = true;
			QueryParams.AddIgnoredActor(GASCharacter);

			if(GetWorld()->LineTraceSingleByChannel(HitResult, Location, Location + FVector::UpVector * -500.f, ECollisionChannel::ECC_WorldStatic, QueryParams))
			{
				if(HitResult.bBlockingHit)
				{
					if(HitResult.PhysMaterial.Get())
					{
						UBasePhysicalMaterial* PhysicalMaterial = Cast<UBasePhysicalMaterial>(HitResult.PhysMaterial.Get());
						if(PhysicalMaterial)
						{
							UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->FootstepSound, Location, 1.f);
						}
						if(DebugShowFootsteps > 0)
						{
							DrawDebugString(GetWorld(), Location, GetNameSafe(PhysicalMaterial), nullptr, FColor::White, 4.f);
						}
					}
					if(DebugShowFootsteps > 0)
					{
						DrawDebugSphere(GetWorld(), Location, 16, 16, FColor::Red, false, 4.f);
					}
				}
				else
				{
					if(DebugShowFootsteps > 0)
					{
						DrawDebugLine(GetWorld(), Location, Location + FVector::UpVector * -50.f, FColor::Red, false, 4, 0, 1);
					}
				}
			}
			else
			{
				if(DebugShowFootsteps > 0)
				{
					DrawDebugLine(GetWorld(), Location, Location + FVector::UpVector * -50.f, FColor::Red, false, 4, 0, 1);
					DrawDebugSphere(GetWorld(), Location, 16, 16, FColor::Red, false, 4.f);
				}
			}
		}
	}
}
void UFootstepsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
