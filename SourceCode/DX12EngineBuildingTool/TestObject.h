
#pragma once

#include "CoreMinimal.h"

/**
 *
 */
UCLASS()
class MMOARPG_API AMMOARPGPlayerState : public APlayerStateBase
{
	GENERATED_BT_BODY()

	UPROPERTY(CodeType = Resources)
	int Num;

	UPROPERTY(CodeType = Resources)
	bool PlayerState;

public:
	UFUNCTION(CodeType = Event)
	void Hello(int c,float b);

	UFUNCTION(CodeType = Describe)
	void Hello1();
};
