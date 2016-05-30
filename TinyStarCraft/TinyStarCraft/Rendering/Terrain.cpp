#include "Precompiled.h"
#include "Terrain.h"
#include "Camera.h"
#include "RenderSystem.h"
#include "Asset/Effect.h"
#include "Asset/EffectManager.h"
#include "Asset/Material.h"
#include "Asset/Mesh.h"
#include "Asset/TextureManager.h"
#include "Utilities/Assert.h"
#include "Utilities/DebugOutput.h"
#include "Utilities/Logging.h"
#include "Utilities/Math.h"
#include "Utilities/Point2.h"
#include "Utilities/Ray.h"

namespace TinyStarCraft
{

static const int    WATER_TILES_BATCH_SIZE = 64;
static const float  WATER_TILE_TEXCOORD_SIZE = 0.2f;

/** Vertex structure for water mesh */
struct WaterVertex
{
    D3DXVECTOR3 pos;
    D3DXVECTOR2 texcoord;
    float index;
};

/** Vertex structure for terrain mesh */
struct TerrainVertex
{
    D3DXVECTOR3 pos;
    D3DXVECTOR3 normal;
    D3DXVECTOR2 blendTexcoord;
    D3DXVECTOR2 controlTexcoord;
};

//-------------------------------------------------------------------------------------------------
const std::array<Terrain::TileGeometry, 15>& Terrain::TILE_GEOMETRIES()
{
    static const std::array<Terrain::TileGeometry, 15> geometries =
    {
        {
            // RAMPTYPE_FLAT
            {
                {
                    { -Tile::SIZE * 0.5f, 0.0f, -Tile::SIZE * 0.5f },
                    { -Tile::SIZE * 0.5f, 0.0f,  Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f, 0.0f, -Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f, 0.0f,  Tile::SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_SOUTH_A
            {
                {
                    { -Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,  -Tile::SIZE * 0.5f },
                    { -Tile::SIZE * 0.5f,                      0.0f,   Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,                      0.0f,  -Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,                      0.0f,   Tile::SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_WEST_A
            {
                {
                    { -Tile::SIZE * 0.5f,                      0.0f,  -Tile::SIZE * 0.5f },
                    { -Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,   Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,                      0.0f,  -Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,                      0.0f,   Tile::SIZE * 0.5f }
                },
                {
                    0, 1, 3, 0, 3, 2
                }
            },
            // RAMPTYPE_NORTH_A
            {
                {
                    { -Tile::SIZE * 0.5f,                      0.0f,  -Tile::SIZE * 0.5f },
                    { -Tile::SIZE * 0.5f,                      0.0f,   Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,                      0.0f,  -Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,   Tile::SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_EAST_A
            {
                {
                    { -Tile::SIZE * 0.5f,                      0.0f,  -Tile::SIZE * 0.5f },
                    { -Tile::SIZE * 0.5f,                      0.0f,   Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,  -Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,                      0.0f,   Tile::SIZE * 0.5f }
                },
                {
                    0, 1, 3, 0, 3, 2
                }
            },
            // RAMPTYPE_SOUTHWEST
            {
                {
                    { -Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,  -Tile::SIZE * 0.5f },
                    { -Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,   Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,                      0.0f,  -Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,                      0.0f,   Tile::SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_NORTHWEST
            {
                {
                    { -Tile::SIZE * 0.5f,                      0.0f,  -Tile::SIZE * 0.5f },
                    { -Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,   Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,                      0.0f,  -Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,   Tile::SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_NORTHEAST
            {
                {
                    { -Tile::SIZE * 0.5f,                     0.0f,  -Tile::SIZE * 0.5f },
                    { -Tile::SIZE * 0.5f,                     0.0f,   Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,   Tile::HEIGHT_PER_LEVEL,  -Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,   Tile::HEIGHT_PER_LEVEL,   Tile::SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_SOUTHEAST
            {
                {
                    { -Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,  -Tile::SIZE * 0.5f },
                    { -Tile::SIZE * 0.5f,                      0.0f,   Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,  -Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,                      0.0f,   Tile::SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_SOUTH_B
            {
                {
                    { -Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,  -Tile::SIZE * 0.5f },
                    { -Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,   Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,  -Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,                      0.0f,   Tile::SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_WEST_B
            {
                {
                    { -Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,  -Tile::SIZE * 0.5f },
                    { -Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,   Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,                      0.0f,  -Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,   Tile::SIZE * 0.5f }
                },
                {
                    0, 1, 3, 0, 3, 2
                }
            },
            // RAMPTYPE_NORTH_B
            {
                {
                    { -Tile::SIZE * 0.5f,                      0.0f,  -Tile::SIZE * 0.5f },
                    { -Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,   Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,  -Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,   Tile::SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_EAST_B
            {
                {
                    { -Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,  -Tile::SIZE * 0.5f },
                    { -Tile::SIZE * 0.5f,                      0.0f,   Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,  -Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,   Tile::SIZE * 0.5f }
                },
                {
                    0, 1, 3, 0, 3, 2
                }
            },
            // RAMPTYPE_DOUBLE_WESTTOEAST
            {
                {
                    { -Tile::SIZE * 0.5f,                      0.0f,  -Tile::SIZE * 0.5f },
                    { -Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,   Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,  -Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,                      0.0f,   Tile::SIZE * 0.5f }
                },
                {
                    0, 1, 3, 0, 3, 2
                }
            },
            // RAMPTYPE_DOUBLE_NORTHTOSOUTH
            {
                {
                    { -Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,  -Tile::SIZE * 0.5f },
                    { -Tile::SIZE * 0.5f,                      0.0f,   Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,                      0.0f,  -Tile::SIZE * 0.5f },
                    {  Tile::SIZE * 0.5f,    Tile::HEIGHT_PER_LEVEL,   Tile::SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            }
        }
    };

    return geometries;
}
//-------------------------------------------------------------------------------------------------
Terrain::Terrain(RenderSystem* renderSystem)
    : mRenderSystem(renderSystem),
      mTerrainMesh(nullptr),
      mWaterTileInstancesMesh(nullptr),
      mWaterTileInstancePositions(WATER_TILES_BATCH_SIZE),
      mWaterTileInstanceTexcoords(WATER_TILES_BATCH_SIZE)
{
}
//-------------------------------------------------------------------------------------------------
Terrain::~Terrain()
{
    delete mTerrainMesh;
    delete mWaterTileInstancesMesh;
}
//-------------------------------------------------------------------------------------------------

bool Terrain::initialize(const Size2d& dimension, const std::vector<Tile>& tilesData)
{
    return _initializeImpl(dimension, tilesData);
}

bool Terrain::initialize(const Size2d& dimension, Tile initialTile)
{
    std::vector<Tile> tilesData(dimension.x * dimension.y, initialTile);
    return _initializeImpl(dimension, tilesData);
}
//-------------------------------------------------------------------------------------------------
void Terrain::setTilesData(const std::vector<Tile>& tilesData)
{
    // Validate tiles data size
    TINYSC_ASSERT(tilesData.size() == mDimension.x * mDimension.y, "Tiles data's length is invalid.");

    // Update tiles data.
    mTilesData = tilesData;

    // Update mesh geometry.
    _updateTerrainMeshGeometry();

    // Rebuild terrain quad tree.
    if (!mTerrainQuadTreeNodes.empty())
        mTerrainQuadTreeNodes.clear();

    _buildTerrainQuadTree();

    // Rebuild water quad tree.
    if (!mWaterQuadTreeNodes.empty())
        mWaterQuadTreeNodes.clear();

    _buildWaterQuadTree();
}
//-------------------------------------------------------------------------------------------------
void Terrain::drawTerrain(Camera* camera)
{
    // Gather visible terrain chunks
    std::vector<int> visibleChunks;
    _gatherVisibleTerrainChunksRecursively(mTerrainQuadTreeNodes.front(), camera->getViewFrustum(), 
        false, &visibleChunks);

    ID3DXEffect* effectPtr = mTerrainEffect->getPointer();

    // Setup blend textures
    std::string blendTextureName;
    for (size_t i = 0; i < 4; ++i)
    {
        blendTextureName = "_BlendTexture" + std::to_string(i);
        effectPtr->SetTexture(blendTextureName.c_str(), mBlendTextures[i]->getPointer());
    }
        
    // Setup control texture
    effectPtr->SetTexture("_ControlTexture", mControlTexture->getPointer());

    UINT passesCount = 0;
    effectPtr->Begin(&passesCount, 0);
    effectPtr->BeginPass(0);

    for (auto& it : visibleChunks)
        mTerrainMesh->getPointer()->DrawSubset(it);

    effectPtr->EndPass();
    effectPtr->End();
}
//-------------------------------------------------------------------------------------------------
void Terrain::drawWater(Camera* camera)
{
    // Gather visible water tiles
    std::vector<int> visibleWaterTiles;
    _gatherVisibleWaterTilesRecursively(mWaterQuadTreeNodes.front(), camera->getViewFrustum(), false, &visibleWaterTiles);

    IDirect3DVertexBuffer9* verticesBuffer = nullptr;
    IDirect3DIndexBuffer9* indicesBuffer = nullptr;
    D3DVERTEXELEMENT9 vertElement[MAX_FVF_DECL_SIZE];
    mWaterTileInstancesMesh->getPointer()->GetVertexBuffer(&verticesBuffer);
    mWaterTileInstancesMesh->getPointer()->GetIndexBuffer(&indicesBuffer);
    mWaterTileInstancesMesh->getPointer()->GetDeclaration(vertElement);

    // Create vertex declaration.
    //

    IDirect3DDevice9* D3DDevice = mRenderSystem->getD3DDevice();
    IDirect3DVertexDeclaration9* vertDecl = nullptr;
    HRESULT hr = D3DDevice->CreateVertexDeclaration(vertElement, &vertDecl);
    TINYSC_ASSERT(SUCCEEDED(hr), "Failed to create vertex declaration.");

    // Bind vertex stream.
    D3DDevice->SetStreamSource(0, verticesBuffer, 0, sizeof(WaterVertex));
    // Bind index
    D3DDevice->SetIndices(indicesBuffer);
    // Set vertex declaration
    D3DDevice->SetVertexDeclaration(vertDecl);

    ID3DXEffect* effectPtr = mWaterEffect->getPointer();

    // Setup wave textures
    effectPtr->SetTexture("_NormalMap0", mWaveTextures[0]->getPointer());
    effectPtr->SetTexture("_NormalMap1", mWaveTextures[1]->getPointer());

    UINT passesCount = 0;
    effectPtr->Begin(&passesCount, 0);
    effectPtr->BeginPass(0);

    size_t tileRemainingCount = visibleWaterTiles.size();
    size_t batchedTilesCount = 0;
    size_t tileStartIndex = 0;
    while (tileRemainingCount) {
        batchedTilesCount = tileRemainingCount < WATER_TILES_BATCH_SIZE ? tileRemainingCount : WATER_TILES_BATCH_SIZE;
        tileRemainingCount -= batchedTilesCount;
        
        // Prepare instance position buffer
        for (size_t i = 0; i < batchedTilesCount; ++i) {
            Point2d tileLocation(visibleWaterTiles[tileStartIndex + i] % mDimension.x, visibleWaterTiles[tileStartIndex + i] / mDimension.x);
            mWaterTileInstancePositions[i] = D3DXVECTOR4(_calcTilePositionFromLocation(tileLocation), 1.0f);
            mWaterTileInstancePositions[i].y = mTilesData[visibleWaterTiles[tileStartIndex + i]].waterAltitude;
        }

        // Prepare instance texcoord buffer
        for (size_t i = 0; i < batchedTilesCount; ++i) {
            Point2d tileLocation(visibleWaterTiles[tileStartIndex + i] % mDimension.x, visibleWaterTiles[tileStartIndex + i] / mDimension.x);
            mWaterTileInstanceTexcoords[i].x = tileLocation.x * WATER_TILE_TEXCOORD_SIZE;
            mWaterTileInstanceTexcoords[i].y = tileLocation.y * WATER_TILE_TEXCOORD_SIZE;
        }

        // Commit instance buffer.
        effectPtr->SetVectorArray("_InstancePositions", &mWaterTileInstancePositions.front(), batchedTilesCount);
        effectPtr->SetVectorArray("_InstanceTexcoords", &mWaterTileInstanceTexcoords.front(), batchedTilesCount);
        effectPtr->CommitChanges();

        // Draw one batch.
        D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, batchedTilesCount * 4, 0, batchedTilesCount * 2);

        // Update tile start index of the next batch
        tileStartIndex += batchedTilesCount;
    }

    effectPtr->EndPass();
    effectPtr->End();

    // Release vertex declaration.
    D3DDevice->SetVertexDeclaration(nullptr);
    vertDecl->Release();
}
//-------------------------------------------------------------------------------------------------
bool Terrain::raycast(const Ray& ray, TerrainRaycastHit* hit) const
{
    // Get all hits.
    std::vector<TerrainRaycastHit> hits;
    _raycastOnQuadTreeRecursively(mTerrainQuadTreeNodes.front(), ray, &hits);

    if (!hits.empty()) {
        // Sort the hit result by distance and return the nearst hit result.
        std::sort(hits.begin(), hits.end());
        *hit = hits.front();
        return true;
    }

    return false;
}
//-------------------------------------------------------------------------------------------------
bool Terrain::raycastAll(const Ray& ray, std::vector<TerrainRaycastHit>* hits) const
{
    // Get all hits.
    _raycastOnQuadTreeRecursively(mTerrainQuadTreeNodes.front(), ray, hits);

    if (!hits->empty()) {
        // Sort the hits by distance
        std::sort(hits->begin(), hits->end());
        return true;
    }

    return false;
}
//-------------------------------------------------------------------------------------------------
void Terrain::setBlendTexture(size_t index, Texture* texture)
{
    TINYSC_ASSERT(index <= 3, "Index out of range.");
    TINYSC_ASSERT(texture != nullptr, "Texture is nullptr.");
    mBlendTextures[index] = texture;
}
//-------------------------------------------------------------------------------------------------
void Terrain::setControlTexture(Texture* texture)
{
    TINYSC_ASSERT(texture != nullptr, "Texture is nullptr.");
    mControlTexture = texture;
}
//-------------------------------------------------------------------------------------------------
void Terrain::setWaveTexture(size_t index, Texture* texture)
{
    TINYSC_ASSERT(index <= 1, "Index out of range");
    TINYSC_ASSERT(texture != nullptr, "Texture is nullptr.");
    mWaveTextures[index] = texture;
}
//-------------------------------------------------------------------------------------------------
bool Terrain::_createTerrainMesh()
{
    D3DVERTEXELEMENT9 vertElement[] = 
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
        D3DDECL_END()
    };

    const DWORD numFaces = mDimension.x * mDimension.y * 2;
    const DWORD numVertices = mDimension.x * mDimension.y * 4;

    mTerrainMesh = new Mesh(mRenderSystem->getD3DDevice());
    if (!mTerrainMesh->create(numFaces, numVertices, D3DXMESH_32BIT | D3DXMESH_MANAGED, vertElement)) {
        TINYSC_LOGLINE_ERR("Failed to create terrain mesh. TerrainDimension(%d*%d).", mDimension.x, mDimension.y);
        return false;
    }

    // Divide the mesh into chunks, e.g. sub-meshes.
    DWORD* attribBuffer = NULL;
    mTerrainMesh->getPointer()->LockAttributeBuffer(0, &attribBuffer);
    const int chunksCount = (mDimension.x * mDimension.y) / TILES_COUNT_PER_CHUNK;
    for (int iChunk = 0; iChunk < chunksCount; ++iChunk) {
        for (int iFace = 0; iFace < TILES_COUNT_PER_CHUNK * 2; ++iFace) {
            *attribBuffer = (DWORD)iChunk;
            attribBuffer++;
        }
    }
    mTerrainMesh->getPointer()->UnlockAttributeBuffer();

    // Blending texture coordinate for one tile.
    const float tileBlendTexcoordStride = 1.0f / BLEND_TEXTURE_DIMENSION;
    D3DXVECTOR2 tileBlendTexcoord[4] =
    {
        { 0.0f, 0.0f },
        { tileBlendTexcoordStride, 0.0f },
        { 0.0f, tileBlendTexcoordStride },
        { tileBlendTexcoordStride, tileBlendTexcoordStride }
    };
    // Control texture coordinate for one tile.
    const float tileControlTexcoordStrideU = 1.0f / mDimension.x;
    const float tileControlTexcoordStrideV = 1.0f / mDimension.y;
    D3DXVECTOR2 tileControlTexcoord[4] =
    {
        { 0.0f, 0.0f },
        { tileControlTexcoordStrideU, 0.0f },
        { 0.0f, tileControlTexcoordStrideV },
        { tileControlTexcoordStrideU, tileControlTexcoordStrideV }
    };

    TerrainVertex* vertices = nullptr;
    unsigned* indices = nullptr; // 32-Bit indices
    mTerrainMesh->getPointer()->LockVertexBuffer(0, (void**)&vertices);
    mTerrainMesh->getPointer()->LockIndexBuffer(0, (void**)&indices);

    const int rowChunksCount = mDimension.x / CHUNK_DIMENSION;
    for (int iChunk = 0; iChunk < chunksCount; ++iChunk) {
        for (int itile = 0; itile < TILES_COUNT_PER_CHUNK; ++itile) {
            const int globalTileIndex = iChunk * TILES_COUNT_PER_CHUNK + itile;

            // Get tile location.
            Point2d tileLocation;
            tileLocation.x = iChunk % rowChunksCount * CHUNK_DIMENSION + itile % CHUNK_DIMENSION;
            tileLocation.y = iChunk / rowChunksCount * CHUNK_DIMENSION + itile / CHUNK_DIMENSION;

            // Modify vertices
            for (int iVert = 0; iVert < 4; ++iVert) {
                const int vertIndex = globalTileIndex * 4 + iVert;

                // Write the blend texture coordinate.
                vertices[vertIndex].blendTexcoord = tileBlendTexcoord[iVert];
                vertices[vertIndex].blendTexcoord.x += tileLocation.x * tileBlendTexcoordStride;
                vertices[vertIndex].blendTexcoord.y += tileLocation.y * tileBlendTexcoordStride;

                // Write the control texture coordinate.
                vertices[vertIndex].controlTexcoord = tileControlTexcoord[iVert];
                vertices[vertIndex].controlTexcoord.x += tileLocation.x * tileControlTexcoordStrideU;
                vertices[vertIndex].controlTexcoord.y += tileLocation.y * tileControlTexcoordStrideV;
            }
        }
    }

    mTerrainMesh->getPointer()->UnlockVertexBuffer();
    mTerrainMesh->getPointer()->UnlockIndexBuffer();

    return true;
}
//-------------------------------------------------------------------------------------------------
bool Terrain::_createWaterTileMesh()
{
    D3DVERTEXELEMENT9 vertElement[] =
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
        D3DDECL_END()
    };

    mWaterTileInstancesMesh = new Mesh(mRenderSystem->getD3DDevice());
    if (!mWaterTileInstancesMesh->create(2 * WATER_TILES_BATCH_SIZE, 4 * WATER_TILES_BATCH_SIZE, D3DXMESH_MANAGED, vertElement)) {
        TINYSC_LOGLINE_ERR("Failed to create water tile mesh.");
        return false;
    }

    WaterVertex* vertices = nullptr;
    mWaterTileInstancesMesh->getPointer()->LockVertexBuffer(0, (void**)&vertices);
    for (int i = 0; i < WATER_TILES_BATCH_SIZE; ++i) {
        vertices[i * 4].pos = D3DXVECTOR3(-Tile::SIZE * 0.5f, 0.0f, -Tile::SIZE * 0.5f);
        vertices[i * 4].texcoord = D3DXVECTOR2(0.0f, 0.0f);
        vertices[i * 4].index = (float)i;

        vertices[i * 4 + 1].pos = D3DXVECTOR3(-Tile::SIZE * 0.5f, 0.0f, Tile::SIZE * 0.5f);
        vertices[i * 4 + 1].texcoord = D3DXVECTOR2(1.0f, 0.0f);
        vertices[i * 4 + 1].index = (float)i;

        vertices[i * 4 + 2].pos = D3DXVECTOR3(Tile::SIZE * 0.5f, 0.0f, Tile::SIZE * 0.5f);
        vertices[i * 4 + 2].texcoord = D3DXVECTOR2(1.0f, 1.0f);
        vertices[i * 4 + 2].index = (float)i;

        vertices[i * 4 + 3].pos = D3DXVECTOR3(Tile::SIZE * 0.5f, 0.0f, -Tile::SIZE * 0.5f);
        vertices[i * 4 + 3].texcoord = D3DXVECTOR2(0.0f, 1.0f);
        vertices[i * 4 + 3].index = (float)i;
    }
    mWaterTileInstancesMesh->getPointer()->UnlockVertexBuffer();

    unsigned short* indices = nullptr;
    mWaterTileInstancesMesh->getPointer()->LockIndexBuffer(0, (void**)&indices);
    for (int i = 0; i < WATER_TILES_BATCH_SIZE; ++i) {
        indices[i * 6] = i * 4;
        indices[i * 6 + 1] = i * 4 + 1;
        indices[i * 6 + 2] = i * 4 + 2;

        indices[i * 6 + 3] = i * 4;
        indices[i * 6 + 4] = i * 4 + 2;
        indices[i * 6 + 5] = i * 4 + 3;
    }
    mWaterTileInstancesMesh->getPointer()->UnlockIndexBuffer();

    return true;
}
//-------------------------------------------------------------------------------------------------
bool Terrain::_initializeImpl(const Size2d& dimension, const std::vector<Tile>& tilesData)
{
    // Validate terrain dimension
    if (dimension.x % CHUNK_DIMENSION > 0 || dimension.y % CHUNK_DIMENSION > 0) {
        TINYSC_LOGLINE_ERR("Invalid terrain dimension(%d*%d).", dimension.x, dimension.y);
        return false;
    }

    mDimension = dimension;
    _updateQuadTreeDimension();

    // Create the terrain mesh.
    if (!_createTerrainMesh())
        return false;

    // Create the water mesh.
    if (!_createWaterTileMesh())
        return false;

    // Update tiles data.
    setTilesData(tilesData);

    // Retrieve terrain effect
    mTerrainEffect = mRenderSystem->getEffectManager()->getEffect(EFFECT_RESOURCE_NAME_TERRAIN);
    TINYSC_ASSERT(mTerrainEffect, "Failed to retrieve terrain effect.");

    // Retrieve water effect
    mWaterEffect = mRenderSystem->getEffectManager()->getEffect(EFFECT_RESOURCE_NAME_WATER);
    TINYSC_ASSERT(mWaterEffect, "Failed to retrieve water effect.");

    // Initialize the textures to default texture
    //

    Texture* defaultTexture = mRenderSystem->getTextureManager()->
        getTexture(TextureManager::DEFAULT_TEXTURE_RESOURCE_NAME);
    TINYSC_ASSERT(defaultTexture, "Failed to retrieve default texture from texture manager.");

    for (size_t i = 0; i < 4; ++i)
        setBlendTexture(i, defaultTexture);

    setControlTexture(defaultTexture);

    for (size_t i = 0; i < 2; ++i)
        setWaveTexture(i, defaultTexture);

    return true;
}
//-------------------------------------------------------------------------------------------------
void Terrain::_updateTerrainMeshGeometry()
{
    TerrainVertex* vertices = nullptr;
    unsigned* indices = nullptr;
    mTerrainMesh->getPointer()->LockVertexBuffer(0, (void**)&vertices);
    mTerrainMesh->getPointer()->LockIndexBuffer(0, (void**)&indices);

    const int chunksCount = (mDimension.x * mDimension.y) / TILES_COUNT_PER_CHUNK;
    const int rowChunksCount = mDimension.x / CHUNK_DIMENSION;
    for (int iChunk = 0; iChunk < chunksCount; ++iChunk) {
        for (int itile = 0; itile < TILES_COUNT_PER_CHUNK; ++itile) {
            const int globaltileIndex = iChunk * TILES_COUNT_PER_CHUNK + itile;

            // Get the tile's geometry.
            Point2d tileLocation;
            tileLocation.x = iChunk % rowChunksCount * CHUNK_DIMENSION + itile % CHUNK_DIMENSION;
            tileLocation.y = iChunk / rowChunksCount * CHUNK_DIMENSION + itile / CHUNK_DIMENSION;

            const int tileIndex = tileLocation.y * mDimension.x + tileLocation.x;
            const Tile& tile = mTilesData[tileIndex];
            const TileGeometry& tileGeometry = TILE_GEOMETRIES()[tile.type];

            // Generate indices
            for (int iIndices = 0; iIndices < 6; ++iIndices) {
                indices[globaltileIndex * 6 + iIndices] = globaltileIndex * 4 + tileGeometry.indices[iIndices];
            }

            // Modify vertex positions
            for (int iVert = 0; iVert < 4; ++iVert) {
                const int vertIndex = globaltileIndex * 4 + iVert;
                // Get the vertex local position from this tile type's geometry vertices position.
                vertices[vertIndex].pos = tileGeometry.vertices[iVert], 1.0f;
                // Move the vertex to world space position.
                vertices[vertIndex].pos += _calcTilePositionFromLocation(tileLocation);
                vertices[vertIndex].pos.y += tile.getAltitude();
            }
        }
    }

    mTerrainMesh->getPointer()->UnlockVertexBuffer();
    mTerrainMesh->getPointer()->UnlockIndexBuffer();

    // Generate adjacency buffer.
    DWORD* adjacencyBuffer = new DWORD[mTerrainMesh->getPointer()->GetNumFaces() * 3];
    mTerrainMesh->getPointer()->GenerateAdjacency(0.01f, adjacencyBuffer);

    // Compute normals.
    ::D3DXComputeNormals(mTerrainMesh->getPointer(), adjacencyBuffer);

    // Optimize the mesh by sorting face attributes.
    mTerrainMesh->getPointer()->OptimizeInplace(D3DXMESHOPT_ATTRSORT, adjacencyBuffer, NULL, NULL, NULL);

    delete[] adjacencyBuffer;
}
//-------------------------------------------------------------------------------------------------
void Terrain::_updateQuadTreeDimension()
{
    // Here uses a square quad tree for the terrain.
    // Since the shape of terrain is not guarenteed to be square and the chunk number at one side is 
    // not guaranteed to be 2^n. So we need to compute a minimum extended square for the terrain
    // and build the quad tree for the virtually extended terrain.

    // Get the chunks number at the long side of the terrain.
    const int longSideChunksCount = Math::max<int>(mDimension.x, mDimension.y) / CHUNK_DIMENSION;
    // Calculate minimum 2^n integer greater than longSideChunksCount.
    const int quadTreeSideDimensionChunksCount = (int)std::pow(2, (int)std::ceil(std::log(longSideChunksCount) / std::log(2)));
    const int quadTreeSideDimension = quadTreeSideDimensionChunksCount * CHUNK_DIMENSION;

    mQuadTreeDimension = Size2d(quadTreeSideDimension, quadTreeSideDimension);
}
//-------------------------------------------------------------------------------------------------
void Terrain::_buildTerrainQuadTree()
{
    const int quadTreeDimensionTilesCount = mQuadTreeDimension.x * mQuadTreeDimension.y;
    const int maxQuadTreeNodesCount = (1 - quadTreeDimensionTilesCount) / (-3) + quadTreeDimensionTilesCount;
    // Reserve space for nodes to avoid reallocation when adding nodes to the array.
    mTerrainQuadTreeNodes.reserve(maxQuadTreeNodesCount);

    // Begin to create the quad tree recursively.
    Rectd rootNodeSquare(Point2d::ZERO(), mQuadTreeDimension);
    _createTerrainQuadTreeNodeRecursively(rootNodeSquare, nullptr);
}
//-------------------------------------------------------------------------------------------------
void Terrain::_buildWaterQuadTree()
{
    const int quadTreeDimensionTilesCount = mQuadTreeDimension.x * mQuadTreeDimension.y;
    const int maxQuadTreeNodesCount = (1 - quadTreeDimensionTilesCount) / (-3) + quadTreeDimensionTilesCount;
    // Reserve space for nodes to avoid reallocation when adding nodes to the array.
    mWaterQuadTreeNodes.reserve(maxQuadTreeNodesCount);

    // Begin to create the quad tree recursively.
    Rectd rootNodeSquare(Point2d::ZERO(), mQuadTreeDimension);
    _createWaterQuadTreeNodeRecursively(rootNodeSquare, nullptr);
}
//-------------------------------------------------------------------------------------------------
void Terrain::_createTerrainQuadTreeNodeRecursively(const Rectd& subSquare, QuadTreeNode* parent)
{
    const Rectd actualTerrainRect = Rectd::makeRect(Point2d::ZERO(), mDimension);
    const int intersectiontilesCount = Rectd::intersectionArea(actualTerrainRect, subSquare);
    if (intersectiontilesCount == 0) {
        // No intersection means the node is not in the actual terrain, add a null pointer to
        // the parent's children array and return immediately.
        parent->children[parent->childrenCount++] = nullptr;
        return;
    }

    mTerrainQuadTreeNodes.emplace_back();
    QuadTreeNode& currentNode = mTerrainQuadTreeNodes.back();

    if (subSquare.getWidth() * subSquare.getHeight() == 1) {
        // If the sub-square only contains one tile, then this node should be a leaf node, update
        // it's tile index.
        currentNode.tile = subSquare.getPosition().y * mDimension.x + subSquare.getPosition().x;
    }
    else {
        // If the rectangle is larger than a tile, then we need to continue sub-dividing.

        if (subSquare.getWidth() == CHUNK_DIMENSION) {
            // First check if the sub-square is equal to the size of a chunk. If so, update the node's
            // chunk index to let the node has a chunk.
            int chunkPosX = subSquare.getPosition().x / CHUNK_DIMENSION;
            int chunkPosY = subSquare.getPosition().y / CHUNK_DIMENSION;
            int rowChunkCounts = mDimension.x / CHUNK_DIMENSION;
            currentNode.chunk = chunkPosY * rowChunkCounts + chunkPosX;
        }

        // Calculate sub-squares for four children.
        const Size2d halfSize = subSquare.getSize() / 2;
        Rectd subSquares[4];
        subSquares[0] = Rectd::makeRect(subSquare.getPosition(), halfSize);
        subSquares[1] = Rectd::makeRect(subSquare.getPosition() + Point2d(subSquare.getWidth() / 2, 0), halfSize);
        subSquares[2] = Rectd::makeRect(subSquare.getPosition() + Point2d(0, subSquare.getHeight() / 2), halfSize);
        subSquares[3] = Rectd::makeRect(subSquare.getPosition() + Point2d(subSquare.getWidth() / 2, subSquare.getHeight() / 2), halfSize);

        for (int i = 0; i < 4; ++i) {
            _createTerrainQuadTreeNodeRecursively(subSquares[i], &currentNode);
        }
    }

    // Update the current node's AABB.
    AABB aabb;
    if (currentNode.childrenCount > 0) {
        // The node is not a leaf node, then its AABB should be the compound of its
        // children's AABBs.
        aabb = currentNode.children[0]->aabb; // It's safe since left-top children can't be nullptr.
        for (int i = 1; i < 4; ++i) {
            QuadTreeNode* node = currentNode.children[i];
            if (node) 
                aabb = AABB::compound(aabb, node->aabb);
        }
    }
    else {
        // If the node is a leaf node then calculate its aabb based on the tile's type.
        //

        // Retrieve the tile
        const int tileIndex = subSquare.getPosition().y * mDimension.x + subSquare.getPosition().x;
        const Tile& tile = mTilesData[tileIndex];

        // Calculate tile's world space position
        D3DXVECTOR3 worldPos = _calcTilePositionFromLocation(subSquare.getPosition());
        worldPos.y = mTilesData[tileIndex].getAltitude();

        const D3DXVECTOR3 halfSize = { Tile::SIZE * 0.5f, 0.0f, Tile::SIZE * 0.5f };
        D3DXVECTOR3 min = worldPos - halfSize;
        D3DXVECTOR3 max = worldPos + halfSize;

        if (tile.type != ETileType::Flat) 
            // The height of AABB is not zero if the tile is not a flat type.
            max.y = worldPos.y + Tile::HEIGHT_PER_LEVEL;

        aabb.setMin(min);
        aabb.setMax(max);
    }

    currentNode.aabb = aabb;

    // Add the current node to its parent's children array.
    if (parent)
        parent->children[parent->childrenCount++] = &currentNode;
}
//-------------------------------------------------------------------------------------------------
void Terrain::_createWaterQuadTreeNodeRecursively(const Rectd& subSquare, QuadTreeNode* parent)
{
    const Rectd actualTerrainRect = Rectd::makeRect(Point2d::ZERO(), mDimension);
    const int intersectiontilesCount = Rectd::intersectionArea(actualTerrainRect, subSquare);
    if (intersectiontilesCount == 0) {
        // No intersection means the node is not in the actual terrain, add a null pointer to
        // the parent's children array and return immediately.
        parent->children[parent->childrenCount++] = nullptr;
        return;
    }

    mWaterQuadTreeNodes.emplace_back();
    QuadTreeNode& currentNode = mWaterQuadTreeNodes.back();

    if (subSquare.getWidth() == 1) {
        // Check if the sub-square is equal to 1. If so, update the node's
        // tile index.
        currentNode.tile = subSquare.getPosition().y * mDimension.x + subSquare.getPosition().x;
    }
    else {
        // If the rectangle is larger than a tile, then we need to continue sub-dividing.
        //

        // Calculate sub-squares for four children.
        const Size2d halfSize = subSquare.getSize() / 2;
        Rectd subSquares[4];
        subSquares[0] = Rectd::makeRect(subSquare.getPosition(), halfSize);
        subSquares[1] = Rectd::makeRect(subSquare.getPosition() + Point2d(subSquare.getWidth() / 2, 0), halfSize);
        subSquares[2] = Rectd::makeRect(subSquare.getPosition() + Point2d(0, subSquare.getHeight() / 2), halfSize);
        subSquares[3] = Rectd::makeRect(subSquare.getPosition() + Point2d(subSquare.getWidth() / 2, subSquare.getHeight() / 2), halfSize);

        for (int i = 0; i < 4; ++i) {
            _createWaterQuadTreeNodeRecursively(subSquares[i], &currentNode);
        }
    }

    // Update the current node's AABB.
    AABB aabb;
    if (currentNode.childrenCount > 0) {
        // The node is not a leaf node, then its AABB should be the compound of its
        // children's AABBs.
        aabb = currentNode.children[0]->aabb; // It's safe since left-top children can't be nullptr.

        // Iterate its children and update the aabb
        for (int i = 1; i < 4; ++i) {
            QuadTreeNode* node = currentNode.children[i];
            if (node) {  
                if (node->tile != -1 && !mTilesData[node->tile].hasWater)
                    // Tile without water doesn't affect parent's aabb
                    continue;

                aabb = AABB::compound(aabb, node->aabb);
            }
        }
    }
    else {
        // If the node is a leaf node then calculate its aabb
        D3DXVECTOR3 worldPos = _calcTilePositionFromLocation(subSquare.getPosition());
        worldPos.y = mTilesData[currentNode.tile].waterAltitude;

        D3DXVECTOR3 halfSize = { Tile::SIZE * 0.5f, 0.0f, Tile::SIZE * 0.5f };
        D3DXVECTOR3 min = worldPos - halfSize;
        D3DXVECTOR3 max = worldPos + halfSize;

        aabb.setMin(min);
        aabb.setMax(max);
    }

    currentNode.aabb = aabb;

    // Add the current node to its parent's children array.
    if (parent)
        parent->children[parent->childrenCount++] = &currentNode;
}
//-------------------------------------------------------------------------------------------------
void Terrain::_gatherVisibleTerrainChunksRecursively(const QuadTreeNode& node, const ViewFrustum& viewFrustum,
    bool isParentInsideFrustum, std::vector<int>* visibleChunks)
{
    int aabbCullingResult = ViewFrustum::AABB_INSIDE;

    if (!isParentInsideFrustum) {
        // If parent is not completely inside the frustum, then perform Frustum-AABB intersection test.
        aabbCullingResult = viewFrustum.hasIntersection(node.aabb);

        if (aabbCullingResult == ViewFrustum::AABB_OUTSIDE)
            // If this node is completely outside of the frustum, then there is no need to check its children.
            return;
    }

    if (node.chunk != -1) {
        // The node has a chunk, add it to the array.
        visibleChunks->push_back(node.chunk);
    }
    else {
        // Check its children rescursively.
        isParentInsideFrustum = (aabbCullingResult == ViewFrustum::AABB_INSIDE);
        for (int i = 0; i < 4; ++i) {
            QuadTreeNode* child = node.children[i];
            if (child) {
                _gatherVisibleTerrainChunksRecursively(*child, viewFrustum, isParentInsideFrustum, visibleChunks);
            }
        }
    }
}
//-------------------------------------------------------------------------------------------------
void Terrain::_gatherVisibleWaterTilesRecursively(const QuadTreeNode& node, const ViewFrustum& viewFrustum, 
    bool isParentInsideFrustum, std::vector<int>* visibleWaterTiles)
{
    int aabbCullingResult = ViewFrustum::AABB_INSIDE;

    if (!isParentInsideFrustum) {
        // If parent is not completely inside the frustum, then perform Frustum-AABB intersection test.
        aabbCullingResult = viewFrustum.hasIntersection(node.aabb);

        if (aabbCullingResult == ViewFrustum::AABB_OUTSIDE)
            // If this node is completely outside of the frustum, then there is no need to check its children.
            return;
    }

    if (node.tile != -1) {
        // Node has a tile, check whether water is visible.
        // If it is visible, add to the visible water tile array.
        if (mTilesData[node.tile].isWaterVisible())
            visibleWaterTiles->push_back(node.tile);
    }
    else {
        // Check its children rescursively.
        isParentInsideFrustum = (aabbCullingResult == ViewFrustum::AABB_INSIDE);
        for (int i = 0; i < 4; ++i) {
            QuadTreeNode* child = node.children[i];
            if (child) {
                _gatherVisibleWaterTilesRecursively(*child, viewFrustum, isParentInsideFrustum, visibleWaterTiles);
            }
        }
    }
}
//-------------------------------------------------------------------------------------------------
void Terrain::_raycastOnQuadTreeRecursively(const QuadTreeNode& node, const Ray& ray, std::vector<TerrainRaycastHit>* hits) const
{
    if (ray.intersectAABB(node.aabb) == false)
        // The ray has no intersection with the AABB of this node.
        return;

    if (node.childrenCount != 0) {
        // Continue to test its children.
        for (int i = 0; i < 4; ++i) {
            QuadTreeNode* child = node.children[i];
            if (child) {
                _raycastOnQuadTreeRecursively(*child, ray, hits);
            }
        }
    }
    else {
        // This node has no children so it must be a tile. Perform Triangle-Ray intersection test.
        
        // Get this tile's geometry.
        const Tile& tile = mTilesData[node.tile];
        const TileGeometry& tileGeometry = TILE_GEOMETRIES()[tile.type];

        // Get tile's world space position
        Point2d tileLocation(node.tile % mDimension.x, node.tile / mDimension.x);
        D3DXVECTOR3 tilePosition = _calcTilePositionFromLocation(tileLocation);
        tilePosition.y = mTilesData[node.tile].getAltitude();

        // Cast ray on tile's geometry.
        float distances[2];
        int hitCount = tileGeometry.raycast(tilePosition, ray, distances);

        // Gather ray cast hit position
        D3DXVECTOR3 hitPosition;
        for (int i = 0; i < hitCount; ++i) {
            hitPosition = ray.getPointOnRay(distances[i]);
            hits->push_back(TerrainRaycastHit(distances[i], hitPosition, tileLocation));
        }
    }
}
//-------------------------------------------------------------------------------------------------
int Terrain::TileGeometry::raycast(const D3DXVECTOR3& tilePos, const Ray& ray, float distances[2]) const
{
    // Move vertices from local space to world space
    D3DXVECTOR3 verticesInWorld[] =
    {
        vertices[0] + tilePos,
        vertices[1] + tilePos,
        vertices[2] + tilePos,
        vertices[3] + tilePos,
    };

    // Test raycast on two triangles
    int hitCount = 0;
    D3DXVECTOR3 v0, v1, v2;
    float dist, u, v;
    for (int i = 0; i < 2; ++i) {
        v0 = verticesInWorld[indices[i * 3]];
        v1 = verticesInWorld[indices[i * 3 + 1]];
        v2 = verticesInWorld[indices[i * 3 + 2]];

        if (::D3DXIntersectTri(&v0, &v1, &v2, &ray.getOrigin(), &ray.getDirection(), &u, &v, &dist)) {
            distances[hitCount++] = dist;
        }
    }

    return hitCount;
}

}
