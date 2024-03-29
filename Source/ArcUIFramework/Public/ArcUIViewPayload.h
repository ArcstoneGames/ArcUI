// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

#include "InstancedStruct.h"

#include "ArcUIViewPayload.generated.h"

USTRUCT(BlueprintType)
struct ARCUIFRAMEWORK_API FArcUIViewPayload
{
	GENERATED_BODY()
};


/**
 * Convenience View Payload containing ints
 */
USTRUCT(BlueprintType)
struct ARCUIFRAMEWORK_API FArcUIViewPayload_Ints final : public FArcUIViewPayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Values;
};


/**
 * Convenience View Payload containing floats
 */
USTRUCT(BlueprintType)
struct ARCUIFRAMEWORK_API FArcUIViewPayload_Floats final : public FArcUIViewPayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> Values;
};


/**
 * Convenience View Payload containing Gameplay Tags
 */
USTRUCT(BlueprintType)
struct ARCUIFRAMEWORK_API FArcUIViewPayload_Tags final : public FArcUIViewPayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> Values;
};


UINTERFACE()
class ARCUIFRAMEWORK_API UArcUIViewPayloadReceiver : public UInterface
{
	GENERATED_BODY()
};

class ARCUIFRAMEWORK_API IArcUIViewPayloadReceiver
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, DisplayName="PushPayload")
	void BP_PushPayload(const TInstancedStruct<FArcUIViewPayload>& Payload);

	void PushPayload(const TInstancedStruct<FArcUIViewPayload>& Payload)
	{
		ensureMsgf(Cast<UObject>(this), TEXT("PushPayload must only be called from UObjects"));
		Execute_BP_PushPayload(Cast<UObject>(this), Payload);
	}
};

namespace ArcUI
{
	/** Creates a new generic ViewPayload from templated struct type. */
	template<typename T, typename = std::enable_if_t<std::is_base_of_v<FArcUIViewPayload, std::decay_t<T>>>>
	static TInstancedStruct<FArcUIViewPayload> MakeViewPayload()
	{
		TInstancedStruct<FArcUIViewPayload> Payload;
		Payload.InitializeAs(TBaseStructure<T>::Get(), nullptr);
		return Payload;
	}

	/** Creates a new generic ViewPayload from templated struct. */
	template<typename T, typename = std::enable_if_t<std::is_base_of_v<FArcUIViewPayload, std::decay_t<T>>>>
	static TInstancedStruct<FArcUIViewPayload> MakeViewPayload(const T& Struct)
	{
		TInstancedStruct<FArcUIViewPayload> Payload;
		Payload.InitializeAs(TBaseStructure<T>::Get(), reinterpret_cast<const uint8*>(&Struct));
		return Payload;
	}

	/** Creates a new generic ViewPayload from the templated type and forward all arguments to constructor. */
	template<typename T, typename... TArgs, typename = std::enable_if_t<std::is_base_of_v<FArcUIViewPayload, std::decay_t<T>>>>
	static TInstancedStruct<FArcUIViewPayload> MakeViewPayload(TArgs&&... InArgs)
	{
		TInstancedStruct<FArcUIViewPayload> Payload;
		Payload.InitializeAs<T>(Forward<TArgs>(InArgs)...);
		return Payload;
	}
}
