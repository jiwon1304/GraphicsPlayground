#pragma once
#include "SlateCore/Widgets/SWindow.h"
#include "Container/Map.h"
#include "fstream"
#include "sstream"

// Contains two SWindow
class SSplitter : public SWindow
{
public:
    // LT and RB windows are not set in constructor
    SSplitter(const FIntRect& InRect);

    SWindow* SideLT; // Left or Top
    SWindow* SideRB; // Right or Bottom

    float GetSplitRatio() const { return SplitRatio; }
    void SetSplitRatio(float InRatio);
    
    // Size in screen pixels
    virtual uint32 GetSplitValue() = 0;
    virtual void SetSplitValue(uint32 InValue) = 0;

    // Not recursive. Update LT and RB's rect based on the split ratio
    virtual void UpdateChilds() = 0;
    
    void SetSplitterHalfThickness(uint32 InThickness) { SplitterHalfThickness = InThickness; }
    
    virtual bool IsInMargin(const FIntPoint& InPoint) = 0;

    //virtual void LoadConfig(const TMap<FString, FString>& Config);
    //virtual void SaveConfig(TMap<FString, FString>& Config) const;

    //template <typename T>
    //T GetValueFromConfig(const TMap<FString, FString>& Config, const FString& Key, T DefaultValue) {
    //    if (const FString* Value = Config.Find(Key))
    //    {
    //        std::istringstream Stream(Value->ToUTF8String());
    //        T ConfigValue;
    //        if (Stream >> ConfigValue)
    //        {
    //            return ConfigValue;
    //        }
    //    }
    //    return DefaultValue;
    //}

protected:
    uint32 SplitterHalfThickness = 5; // Pixel Value

private:
    float SplitRatio = 0.5f; // 값 범위: [0, 1]

    constexpr static float SplitterMinRatio = 0.2f; // 최소 비율
};

// Two windows are arranged horizontally
class SSplitterH : public SSplitter
{
public:
    SSplitterH(const FIntRect& InRect);

    virtual uint32 GetSplitValue() override;
    virtual void SetSplitValue(uint32 InValue) override;

    virtual bool IsInMargin(const FIntPoint& InPoint) override;

    virtual void UpdateChilds() override;
};

// Two windows are arranged vertically
class SSplitterV : public SSplitter
{
public:
    SSplitterV(const FIntRect& InRect);

    virtual uint32 GetSplitValue() override;
    virtual void SetSplitValue(uint32 InValue) override;

    virtual bool IsInMargin(const FIntPoint& InPoint) override;

    virtual void UpdateChilds() override;
};
