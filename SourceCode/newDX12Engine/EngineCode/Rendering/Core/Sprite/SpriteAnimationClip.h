#pragma once

#include "../../../EngineMinimal.h"


struct FSpriteAnimationFrame
{
	FSpriteAnimationFrame();
	FSpriteAnimationFrame(const string& InFrameName, float InDuration);

	string FrameName;
	float Duration;
};

class CSpriteAnimationClip
{
public:
	CSpriteAnimationClip();

	void SetName(const string& InName) { Name = InName; }
	const string& GetName() const { return Name; }

	void SetFramesPerSecond(float InFramesPerSecond);
	float GetFramesPerSecond() const { return FramesPerSecond; }

	void SetLoop(bool bInLoop) { bLoop = bInLoop; }
	bool IsLoop() const { return bLoop; }

	void AddFrame(const string& InFrameName, float InDuration = -1.f);
	void ClearFrames();

	int GetFrameCount() const { return (int)Frames.size(); }
	const FSpriteAnimationFrame* GetFrameByIndex(int InFrameIndex) const;
	int GetFrameIndexAtTime(float InTime) const;
	float GetTotalDuration() const;

protected:
	float GetDefaultFrameDuration() const;

protected:
	string Name;
	float FramesPerSecond;
	bool bLoop;
	vector<FSpriteAnimationFrame> Frames;
};
