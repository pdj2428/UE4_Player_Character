// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class APlayerMovementState : uint8
{
	PMS_Common			UMETA(DeplayName = "Common"),
	PMS_Climbing		UMETA(DeplayName = "Climbing"),
	PMS_Dodgging		UMETA(DeplayName = "Dodgging"),
	PMS_Swimming		UMETA(DeplayName = "Swimming")
};


UCLASS()
class CHARACTER_BR_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FPCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TPCamera;

public:

	// Sets default values for this character's properties
	APlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movemnet")
	APlayerMovementState PlayMovementState;

	FORCEINLINE void SetPlayerMovementStatus(APlayerMovementState Status) { PlayMovementState = Status; }

	int NormalSpeed;

	int AimingSpeed;

	int SprintSpeed;

	int NormalJump;

	int AimingJump;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	bool IsEquippedWeapon;

	FVector ClimbingLocation;

	float ClimbTraceDistance;

	float TraceDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	bool ClimbReady;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	bool ClimbUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsClimbing;

	FTimerHandle ClimbDelay;

	FTimerHandle ClimbUpDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UAnimMontage* RollMontage;

	bool IsDogging;

	FVector DoggingVector;
	int DoggingForce;

	FTimerHandle DoggingDelay;
	FTimerHandle ReleaseDoggingDelay;

	FVector DoggingLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsSwimming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerStat)
	float MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerStat)
	float Health;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerStat)
	float MaxStamina;

	//void TakeDamage(float DamageAmount, APlayerCharacter* PlayerCharacter);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interaction")
	class UBoxComponent* InteractionCollision;

	//Weapon
	UFUNCTION()
	void InteractionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void InteractionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapon)
	int WeaponDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapon)
	int EquippedWeaponNumber;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	class AWeapon* RightHandEquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	class AWeapon* FirstEquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	class AWeapon* SecondEquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	class AWeapon* HitWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UAnimMontage* EquipAnimMonatage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UAnimMontage* UnEquipAnimMonatage;

	bool IsEquipping;

	FTimerHandle EquipDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerStat)
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	TSubclassOf<class UUserWidget> PlayerStatusAsset;

	UPROPERTY()
	UUserWidget* PlayerStatus;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	bool IsSwitched;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	float MoveForwardValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	float MoveRightValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	class USoundCue* OnEquipSound;

	UPROPERTY(BlueprintReadWrite, Category = Rifle)
	bool IsAiming;

	UPROPERTY(BlueprintReadWrite, Category = Rifle)
	bool IsFiring;
		
	UPROPERTY(BlueprintReadWrite, Category = Rifle)
	bool BulletFire;

	int ContinuityFire;

	int MaxContinuityFire;

	FTimerHandle FireDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UAnimMontage* FireAnimMontage; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UAnimMontage* FireHandGunAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	float GunRebound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsJumping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsSprinting;

	bool Sprinted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UAnimMontage* RifleReloadingAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsRifleReloading;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int WeaponMaxBullet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int LoadedBullet;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int InventoryBulletCount;

	FTimerHandle ReloadDelay;

protected:

	virtual void BeginPlay() override;

	void FindFrontObject();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void Sprint();

	void ReleaseSprint();

	void ClimbTracer();

	void StartClimbing();

	void Climbing();

	void ClimbingMovement();

	void ReleaseClimbing();

	void ClimbingUp();

	void ClimbingUpMovement();

	void Rolling();

	void RollingMovement();

	void ReleaseRolling();

	/** Called for CameraX rotate */
	void TurnAtRate(float Rate);

	/** Called for CameraY rotate */
	void LookUpAtRate(float Rate);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TakeItem();

	void EquipFirstWeapon();

	void EquipSecondWeapon();
	
	void AttachWeapon();

	void UnEquipWeapon();

	void Aiming();

	void ReleaseAiming();

	void StartFire();
	
	void Fire();

	void ReleaseFire();

	void SwitchCamera();

	void Reload();

	void FinishReload();

public:	

	virtual void Tick(float DeltaTime) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FPCamera; }
};
                                                                                                                                                                                                                                                                                                                                                                            