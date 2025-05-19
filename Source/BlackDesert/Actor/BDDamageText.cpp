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

    // 위젯 설정
    TextWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // Screen으로 변경 시도
    TextWidgetComponent->SetDrawAtDesiredSize(true);
    TextWidgetComponent->SetVisibility(true);
    TextWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 크기 설정
    TextWidgetComponent->SetDrawSize(FVector2D(200.0f, 100.0f));

    // Z-Order 높게 설정
    TextWidgetComponent->SetRelativeLocation(FVector(0, 0, 30.0f));

    // 기본 값
    SetLifeSpan(LifeSpan);
    // /Script/UMGEditor.WidgetBlueprint'/Game/Blueprint/UI/BP_DamageText.BP_DamageText'
    // 위젯 클래스 로드
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

// BeginPlay도 수정
void ABDDamageText::BeginPlay()
{
    Super::BeginPlay();

    // 위젯 클래스가 설정되었는지 확인
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

    // 위젯 인스턴스 생성 (필요한 경우)
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


    // 항상 카메라를 향하도록 설정
    TextWidgetComponent->SetWorldRotation(FRotator(0, 180, 0));
}

void ABDDamageText::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 텍스트가 천천히 위로 올라가도록 설정
    FVector NewLocation = GetActorLocation();
    NewLocation.Z += RiseSpeed * DeltaTime;
    SetActorLocation(NewLocation);

    // 항상 카메라를 향하도록 설정
    if (GEngine && GEngine->GetFirstLocalPlayerController(GetWorld()))
    {
        FVector CameraLocation = GEngine->GetFirstLocalPlayerController(GetWorld())->PlayerCameraManager->GetCameraLocation();
        FRotator LookAtRotation = (CameraLocation - GetActorLocation()).Rotation();
        TextWidgetComponent->SetWorldRotation(FRotator(0, LookAtRotation.Yaw + 180.0f, 0));
    }
}


void ABDDamageText::SetDamageText(float Damage)
{
    // 위젯 업데이트
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