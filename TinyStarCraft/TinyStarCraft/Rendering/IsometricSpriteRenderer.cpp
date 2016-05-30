#include "Precompiled.h"
#include "IsometricSpriteRenderer.h"
#include "IsometricSprite.h"
#include "Asset/Mesh.h"
#include "Utilities/Assert.h"
#include "Utilities/Rect2.h"

namespace TinyStarCraft
{

struct IsometricSpriteVertex
{
    D3DXVECTOR3 pos;
    D3DXVECTOR2 texcoord;
    float index;
};

//-------------------------------------------------------------------------------------------------
IsometricSpriteRenderer::IsometricSpriteRenderer() 
    : mInstancesMesh(nullptr),
      mInstancesWorldMatrices(BATCH_SIZE),
      mInstancesTextureRects(BATCH_SIZE),
      mInstancesHeight(BATCH_SIZE)
{
}
//-------------------------------------------------------------------------------------------------
IsometricSpriteRenderer::~IsometricSpriteRenderer()
{
    delete mInstancesMesh;
}
//-------------------------------------------------------------------------------------------------
bool IsometricSpriteRenderer::initialize(IDirect3DDevice9* D3DDevice)
{
    // Create the instance mesh.
    //

    mInstancesMesh = new Mesh(D3DDevice);

    D3DVERTEXELEMENT9 elements[] =
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
        D3DDECL_END()
    };

    if (!mInstancesMesh->create(2 * BATCH_SIZE, 4 * BATCH_SIZE, D3DXMESH_MANAGED, elements))
        return false;

    IsometricSpriteVertex* vertices = nullptr;
    mInstancesMesh->getPointer()->LockVertexBuffer(0, (void**)&vertices);

    for (int i = 0; i < BATCH_SIZE; ++i)
    {
        vertices[i * 4].pos = D3DXVECTOR3(-0.5f, 0.5f, 0.0f);
        vertices[i * 4].texcoord = D3DXVECTOR2(1.0f, 0.0f);
        vertices[i * 4].index = (float)i;
        vertices[i * 4 + 1].pos = D3DXVECTOR3(0.5f, 0.5f, 0.0f);
        vertices[i * 4 + 1].texcoord = D3DXVECTOR2(0.0f, 0.0f);
        vertices[i * 4 + 1].index = (float)i;
        vertices[i * 4 + 2].pos = D3DXVECTOR3(0.5f, -0.5f, 0.0f);
        vertices[i * 4 + 2].texcoord = D3DXVECTOR2(0.0f, 1.0f);
        vertices[i * 4 + 2].index = (float)i;
        vertices[i * 4 + 3].pos = D3DXVECTOR3(-0.5f, -0.5f, 0.0f);
        vertices[i * 4 + 3].texcoord = D3DXVECTOR2(1.0f, 1.0f);
        vertices[i * 4 + 3].index = (float)i;
    }

    mInstancesMesh->getPointer()->UnlockVertexBuffer();

    unsigned short* indices = nullptr;
    mInstancesMesh->getPointer()->LockIndexBuffer(0, (void**)&indices);

    for (int i = 0; i < BATCH_SIZE; ++i)
    {
        indices[i * 6] = i * 4;
        indices[i * 6 + 1] = i * 4 + 2;
        indices[i * 6 + 2] = i * 4 + 1;
        indices[i * 6 + 3] = i * 4;
        indices[i * 6 + 4] = i * 4 + 3;
        indices[i * 6 + 5] = i * 4 + 2;
    }

    mInstancesMesh->getPointer()->UnlockIndexBuffer();

    return true;
}
//-------------------------------------------------------------------------------------------------
void IsometricSpriteRenderer::draw(const std::vector<IsometricSprite*>& sprites, IDirect3DDevice9* D3DDevice, 
    ID3DXEffect* effect)
{
    IDirect3DVertexBuffer9* verticesBuffer = nullptr;
    IDirect3DIndexBuffer9* indicesBuffer = nullptr;
    D3DVERTEXELEMENT9 vertElement[MAX_FVF_DECL_SIZE];
    mInstancesMesh->getPointer()->GetVertexBuffer(&verticesBuffer);
    mInstancesMesh->getPointer()->GetIndexBuffer(&indicesBuffer);
    mInstancesMesh->getPointer()->GetDeclaration(vertElement);

    // Create vertex declaration.
    IDirect3DVertexDeclaration9* vertDecl = nullptr;
    HRESULT hr = D3DDevice->CreateVertexDeclaration(vertElement, &vertDecl);
    TINYSC_ASSERT(SUCCEEDED(hr), "Failed to create vertex declaration.");

    // Bind vertex stream.
    D3DDevice->SetStreamSource(0, verticesBuffer, 0, sizeof(IsometricSpriteVertex));
    // Bind index
    D3DDevice->SetIndices(indicesBuffer);
    // Set vertex declaration
    D3DDevice->SetVertexDeclaration(vertDecl);

    size_t instanceRemain = sprites.size();
    size_t batchedInstancesCount = 0;
    size_t startInstanceIndex = 0;

    while (instanceRemain > 0)
    {
        batchedInstancesCount = instanceRemain > BATCH_SIZE ? BATCH_SIZE : instanceRemain;
        instanceRemain -= batchedInstancesCount;

        for (size_t i = 0; i < batchedInstancesCount; ++i)
        {
            mInstancesWorldMatrices[i] = sprites[startInstanceIndex + i]->calculateWorldTransformMatrix();
        }

        for (size_t i = 0; i < batchedInstancesCount; ++i)
        {
            const Rectf& textureRect = sprites[startInstanceIndex + i]->getTextureRectangle();
            mInstancesTextureRects[i].x = textureRect.getLeft();
            mInstancesTextureRects[i].y = textureRect.getTop();
            mInstancesTextureRects[i].z = textureRect.getWidth();
            mInstancesTextureRects[i].w = textureRect.getHeight();
        }

        for (size_t i = 0; i < batchedInstancesCount; ++i)
        {
            mInstancesHeight[i].x = sprites[startInstanceIndex + i]->getPosition().y;
            mInstancesHeight[i].y = sprites[startInstanceIndex + i]->getHeightScale();
        }

        effect->SetMatrixArray("_IsometricSpriteWorldMatrices", &mInstancesWorldMatrices.front(), batchedInstancesCount);
        effect->SetVectorArray("_IsometricSpriteTextureRects", &mInstancesTextureRects.front(), batchedInstancesCount);
        effect->SetVectorArray("_IsometricSpriteHeight", &mInstancesHeight.front(), batchedInstancesCount);
        effect->CommitChanges();

        D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, batchedInstancesCount * 4, 0, batchedInstancesCount * 2);

        startInstanceIndex += batchedInstancesCount;
    }

    vertDecl->Release();
}

}