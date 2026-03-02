#pragma once
#include "../EngineMinimal.h"

// float 总长度
// float 帧时间
// float 全体の長さ
// float フレーム時間
DEFINITION_SIMPLE_SINGLE_DELEGATE(FTimelineDelegate, void, float, float);

// 时间轴
// タイムライン
struct FTimeline
{
	FTimeline();

	void Tick(float DeltaTime);

	void BindTimeLineEvent(float InTime, const FTimelineDelegate& InTimelineDelegate);

private:
	float TimeLength;
	float CurrentTimeLength;
	FTimelineDelegate TimelineDelegate;
};