// Copyright (C) Fabien Poupineau. All Rights Reserved.

#pragma once

#include "InstancedStruct.h"

#include "ArcUIModel.generated.h"

USTRUCT(BlueprintType)
struct ARCUIFRAMEWORK_API FArcUIModel
{
	GENERATED_BODY()

	//virtual ~FArcUIModel() = default;
};


UINTERFACE()
class ARCUIFRAMEWORK_API UArcUIModelReceiver : public UInterface
{
	GENERATED_BODY()
};

class ARCUIFRAMEWORK_API IArcUIModelReceiver
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, DisplayName="UpdateWithModel")
	void BP_UpdateWithModel(const TInstancedStruct<FArcUIModel>& Model);

	void UpdateWithModel(const TInstancedStruct<FArcUIModel>& Model)
	{
		ensureMsgf(Cast<UObject>(this), TEXT("UpdateWithModel must only be called from UObjects"));
		Execute_BP_UpdateWithModel(Cast<UObject>(this), Model);
	}
};

namespace ArcUI
{
	/** Creates a new TInstancedStruct from templated struct type. */
	template<typename T, typename = std::enable_if_t<std::is_base_of_v<FArcUIModel, std::decay_t<T>>>>
	static TInstancedStruct<FArcUIModel> MakeModel()
	{
		TInstancedStruct<FArcUIModel> Model;
		Model.InitializeAs(TBaseStructure<T>::Get(), nullptr);
		return Model;
	}

	/** Creates a new TInstancedStruct from templated struct. */
	template<typename T, typename = std::enable_if_t<std::is_base_of_v<FArcUIModel, std::decay_t<T>>>>
	static TInstancedStruct<FArcUIModel> MakeModel(const T& Struct)
	{
		TInstancedStruct<FArcUIModel> Model;
		Model.InitializeAs(TBaseStructure<T>::Get(), reinterpret_cast<const uint8*>(&Struct));
		return Model;
	}

	/** Creates a new TInstancedStruct from the templated type and forward all arguments to constructor. */
	template<typename T, typename... TArgs, typename = std::enable_if_t<std::is_base_of_v<FArcUIModel, std::decay_t<T>>>>
	static TInstancedStruct<FArcUIModel> MakeModel(TArgs&&... InArgs)
	{
		TInstancedStruct<FArcUIModel> Model;
		Model.InitializeAs<T>(Forward<TArgs>(InArgs)...);
		return Model;
	}
}