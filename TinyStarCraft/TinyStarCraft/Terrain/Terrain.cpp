#include "Precompiled.h"
#include "Terrain.h"
#include "Rendering/RenderResources.h"
#include "Rendering/RenderSystem.h"
#include "Utilities/Math.h"
#include "Utilities/Point2.h"

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
    mRenderSystem->destroyMesh(mMesh);
}

bool Terrain::init(const Size2d& dimension, const std::vector<int>& cellsData, const std::vector<int>& altitudeLevelData)
{
    if (mDimension.x % CHUNK_SIZE_N > 0 || mDimension.y % CHUNK_SIZE_N > 0)
        return false;

    mDimension = dimension;
    mCellsData = cellsData;
    mAltitudeLevelData = altitudeLevelData;

    if (!_createTerrainMesh())
        return false;

    _modifyTerrainMesh();
    _buildQuadTree();

    return true;
}

bool Terrain::_createTerrainMesh()
{
    // Create mesh.
    D3DVERTEXELEMENT9 vertElement[] = 
    {
        { 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 40, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
        D3DDECL_END()
    };

    const DWORD numFaces = mDimension.x * mDimension.y * 2;
    const DWORD numVertices = mDimension.x * mDimension.y * 4;
    mMesh = mRenderSystem->createMesh(numVertices, numFaces, vertElement);

    if (mMesh == nullptr)
        return false;

    // Divide the mesh into chunks, e.g. sub-meshes.
    const int cellsCountPerChunk = CHUNK_SIZE_N * CHUNK_SIZE_N;
    const int chunksCount = (mDimension.x * mDimension.y) / cellsCountPerChunk;
    std::vector<D3DXATTRIBUTERANGE> attribTable(chunksCount);
    for (size_t i = 0; i < attribTable.size(); ++i) {
        attribTable[i].AttribId = i;
        attribTable[i].FaceCount = cellsCountPerChunk * 2;
        attribTable[i].FaceStart = i * cellsCountPerChunk * 2;
        attribTable[i].VertexCount = cellsCountPerChunk * 4;
        attribTable[i].VertexStart = i * cellsCountPerChunk * 4;
    }

    mMesh->get()->SetAttributeTable(&attribTable.front(), attribTable.size());

    return true;
}

void Terrain::_modifyTerrainMesh()
{
    struct Vertex
    {
        D3DXVECTOR4 pos;
        D3DXVECTOR4 normal;
        D3DXVECTOR4 blendTexcoord;
        D3DXVECTOR4 controlTexcoord;
    };

    Vertex* vertices = nullptr;
    mMesh->get()->LockVertexBuffer(0, (void**)&vertices);

    const int cellsCountPerChunk = CHUNK_SIZE_N * CHUNK_SIZE_N;
    const int rowChunksCount = mDimension.x / CHUNK_SIZE_N;
    const int columnChunksCount = mDimension.y / CHUNK_SIZE_N;
    const int chunksCount = rowChunksCount * columnChunksCount;

    // Change vertices position based on the cell's type and altitude level
    for (int iChunk = 0; iChunk < chunksCount; ++iChunk) {
        for (int iCell = 0; iCell < cellsCountPerChunk; ++iCell) {
            Point2d cellLocation;
            cellLocation.x = iChunk % rowChunksCount + iCell % CHUNK_SIZE_N;
            cellLocation.y = iChunk / rowChunksCount + iCell / CHUNK_SIZE_N;
            const int cellIndex = cellLocation.y * mDimension.x + cellLocation.x;
            const int cellType = mCellsData[cellIndex];
            const int altitudeLevel = mAltitudeLevelData[cellIndex];
            const CellGeometry& cellGeometry = CELL_GEOMETRIES()[cellType];

            for (int iVert = 0; iVert < 4; ++iVert) {
                const int vertIndex = (iChunk * cellsCountPerChunk + iCell) * 4 + iVert;

                vertices[vertIndex].pos = D3DXVECTOR4(cellGeometry.vertices[iVert], 1.0f);

                // Calculate cell origin's world space position
                D3DXVECTOR4 worldOrigin(
                    cellLocation.x * CELL_SIZE,
                    altitudeLevel * ALTITUDE_LEVEL_HEIGHT,
                    cellLocation.y * CELL_SIZE, 
                    0.0f
                    );

                // Move the cell to world space position.
                vertices[vertIndex].pos += worldOrigin;
            }
        }
    }

    // TODO:

    mMesh->get()->UnlockVertexBuffer();
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
    // Calculate the maximum number of the nodes will be created in quad tree.
    const int minExtSqrChunksCount = minExtSqrSideChunksCount * minExtSqrSideChunksCount;
    const int minExtSqrCellsCount = minExtSqrChunksCount * CHUNK_SIZE_N;
    const int maxNodesCount = (1 - minExtSqrCellsCount) / (-3) + minExtSqrCellsCount;

    // Reserve space for nodes to avoid reallocation when adding nodes to the array.
    mQuadTreeNodes.reserve(maxNodesCount);

    Rectd rootNodeSquare(Point2d::ZERO(), Point2d(minExtSqrCellsCount, minExtSqrCellsCount));
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
        D3DXVECTOR3 worldPos =
        {
            CELL_SIZE * subSquare.getPosition().x,
            ALTITUDE_LEVEL_HEIGHT * altitudeLevel,
            CELL_SIZE * subSquare.getPosition().y
        };
        D3DXVECTOR3 min = worldPos - halfSize;
        D3DXVECTOR3 max = worldPos + halfSize;

        const int cellType = mCellsData[cellIndex];
        if (cellType != FLAT) {
            // The height of AABB is not zero if the cell is not a flat type.
            max.y = worldPos.y + ALTITUDE_LEVEL_HEIGHT;
        }

        aabb.setMin(min);
        aabb.setMax(max);
    }

    currentNode.aabb = aabb;

    // Add the current node to its parent's children array.
    if (parent)
        parent->children[parent->childrenCount++];
}

}
