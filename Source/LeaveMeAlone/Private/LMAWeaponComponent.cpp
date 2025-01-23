// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "LMAWeaponComponent.h"
#include "GameFramework/Character.h"
#include "LMABaseWeapon.h"
#include "LMAReloadFinishedAnimNotify.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeapon, All, All);

ULMAWeaponComponent::ULMAWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULMAWeaponComponent::Fire()
{
	if (Weapon && !AnimReloading)
	{
		Weapon->Fire();
		ACharacter* Character = Cast<ACharacter>(GetOwner());
		Character->PlayAnimMontage(FireMontage);		
	}
}


void ULMAWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();
	InitAnimNotify();	
}


void ULMAWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (Weapon->IsCurrentClipEmpty())
	{
		ULMAWeaponComponent::Reload();
	}

}

void ULMAWeaponComponent::SpawnWeapon()
{
	Weapon = GetWorld()->SpawnActor<ALMABaseWeapon>(WeaponClass);
	if (Weapon)
	{
		const auto Character = Cast<ACharacter>(GetOwner());
		if (Character)
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
			Weapon->AttachToComponent(Character->GetMesh(), AttachmentRules, "r_Weapon_Socket");	
		}
	}
}

void ULMAWeaponComponent::InitAnimNotify()
{
	if(!ReloadMontage) return;
	const auto NotifiesEvents = ReloadMontage->Notifies;
	for (auto NotifyEvent : NotifiesEvents)
	{
		auto ReloadFinish = Cast<ULMAReloadFinishedAnimNotify>(NotifyEvent.Notify);
		if (ReloadFinish)
		{
			ReloadFinish->OnNotifyReloadFinished.AddUObject(this,&ULMAWeaponComponent::OnNotifyReloadFinished);
			break;
		}
	}
}

void ULMAWeaponComponent::OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh)
{
	const auto Character = Cast<ACharacter>(GetOwner());
	if (Character->GetMesh() == SkeletalMesh)
	{
		AnimReloading = false;
		UE_LOG(LogWeapon, Display, TEXT("AnimLoading = false"));
	}
}

bool ULMAWeaponComponent::CanReload() const
{
	return !AnimReloading;
}

void ULMAWeaponComponent::Reload()
{
	if(!CanReload()) return;
	Weapon->ChangeClip();
	AnimReloading = true;
	UE_LOG(LogWeapon, Display, TEXT("AnimLoading = true"));
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	Character->PlayAnimMontage(ReloadMontage);
}

int32 ULMAWeaponComponent::GetAmmoWeapon()
{
	UE_LOG(LogWeapon, Display, TEXT("from baseBullets = %s"), *FString::FromInt(Weapon->GetAmmoWeapon()));
	return Weapon->GetAmmoWeapon();
}