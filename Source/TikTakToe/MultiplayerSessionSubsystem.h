// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerCreateDelegate, bool, WasSuccessfull);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerJoinDelegate, bool, WasSuccessfull);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerFindAllSessioneDelegate, const TArray<FString>&, Sessions);


UCLASS()
class TIKTAKTOE_API UMultiplayerSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	bool CreateServerAfterDestroy;
	FString DestroyServerName;
	FString ServerNameToFind;
	FName MySessionName;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;


	UPROPERTY(BlueprintAssignable)
	FServerCreateDelegate ServerCreateDel;

	UPROPERTY(BlueprintAssignable)
	FServerJoinDelegate ServerJoinDel;

	UPROPERTY(BlueprintAssignable)
	FServerFindAllSessioneDelegate ServerFindAllDel;

	UPROPERTY(BlueprintReadWrite)
	FString GameMapPath;

public:

	UMultiplayerSessionSubsystem();

	/*void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	IOnlineSessionPtr SessionInterface;

	UFUNCTION(BlueprintCallable)
	void CreateServer(FString ServerName);

	UFUNCTION(BlueprintCallable)
	void FindServer(FString ServerName);

	UFUNCTION(BlueprintCallable)
	void FindAllSessions();

	UFUNCTION(BlueprintCallable)
	void JoinServer(FString SessionName);

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	*/
};
