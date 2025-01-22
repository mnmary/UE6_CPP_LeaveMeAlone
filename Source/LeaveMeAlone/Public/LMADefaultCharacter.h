// LeaveMeAlone Game by Netologiya. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LMAWeaponComponent.h"
#include "LMADefaultCharacter.generated.h"


class UCameraComponent;
class USpringArmComponent;
class ULMAHealthComponent;

UCLASS()
class LEAVEMEALONE_API ALMADefaultCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	ALMADefaultCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY()
	UDecalComponent* CurrentCursor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Zoom", 
		meta = (ClampMin = "20", UIMin = "20", ClampMax = "500", UIMax = "500"))
	float StepArmLength = 70.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Zoom",
		meta = (ClampMin = "500", UIMin = "500", ClampMax = "3000", UIMax = "3000"))
	float MaxArmLength = 2000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Zoom", 
		meta = (ClampMin = "300", UIMin = "300", ClampMax = "500", UIMax = "500"))
	float MinArmLength = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float SprintSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float WalkSpeed = 300.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprint")
	bool isSprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprint")
	int MaxStamina = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprint")
	int Stamina = 100;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	int StaminaDrain = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	int StaminaRefill = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float DelayBeforeRefill = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float DelayBeforeDrain = 10.f;

	float CurrentDelayRefillTime;
	float CurrentDelayDrainTime;
	bool bHeasStamina;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	ULMAWeaponComponent* WeaponComponent;



	bool bIsFire;
	float MaxTimerFire = 100.f;
	float CurrentTimerFire;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components|Health")
	ULMAHealthComponent* HealthComponent;	

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* DeathMontage;	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	float YRotation = -75.0f;  
	float ArmLength = 1400.0f; 
	float FOV = 55.0f;		   

	void MoveForward(float Value);
	void MoveRight(float Value);

	//sprint action
	void StartSprint();
	void StopSprint();

	//stamina
	void UpdateStamina();


	void ZoomIn();
	void ZoomOut();

	void Fire();
	void StopFire();
	void UpdateFire();

	void Reload();

	void OnDeath();
	void OnHealthChanged(float NewHealth);

	void RotationPlayerOnCursor();	

};