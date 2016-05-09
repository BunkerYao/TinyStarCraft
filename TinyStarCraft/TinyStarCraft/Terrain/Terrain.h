#pragma once

#include "Utilities/AABB.h"
#include "Utilities/Rect2.h"
#include "Utilities/Size2.h"

namespace TinyStarCraft
{

class Camera;
class Ray;
class RenderSystem;
class TerrainMaterial;
class ViewFrustum;


struct TerrainRaycastHit
{
    float distance;
    D3DXVECTOR3 hitPosition;
    Point2d cellLocation;

    /** Two raycast hit are comparable by distance. */
    bool operator<(const TerrainRaycastHit& other) const
    {
        return distance < other.distance;
    }

    TerrainRaycastHit() = default;

    TerrainRaycastHit(float distance, const D3DXVECTOR3& hitPosition, const Point2d& location)
        : distance(distance), hitPosition(hitPosition), cellLocation(location)
    {}
};


class Terrain
{
public:
    /** The side length of one "cell" in terrain's mesh. */
    static constexpr float CELL_SIZE = 45.255f;            
    /** Per-Altitude level height */
    static constexpr float ALTITUDE_LEVEL_HEIGHT = 18.475f; 
    /* Chunk side length measured in cells number. */
    static const int CHUNK_SIZE_N = 8;
    /* Number of cells per chunk. */
    static const int CELLS_COUNT_PER_CHUNK = CHUNK_SIZE_N * CHUNK_SIZE_N;
    /* Blend texture side length measured in cells number. */
    static const int BLEND_TEXTURE_SIZE_N = 4;

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

        /**
         *	Perform a Ray-Triangle intersection test.
         *  
         *  @return
         *  Returns the number of hits ranges from 0~2.
         */
        int raycast(const D3DXVECTOR3& tilePos, const Ray& ray, float distances[2]) const;
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
        : mRenderSystem(renderSystem), mMesh(NULL)
    {}

    ~Terrain();

    /**
     *	Create the terrain.
     *
     *  @return
     *  If the terrain is created, returns true. Otherwise returns false.
     */
    bool create(const Size2d& dimension, const std::vector<int>& cellsData, const std::vector<int>& altitudeLevelData, 
        TerrainMaterial* material);

    /**
     *	Draw the terrain.
     */
    void draw(Camera* camera);

    /**
     *	Cast a ray on the terrain and return the nearest hit point.
     *  
     *  @Return
     *  Returns true if there is a hit. Returns false otherwise.
     */
    bool raycast(const Ray& ray, TerrainRaycastHit* hit) const;

    /**
     *	Cast a ray on the terrain and return all hit points from the nearest to the farthest.
     */
    bool raycastAll(const Ray& ray, std::vector<TerrainRaycastHit>* hits) const;

private:
    /**	Create the terrain mesh and devide it into chunks(sub-meshes).
     *  Operations includes:
     *  1. Generate indices.
     *  2. Changing vertices position based on the cell's type and altitude level
     *  3. Computing vertex normals and smooth them
     *  4. Generate texture coordinate for the control texture & blend textures.
     */
    bool _createTerrainMesh();

    /**
     *	Build quad tree for the terrain.
     */
    void _buildQuadTree();

    /**
     *	Subdivide the terrain and create quad tree nodes recursively.
     */
    void _createQuadTreeNodeRecursively(const Rectd& subSquare, QuadTreeNode* parent);

    /**
     *	Gather visible chunks of the terrain mesh.
     *  This will traverse the entire quad tree and do frustum-AABB intersection test
     *  to determine each chunk's visibility. Visible chunk's id will be gathered to 
     *  the visible chunks array.
     *
     *  @param isParentInsideFrustum
     *  To indicate whether this node's parent is completely inside the camera view frustum.
     *  If set to true, the Frustum-AABB intersection test will be skipped and the node and all its
     *  children will be considered as visible nodes since they must be inside the frustum as well.
     *  Otherwise, a Frustum-AABB intersection test is performed to check whether the node is visible.
     */
    void _gatherVisibleChunksRecursively(const QuadTreeNode& node, const ViewFrustum& viewFrustum, bool isParentInsideFrustum, 
        std::vector<int>* visibleChunks);

    /**
     *	Traverse the quad tree to find cells which have intersections with the ray, and perform Triangle-Ray intersection
     *  test on those cells to compute the hit points.
     */
    void _findRaycastHitPointsRecursively(const QuadTreeNode& node, const Ray& ray, std::vector<TerrainRaycastHit>* hits) const;

    /** Calculate world space position from cell's location and altitude level */
    D3DXVECTOR3 _getCellWorldSpacePosition(const Point2d& location, int altitudeLevel) const;

private:
    RenderSystem* mRenderSystem;
    ID3DXMesh* mMesh;
    TerrainMaterial* mMaterial;
    Size2d mDimension;
    std::vector<int> mCellsData;
    std::vector<int> mAltitudeLevelData;
    std::vector<QuadTreeNode> mQuadTreeNodes;
};

};

