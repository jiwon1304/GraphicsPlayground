OpenGL에서는 버퍼 바인딩 다음 레이아웃 바인딩을 해야하는데, 이를 어떻게 강제할지

마이그레이션
physics : jolt physics - cpu-based라서 fallback으로 사용
opengl + stb - windows환경 아닐때 fallback
fmod는 macos되도록


# 렌더 스레드
[메시 그리기 파이프라인](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/mesh-drawing-pipeline-in-unreal-engine)
FRenderModule에서 FScene을 참조해서 드로우
각각의 드로우 대상은 FMeshPassProcessor를 상속받은 곳에서 렌더
FMeshPassProcessor는 const FScene*을 가지고 있음(생성자에서 받음)

SceneProxy는 GT에서 생성하고 ENQUEUE_RENDER_COMMAND를 통해서 RT에서 등록
마찬가지로 업데이트도 GT에서 생성하고 RT에 enqueue
이때 dirty flag를 통해서 한 프레임 내에서 변했는지 확인(변할때마다 enqueue하면 한 프레임에 여러개 보낼 수 있음)
GT Tick 끝나면 dirty flag 찾고서 해당하는 것 찾아서 fscene에서 update호출

다음은 `FScene`의 선언부
Renderer/Private/ScenePrivate.h:2928 참조하면
``` cpp
struct FPrimitiveUpdateParams
{
    FScene* Scene;
    FPrimitiveSceneProxy* PrimitiveSceneProxy;
    FBoxSphereBounds WorldBounds;
    FBoxSphereBounds LocalBounds;
    FMatrix LocalToWorld;
    TOptional<FTransform> PreviousTransform;
    FVector AttachmentRootPosition;
};

/** DoDeferredRenderUpdates_Concurrent can be called not only inside SendAllEndOfFrameUpdates so we should be able to enqueue commands also in immediate mode. */
bool bPrimitivesUpdateBatching = false;
std::atomic_int32_t PrimitiveUpdateIndex = 0;
TArray<FPrimitiveUpdateParams> PrimitivesUpdates;
```

그리고 Line3355참고하면
```cpp
	void Update(FRDGBuilder& GraphBuilder, const FUpdateParameters& Parameters);

	// FSceneInterface interface.
	virtual void AddPrimitive(UPrimitiveComponent* Primitive) override;
	virtual void RemovePrimitive(UPrimitiveComponent* Primitive) override;
	virtual void ReleasePrimitive(UPrimitiveComponent* Primitive) override;
	virtual void BatchAddPrimitives(TArrayView<UPrimitiveComponent*> InPrimitives) override;

	virtual void UpdatePrimitiveAttachment(UPrimitiveComponent* Primitive) override;

	virtual void AddLight(ULightComponent* Light) override;
	virtual void RemoveLight(ULightComponent* Light) override;
	virtual void AddInvisibleLight(ULightComponent* Light) override;
	virtual void SetSkyLight(FSkyLightSceneProxy* Light) override;
	virtual void DisableSkyLight(FSkyLightSceneProxy* Light) override;
	virtual bool HasSkyLightRequiringLightingBuild() const override;
	virtual bool HasAtmosphereLightRequiringLightingBuild() const override;
	virtual void UpdateLightTransform(ULightComponent* Light) override;
	virtual void UpdateLightColorAndBrightness(ULightComponent* Light) override;
	virtual void AddExponentialHeightFog(uint64 Id, const FExponentialHeightFogDynamicState& State) override;
```

https://scahp.tistory.com/93 참고

웬만한 FScene의 구현은 RendererScene.cpp에 있음

라이트의 경우에는
ULightComponent -> FLightSceneProxy(GT) -> FLightSceneInfo (RT)

LightSceneProxy는 GT에서 write, RT에서 read
하지만 실제 데이터 사용은 FLightSceneInfo로 한다.

펜스를 어떻게해야할지 생각해보자

# build
renderer / editor / runtime 나눠서 library로 빌드하기