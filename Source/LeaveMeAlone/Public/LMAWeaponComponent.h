// LeaveMeAlone Game by Netologiya. All RightsReserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LMAWeaponComponent.generated.h"

class ALMABaseWeapon;
class UAnimMontage;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LEAVEMEALONE_API ULMAWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULMAWeaponComponent();

	void Fire();
	void Reload();

protected:
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<ALMABaseWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	UAnimMontage* FireMontage;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	UAnimMontage* ReloadMontage;
	
	UFUNCTION(BlueprintCallable)
	int32 GetAmmoWeapon();
	
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	UPROPERTY()
	ALMABaseWeapon* Weapon = nullptr;
	void SpawnWeapon();

	bool AnimReloading = false;
	void InitAnimNotify();
	void OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh);
	bool CanReload() const;	
};
