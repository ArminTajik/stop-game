
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HoldableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHoldableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STOP_API IHoldableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction")
	void OnHoldStart(APlayerController* Caller);
	
	UFUNCTION()
	virtual void OnHoldStartPure(APlayerController* Caller) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction")
	void OnHoldEnd(APlayerController* Caller);

	UFUNCTION()
	virtual void OnHoldEndPure(APlayerController* Caller) = 0;
};
