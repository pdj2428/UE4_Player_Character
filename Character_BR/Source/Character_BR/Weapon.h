// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_NoOwner		UMETA(DisplayName = "NoOwner"),
	EWS_PickUp		UMETA(DisplayName = "Pickup"),
	EWS_Equipped	UMETA(DisplayName = "Equipped"),

	EWS_MAX			UMETA(DisplayName = "Default")
};

UENUM(BlueprintType)
enum class EWeaponKind : uint8
{
	EWK_AssaultRifle		UMETA(DisplayName = "AssaultRifle"),
	EWk_HandGun				UMETA(DisplayName = "HandGun")
};

UCLASS()
class CHARACTER_BR_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UPROPERTY(EditDefaultsOnly, Category = "SavedData")
		FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
		EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
		EWeaponKind WeaponKind;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
		class USoundCue* OnEquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
		USoundCue* SwingSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SceneComponent")
		class USceneComponent* SceneCompoennt;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
		class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat")
		class UBoxComponent* CombatCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
		float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | ItemProperties")
		bool bRotate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		UAnimMontage* FireMontage;

protected:

	void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Equip(class APlayerCharacter* Char);
	void SetWeaponRightHand(class APlayerCharacter* Char);
	void SetWeaponBack(class APlayerCharacter* Char, int Number);

	void PlayFireMontage();

	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }
	FORCEINLINE EWeaponState GetWaponState() { return WeaponState; }

	FORCEINLINE void SetWeaponKind(EWeaponKind State) { WeaponKind = State; }
	FORCEINLINE EWeaponKind GetWeaponKind() { return WeaponKind; }

	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AController* WeaponInstigator;

	FORCEINLINE void SetInstigator(AController* Inst) { WeaponInstigator = Inst; }

};
