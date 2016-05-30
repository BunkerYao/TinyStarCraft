#include "Precompiled.h"
#include "Texture.h"
#include "Utilities/Assert.h"
#include "Utilities/Logging.h"

namespace TinyStarCraft
{

Texture::Texture(ResourceManager* owner, const std::string& name)
    : Resource(owner, name),
      mTexture(nullptr),
      mHasTexture(false)
{
}

Texture::~Texture()
{
    if (mTexture) 
    {
        ULONG number = mTexture->Release();
        TINYSC_ASSERT(number == 0, "Texture released unsuccessfully.");
    }
}

bool Texture::create(IDirect3DDevice9* D3DDevice, const Size2<UINT>& size, UINT mipLevels, D3DFORMAT format, bool isRenderTarget)
{
    TINYSC_ASSERT(mHasTexture == false, "A texture has been created before.");
    
    if (!_createTextureImpl(D3DDevice, size, mipLevels, format, isRenderTarget))
        return false;

    mSize = size;
    mMipLevels = mipLevels;
    mFormat = format;
    mIsRenderTarget = isRenderTarget;
    mHasTexture = true;

    return true;
}

bool Texture::createFromFile(IDirect3DDevice9* D3DDevice, const std::string& srcFilename, const Size2<UINT>& size, UINT mipLevels,
    D3DFORMAT format, bool isRenderTarget)
{
    TINYSC_ASSERT(mHasTexture == false, "A texture has been created before.");

    if (!_createTextureFromFileImpl(D3DDevice, srcFilename, size, mipLevels, format, isRenderTarget))
        return false;

    mSize = size;
    mMipLevels = mipLevels;
    mFormat = format;
    mIsRenderTarget = isRenderTarget;
    mSrcFilename = srcFilename;
    mHasTexture = true;

    return true;
}

void Texture::unload()
{
    TINYSC_ASSERT(mHasTexture == true, "Texture is not created.");
    TINYSC_ASSERT(mTexture != nullptr, "Texture is already unloaded.");
    _unloadImpl();
}

bool Texture::reload(IDirect3DDevice9* D3DDevice)
{
    TINYSC_ASSERT(mHasTexture == true, "Texture is not created.");
    TINYSC_ASSERT(mTexture == nullptr, "Texture is not unloaded.");
    return _reloadImpl(D3DDevice);
}

bool Texture::_createTextureImpl(IDirect3DDevice9* D3DDevice, const Size2<UINT>& size, UINT mipLevels, D3DFORMAT format,
    bool isRenderTarget)
{
    HRESULT hr = ::D3DXCreateTexture(
        D3DDevice,
        size.x,
        size.y,
        mipLevels,
        isRenderTarget ? D3DUSAGE_RENDERTARGET : 0,
        format,
        isRenderTarget ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED,
        &mTexture
        );

    if (FAILED(hr)) 
    {
        TINYSC_LOGLINE_ERR("::D3DXCreateTexture failed 0x%08x %s.", hr, ::DXGetErrorString(hr));
        return false;
    }

    return true;
}

bool Texture::_createTextureFromFileImpl(IDirect3DDevice9* D3DDevice, const std::string& srcFilename, const Size2<UINT>& size,
    UINT mipLevels, D3DFORMAT format, bool isRenderTarget)
{
    HRESULT hr = ::D3DXCreateTextureFromFileEx(
        D3DDevice,
        srcFilename.c_str(),
        size.x,
        size.y,
        mipLevels,
        isRenderTarget ? D3DUSAGE_RENDERTARGET : 0,
        format,
        isRenderTarget ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        0,
        nullptr,
        nullptr,
        &mTexture
        );

    if (FAILED(hr)) 
    {
        TINYSC_LOGLINE_ERR("::D3DXCreateTextureFromFileEx failed 0x%08x %s.", hr, ::DXGetErrorString(hr))
        return false;
    }

    return true;
}

void Texture::_unloadImpl()
{
    ULONG result = mTexture->Release();
    mTexture = nullptr;
    TINYSC_ASSERT(result == 0, "Texture released unsuccessfully.");
}

bool Texture::_reloadImpl(IDirect3DDevice9* D3DDevice)
{
    if (mSrcFilename == "") 
    {
        // The original texture is not created from file.
        return _createTextureImpl(D3DDevice, mSize, mMipLevels, mFormat, mIsRenderTarget);
    }
    else 
    {
        // The original texture is created from file.
        return _createTextureFromFileImpl(D3DDevice, mSrcFilename, mSize, mMipLevels, mFormat, mIsRenderTarget);
    }
}

}