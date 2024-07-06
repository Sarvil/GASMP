// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MenuWidget.h"
#include "Components/Button.h"
#include "MultiplayerSessions/Public/Subsystems/MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"


void UMenuWidget::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
    PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
    NumPublicConnections = NumberOfPublicConnections;
    MatchType = TypeOfMatch;
    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable - true;

    UWorld* World = GetWorld();
    if(World)
    {
        APlayerController* PlayerController = World->GetFirstPlayerController();
        if(PlayerController)
        {
            FInputModeUIOnly InputModeData;
            InputModeData.SetWidgetToFocus(TakeWidget());
            InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(true);
        }
    }
    UGameInstance* GameInstance = GetGameInstance();
    if(GameInstance)
    {
        MultiplayerSessionSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
    }
    if(MultiplayerSessionSubsystem)
    {
        MultiplayerSessionSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
        MultiplayerSessionSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
        MultiplayerSessionSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::onJoinSession);
        MultiplayerSessionSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
        MultiplayerSessionSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
    }
}

bool UMenuWidget::Initialize()
{

    if(!Super::Initialize())
    {
        return false;
    }
    if(HostButton)
    {
        HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
    }
    if(JoinButton)
    {
        JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
    }

    return true;
}

void UMenuWidget::NativeDestruct()
{
    MenuTearDown();

    Super::NativeDestruct();
}

void UMenuWidget::OnCreateSession(bool bWasSuccessful)
{
    if(bWasSuccessful)
    {
        if(GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.f,FColor::Yellow, FString(TEXT("Session Created Successfully!")));
        }
        UWorld* World = GetWorld();
        if(World)
        {
            World->ServerTravel(PathToLobby);
        }
    }
    else
    {
        if(GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.f,FColor::Red, FString(TEXT("Failed to Create Session!")));
        }
        HostButton->SetIsEnabled(true);
        
    }
}

void UMenuWidget::OnFindSessions(const TArray<FOnlineSessionSearchResult> &SessionResults, bool bWasSuccessful)
{
    if(MultiplayerSessionSubsystem == nullptr)
    {
        return;
    }
    for(auto Result : SessionResults)
    {
        FString SettingsValue;
        Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
        if(SettingsValue == MatchType)
        {
            MultiplayerSessionSubsystem->JoinSession(Result);
            return;
        }
    }
    if(bWasSuccessful || SessionResults.Num() == 0)
    {
        JoinButton->SetIsEnabled(true);
    }
}

void UMenuWidget::onJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if(Subsystem)
    {
        IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
        if(SessionInterface.IsValid())
        {
            FString Address;
            SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);
            APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		    if(PlayerController)
		    {
			    PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		    }
        }
    }
    if(Result != EOnJoinSessionCompleteResult::Success)
    {
        JoinButton->SetIsEnabled(true);
    }
}

void UMenuWidget::OnDestroySession(bool bWasSuccessful)
{

}

void UMenuWidget::OnStartSession(bool bWasSuccessful)
{

}

void UMenuWidget::HostButtonClicked()
{
    HostButton->SetIsEnabled(false);
    if(MultiplayerSessionSubsystem)
    {
        MultiplayerSessionSubsystem->CreateSession(NumPublicConnections, MatchType);
    }
}

void UMenuWidget::JoinButtonClicked()
{
    JoinButton->SetIsEnabled(false);
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.f,FColor::Yellow, FString(TEXT("Join Button Clicked")));
    }
}

void UMenuWidget::MenuTearDown()
{
    RemoveFromParent();
    UWorld* World = GetWorld();
    if(World)
    {
        APlayerController* PlayerController = World->GetFirstPlayerController();
        if(PlayerController)
        {
            FInputModeGameOnly InputModeData;
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(false);
        }
    }
}
