
9/4일 새벽:
FOpenGLVertexDeclaration같이 
OpenGLResources.h 만들기
FDynamicRHI::RHISetStreamSource 만들기
지피티 답변보고 FDynamicRHI 수정하기

OpenGL에서는 버퍼 바인딩 다음 레이아웃 바인딩을 해야하는데, 이를 어떻게 강제할지

RHI하면 셰이더코드도 변환시켜야함...

일단 목표

macos 컴파일 (windows dependent 다 꺼버리기)
opengl rhi로 창 띄우기
메시 띄우기


directx rhi 완성
정상작동 확인

셰이더 코드 번역기 만들기 ???

-- 한 한달걸릴듯... ---

마이그레이션
physics : jolt physics - cpu-based라서 fallback으로 사용
opengl + stb - windows환경 아닐때 fallback
fmod는 macos되도록