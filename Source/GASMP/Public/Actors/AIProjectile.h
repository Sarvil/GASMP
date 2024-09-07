// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "AIProjectile.generated.h"

UCLASS()
class GASMP_API AAIProjectile : public AActor
{
	GENERATED_BODY()
	
public:
    AAIProjectile();

protected:
    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable, Category = "Projectile")
    void FireInDirection(const FVector& ShootDirection);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
    UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
    USphereComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    UProjectileMovementComponent* ProjectileMovementComponent;

    UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
    UStaticMeshComponent* ProjectileMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
    float Damage = 20.0f;

};
