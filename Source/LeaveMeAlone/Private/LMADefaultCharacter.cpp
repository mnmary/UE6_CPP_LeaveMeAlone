// LeaveMeAlone Game by Netologiya. All Rights Reserved.

#include "LMADefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "LMAHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeapon, All, All);

// Sets default values
ALMADefaultCharacter::ALMADefaultCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true);	
	SpringArmComponent->TargetArmLength = ArmLength;
	SpringArmComponent->SetRelativeRotation(FRotator(YRotation, 0.0f, 0.0f));
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bEnableCameraLag = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetFieldOfView(FOV);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	WeaponComponent = CreateDefaultSubobject<ULMAWeaponComponent>("Weapon");	
	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");
	
}

// Called when the game starts or when spawned
void ALMADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(CursorMaterial))
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}


	if (ArmLength < MinArmLength)
	{
		ArmLength = MinArmLength;
	}
	else if (ArmLength > MaxArmLength)
	{
		ArmLength = MaxArmLength;
	}
	SpringArmComponent->TargetArmLength = ArmLength;

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Stamina = %d  MAX = %d"), Stamina, MaxStamina));

	OnHealthChanged(HealthComponent->GetHealth());
	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);
	HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);	
}

// Called every frame
void ALMADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (CurrentCursor)
		{
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		}
	}

	UpdateStamina();

	UpdateFire();

	if (!(HealthComponent->IsDead()))
	{
		RotationPlayerOnCursor();
	}

}

// Called to bind functionality to input
void ALMADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALMADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALMADefaultCharacter::MoveRight);

	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &ALMADefaultCharacter::ZoomIn);
	PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, this, &ALMADefaultCharacter::ZoomOut);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ALMADefaultCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ALMADefaultCharacter::StopSprint);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ALMADefaultCharacter::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ALMADefaultCharacter::StopFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ALMADefaultCharacter::Reload);

}


void ALMADefaultCharacter::Reload()
{
	if (WeaponComponent)
	{
		WeaponComponent->Reload();
	}
}

void ALMADefaultCharacter::Fire() 
{
	if (WeaponComponent) bIsFire = true;
	WeaponComponent->Fire();
	CurrentTimerFire = MaxTimerFire;
}
void ALMADefaultCharacter::StopFire() 
{
		bIsFire = false;
}

void ALMADefaultCharacter::MoveForward(float Value) 
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void ALMADefaultCharacter::MoveRight(float Value) 
{
	AddMovementInput(GetActorRightVector(), Value);
}

void ALMADefaultCharacter::StartSprint()
{
	
	if (bHeasStamina)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		if (GetVelocity().Size() >= 0.5)
		{
			isSprinting = true;
		}
		else
		{
			isSprinting = false;		
		}
	}
}

void ALMADefaultCharacter::StopSprint()
{
	
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	isSprinting = false;
}

void ALMADefaultCharacter::UpdateStamina()
{
	if (isSprinting && Stamina > 0)
	{
		CurrentDelayDrainTime --;
		if (CurrentDelayDrainTime <= 0)
		{
			Stamina -= StaminaDrain;
			CurrentDelayDrainTime = DelayBeforeDrain;
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Stamina = %d"), Stamina));
		}
		CurrentDelayRefillTime = DelayBeforeRefill;
	}

	if (!isSprinting && Stamina < MaxStamina)
	{
		CurrentDelayRefillTime --;
		if (CurrentDelayRefillTime <= 0)
		{
			Stamina += StaminaRefill;
			CurrentDelayRefillTime = DelayBeforeRefill;
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Stamina = %d"), Stamina));
		}
		CurrentDelayDrainTime = DelayBeforeDrain;
	}

	if (Stamina <= 0)
	{
		bHeasStamina = false;
		StopSprint();
	}
	else
	{
		bHeasStamina = true;
	}
	
}

void ALMADefaultCharacter::UpdateFire()
{
	if (bIsFire)
	{
		CurrentTimerFire --;
		if (CurrentTimerFire <= 0)
		{
			CurrentTimerFire = MaxTimerFire;
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString(TEXT("FIRE")));
			WeaponComponent->Fire();


		}

	}
}

void ALMADefaultCharacter::ZoomIn() 
{
	ArmLength -= StepArmLength;
	if (ArmLength < MinArmLength)
	{
		ArmLength = MinArmLength;
	}	
	SpringArmComponent->TargetArmLength = ArmLength;
}

void ALMADefaultCharacter::ZoomOut() 
{	
	ArmLength += StepArmLength;
	if (ArmLength > MaxArmLength)
	{
		ArmLength = MaxArmLength;
	}	
	SpringArmComponent->TargetArmLength = ArmLength;
}

void ALMADefaultCharacter::OnDeath()
{
	CurrentCursor->DestroyRenderState_Concurrent();

	PlayAnimMontage(DeathMontage);

	GetCharacterMovement()->DisableMovement();

	SetLifeSpan(5.0f);

	if (Controller)
	{
		Controller->ChangeState(NAME_Spectating);
	}
	
	WeaponComponent->DestroyComponent();

	UE_LOG(LogWeapon, Display, TEXT("IS DEAD"))
	
}

void ALMADefaultCharacter::OnHealthChanged(float NewHealth)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Health = %f"), NewHealth));
}

void ALMADefaultCharacter::RotationPlayerOnCursor()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (CurrentCursor)
		{
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		}
	}
}
