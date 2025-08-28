﻿#pragma once
#include "Animation/Animinstance.h"
#include "Animation/AnimStateMachine.h"
#include "UObject/ObjectMacros.h"

class UAnimSequence;
class UAnimationAsset;

class UMyAnimInstance : public UAnimInstance
{
    DECLARE_CLASS(UMyAnimInstance, UAnimInstance)

public:
    UMyAnimInstance();
    
    virtual void NativeInitializeAnimation() override;

    virtual void NativeUpdateAnimation(float DeltaSeconds, FPoseContext& OutPose) override;
    
    void SetPlaying(bool bIsPlaying)
    {
        bPlaying = bIsPlaying;
    }

    bool IsPlaying() const
    {
        return bPlaying;
    }

    void SetReverse(bool bIsReverse)
    {
        bReverse = bIsReverse;
    }
    
    bool IsReverse() const
    {
        return bReverse;
    }

    void SetLooping(bool bIsLooping)
    {
        bLooping = bIsLooping;
    }

    bool IsLooping() const
    {
        return bLooping;
    }

    void SetElapsedTime(float InElapsedTime)
    {
        ElapsedTime = InElapsedTime;
    }

    float GetElapsedTime() const
    {
        return ElapsedTime;
    }

    float GetPlayRate() const
    {
        return PlayRate;
    }

    void SetPlayRate(float InPlayRate)
    {
        PlayRate = InPlayRate;
    }

    int32 GetLoopStartFrame() const
    {
        return LoopStartFrame;
    }

    void SetLoopStartFrame(int32 InLoopStartFrame)
    {
        LoopStartFrame = InLoopStartFrame;
    }

    int32 GetLoopEndFrame() const
    {
        return LoopEndFrame;
    }

    void SetLoopEndFrame(int32 InLoopEndFrame)
    {
        LoopEndFrame = InLoopEndFrame;
    }

    int GetCurrentKey() const
    {
        return CurrentKey;
    }

    void SetCurrentKey(int InCurrentKey)
    {
        CurrentKey = InCurrentKey;
    }

    virtual void SetAnimState(EAnimState InAnimState) override;

    UAnimSequence* GetAnimForState(EAnimState InAnimState);

    virtual UAnimStateMachine* GetStateMachine() const override { return StateMachine; }

    virtual UAnimSequence* GetCurrAnim() const override { return CurrAnim; }

    virtual UAnimSequence* GetPrevAnim() const override { return PrevAnim; }

    virtual float GetBlendDuration() const override { return BlendDuration; }

    virtual void SetBlendDuration(float InBlendDuration) override { BlendDuration = InBlendDuration; }
private:
    UAnimationAsset* IDLE;
    UAnimationAsset* Dance;
    UAnimationAsset* SlowRun;
    UAnimationAsset* NarutoRun;
    UAnimationAsset* FastRun;

    float ElapsedTime;
    
    float PlayRate;
    
    bool bLooping;
    
    bool bPlaying;
    
    bool bReverse;

    int32 LoopStartFrame;

    int32 LoopEndFrame;

    int CurrentKey;
    
    UAnimSequence* PrevAnim;
    
    UAnimSequence* CurrAnim;
    
    float BlendAlpha;

    float BlendStartTime;
    
    float BlendDuration;

    bool bIsBlending;
    
    UAnimStateMachine* StateMachine;
};
