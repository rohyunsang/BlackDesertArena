// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/BDDamageText.h"
#include "UI/BDHUDUI.h"
#include "UI/BDDamageTextWidget.h"

ABDDamageText::ABDDamageText()
{
    UE_LOG(LogTemp, Warning, TEXT("DamageTextLog ABDDamageText() "));
    PrimaryActorTick.bCanEverTick = true;

    TextWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("TextWidget"));
    RootComponent = TextWidgetComponent;

    // ���� ����
    TextWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // Screen���� ���� �õ�
    TextWidgetComponent->SetDrawAtDesiredSize(true);
    TextWidgetComponent->SetVisibility(true);
    TextWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // ũ�� ����
    TextWidgetComponent->SetDrawSize(FVector2D(200.0f, 100.0f));

    // Z-Order ���� ����
    TextWidgetComponent->SetRelativeLocation(FVector(0, 0, 30.0f));

    // �⺻ ��
    SetLifeSpan(LifeSpan);
    // /Script/UMGEditor.WidgetBlueprint'/Game/Blueprint/UI/BP_DamageText.BP_DamageText'
    // ���� Ŭ���� �ε�
    static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/Blueprint/UI/BP_DamageText"));
    if (WidgetClassFinder.Succeeded())
    {
        DamageWidgetClass = WidgetClassFinder.Class;
        TextWidgetComponent->SetWidgetClass(DamageWidgetClass);
        UE_LOG(LogTemp, Warning, TEXT("DamageTextLog WidgetClassFinder Succeeded"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("DamageTextLog BP_DamageText is not found"));
    }
}

// BeginPlay�� ����
void ABDDamageText::BeginPlay()
{
    Super::BeginPlay();

    // ���� Ŭ������ �����Ǿ����� Ȯ��
    if (!TextWidgetComponent->GetWidgetClass())
    {
        UE_LOG(LogTemp, Error, TEXT("DamageTextLog ABDDamageText: Widget class not set!"));
        if (DamageWidgetClass)
        {
            TextWidgetComponent->SetWidgetClass(DamageWidgetClass);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("DamageTextLog ABDDamageText:DamageWidgetClass not "));
        }
    }

    // ���� �ν��Ͻ� ���� (�ʿ��� ���)
    if (!TextWidgetComponent->GetUserWidgetObject() && DamageWidgetClass)
    {
        UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), DamageWidgetClass);
        if (WidgetInstance)
        {
            TextWidgetComponent->SetWidget(WidgetInstance);
            UE_LOG(LogTemp, Warning, TEXT("DamageTextLog Widget created successfully"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("DamageTextLog Failed to create widget instance"));
        }
    }


    // �׻� ī�޶� ���ϵ��� ����
    TextWidgetComponent->SetWorldRotation(FRotator(0, 180, 0));
}

void ABDDamageText::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // �ؽ�Ʈ�� õõ�� ���� �ö󰡵��� ����
    FVector NewLocation = GetActorLocation();
    NewLocation.Z += RiseSpeed * DeltaTime;
    SetActorLocation(NewLocation);

    // �׻� ī�޶� ���ϵ��� ����
    if (GEngine && GEngine->GetFirstLocalPlayerController(GetWorld()))
    {
        FVector CameraLocation = GEngine->GetFirstLocalPlayerController(GetWorld())->PlayerCameraManager->GetCameraLocation();
        FRotator LookAtRotation = (CameraLocation - GetActorLocation()).Rotation();
        TextWidgetComponent->SetWorldRotation(FRotator(0, LookAtRotation.Yaw + 180.0f, 0));
    }
}


void ABDDamageText::SetDamageText(float Damage)
{
    // ���� ������Ʈ
    UBDDamageTextWidget* DamageWidget = Cast<UBDDamageTextWidget>(TextWidgetComponent->GetUserWidgetObject());
    if (DamageWidget)
    {
        DamageWidget->SetDamageValue(Damage);
        UE_LOG(LogTemp, Warning, TEXT("DamageTextLog DamageWidget is not null"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("DamageTextLog DamageWidget is null"));
    }
}