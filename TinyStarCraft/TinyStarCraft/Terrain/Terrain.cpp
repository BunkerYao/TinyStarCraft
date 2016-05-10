#include "Precompiled.h"
#include "Terrain.h"
#include "TerrainMaterial.h"
#include "Rendering/Camera.h"
#include "Rendering/RenderSystem.h"
#include "Rendering/Texture.h"
#include "Utilities/DebugOutput.h"
#include "Utilities/Logging.h"
#include "Utilities/Math.h"
#include "Utilities/Point2.h"
#include "Utilities/Ray.h"

namespace TinyStarCraft
{

const std::array<Terrain::CellGeometry, 15>& Terrain::CELL_GEOMETRIES()
{
    static const std::array<Terrain::CellGeometry, 15> geometries =
    {
        {
            // RAMPTYPE_FLAT
            {
                {
                    { -CELL_SIZE * 0.5f, 0.0f, -CELL_SIZE * 0.5f },
                    { -CELL_SIZE * 0.5f, 0.0f,  CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f, 0.0f, -CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f, 0.0f,  CELL_SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_SOUTH_A
            {
                {
                    { -CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,  -CELL_SIZE * 0.5f },
                    { -CELL_SIZE * 0.5f,                     0.0f,   CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,                     0.0f,  -CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,                     0.0f,   CELL_SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_WEST_A
            {
                {
                    { -CELL_SIZE * 0.5f,                     0.0f,  -CELL_SIZE * 0.5f },
                    { -CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,   CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,                     0.0f,  -CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,                     0.0f,   CELL_SIZE * 0.5f }
                },
                {
                    0, 1, 3, 3, 0, 2
                }
            },
            // RAMPTYPE_NORTH_A
            {
                {
                    { -CELL_SIZE * 0.5f,                     0.0f,  -CELL_SIZE * 0.5f },
                    { -CELL_SIZE * 0.5f,                     0.0f,   CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,                     0.0f,  -CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,   CELL_SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_EAST_A
            {
                {
                    { -CELL_SIZE * 0.5f,                     0.0f,  -CELL_SIZE * 0.5f },
                    { -CELL_SIZE * 0.5f,                     0.0f,   CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,  -CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,                     0.0f,   CELL_SIZE * 0.5f }
                },
                {
                    0, 1, 3, 3, 0, 2
                }
            },
            // RAMPTYPE_SOUTHWEST
            {
                {
                    { -CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,  -CELL_SIZE * 0.5f },
                    { -CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,   CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,                     0.0f,  -CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,                     0.0f,   CELL_SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_NORTHWEST
            {
                {
                    { -CELL_SIZE * 0.5f,                     0.0f,  -CELL_SIZE * 0.5f },
                    { -CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,   CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,                     0.0f,  -CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,   CELL_SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_NORTHEAST
            {
                {
                    { -CELL_SIZE * 0.5f,                     0.0f,  -CELL_SIZE * 0.5f },
                    { -CELL_SIZE * 0.5f,                     0.0f,   CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,  -CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,   CELL_SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_SOUTHEAST
            {
                {
                    { -CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,  -CELL_SIZE * 0.5f },
                    { -CELL_SIZE * 0.5f,                     0.0f,   CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,  -CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,                     0.0f,   CELL_SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_SOUTH_B
            {
                {
                    { -CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,  -CELL_SIZE * 0.5f },
                    { -CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,   CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,  -CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,                     0.0f,   CELL_SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_WEST_B
            {
                {
                    { -CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,  -CELL_SIZE * 0.5f },
                    { -CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,   CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,                     0.0f,  -CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,   CELL_SIZE * 0.5f }
                },
                {
                    0, 1, 3, 3, 0, 2
                }
            },
            // RAMPTYPE_NORTH_B
            {
                {
                    { -CELL_SIZE * 0.5f,                     0.0f,  -CELL_SIZE * 0.5f },
                    { -CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,   CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,  -CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,   CELL_SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            },
            // RAMPTYPE_EAST_B
            {
                {
                    { -CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,  -CELL_SIZE * 0.5f },
                    { -CELL_SIZE * 0.5f,                     0.0f,   CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,  -CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,   CELL_SIZE * 0.5f }
                },
                {
                    0, 1, 3, 3, 0, 2
                }
            },
            // RAMPTYPE_DOUBLE_WESTTOEAST
            {
                {
                    { -CELL_SIZE * 0.5f,                     0.0f,  -CELL_SIZE * 0.5f },
                    { -CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,   CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,  -CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,                     0.0f,   CELL_SIZE * 0.5f }
                },
                {
                    0, 1, 3, 3, 0, 2
                }
            },
            // RAMPTYPE_DOUBLE_NORTHTOSOUTH
            {
                {
                    { -CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,  -CELL_SIZE * 0.5f },
                    { -CELL_SIZE * 0.5f,                     0.0f,   CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,                     0.0f,  -CELL_SIZE * 0.5f },
                    {  CELL_SIZE * 0.5f,    ALTITUDE_LEVEL_HEIGHT,   CELL_SIZE * 0.5f }
                },
                {
                    0, 1, 2, 2, 1, 3
                }
            }
        }
    };

    return geometries;
}

Terrain::~Terrain()
{
    if (mMesh)
        mRenderSystem->destroyMesh(mMesh);
}

bool Terrain::create(const Size2d& dimension, const std::vector<int>& cellsData, const std::vector<int>& altitudeLevelData, 
    TerrainMaterial* material)
{
    // Validate terrain dimension
    if (dimension.x % CHUNK_SIZE_N > 0 || dimension.y % CHUNK_SIZE_N > 0) {
        TINYSC_LOGLINE_ERR("Invalid terrain dimension(%d*%d).", dimension.x, dimension.y);
        return false;
    }

    mDimension = dimension;
    mCellsData = cellsData;
    mAltitudeLevelData = altitudeLevelData;
    mMaterial = material;

    // Create the mesh.
    if (!_createTerrainMesh())
        return false;

    // Create quad tree nodes.
    _buildQuadTree();

    return true;
}

void Terrain::draw(Camera* camera)
{
    // Gather visible chunks
    std::vector<int> visibleChunks;
    _gatherVisibleChunksRecursively(mQuadTreeNodes.front(), camera->getViewFrustum(), false, &visibleChunks);
    //OutputDebugStringF("Visible chunks count:%d.\n", visibleChunks.size());

    ID3DXEffect* terrainEffect = mMaterial->getEffect();
    D3DXMATRIX viewProjMatrix = camera->getViewMatrix() * camera->getProjMatrix();
    terrainEffect->SetMatrix("_ViewProjMatrix", &viewProjMatrix);

    // Apply material parameters.
    mMaterial->apply();

    UINT passesCount = 0;
    terrainEffect->Begin(&passesCount, 0);
    terrainEffect->BeginPass(0);
    
    for (auto& it : visibleChunks)
        mMesh->DrawSubset(it);

    terrainEffect->EndPass();
    terrainEffect->End();
}

bool Terrain::raycast(const Ray& ray, TerrainRaycastHit* hit) const
{
    // Get all hits.
    std::vector<TerrainRaycastHit> hits;
    _findRaycastHitPointsRecursively(mQuadTreeNodes.front(), ray, &hits);

    if (!hits.empty()) {
        // Sort the hits by distance to find the nearst hit.
        std::sort(hits.begin(), hits.end());
        *hit = hits.front();
        return true;
    }

    return false;
}

bool Terrain::raycastAll(const Ray& ray, std::vector<TerrainRaycastHit>* hits) const
{
    // Get all hits.
    _findRaycastHitPointsRecursively(mQuadTreeNodes.front(), ray, hits);

    if (!hits->empty()) {
        // Sort the hits by distance
        std::sort(hits->begin(), hits->end());
        return true;
    }

    return false;
}

bool Terrain::_createTerrainMesh()
{
    // Create mesh.
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
    mMesh = mRenderSystem->createMesh(numVertices, numFaces, vertElement);
    if (mMesh == nullptr) {
        TINYSC_LOGLINE_ERR("Failed to create terrain mesh. TerrainDimension(%d*%d).", mDimension.x, mDimension.y);
        return false;
    }

    // Divide the mesh into chunks, e.g. sub-meshes.
    DWORD* attribBuffer = NULL;
    mMesh->LockAttributeBuffer(0, &attribBuffer);
    const int chunksCount = (mDimension.x * mDimension.y) / CELLS_COUNT_PER_CHUNK;
    for (int iChunk = 0; iChunk < chunksCount; ++iChunk) {
        for (int iFace = 0; iFace < CELLS_COUNT_PER_CHUNK * 2; ++iFace) {
            *attribBuffer = (DWORD)iChunk;
            attribBuffer++;
        }
    }
    mMesh->UnlockAttributeBuffer();

    // Optimize the mesh to increase draw performence.
    DWORD* adjacencyBuffer = new DWORD[mMesh->GetNumFaces() * 3];
    mMesh->GenerateAdjacency(0.0f, adjacencyBuffer);
    mMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, adjacencyBuffer, NULL, NULL, NULL);
    delete[] adjacencyBuffer;

    // Modify mesh vertex properties and generate indices.

    // Blending texture coordinate for one cell.
    const float cellBlendTexcoordStride = 1.0f / BLEND_TEXTURE_SIZE_N;
    D3DXVECTOR2 cellBlendTexcoord[4] =
    {
        { 0.0f, 0.0f },
        { cellBlendTexcoordStride, 0.0f },
        { 0.0f, cellBlendTexcoordStride },
        { cellBlendTexcoordStride, cellBlendTexcoordStride }
    };
    // Control texture coordinate for one cell.
    const float cellControlTexcoordStrideU = 1.0f / mDimension.x;
    const float cellControlTexcoordStrideV = 1.0f / mDimension.y;
    D3DXVECTOR2 cellControlTexcoord[4] =
    {
        { 0.0f, 0.0f },
        { cellControlTexcoordStrideU, 0.0f },
        { 0.0f, cellControlTexcoordStrideV },
        { cellControlTexcoordStrideU, cellControlTexcoordStrideV }
    };

    struct Vertex
    {
        D3DXVECTOR3 pos;
        D3DXVECTOR3 normal;
        D3DXVECTOR2 blendTexcoord;
        D3DXVECTOR2 controlTexcoord;
    };

    Vertex* vertices = nullptr;
    unsigned* indices = nullptr; // 32-Bit indices
    mMesh->LockVertexBuffer(0, (void**)&vertices);
    mMesh->LockIndexBuffer(0, (void**)&indices);

    const int rowChunksCount = mDimension.x / CHUNK_SIZE_N;
    for (int iChunk = 0; iChunk < chunksCount; ++iChunk) {
        for (int iCell = 0; iCell < CELLS_COUNT_PER_CHUNK; ++iCell) {
            // Get the cell's geometry.
            Point2d cellLocation;
            cellLocation.x = iChunk % rowChunksCount * CHUNK_SIZE_N + iCell % CHUNK_SIZE_N;
            cellLocation.y = iChunk / rowChunksCount * CHUNK_SIZE_N + iCell / CHUNK_SIZE_N;
            const int cellIndex = cellLocation.y * mDimension.x + cellLocation.x;
            const int cellType = mCellsData[cellIndex];
            const int altitudeLevel = mAltitudeLevelData[cellIndex];
            const CellGeometry& cellGeometry = CELL_GEOMETRIES()[cellType];

            const int globalCellIndex = iChunk * CELLS_COUNT_PER_CHUNK + iCell;

            // Generate indices for this cell.
            for (int iIndices = 0; iIndices < 6; ++iIndices) {
                indices[globalCellIndex * 6 + iIndices] = globalCellIndex * 4 + cellGeometry.indices[iIndices];
            }

            // Modify vertices
            for (int iVert = 0; iVert < 4; ++iVert) {
                const int vertIndex = globalCellIndex * 4 + iVert;

                // Get the vertex local position from this cell type's geometry vertices position.
                vertices[vertIndex].pos = cellGeometry.vertices[iVert], 1.0f;
                // Move the vertex to world space position.
                vertices[vertIndex].pos.x += cellLocation.y * CELL_SIZE;
                vertices[vertIndex].pos.y += altitudeLevel * ALTITUDE_LEVEL_HEIGHT;
                vertices[vertIndex].pos.z += cellLocation.x * CELL_SIZE;

                // Write the blend texture coordinate.
                vertices[vertIndex].blendTexcoord = cellBlendTexcoord[iVert];
                vertices[vertIndex].blendTexcoord.x += cellLocation.x * cellBlendTexcoordStride;
                vertices[vertIndex].blendTexcoord.y += cellLocation.y * cellBlendTexcoordStride;

                // Write the control texture coordinate.
                vertices[vertIndex].controlTexcoord = cellControlTexcoord[iVert];
                vertices[vertIndex].controlTexcoord.x += cellLocation.x * cellControlTexcoordStrideU;
                vertices[vertIndex].controlTexcoord.y += cellLocation.y * cellControlTexcoordStrideV;
            }
        }
    }

    mMesh->UnlockVertexBuffer();
    mMesh->UnlockIndexBuffer();

    return true;
}

void Terrain::_buildQuadTree()
{
    // Here uses a square quad tree for the terrain.
    // Since the shape of terrain is not guarenteed to be square and the chunk number at one side is 
    // not guaranteed to be 2^n. So we need to compute a minimum extended square for the terrain
    // and build the quad tree for the virtually extended terrain.

    // Get the chunks number at the long side of the terrain.
    const int longSideChunksCount = Math::max<int>(mDimension.x, mDimension.y) / CHUNK_SIZE_N;
    // Calculate minimum 2^n integer greater than terrainLongSideCellsCount.
    const int minExtSqrSideChunksCount = (int)std::pow(2, (int)std::ceil(std::log(longSideChunksCount) / std::log(2)));
    // Calculate the maximum number of the nodes.
    const int minExtSqrChunksCount = minExtSqrSideChunksCount * minExtSqrSideChunksCount;
    const int minExtSqrCellsCount = minExtSqrChunksCount * CELLS_COUNT_PER_CHUNK;
    const int maxNodesCount = (1 - minExtSqrCellsCount) / (-3) + minExtSqrCellsCount;
    // Reserve space for nodes to avoid reallocation when adding nodes to the array.
    mQuadTreeNodes.reserve(maxNodesCount);

    // Begin to create the quad tree recursively.
    const int minExtSqrSideCellsCount = (int)sqrt(minExtSqrCellsCount);
    Rectd rootNodeSquare(Point2d::ZERO(), Point2d(minExtSqrSideCellsCount, minExtSqrSideCellsCount));
    _createQuadTreeNodeRecursively(rootNodeSquare, nullptr);
}

void Terrain::_createQuadTreeNodeRecursively(const Rectd& subSquare, QuadTreeNode* parent)
{
    const Rectd actualTerrainRect = Rectd::makeRect(Point2d::ZERO(), mDimension);
    const int intersectionCellsCount = Rectd::intersectionArea(actualTerrainRect, subSquare);
    if (intersectionCellsCount == 0) {
        // No intersection means the node is not in the actual terrain, add a null pointer to
        // the parent's children array and return immediately.
        parent->children[parent->childrenCount++] = nullptr;
        return;
    }

    mQuadTreeNodes.emplace_back();
    QuadTreeNode& currentNode = mQuadTreeNodes.back();

    if (subSquare.getWidth() * subSquare.getHeight() == 1) {
        // If the sub-square only contains one cell, then this node should be a leaf node, update
        // it's cell index.
        currentNode.cell = subSquare.getPosition().y * mDimension.x + subSquare.getPosition().x;
    }
    else {
        // If the rectangle is larger than a cell, then we need to continue sub-dividing.

        if (subSquare.getWidth() == CHUNK_SIZE_N) {
            // First check if the sub-square is equal to the size of a chunk. If so, update the node's
            // chunk index to let the node has a chunk.
            int chunkPosX = subSquare.getPosition().x / CHUNK_SIZE_N;
            int chunkPosY = subSquare.getPosition().y / CHUNK_SIZE_N;
            int rowChunkCounts = mDimension.x / CHUNK_SIZE_N;
            currentNode.chunk = chunkPosY * rowChunkCounts + chunkPosX;
        }

        // Calculate sub-squares for four children.
        Rectd subSquares[4];
        subSquares[0] = Rectd::makeRect(subSquare.getPosition(), subSquare.getSize() / 2);
        subSquares[1] = Rectd::makeRect(subSquare.getPosition() + Point2d(subSquare.getWidth() / 2, 0), subSquare.getSize() / 2);
        subSquares[2] = Rectd::makeRect(subSquare.getPosition() + Point2d(0, subSquare.getHeight() / 2), subSquare.getSize() / 2);
        subSquares[3] = Rectd::makeRect(subSquare.getPosition() + Point2d(subSquare.getWidth() / 2, subSquare.getHeight() / 2),
            subSquare.getSize() / 2);

        for (int i = 0; i < 4; ++i) {
            _createQuadTreeNodeRecursively(subSquares[i], &currentNode);
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
            if (node) {
                AABB childAABB = node->aabb;

                if (childAABB.getMax().x > aabb.getMax().x)
                    aabb.getMax().x = childAABB.getMax().x;
                if (childAABB.getMax().y > aabb.getMax().y)
                    aabb.getMax().y = childAABB.getMax().y;
                if (childAABB.getMax().z > aabb.getMax().z)
                    aabb.getMax().z = childAABB.getMax().z;

                if (childAABB.getMin().x < aabb.getMin().x)
                    aabb.getMin().x = childAABB.getMin().x;
                if (childAABB.getMin().y < aabb.getMin().y)
                    aabb.getMin().y = childAABB.getMin().y;
                if (childAABB.getMin().z < aabb.getMin().z)
                    aabb.getMin().z = childAABB.getMin().z;
            }
        }
    }
    else {
        // If the node is a leaf node then calculate its aabb based on the cell's type.
        D3DXVECTOR3 halfSize = { CELL_SIZE * 0.5f, 0.0f, CELL_SIZE * 0.5f };
        const int cellIndex = subSquare.getPosition().y * mDimension.x + subSquare.getPosition().x;
        const int altitudeLevel = mAltitudeLevelData[cellIndex];
        D3DXVECTOR3 worldPos = _getCellWorldSpacePosition(subSquare.getPosition(), altitudeLevel);
        D3DXVECTOR3 min = worldPos - halfSize;
        D3DXVECTOR3 max = worldPos + halfSize;

        // The height of AABB is not zero if the cell is not a flat type.
        const int cellType = mCellsData[cellIndex];
        if (cellType != FLAT) {
            max.y = worldPos.y + ALTITUDE_LEVEL_HEIGHT;
        }

        aabb.setMin(min);
        aabb.setMax(max);
    }

    currentNode.aabb = aabb;

    // Add the current node to its parent's children array.
    if (parent)
        parent->children[parent->childrenCount++] = &currentNode;
}

void Terrain::_gatherVisibleChunksRecursively(const QuadTreeNode& node, const ViewFrustum& viewFrustum, bool isParentInsideFrustum,
    std::vector<int>* visibleChunks)
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
        return;
    }
    else {
        // Check its children rescursively.
        isParentInsideFrustum = (aabbCullingResult == ViewFrustum::AABB_INSIDE);
        for (int i = 0; i < 4; ++i) {
            QuadTreeNode* child = node.children[i];
            if (child) {
                _gatherVisibleChunksRecursively(*child, viewFrustum, isParentInsideFrustum, visibleChunks);
            }
        }
    }
}

void Terrain::_findRaycastHitPointsRecursively(const QuadTreeNode& node, const Ray& ray, std::vector<TerrainRaycastHit>* hits) const
{
    if (ray.intersectAABB(node.aabb) == false)
        // The ray has no intersection with the AABB of this node.
        return;

    if (node.childrenCount != 0) {
        // Continue to test its children.
        for (int i = 0; i < 4; ++i) {
            QuadTreeNode* child = node.children[i];
            if (child) {
                _findRaycastHitPointsRecursively(*child, ray, hits);
            }
        }
    }
    else {
        // This node has no children so it must be a cell. Perform Triangle-Ray intersection test.
        
        // Get this cell's geometry.
        const int cellType = mCellsData[node.cell];
        const int altitudeLevel = mAltitudeLevelData[node.cell];
        const CellGeometry& cellGeometry = CELL_GEOMETRIES()[cellType];

        float distances[2];
        Point2d cellLocation(node.cell % mDimension.x, node.cell / mDimension.x);
        int hitCount = cellGeometry.raycast(_getCellWorldSpacePosition(cellLocation, altitudeLevel), ray, distances);

        D3DXVECTOR3 hitPosition;
        for (int i = 0; i < hitCount; ++i) {
            hitPosition = ray.getPointOnRay(distances[i]);
            hits->push_back(TerrainRaycastHit(distances[i], hitPosition, cellLocation));
        }
    }
}

D3DXVECTOR3 Terrain::_getCellWorldSpacePosition(const Point2d& location, int altitudeLevel) const
{
    return D3DXVECTOR3(CELL_SIZE * location.y, ALTITUDE_LEVEL_HEIGHT * altitudeLevel, CELL_SIZE * location.x);
}

int Terrain::CellGeometry::raycast(const D3DXVECTOR3& tilePos, const Ray& ray, float distances[2]) const
{
    D3DXVECTOR3 verticesInWorld[] =
    {
        vertices[0] + tilePos,
        vertices[1] + tilePos,
        vertices[2] + tilePos,
        vertices[3] + tilePos,
    };

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
