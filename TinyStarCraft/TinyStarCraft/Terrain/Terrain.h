#pragma once

#include "Utilities/AABB.h"
#include "Utilities/Rect2.h"
#include "Utilities/Size2.h"

namespace TinyStarCraft
{

class Mesh;
class RenderSystem;

class Terrain
{
public:
    /** The side length of one "cell" of terrain's mesh. */
    static constexpr float CELL_SIZE = 45.255f;            
    /** Per-Altitude level height */
    static constexpr float ALTITUDE_LEVEL_HEIGHT = 18.475f; 
    /* Chunk side length in cells number. */
    static const int CHUNK_SIZE_N = 8;
    /* Blend texture side length in cells number. */
    static const int BLEND_TEXTURE_SIZE_N = 8;

    /** 15 cell types */
    static const int FLAT = 0;
    static const int SOUTH_A = 1;
    static const int WEST_A = 2;
    static const int NORTH_A = 3;
    static const int EAST_A = 4;
    static const int SOUTHWEST = 5;
    static const int NORTHWEST = 6;
    static const int NORTHEAST = 7;
    static const int SOUTHEAST = 8;
    static const int SOUTH_B = 9;
    static const int WEST_B = 10;
    static const int NORTH_B = 11;
    static const int EAST_B = 12;
    static const int DOUBLE_WESTTOEAST = 13;
    static const int DOUBLE_NORTHTOSOUTH = 14;

private:
    /** Geometry of a cell. */
    struct CellGeometry
    {
        D3DXVECTOR3 vertices[4];
        int indices[6];
    };

    /** Geometries of 15 type of cells. */
    static const std::array<CellGeometry, 15>& CELL_GEOMETRIES();

    struct QuadTreeNode
    {
        QuadTreeNode* children[4];
        size_t childrenCount;
        AABB aabb;
        int chunk;  // Will be chunk's index if the node has a chunk, otherwise it will be -1.
        int cell;   // Will be cell's index if the node has a cell, otherwise it will be -1.

        QuadTreeNode()
            : children { 0 }, childrenCount(0), chunk(-1), cell(-1)
        {}
    };

public:
    /**
     *	Constructor
     */
    explicit Terrain(RenderSystem* renderSystem)
        : mRenderSystem(renderSystem)
    {}

    ~Terrain();

    bool init(const Size2d& dimension, const std::vector<int>& cellsData, const std::vector<int>& altitudeLevelData);

private:
    /**	Create the terrain mesh and devide it into chunks(sub-meshes). */
    bool _createTerrainMesh();

    /**
     *  Modify terrain's mesh.
     *  Operations includes:
     *  1. Changing vertices position based on the cell's type and altitude level
     *  2. Computing vertex normals and smooth them
     *  3. Generate texture coordinate for the control texture & blend textures. 
     */
    void _modifyTerrainMesh();

    /**
     *	Build quad tree for the terrain.
     */
    void _buildQuadTree();

    /**
     *	Subdivide the terrain and create quad tree nodes recursively.
     */
    void _createQuadTreeNodeRecursively(const Rectd& subSquare, QuadTreeNode* parent);

private:
    RenderSystem* mRenderSystem;
    Mesh* mMesh;
    Size2d mDimension;
    std::vector<int> mCellsData;
    std::vector<int> mAltitudeLevelData;
    std::vector<QuadTreeNode> mQuadTreeNodes;
};

};

