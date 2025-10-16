
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameModeDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class STOP_API UGameModeDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Game Mode")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Game Mode", meta=(MultiLine=true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Game Mode")
	TSoftClassPtr<AGameModeBase> GameModeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Game Mode")
	FName ID; // Unique identifier like "Endless", "Training"
};
