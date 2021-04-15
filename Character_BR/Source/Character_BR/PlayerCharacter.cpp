// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Actions/PawnAction.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Weapon.h"


APlayerCharacter::APlayerCharacter()
{
	/**Set HP and Armor*/
	MaxHealth = 100;
	Health = 100;
	MaxStamina = 100;
	Stamina = 100;

	WeaponMaxBullet = 30;

	LoadedBullet = 30;

	InventoryBulletCount = 100;

	// Sets default values
	NormalSpeed = 300;

	AimingSpeed = 200;

	SprintSpeed = 450;

	NormalJump = 500;

	AimingJump = 300;

	IsSprinting = false;

	IsSwitched = false;

	IsJumping = false;

	IsFiring = false;

	BulletFire = false;

	IsAiming = false;

	IsRifleReloading = false;

	ClimbUp = false;

	WeaponDamage = 0;

	IsEquipping = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(85.f, 65.0f);

	InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetupAttachment(GetRootComponent());

	ClimbReady = false;

	// set our turn rates for input
	BaseTurnRate = 30.f;
	BaseLookUpRate = 30.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = NormalJump;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPCamera"));
	FPCamera->SetupAttachment(GetMesh(), FName("Head")); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FPCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm


	// Create a follow camera
	TPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPCamera"));
	TPCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	TPCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(Controller);

	if (PlayerController)
	{
		if (PlayerController->PlayerCameraManager)
		{
			PlayerController->PlayerCameraManager->ViewPitchMin = -55.0;
			PlayerController->PlayerCameraManager->ViewPitchMax = 60.0;
		}
	}

	if (PlayerStatusAsset)
	{
		PlayerStatus = CreateWidget<UUserWidget>(GetWorld(), PlayerStatusAsset);
	}
	PlayerStatus->AddToViewport();
	PlayerStatus->SetVisibility(ESlateVisibility::Visible);

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	FPCamera->SetActive(false);

	InteractionCollision->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::InteractionOnOverlapBegin);
	InteractionCollision->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::InteractionOnOverlapEnd);

	GunRebound = 0.2f;
	EquippedWeaponNumber = 0;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//FindFrontObject();
	ClimbTracer();

	//Set Stamina
	if (PlayMovementState != APlayerMovementState::PMS_Common || IsSprinting)
	{
		if (PlayMovementState == APlayerMovementState::PMS_Climbing)
		{
			if (Stamina > 0) Stamina -= 2.5f * DeltaTime;
			else if (Stamina <= 0) Health -= 5 * DeltaTime;
		}
		else if (PlayMovementState == APlayerMovementState::PMS_Dodgging)
		{
			if (Stamina > 0) Stamina -= 2 * DeltaTime;
			else if (Stamina <= 0) Health -= 4 * DeltaTime;
		}
		else if (PlayMovementState == APlayerMovementState::PMS_Swimming)
		{
			if (Stamina > 0) Stamina -= 2 * DeltaTime;
			else if (Stamina <= 0) Health -= 4 * DeltaTime;
		}
		else if (IsSprinting)
		{
			if (Stamina > 0) Stamina -= 1 * DeltaTime;
			else if (Stamina <= 0) Health -= 2 * DeltaTime;
		}
	}
}

void APlayerCharacter::FindFrontObject()
{
	if (!IsAiming)
	{
		FVector Loc;
		FRotator Rot;
		FHitResult Hit;

		GetController()->GetPlayerViewPoint(Loc, Rot);

		TraceDistance = (IsSwitched) ? 200 : 500;

		FVector Start = Loc;
		FVector End = Loc + (Rot.Vector() * TraceDistance);

		FCollisionQueryParams TraceParams;
		GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility, TraceParams);
		if (Hit.Actor != NULL && Hit.Actor != this)
		{
			UE_LOG(LogTemp, Log, TEXT("name : %s"), *Hit.Actor->GetName());
			//If hit actor is Item
			if (Hit.Actor->IsA<AWeapon>())
			{
				UE_LOG(LogTemp, Log, TEXT("Weapon hit"));
				//AWeapon* HitActor = Cast<AWeapon>(Hit.Actor);	

				//HitWeapon = HitActor;
			}
		}
		/*else if (Hit.bBlockingHit == false)
		{
			HitWeapon = nullptr;
		}*/
	}
}

void APlayerCharacter::ClimbTracer()
{
	if (!IsEquippedWeapon)
	{
		FVector Loc;
		FRotator Rot;
		FHitResult Hit;

		Loc = GetActorLocation();
		Rot = GetActorRotation();
		
		ClimbTraceDistance = 70;

		FVector Start = FVector(Loc.X, Loc.Y, Loc.Z + 70);
		FVector End = Loc + (FVector(Rot.Vector().X * ClimbTraceDistance, Rot.Vector().Y * ClimbTraceDistance, Rot.Vector().Z + 70));

		FCollisionQueryParams TraceParams;
		GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

		if (Hit.bBlockingHit && Hit.Actor != this)
		{
			ClimbReady = true;
		}
		else if (Hit.bBlockingHit == false && ClimbReady)
		{
			ClimbReady = false;

			if (PlayMovementState == APlayerMovementState::PMS_Climbing)
			{
				ClimbUp = true;
				IsClimbing = false;
				SetPlayerMovementStatus(APlayerMovementState::PMS_Common);
				GetCharacterMovement()->SetMovementMode(MOVE_None);
				ClimbingUpMovement();
				ClimbingLocation = GetActorLocation();
				ClimbingLocation.Z += 100.f;
				SetActorLocation(ClimbingLocation);
				GetWorld()->GetTimerManager().SetTimer(ClimbUpDelay, this, &APlayerCharacter::ClimbingUpMovement, 0.5f, false);
				GetWorld()->GetTimerManager().SetTimer(ClimbDelay, this, &APlayerCharacter::ClimbingUp, 1.3f, false);
			}
		}
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::StartClimbing);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::ReleaseClimbing);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::ReleaseSprint);

	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &APlayerCharacter::Rolling);

	PlayerInputComponent->BindAction("TakeItem", IE_Pressed, this, &APlayerCharacter::TakeItem);

	PlayerInputComponent->BindAction("EquipFirstWeapon", IE_Pressed, this, &APlayerCharacter::EquipFirstWeapon);
	PlayerInputComponent->BindAction("EquipSecondWeapon", IE_Pressed, this, &APlayerCharacter::EquipSecondWeapon);
	PlayerInputComponent->BindAction("UnEquipWeapon", IE_Pressed, this, &APlayerCharacter::UnEquipWeapon);

	PlayerInputComponent->BindAction("Aiming", IE_Pressed, this, &APlayerCharacter::Aiming);
	PlayerInputComponent->BindAction("Aiming", IE_Released, this, &APlayerCharacter::ReleaseAiming);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerCharacter::ReleaseFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacter::Reload);

	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &APlayerCharacter::SwitchCamera); 

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);

}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::MoveForward(float Value)
{
	if (GetCharacterMovement()->MovementMode == MOVE_Swimming)
	{
		IsSwimming = true;
		SetPlayerMovementStatus(APlayerMovementState::PMS_Swimming);

		MoveForwardValue = Value;
		if ((Controller != NULL) && (Value != 0.0f))
		{
			FRotator Rotation;

			if (IsSwitched)
			{
				Rotation = Controller->GetControlRotation();
			}
			else
			{
				Rotation = Controller->GetControlRotation();
			}

			const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Value);
		}
	}
	else if (PlayMovementState == APlayerMovementState::PMS_Common || (GetCharacterMovement()->MovementMode != MOVE_Swimming && PlayMovementState == APlayerMovementState::PMS_Swimming))
	{
		IsSwimming = false;
		SetPlayerMovementStatus(APlayerMovementState::PMS_Common);
		MoveForwardValue = Value;
		if ((Controller != NULL) && (Value != 0.0f))
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Value);
		}
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (PlayMovementState == APlayerMovementState::PMS_Common || PlayMovementState == APlayerMovementState::PMS_Swimming)
	{
		MoveRightValue = Value;
		if ((Controller != NULL) && (Value != 0.0f))
		{
			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			// add movement in that direction
			AddMovementInput(Direction, Value);
		}
	}

}

void APlayerCharacter::Sprint()
{
	if (PlayMovementState == APlayerMovementState::PMS_Common && !IsAiming)
	{
		IsSprinting = true;

		ReleaseAiming();

		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void APlayerCharacter::ReleaseSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	Sprinted = false;
	IsSprinting = false;
}

void APlayerCharacter::StartClimbing()
{
	if (ClimbReady && !IsEquippedWeapon && PlayMovementState == APlayerMovementState::PMS_Common)
	{
		IsClimbing = true;
		SetPlayerMovementStatus(APlayerMovementState::PMS_Climbing);
		ClimbingLocation = GetActorLocation();
		Climbing();
	}
}

void APlayerCharacter::Climbing()
{
	if (PlayMovementState == APlayerMovementState::PMS_Climbing && !IsEquippedWeapon)
	{   
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);		
		ClimbingMovement();
	}
}

void APlayerCharacter::ClimbingMovement()
{
	if (PlayMovementState == APlayerMovementState::PMS_Climbing)
	{
		ClimbingLocation.Z += (GetWorld()->GetDeltaSeconds() * 200.f);
		SetActorLocation(ClimbingLocation);
		GetWorld()->GetTimerManager().SetTimer(ClimbDelay, this, &APlayerCharacter::ClimbingMovement,  GetWorld()->GetDeltaSeconds(), false);
	}
}

void APlayerCharacter::ClimbingUp()
{
	ClimbUp = false;
	ClimbReady = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void APlayerCharacter::ClimbingUpMovement()
{
	if (ClimbUp)
	{
		ClimbingLocation = GetActorLocation();
		ClimbingLocation += GetActorForwardVector() * 3.f;
		SetActorLocation(ClimbingLocation);
		GetWorld()->GetTimerManager().SetTimer(ClimbUpDelay, this, &APlayerCharacter::ClimbingUpMovement, GetWorld()->GetDeltaSeconds(), false);
	}
}

void APlayerCharacter::ReleaseClimbing()
{
	if (PlayMovementState == APlayerMovementState::PMS_Climbing)
	{
		ClimbReady = false;
		ClimbUp = false;
		IsClimbing = false;
		SetPlayerMovementStatus(APlayerMovementState::PMS_Common);
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void APlayerCharacter::Rolling()
{
	if (GetCharacterMovement()->IsFalling() == false && IsRifleReloading == false && PlayMovementState == APlayerMovementState::PMS_Common)
	{
		SetPlayerMovementStatus(APlayerMovementState::PMS_Dodgging);
		DoggingForce = 40000;
		DoggingVector = GetActorForwardVector();
		PlayAnimMontage(RollMontage, 1, NAME_None);
		RollingMovement();
		GetWorld()->GetTimerManager().SetTimer(ReleaseDoggingDelay, this, &APlayerCharacter::ReleaseRolling, 1.f, false);
	}
}

void APlayerCharacter::RollingMovement()
{
	if (PlayMovementState == APlayerMovementState::PMS_Dodgging && GetCharacterMovement()->IsFalling() == false)
	{
		GetCharacterMovement()->AddImpulse(DoggingVector * DoggingForce);
		GetWorld()->GetTimerManager().SetTimer(DoggingDelay, this, &APlayerCharacter::RollingMovement, GetWorld()->GetDeltaSeconds(), false);
	}
}

void APlayerCharacter::ReleaseRolling()
{
	SetPlayerMovementStatus(APlayerMovementState::PMS_Common);
}

void APlayerCharacter::InteractionOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		//If other actor is item
		if (OtherActor->IsA<AWeapon>() && HitWeapon == nullptr)
		{
			AWeapon* OverlapWeapon = Cast<AWeapon>(OtherActor);	

			HitWeapon = OverlapWeapon;
		}
	}
}

void APlayerCharacter::InteractionOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		HitWeapon = nullptr;
	}

}

void APlayerCharacter::TakeItem()
{
	//Take weapon
	if (HitWeapon)
	{
		if (FirstEquippedWeapon == nullptr)
		{
			HitWeapon->Equip(this);
			FirstEquippedWeapon = HitWeapon;
		}
		else if (SecondEquippedWeapon == nullptr)
		{
			HitWeapon->Equip(this);
			SecondEquippedWeapon = HitWeapon;
		}
		else
		{
			//Change Weapon
			if (EquippedWeaponNumber == 1)
			{
				FirstEquippedWeapon = nullptr;
				HitWeapon->Equip(this);
				FirstEquippedWeapon = HitWeapon;
				FirstEquippedWeapon->SetWeaponRightHand(this);
			}
			else if (EquippedWeaponNumber == 2)
			{
				SecondEquippedWeapon = nullptr;
				HitWeapon->Equip(this);
				SecondEquippedWeapon = HitWeapon;
				SecondEquippedWeapon->SetWeaponRightHand(this);
			}
		}
		HitWeapon = nullptr;
	}
}

void APlayerCharacter::EquipFirstWeapon()
{
	if (IsAiming == false && IsJumping == false && !IsRifleReloading && PlayMovementState == APlayerMovementState::PMS_Common && EquippedWeaponNumber != 1 && FirstEquippedWeapon && !IsEquipping)
	{
		ClimbReady = false;

		if (EquipAnimMonatage)
			PlayAnimMontage(EquipAnimMonatage, 1, NAME_None);

		IsEquipping = true;

		APawn::bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;


		EquippedWeaponNumber = 1;

		GetWorld()->GetTimerManager().SetTimer(EquipDelay, this, &APlayerCharacter::AttachWeapon, 0.6f, false);

		//if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);
	}
}

void APlayerCharacter::EquipSecondWeapon()
{
	if (IsAiming == false && IsJumping == false && !IsRifleReloading && PlayMovementState == APlayerMovementState::PMS_Common && EquippedWeaponNumber != 2 && SecondEquippedWeapon && !IsEquipping)
	{
		ClimbReady = false;

		if (EquipAnimMonatage)
			PlayAnimMontage(EquipAnimMonatage, 1, NAME_None);

		IsEquipping = true;

		APawn::bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		EquippedWeaponNumber = 2;

		GetWorld()->GetTimerManager().SetTimer(EquipDelay, this, &APlayerCharacter::AttachWeapon, 0.6f, false);
		//if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);
	}
}

void APlayerCharacter::UnEquipWeapon()
{
	if (IsAiming == false && IsJumping == false && !IsRifleReloading && PlayMovementState == APlayerMovementState::PMS_Common && EquippedWeaponNumber != 0 && !IsEquipping)
	{
		ClimbReady = false;

		if (UnEquipAnimMonatage)
			PlayAnimMontage(UnEquipAnimMonatage, 1, NAME_None);

		IsEquipping = true;

		if (!IsSwitched)
		{
			APawn::bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}

		IsEquippedWeapon = false;
		EquippedWeaponNumber = 0;

		GetWorld()->GetTimerManager().SetTimer(EquipDelay, this, &APlayerCharacter::AttachWeapon, 0.6f, false);
		//if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);
	}
}

void APlayerCharacter::AttachWeapon()
{
	if (EquippedWeaponNumber == 1)
	{
		IsEquipping = false;

		IsEquippedWeapon = true;

		if(SecondEquippedWeapon != nullptr)
			SecondEquippedWeapon->SetWeaponBack(this, 2);

		RightHandEquippedWeapon = FirstEquippedWeapon;
		RightHandEquippedWeapon->SetWeaponRightHand(this);

		WeaponDamage = RightHandEquippedWeapon->Damage;

		//check rifle
		if (RightHandEquippedWeapon->WeaponKind == EWeaponKind::EWK_AssaultRifle)
		{
			MaxContinuityFire = 3;
		}
		else if (RightHandEquippedWeapon->WeaponKind == EWeaponKind::EWk_HandGun)
		{
			MaxContinuityFire = 1;
		}
	}
	else if (EquippedWeaponNumber == 2)
	{
		IsEquipping = false;

		IsEquippedWeapon = true;

		if (FirstEquippedWeapon != nullptr)
			FirstEquippedWeapon->SetWeaponBack(this, 1);

		RightHandEquippedWeapon = SecondEquippedWeapon;
		RightHandEquippedWeapon->SetWeaponRightHand(this);

		WeaponDamage = RightHandEquippedWeapon->Damage;

		//check rifle
		if (RightHandEquippedWeapon->WeaponKind == EWeaponKind::EWK_AssaultRifle)
		{
			MaxContinuityFire = 3;
		}
		else if (RightHandEquippedWeapon->WeaponKind == EWeaponKind::EWk_HandGun)
		{
			MaxContinuityFire = 1;
		}
	}
	else if (EquippedWeaponNumber == 0)
	{
		IsEquipping = false;

		if (FirstEquippedWeapon != nullptr)
			FirstEquippedWeapon->SetWeaponBack(this, 1);

		if (SecondEquippedWeapon != nullptr)
			SecondEquippedWeapon->SetWeaponBack(this, 2);

		RightHandEquippedWeapon = nullptr;

		WeaponDamage = 0;
	}
}

void APlayerCharacter::Aiming()
{
	if (!RightHandEquippedWeapon)
		return;

	if ((RightHandEquippedWeapon->WeaponKind == EWeaponKind::EWK_AssaultRifle)
		&& !IsRifleReloading && PlayMovementState == APlayerMovementState::PMS_Common && !IsEquipping)
	{
		if (IsSprinting)
		{
			ReleaseSprint();
			Sprinted = true;
		}

		IsAiming = true;
		//GetWeaponAimSocket();
		
		GetCharacterMovement()->MaxWalkSpeed = AimingSpeed;
		GetCharacterMovement()->JumpZVelocity = AimingJump;
	}
}

void APlayerCharacter::ReleaseAiming()
{
	if (!RightHandEquippedWeapon)
		return;

	if (RightHandEquippedWeapon->WeaponKind == EWeaponKind::EWK_AssaultRifle)
	{
		if (Sprinted)
		{
			Sprinted = false;
			IsSprinting = true;
			GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
			GetCharacterMovement()->JumpZVelocity = NormalJump;
		}

		IsAiming = false;
	}
}

void APlayerCharacter::StartFire()
{
	if (!RightHandEquippedWeapon)
		return;

	if (FireAnimMontage 
		&& (RightHandEquippedWeapon->WeaponKind == EWeaponKind::EWk_HandGun || RightHandEquippedWeapon->WeaponKind == EWeaponKind::EWK_AssaultRifle) 
		&& !IsRifleReloading && LoadedBullet > 0)
	{
		ContinuityFire = 0;
		IsFiring = true;
		Fire();
	}
}

void APlayerCharacter::Fire()
{
	if (PlayMovementState == APlayerMovementState::PMS_Dodgging && IsFiring)
	{
		GetWorld()->GetTimerManager().SetTimer(FireDelay, this, &APlayerCharacter::Fire, 0.1f, false);
		return;
	}
	if (FireAnimMontage && IsFiring && !IsRifleReloading && LoadedBullet > 0 && ContinuityFire < MaxContinuityFire)
	{	
		AddControllerPitchInput(-1 * GunRebound * BaseTurnRate * GetWorld()->GetDeltaSeconds());

		ContinuityFire++;
		BulletFire = true;
		if(RightHandEquippedWeapon->WeaponKind == EWeaponKind::EWk_HandGun) PlayAnimMontage(FireHandGunAnimMontage, 1, NAME_None);
		if(RightHandEquippedWeapon->WeaponKind == EWeaponKind::EWK_AssaultRifle) PlayAnimMontage(FireAnimMontage, 1, NAME_None);
		PlayAnimMontage(RightHandEquippedWeapon->FireMontage, 1, NAME_None);
		GetWorld()->GetTimerManager().SetTimer(FireDelay, this, &APlayerCharacter::Fire, 0.1f, false);
		LoadedBullet--;
	}
	else if (LoadedBullet <= 0)
	{
		BulletFire = false;
		Reload();
	}
}

void APlayerCharacter::ReleaseFire()
{
	if (!RightHandEquippedWeapon)
		return;

	if (FireAnimMontage 
		&& (RightHandEquippedWeapon->WeaponKind == EWeaponKind::EWk_HandGun || RightHandEquippedWeapon->WeaponKind == EWeaponKind::EWK_AssaultRifle))
	{
		BulletFire = false;
		IsFiring = false;
	}
}

void APlayerCharacter::Reload()
{
	if (!RightHandEquippedWeapon)
		return;

	if ((LoadedBullet == WeaponMaxBullet || InventoryBulletCount == 0)
		&& (RightHandEquippedWeapon->WeaponKind == EWeaponKind::EWk_HandGun || RightHandEquippedWeapon->WeaponKind == EWeaponKind::EWK_AssaultRifle)
		&& (PlayMovementState != APlayerMovementState::PMS_Common || PlayMovementState != APlayerMovementState::PMS_Swimming))
		return;

	ReleaseAiming();
	BulletFire = false;
	IsRifleReloading = true;
	PlayAnimMontage(RifleReloadingAnimMontage, 1, NAME_None);
	GetWorld()->GetTimerManager().SetTimer(ReloadDelay, this, &APlayerCharacter::FinishReload, 2.5f, false);
}

void APlayerCharacter::FinishReload()
{
	int ReloadBullet = (InventoryBulletCount + LoadedBullet >= WeaponMaxBullet) ? WeaponMaxBullet - LoadedBullet : InventoryBulletCount;
	
	InventoryBulletCount -= ReloadBullet;
	LoadedBullet += ReloadBullet;

	IsRifleReloading = false;
	if (IsFiring)
		Fire();
}

void APlayerCharacter::SwitchCamera()
{
	if (IsAiming == false && IsFiring == false)
	{
		TPCamera->SetFieldOfView(70);
		if (IsSwitched)
		{
			if (RightHandEquippedWeapon)
			{
				if (!(RightHandEquippedWeapon->WeaponKind == EWeaponKind::EWk_HandGun || RightHandEquippedWeapon->WeaponKind == EWeaponKind::EWK_AssaultRifle))
				{
					APawn::bUseControllerRotationYaw = false;
					GetCharacterMovement()->bOrientRotationToMovement = true;
				}
			}
			
			TPCamera->SetActive(true);
			FPCamera->SetActive(false);
			IsSwitched = false;
		}
		else
		{
			APawn::bUseControllerRotationYaw = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			TPCamera->SetActive(false);
			FPCamera->SetActive(true);
			IsSwitched = true;
		}
	}
}

