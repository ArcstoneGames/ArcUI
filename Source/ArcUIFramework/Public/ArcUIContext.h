// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

// UE5
#include "StructUtils/InstancedStruct.h"
// generated
#include "ArcUIContext.generated.h"

USTRUCT(BlueprintType)
struct ARCUIFRAMEWORK_API FArcUIContextPayload
{
	GENERATED_BODY()

	virtual ~FArcUIContextPayload() = default;
};

namespace ArcUI
{
	/** Creates a new generic ContextPayload from templated struct type. */
	template<typename T, typename = std::enable_if_t<std::is_base_of_v<FArcUIContextPayload, std::decay_t<T>>>>
	static TInstancedStruct<FArcUIContextPayload> MakeContextPayload()
	{
		TInstancedStruct<FArcUIContextPayload> Payload;
		Payload.InitializeAs(TBaseStructure<T>::Get(), nullptr);
		return Payload;
	}

	/** Creates a new generic ContextPayload from templated struct. */
	template<typename T, typename = std::enable_if_t<std::is_base_of_v<FArcUIContextPayload, std::decay_t<T>>>>
	static TInstancedStruct<FArcUIContextPayload> MakeContextPayload(const T& Struct)
	{
		TInstancedStruct<FArcUIContextPayload> Payload;
		Payload.InitializeAs(TBaseStructure<T>::Get(), reinterpret_cast<const uint8*>(&Struct));
		return Payload;
	}

	/** Creates a new generic ContextPayload from the templated type and forward all arguments to constructor. */
	template<typename T, typename... TArgs, typename = std::enable_if_t<std::is_base_of_v<FArcUIContextPayload, std::decay_t<T>>>>
	static TInstancedStruct<FArcUIContextPayload> MakeContextPayload(TArgs&&... InArgs)
	{
		TInstancedStruct<FArcUIContextPayload> Payload;
		Payload.InitializeAs<T>(Forward<TArgs>(InArgs)...);
		return Payload;
	}
}
