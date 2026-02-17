#include "MultiplayerSessionSubsystem.h"
#include "OnlineSubsystem.h"
#include <Online/OnlineSessionNames.h>

void PrintString(const FString& Str)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, Str);
	}
}

UMultiplayerSessionSubsystem::UMultiplayerSessionSubsystem()
{
	CreateServerAfterDestroy = false;
	DestroyServerName = "";
	ServerNameToFind = "";
	MySessionName = FName("Tik Tak Toe session name");
}

/*
void UMultiplayerSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		FString SubsystemName = OnlineSubsystem->GetSubsystemName().ToString();
		PrintString(SubsystemName);

		SessionInterface = OnlineSubsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionSubsystem::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionSubsystem::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerSessionSubsystem::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionSubsystem::OnJoinSessionComplete);
		}
	}
}

void UMultiplayerSessionSubsystem::Deinitialize()
{
}

void UMultiplayerSessionSubsystem::CreateServer(FString ServerName)
{
	if (ServerName.IsEmpty())
	{
		PrintString("Server name is empty!");
		//ServerCreateDel.Broadcast(false);
		return;
	}

	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(MySessionName);

	if (ExistingSession)
	{
		FString Msg = FString::Printf(TEXT("Session with name %s already exists, destroying it."), *MySessionName.ToString());
		PrintString(Msg);
		CreateServerAfterDestroy = true;
		DestroyServerName = ServerName;
		SessionInterface->DestroySession(MySessionName);
		return;
	}

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.NumPublicConnections = 2; // Number of players allowed
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinViaPresence = true;
	bool bIsLANMatch = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		bIsLANMatch = true;
	}
	SessionSettings.bIsLANMatch = bIsLANMatch;

	SessionSettings.Set(FName("SERVE_NAME"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	SessionInterface->CreateSession(0, MySessionName, SessionSettings);
}

void UMultiplayerSessionSubsystem::FindServer(FString ServerName)
{
	PrintString("Find server called");
	if (ServerName.IsEmpty())
	{
		PrintString("Server name is empty!");
		//ServerJoinDel.Broadcast(false);
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	bool bIsLAN = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		bIsLAN = true;
	}

	SessionSearch->bIsLanQuery = bIsLAN;
	SessionSearch->MaxSearchResults = 9999;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	ServerNameToFind = ServerName;
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMultiplayerSessionSubsystem::FindAllSessions()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	bool bIsLAN = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		bIsLAN = true;
	}
	SessionSearch->bIsLanQuery = bIsLAN;
	SessionSearch->MaxSearchResults = 9999;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMultiplayerSessionSubsystem::JoinServer(FString SessionName)
{
	FindServer(SessionName);
	return;
	PrintString("Join server");
	FOnlineSessionSearchResult* CorrectResult = 0;

	if (SessionName.IsEmpty())
	{
		PrintString("Session ID is empty!");
		//ServerJoinDel.Broadcast(false);
		return;
	}
	if (!SessionInterface.IsValid())
	{
		PrintString("Session interface is not valid!");
		//ServerJoinDel.Broadcast(false);
		return;
	}
	for (FOnlineSessionSearchResult Result : SessionSearch->SearchResults)
	{
		if (Result.IsValid())
		{
			FString ServerName = "No-name";
			Result.Session.SessionSettings.Get(FName("SERVE_NAME"), ServerName);
			PrintString(FString::Printf(TEXT("Found session with ID: %s"), *ServerName));
			if (ServerName.Equals(SessionName))
			{
				CorrectResult = &Result;
				break;
			}
		}
	}

	if (!CorrectResult)
	{
		PrintString(FString::Printf(TEXT("Couldn't find session with ID: %s"), *SessionName));
		//ServerJoinDel.Broadcast(false);
		return;
	}

	SessionInterface->JoinSession(0, MySessionName, *CorrectResult);
}

void UMultiplayerSessionSubsystem::OnCreateSessionComplete(FName SessionName, bool WasSuccessful)
{
	PrintString(FString::Printf(TEXT("OnCreateSessionComplete: SessionName=%s was sucess %d"), *SessionName.ToString(), WasSuccessful));

	//ServerCreateDel.Broadcast(WasSuccessful);

	if (WasSuccessful)
	{
		/*FString Path = "/Game/ThirdPerson/Maps/ThirdPersonMap?listen";
		if (!GameMapPath.IsEmpty())
		{
			Path = FString::Printf(TEXT("%s?listen"), *GameMapPath);
		}
		GetWorld()->ServerTravel(Path);
	}
}

void UMultiplayerSessionSubsystem::OnDestroySessionComplete(FName SessionName, bool WasSuccessful)
{
	PrintString("OnDestroySessionComplete: SessionName=" + SessionName.ToString() + " was successful: " + FString::FromInt(WasSuccessful) + "\n");
	if (CreateServerAfterDestroy) {
		CreateServerAfterDestroy = false;
		CreateServer(DestroyServerName);
	}
}

void UMultiplayerSessionSubsystem::OnFindSessionsComplete(bool WasSuccessful)
{
	if (!WasSuccessful) return;
	TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;
	FOnlineSessionSearchResult* CorrectResult = 0;

	TArray<FString> SessionNames;

	if (Results.Num() > 0)
	{
		for (FOnlineSessionSearchResult Result : Results)
		{
			if (Result.IsValid())
			{
				FString ServerName = "No-name";
				Result.Session.SessionSettings.Get(FName("SERVE_NAME"), ServerName);
				SessionNames.Add(ServerName);
			}
		}

		//ServerFindAllDel.Broadcast(SessionNames);
	}

	if (ServerNameToFind.IsEmpty()) return;


	if (Results.Num() > 0)
	{
		FString Msg = FString::Printf(TEXT("Found %d sessions:"), Results.Num());
		PrintString(Msg);

		for (FOnlineSessionSearchResult Result : Results)
		{
			if (Result.IsValid())
			{
				FString ServerName = "No-name";
				Result.Session.SessionSettings.Get(FName("SERVE_NAME"), ServerName);

				if (ServerName.Equals(ServerNameToFind))
				{
					CorrectResult = &Result;
					FString ServerMng = FString::Printf(TEXT("Found server with name: %s"), *ServerName);
					PrintString(ServerMng);
					break;
				}
			}
		}

		if (CorrectResult)
		{
			SessionInterface->JoinSession(0, MySessionName, *CorrectResult);
		}
		else
		{
			PrintString(FString::Printf(TEXT("couldn't find server: %s"), *ServerNameToFind));
			//ServerJoinDel.Broadcast(false);
		}
	}
	else
	{
		PrintString("Zero sessions found.");
		//ServerJoinDel.Broadcast(false);
	}
}

void UMultiplayerSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	//ServerJoinDel.Broadcast(Result == EOnJoinSessionCompleteResult::Success);
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString Msg = FString::Printf(TEXT("Successfully joined session %s"), *SessionName.ToString());
		PrintString(Msg);

		FString Address = "";
		bool Success = SessionInterface->GetResolvedConnectString(MySessionName, Address);
		if (Success)
		{
			PrintString(FString::Printf(TEXT("Address: %s"), *Address));
			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
		else
		{
			PrintString("GetResolvedConnectString returned false!");
		}
	}
	else
	{
		PrintString("OnJoinSessionComplete failed");
	}
}
*/