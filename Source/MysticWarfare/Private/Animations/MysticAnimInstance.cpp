// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/MysticAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "../MysticWarfareCharacter.h"

void UMysticAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Character = Cast<AMysticWarfareCharacter>(TryGetPawnOwner());
}

void UMysticAnimInstance::NativeUpdateAnimation(float deltaSeconds)
{
	Super::NativeUpdateAnimation(deltaSeconds);
	if(IsValid(Character))
	{
		FVector Velocity = Character->GetVelocity();
		Speed = Velocity.SquaredLength();
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());
		bIsInAir = Character->GetMovementComponent()->IsFalling();

		FRotator aim = Character->GetBaseAimRotation();
		FRotator rot = Character->GetActorRotation();
		Yaw = aim.Yaw - rot.Yaw;
		Pitch = aim.Pitch;
		if (Pitch > 180)
		{
			Pitch -= 360;
		}

	}
}