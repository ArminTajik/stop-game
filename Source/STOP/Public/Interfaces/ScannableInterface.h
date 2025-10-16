
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ScannableInterface.generated.h"

class AScannerActor;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UScannableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STOP_API IScannableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Scanning")
	void OnStartScan(AScannerActor* ScannerActor);

	UFUNCTION()
	virtual void OnStartScanPure(AScannerActor* ScannerActor) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Scanning")
	void OnEndScan(AScannerActor* ScannerActor);

	UFUNCTION()
	virtual void OnEndScanPure(AScannerActor* ScannerActor) = 0;
};
