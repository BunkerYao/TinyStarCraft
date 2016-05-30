#include "Precompiled.h"
#include "Mesh.h"
#include "Utilities/Assert.h"
#include "Utilities/Logging.h"

namespace TinyStarCraft
{

Mesh::Mesh(IDirect3DDevice9* D3DDevice)
    : mD3DDevice(D3DDevice), mMesh(nullptr)
{
}

Mesh::~Mesh()
{
    if (mMesh) 
    {
        ULONG number = mMesh->Release();
        TINYSC_ASSERT(number == 0, "Mesh released unsuccessfully.");
    }
}

bool Mesh::create(DWORD numFaces, DWORD numVertices, DWORD options, const D3DVERTEXELEMENT9* declaration)
{
    HRESULT hr = ::D3DXCreateMesh(
        numFaces,
        numVertices,
        options,
        declaration,
        mD3DDevice,
        &mMesh
        );

    if (FAILED(hr)) 
        TINYSC_LOGLINE_ERR("D3DXCreateMesh failed 0x%08x %s.", hr, ::DXGetErrorString(hr));

    return SUCCEEDED(hr);
}

}
