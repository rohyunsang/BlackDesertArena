// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "UI/BDDamageTextWidget.h" 
#include "BDDamageText.generated.h"

UCLASS()
class BLACKDESERT_API ABDDamageText : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABDDamageText();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWidgetComponent* TextWidgetComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage Text")
    float LifeSpan = 2.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage Text")
    float RiseSpeed = 100.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage Text")
    bool bCriticalHit = false;

    void SetDamageText(float Damage);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 위젯 클래스 참조 수정
    UPROPERTY(EditDefaultsOnly, Category = "Widget")
    TSubclassOf<UBDDamageTextWidget> DamageWidgetClass;

};
