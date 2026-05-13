#include "SpriteAnimationComponent.h"

#include "SpriteComponent.h"

CSpriteAnimationComponent::CSpriteAnimationComponent()
	: SpriteComponent(nullptr)
	, CurrentFrameIndex(-1)
	, ElapsedTime(0.f)
	, PlayRate(1.f)
	, bPlaying(false)
	, bPaused(false)
{
}

void CSpriteAnimationComponent::Tick(float DeltaTime)
{
	if (!bPlaying || bPaused)
	{
		return;
	}

	const CSpriteAnimationClip* CurrentClip = FindClip(CurrentClipName);
	if (CurrentClip == nullptr || CurrentClip->GetFrameCount() <= 0)
	{
		return;
	}

	const float TotalDuration = CurrentClip->GetTotalDuration();
	if (TotalDuration <= 0.f)
	{
		SetCurrentFrame(0);
		return;
	}

	// 累积播放时间，并根据是否循环决定时间回绕方式
	// 再生時間を累積し、ループの有無に応じて時間の巻き戻し方法を決定する
	ElapsedTime += max(0.f, DeltaTime * PlayRate);

	if (CurrentClip->IsLoop())
	{
		while (ElapsedTime >= TotalDuration)
		{
			ElapsedTime -= TotalDuration;
		}
	}
	else if (ElapsedTime >= TotalDuration)
	{
		ElapsedTime = TotalDuration;
		bPlaying = false;
	}

	int NextFrameIndex = CurrentClip->GetFrameIndexAtTime(ElapsedTime);
	if (!CurrentClip->IsLoop() && !bPlaying)
	{
		NextFrameIndex = CurrentClip->GetFrameCount() - 1;
	}

	if (NextFrameIndex != CurrentFrameIndex)
	{
		CurrentFrameIndex = NextFrameIndex;
		ApplyCurrentFrame();
	}
}

// 绑定 sprite 组件
//  spriteコンポーネントをバインドする
void CSpriteAnimationComponent::SetSpriteComponent(CSpriteComponent* InSpriteComponent)
{
	SpriteComponent = InSpriteComponent;
}

// 添加一个可播放的动画片段
// 再生可能な動画を追加する
void CSpriteAnimationComponent::AddClip(const CSpriteAnimationClip& InClip)
{
	if (InClip.GetName().empty())
	{
		return;
	}

	Clips[InClip.GetName()] = InClip;
}

CSpriteAnimationClip* CSpriteAnimationComponent::FindClip(const string& InClipName)
{
	auto FindClipIterator = Clips.find(InClipName);
	if (FindClipIterator != Clips.end())
	{
		return &FindClipIterator->second;
	}

	return nullptr;
}

const CSpriteAnimationClip* CSpriteAnimationComponent::FindClip(const string& InClipName) const
{
	auto FindClipIterator = Clips.find(InClipName);
	if (FindClipIterator != Clips.end())
	{
		return &FindClipIterator->second;
	}

	return nullptr;
}

// 播放指定 clip
// 指定動画を再生する
bool CSpriteAnimationComponent::Play(const string& InClipName, bool bRestart)
{
	CSpriteAnimationClip* Clip = FindClip(InClipName);
	if (Clip == nullptr || Clip->GetFrameCount() <= 0)
	{
		return false;
	}

	if (bRestart || CurrentClipName != InClipName)
	{
		CurrentClipName = InClipName;
		ElapsedTime = 0.f;
		CurrentFrameIndex = -1;
		SetCurrentFrame(0);
	}

	bPlaying = true;
	bPaused = false;

	if (CurrentFrameIndex < 0)
	{
		SetCurrentFrame(0);
	}

	return true;
}

void CSpriteAnimationComponent::Stop()
{
	bPlaying = false;
	bPaused = false;
	ElapsedTime = 0.f;
	CurrentFrameIndex = -1;
}

void CSpriteAnimationComponent::Pause()
{
	bPaused = true;
}

void CSpriteAnimationComponent::Resume()
{
	if (FindClip(CurrentClipName) != nullptr)
	{
		bPlaying = true;
		bPaused = false;
	}
}

void CSpriteAnimationComponent::SetPlayRate(float InPlayRate)
{
	PlayRate = max(0.f, InPlayRate);
}

//　设置当前帧
// 現在再生しているのフレームを設定する
void CSpriteAnimationComponent::SetCurrentFrame(int InFrameIndex)
{
	const CSpriteAnimationClip* Clip = FindClip(CurrentClipName);
	if (Clip == nullptr)
	{
		return;
	}

	if (InFrameIndex < 0 || InFrameIndex >= Clip->GetFrameCount())
	{
		return;
	}

	CurrentFrameIndex = InFrameIndex;
	ApplyCurrentFrame();
}

// 读取当前 clip 的帧名并通知 sprite 切换显示
// 現在のclipのフレーム名を読み取り、spriteに切り替えを表示するよう通知する
void CSpriteAnimationComponent::ApplyCurrentFrame()
{
	if (SpriteComponent == nullptr)
	{
		return;
	}

	const CSpriteAnimationClip* Clip = FindClip(CurrentClipName);
	if (Clip == nullptr)
	{
		return;
	}

	const FSpriteAnimationFrame* Frame = Clip->GetFrameByIndex(CurrentFrameIndex);
	if (Frame == nullptr)
	{
		return;
	}

	SpriteComponent->SetFrame(Frame->FrameName);
}
