#include "ImageLoader.h"

#include "Engine/UserInterface/Console.h"

#if defined(BUILD_PLATFORM_WINDOWS)
#include "ThirdParty/DirectXTK/Include/DirectXTK/DDSTextureLoader.h"

bool FImageLoader::LoadImage(const FWString &InFilePath, EPixelFormat PixelFormat, FImageLoadResult &OutLoadResult)
{
    assert( false ); // SRGB랑 PixelFormat 처리 필요
    IWICImagingFactory* WicFactory = nullptr;
    IWICBitmapDecoder* Decoder = nullptr;
    IWICBitmapFrameDecode* Frame = nullptr;
    IWICFormatConverter* Converter = nullptr;

    // WIC 팩토리 생성
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&WicFactory));
    if (FAILED(hr))
    {
        return hr;
    }


    // 이미지 파일 디코딩
    hr = WicFactory->CreateDecoderFromFilename(Filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &Decoder);
    if (FAILED(hr))
    {
        return hr;
    }


    hr = Decoder->GetFrame(0, &Frame);
    if (FAILED(hr))
    {
        return hr;
    }

    // WIC 포맷 변환기 생성 (픽셀 포맷 변환)
    hr = WicFactory->CreateFormatConverter(&Converter);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = Converter->Initialize(Frame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
    if (FAILED(hr))
    {
        return hr;
    }

    // 이미지 크기 가져오기
    UINT Width, Height;
    Frame->GetSize(&Width, &Height);

    // 픽셀 데이터 로드
    BYTE* ImageData = new BYTE[Width * Height * 4];
    hr = Converter->CopyPixels(nullptr, Width * 4, Width * Height * 4, ImageData);
    if (FAILED(hr)) {
        delete[] ImageData;
        return hr;
    }

}
#endif

#if defined (BUILD_PLATFORM_MACOS)

#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"

bool FImageLoader::LoadImage(const FWString &InFilePath, EPixelFormat PixelFormat, FImageLoadResult &OutLoadResult)
{
    if (PixelFormat == PF_Unknown)
    {
        UE_LOG(ELogLevel::Warning, "FImageLoader::LoadImage - Pixel format not set. Set to default PF_R8G8B8A8_UINT");
        PixelFormat = PF_R8G8B8A8_UINT;
    }

    // Convert FWString (wide string) to UTF-8 std::string for stb_image
    std::string Utf8Path = WStringToString(InFilePath);

    // Request 4 channels (RGBA) to match the Windows path which produces 32bpp RGBA
    int Width = 0;
    int Height = 0;
    int ChannelsInFile = 0;
    uint8 BitsPerChannel;

    uint8* StbData = nullptr;
    // 8 bit * 4 channels
    if (PixelFormat >= PF_R8G8B8A8_UNORM && PixelFormat <= PF_B8G8R8X8_UNORM)
    {
        unsigned char* StbData8 = nullptr;
        StbData8 = stbi_load(Utf8Path.c_str(), &Width, &Height, &ChannelsInFile, 4);
        if (ChannelsInFile != 4)
        {
            UE_LOG(ELogLevel::Warning, "FImageLoader::LoadImage - Loaded image does not have 4 channels as requested. Actual channels: %d\n%s", ChannelsInFile, Utf8Path.c_str());
        }
        StbData = reinterpret_cast<uint8*>(StbData8);
        BitsPerChannel= 8;
    }
    // 16 bit * 4 channels
    else if (PixelFormat >= PF_R16G16B16A16_FLOAT && PixelFormat <= PF_R16G16B16A16_UINT)
    {
        unsigned short* StbData16 = nullptr;
        StbData16 = stbi_load_16(Utf8Path.c_str(), &Width, &Height, &ChannelsInFile, 4);
        if (ChannelsInFile != 4)
        {
            UE_LOG(ELogLevel::Warning, "FImageLoader::LoadImage - Loaded image does not have 4 channels as requested. Actual channels: %d\n%s", ChannelsInFile, Utf8Path.c_str());
        }
        StbData = reinterpret_cast<uint8*>(StbData16);
        BitsPerChannel= 16;
    }
    // 32 bit * 4 channels
    else if (PixelFormat >= PF_R32G32B32A32_FLOAT && PixelFormat <= PF_R32G32B32A32_SINT)
    {
        float* StbData32 = nullptr;
        StbData32 = stbi_loadf(Utf8Path.c_str(), &Width, &Height, &ChannelsInFile, 4);
        if (ChannelsInFile != 4)
        {
            UE_LOG(ELogLevel::Warning, "FImageLoader::LoadImage - Loaded image does not have 4 channels as requested. Actual channels: %d\n%s", ChannelsInFile, Utf8Path.c_str());
        }
        StbData = reinterpret_cast<uint8*>(StbData32);
        BitsPerChannel= 32;
    }
    // 16 bit * 2 channels
    else if (PixelFormat >= PF_R16G16_FLOAT && PixelFormat <= PF_R16G16_UNORM)
    {
        unsigned short* StbData16 = nullptr;
        StbData16 = stbi_load_16(Utf8Path.c_str(), &Width, &Height, &ChannelsInFile, 2);
        if (ChannelsInFile != 2)
        {
            UE_LOG(ELogLevel::Warning, "FImageLoader::LoadImage - Loaded image does not have 2 channels as requested. Actual channels: %d\n%s", ChannelsInFile, Utf8Path.c_str());
        }
        StbData = reinterpret_cast<uint8*>(StbData16);
        BitsPerChannel= 16;
    }
    // 32 bit * 1 channel
    else if (PixelFormat >= PF_R32_FLOAT && PixelFormat <= PF_R32_UINT)
    {
        float* StbData32 = nullptr;
        StbData32 = stbi_loadf(Utf8Path.c_str(), &Width, &Height, &ChannelsInFile, 1);
        if (ChannelsInFile != 1)
        {
            UE_LOG(ELogLevel::Warning, "FImageLoader::LoadImage - Loaded image does not have 1 channel as requested. Actual channels: %d\n%s", ChannelsInFile, Utf8Path.c_str());
        }
        StbData = reinterpret_cast<uint8*>(StbData32);
        BitsPerChannel= 32;
    }
    else
    {
        UE_LOG(ELogLevel::Error, "FImageLoader::LoadImage - Unsupported pixel format.\n%s", Utf8Path.c_str());
        // Unsupported format
        OutLoadResult.bValid = false;
        OutLoadResult.ImageData = nullptr;
        return false;
    }

    if (!StbData)
    {
        UE_LOG(ELogLevel::Error, "FImageLoader::LoadImage - Failed to load image: %s", Utf8Path.c_str());
        OutLoadResult.bValid = false;
        OutLoadResult.ImageData = nullptr;
        return false;
    }

    const size_t PixelCount = static_cast<size_t>(Width) * static_cast<size_t>(Height);
    const size_t BytesPerPixel = static_cast<size_t>(ChannelsInFile) * (static_cast<size_t>(BitsPerChannel) / 8);
    const size_t BufferSize = PixelCount * BytesPerPixel;

    // Allocate engine-owned buffer using new[] so callers that expect delete[] work consistently
    uint8* ImageData = new uint8[BufferSize];
    FPlatformMemory::Memcpy(ImageData, StbData, BufferSize);

    // Free stb buffer
    stbi_image_free(StbData);

    // Fill OutLoadResult
    OutLoadResult.FilePath = InFilePath;
    OutLoadResult.Width = static_cast<uint32>(Width);
    OutLoadResult.Height = static_cast<uint32>(Height);
    OutLoadResult.NumChannels = static_cast<uint8>(ChannelsInFile);
    OutLoadResult.BitsPerChannel = BitsPerChannel;
    OutLoadResult.ImageData = ImageData;
    OutLoadResult.bValid = true;

    return true;
}

#endif