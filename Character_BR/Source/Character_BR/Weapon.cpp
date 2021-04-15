// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "PlayerCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/Actor.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);

	SceneCompoennt = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SceneCompoennt->SetupAttachment(GetRootComponent());

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(SceneCompoennt);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(SceneCompoennt);

	WeaponState = EWeaponState::EWS_NoOwner;

	Damage = 25.f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	bRotate = true;

}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bRotate)
	{
		FRotator Rotation = GetActorRotation();
		Rotation.Yaw += DeltaTime * 45.f;
		SetActorRotation(Rotation);
	}
}

//Player Equip
void AWeapon::Equip(APlayerCharacter* Char)
{
	if (Char && WeaponState == EWeaponState::EWS_NoOwner)
	{
		bRotate = false;

		WeaponState = EWeaponState::EWS_PickUp;

		SetInstigator(Char->GetController());

		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		SkeletalMesh->SetSimulatePhysics(false);

		const USkeletalMeshSocket* Socket;

		if (Char->FirstEquippedWeapon == nullptr)
		{
			Socket = Char->GetMesh()->GetSocketByName("BackWeaponSocket1");
			Socket->AttachActor(this, Char->GetMesh());
		}
		else if (Char->SecondEquippedWeapon == nullptr)
		{
			Socket = Char->GetMesh()->GetSocketByName("BackWeaponSocket2");
			Socket->AttachActor(this, Char->GetMesh());
		}

		//if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);
	}
}

void AWeapon::SetWeaponRightHand(class APlayerCharacter* Char)
{
	if (Char && WeaponState == EWeaponState::EWS_PickUp)
	{
		const USkeletalMeshSocket* Socket;

		WeaponState = EWeaponState::EWS_Equipped;
		Socket = Char->GetMesh()->GetSocketByName("RightWeaponSocket");
		Socket->AttachActor(this, Char->GetMesh());
		Char->HitWeapon = nullptr;

		//if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);
	}
}

void AWeapon::SetWeaponBack(class APlayerCharacter* Char, int Number)
{
	if (Char)
	{
		const USkeletalMeshSocket* Socket;

		if (Number == 1)
		{
			WeaponState = EWeaponState::EWS_PickUp;
			Socket = Char->GetMesh()->GetSocketByName("BackWeaponSocket1");
			Socket->AttachActor(this, Char->GetMesh());
			Char->HitWeapon = nullptr;
		}
		else if (Number == 2)
		{
			WeaponState = EWeaponState::EWS_PickUp;
			Socket = Char->GetMesh()->GetSocketByName("BackWeaponSocket2");
			Socket->AttachActor(this, Char->GetMesh());
			Char->HitWeapon = nullptr;
		}

		//if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);
	}
}

void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		/*AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (Enemy->HitParticles)
			{
				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
				if (WeaponSocket)
				{
					FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
			if (Enemy->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}
			if (DamageTypeClass)
			{
				Enemy->TakeDamage(Damage);
			}
		}*/
	}
}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWeapon::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}