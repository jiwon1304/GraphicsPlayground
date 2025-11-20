Asset 로드 구조

로드해야하는 파일
1. obj
2. fbx
3. mtl
4. jpg / png
5. (physics asset)

1. jpg / png
2. mtl -> jpg / png
3. obj -> mtl
4. fbx -> mtl
5. (physics asset)

그러면 
FImageLoader
FMaterialLoader
FObjLoader
FFbxLoader
순서대로 만들어야함

Image / Material은 immediate로 로드



---

UAssetManager는 U로 시작하는 것만 관리하고 (UMaterial, UTexture, UStaticMesh, ...)
내부적으로는 F로 시작하는것을 처리