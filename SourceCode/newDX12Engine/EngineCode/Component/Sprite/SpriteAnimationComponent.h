#pragma once

#include "Component/Component.h"

#include "../../Rendering/Core/Sprite/SpriteAnimationClip.h"


class CSpriteComponent;

class CSpriteAnimationComponent : public CComponent
{
public:
	CSpriteAnimationComponent();

	virtual void Tick(float DeltaTime) override;

	void SetSpriteComponent(CSpriteComponent* InSpriteComponent);
	CSpriteComponent* GetSpriteComponent() const { return SpriteComponent; }

	void AddClip(const CSpriteAnimationClip& InClip);
	CSpriteAnimationClip* FindClip(const string& InClipName);
	const CSpriteAnimationClip* FindClip(const string& InClipName) const;

	bool Play(const string& InClipName, bool bRestart = true);
	void Stop();
	void Pause();
	void Resume();

	void SetPlayRate(float InPlayRate);
	float GetPlayRate() const { return PlayRate; }

	void SetCurrentFrame(int InFrameIndex);
	bool IsPlaying() const { return bPlaying && !bPaused; }

protected:
	void ApplyCurrentFrame();

protected:
	map<string, CSpriteAnimationClip> Clips;
	CSpriteComponent* SpriteComponent;

	string CurrentClipName;

	int CurrentFrameIndex;

	float ElapsedTime;
	float PlayRate;

	bool bPlaying;
	bool bPaused;
};
