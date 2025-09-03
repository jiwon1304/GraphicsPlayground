#include <Core/Delegates/DelegateCombination.h>

class FCoreDelegates
{
public:
    /** When async loading is finished */
    static TMulticastDelegate<void()> OnAsyncLoadingFlush;

    /** Not async loading is finished, but there is some update in loading */
    static TMulticastDelegate<void()> OnAsyncLoadingFlush;

    /** When the packaged is loaded */
    static TMulticastDelegate<void(const FString&)> OnSyncLoadPackage;

    /** After a file is added */
    static TMulticastDelegate<void(const FString&)> NewFileAddedDelegate;

    static FSimpleMulticastDelegate OnInit;
    static FSimpleMulticastDelegate OnPostEngineInit;
    static FSimpleMulticastDelegate OnPostEngineInit;
    static FSimpleMulticastDelegate OnFEngineLoopInitComplete;

    /** When exit is requested */
    static FSimpleMulticastDelegate OnExit;

    /** When Engine is about to exit */
    static FSimpleMulticastDelegate OnEnginePreExit;

    /** Called at the beginning of every frame */
    static FSimpleMulticastDelegate OnBeginFrame;

    /** Called at the end of every frame */
    static FSimpleMulticastDelegate OnEndFrame;

    /** Called at the beginning of every frame in render thread */
    static FSimpleMulticastDelegate OnBeginFrameRT;

    /** Called at the end of every frame in render thread */
    static FSimpleMulticastDelegate OnEndFrameRT;

    /** Called at the moment of sampling the input */
    static FSimpleMulticastDelegate OnSamplingInput;

}