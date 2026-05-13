#include "SpriteAnimationClip.h"

FSpriteAnimationFrame::FSpriteAnimationFrame()
	: Duration(0.1f)
{
}

FSpriteAnimationFrame::FSpriteAnimationFrame(const string& InFrameName, float InDuration)
	: FrameName(InFrameName)
	, Duration(InDuration)
{
}

CSpriteAnimationClip::CSpriteAnimationClip()
	: FramesPerSecond(12.f)
	, bLoop(true)
{
}

// 设置默认帧率
// デフォルトのフレームレートを設定する
void CSpriteAnimationClip::SetFramesPerSecond(float InFramesPerSecond)
{
	FramesPerSecond = max(0.001f, InFramesPerSecond);
}

// 向動画中追加一帧
// 動画に新しいフレームを追加する
void CSpriteAnimationClip::AddFrame(const string& InFrameName, float InDuration)
{
	if (InFrameName.empty())
	{
		return;
	}

	const float FrameDuration = InDuration > 0.f ? InDuration : GetDefaultFrameDuration();
	Frames.push_back(FSpriteAnimationFrame(InFrameName, FrameDuration));
}

void CSpriteAnimationClip::ClearFrames()
{
	Frames.clear();
}

const FSpriteAnimationFrame* CSpriteAnimationClip::GetFrameByIndex(int InFrameIndex) const
{
	if (InFrameIndex >= 0 && InFrameIndex < (int)Frames.size())
	{
		return &Frames[InFrameIndex];
	}

	return nullptr;
}

// 根据累计时间查找当前应显示的帧索引
//  累積時間に基づいて、現在表示すべきフレームのインデックスを検索する
int CSpriteAnimationClip::GetFrameIndexAtTime(float InTime) const
{
	if (Frames.empty())
	{
		return -1;
	}

	if (InTime <= 0.f)
	{
		return 0;
	}

	float AccumulatedTime = 0.f;
	for (int FrameIndex = 0; FrameIndex < (int)Frames.size(); ++FrameIndex)
	{
		AccumulatedTime += max(0.0001f, Frames[FrameIndex].Duration);
		if (InTime < AccumulatedTime)
		{
			return FrameIndex;
		}
	}

	return (int)Frames.size() - 1;
}

// 统计整个動画的总时长
// 動画の全体再生時間を計算する
float CSpriteAnimationClip::GetTotalDuration() const
{
	float TotalDuration = 0.f;
	for (const FSpriteAnimationFrame& Frame : Frames)
	{
		TotalDuration += max(0.0001f, Frame.Duration);
	}

	return TotalDuration;
}

// 由帧率换算单帧默认时长
// フレームレートから1フレームの長さを算出
float CSpriteAnimationClip::GetDefaultFrameDuration() const
{
	return 1.f / max(0.001f, FramesPerSecond);
}
