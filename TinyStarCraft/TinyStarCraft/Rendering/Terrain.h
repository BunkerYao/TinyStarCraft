#pragma once

#include "Utilities/AABB.h"
#include "Utilities/Rect2.h"
#include "Utilities/Size2.h"

namespace TinyStarCraft
{

class Camera;
class Effect;
class Mesh;
class Material;
class Ray;
class RenderSystem;
class Texture;
class ViewFrustum;


/**
  	A structure contains raycast hit result message.
 */
struct TerrainRaycastHit
{
    float distance;
    D3DXVECTOR3 hitPosition;
    Point2d tileLocation;

    /** Two raycast hit are comparable by distance. */
    bool operator<(const TerrainRaycastHit& other) const
    {
        return distance < other.distance;
    }

    TerrainRaycastHit() = default;

    TerrainRaycastHit(float distance, const D3DXVECTOR3& hitPosition, const Point2d& location)
        : distance(distance), hitPosition(hitPosition), tileLocation(location)
    {}
};


/** 15 Tile types */
enum ETileType
{
    Flat,
    SouthA,
    WestA,
    NorthA,
    EastA,
    SouthWest,
    NorthWest,
    NorthEast,
    SouthEast,
    SouthB,
    WestB,
    NorthB,
    EastB,
    VShapeWestToEast,
    VShapeSouthToNorth
};


/**
  	Properties of a tile.
 */
struct Tile
{
    /** Side length in world unit. */
    static constexpr float SIZE = 45.255f;
    /** Height per-level in world unit */
    static constexpr float HEIGHT_PER_LEVEL = 18.475f;

    ETileType type;
    int level;
    float waterAltitude;
    bool hasWater;

    /** Constructor */
    Tile() 
        : type(ETileType::Flat), level(0), hasWater(false), waterAltitude(0.0f)
    {}

    /** Constrcutor */
    Tile(ETileType type, int level, bool hasWater, float waterAltitude)
        : type(type), level(level), hasWater(hasWater), waterAltitude(waterAltitude)
    {}

    /** Get altitude in world unit */
    float getAltitude() const { return level * HEIGHT_PER_LEVEL; }

    /**
    	Determine if water is visible on this tile.
    @remarks
        Water is visible only if tile has water and water is higher than tile.
     */
    bool isWaterVisible() const { return hasWater && waterAltitude > getAltitude(); }
};


class Terrain
{
public:
    /* Chunk side length measured in tiles number. */
    static const int CHUNK_DIMENSION = 8;
    /* Number of tiles per chunk. */
    static const int TILES_COUNT_PER_CHUNK = CHUNK_DIMENSION * CHUNK_DIMENSION;
    /* Blend texture side length measured in tiles number. */
    static const int BLEND_TEXTURE_DIMENSION = 4;

    static constexpr char EFFECT_RESOURCE_NAME_TERRAIN[12] = "__terrain__";
    static constexpr char EFFECT_RESOURCE_NAME_WATER[10] = "__water__";

private:
    /** Geometry of a tile. */
    struct TileGeometry
    {
        D3DXVECTOR3 vertices[4];
        int indices[6];

        /**
          	Perform a Ray-Triangle intersection test.
        @return
            Returns the number of hits ranges from 0~2.
         */
        int raycast(const D3DXVECTOR3& tilePos, const Ray& ray, float distances[2]) const;
    };

    /** Geometries of 15 type of tiles. */
    static const std::array<TileGeometry, 15>& TILE_GEOMETRIES();

    /** Definition of a quad tree node */
    struct QuadTreeNode
    {
        QuadTreeNode* children[4];
        size_t childrenCount;
        AABB aabb;
        int chunk;  // Will be chunk's index if the node has a chunk, otherwise it will be -1.
        int tile;   // Will be tile's index if the node has a tile, otherwise it will be -1.

        QuadTreeNode()
            : children { 0 }, childrenCount(0), chunk(-1), tile(-1)
        {}
    };

public:
    /** Constructor */
    explicit Terrain(RenderSystem* renderSystem);

    /** Destructor */
    ~Terrain();

    /**
      	Initialize the terrain.
    @return
        If the terrain is initialized, returns true. Otherwise returns false.
     */
    bool initialize(const Size2d& dimension, const std::vector<Tile>& tilesData);

    /**
      	Create the terrain.
     */
    bool initialize(const Size2d& dimension, Tile initialTile = Tile(ETileType::Flat, 0, false, 0.0f));

    /** Get tiles data */
    const std::vector<Tile>& getTilesData() const { return mTilesData; }

    /**
      	Modify terrain's tiles data.
    @remarks
        Dimension of the terrain remains unchanged.
     */
    void setTilesData(const std::vector<Tile>& tilesData);

    /** Get dimension */
    const Size2d& getDimension() const { return mDimension; }

    /**
      	Draw the terrain.
     */
    void drawTerrain(Camera* camera);

    /**
      	Draw the water.
     */
    void drawWater(Camera* camera);

    /**
      	Cast a ray on the terrain and return the nearest hit point.
    @Return
        Returns true if there is a hit. Returns false otherwise.
     */
    bool raycast(const Ray& ray, TerrainRaycastHit* hit) const;

    /**
      	Cast a ray on the terrain and return all hit points sorted from the nearest to the farthest.
     */
    bool raycastAll(const Ray& ray, std::vector<TerrainRaycastHit>* hits) const;

    /**
        Set a blend texture.
    @param index
        A interger range from 0 to 3.
    @param texture
        A texture to be set as blend texture.
    @remarks
        Terrain has maximum of 4 blend texture to be used.
     */
    void setBlendTexture(size_t index, Texture* texture);

    /** Set a control texture */
    void setControlTexture(Texture* texture);

    /**
    	Set a wave texture.
    @param index
        A integer range from 0 to 1.
    @param texture 
        A texture to be set as wave texture.
    @param remarks
        Wave textures are normal maps to render water tiles.
        Terrain has maximum of 2 wave textures to be used.
     */
    void setWaveTexture(size_t index, Texture* texture);

private:
    bool _initializeImpl(const Size2d& dimension, const std::vector<Tile>& tilesData);

    /**	
        Create the terrain mesh
        Texture coordinate for the control texture & blend textures are also generated.
     */
    bool _createTerrainMesh();

    bool _createWaterTileMesh();

    /** Modify the terrain mesh's vertices positions and indices according to tiles data. */
    void _updateTerrainMeshGeometry();

    /** 
        Calculate and update quad tree dimension. 
        Since the dimension of the terrain is fixed after init, so the function just need to be
        called once at initialization.
     */
    void _updateQuadTreeDimension();

    /**	Build quad tree for the terrain. */
    void _buildTerrainQuadTree();

    /** Build quad tree for water. */
    void _buildWaterQuadTree();

    /**	Subdivide the terrain and create quad tree nodes recursively. */
    void _createTerrainQuadTreeNodeRecursively(const Rectd& subSquare, QuadTreeNode* parent);

    /**	Subdivide the water and create quad tree nodes recursively. */
    void _createWaterQuadTreeNodeRecursively(const Rectd& subSquare, QuadTreeNode* parent);

    /**
      	Gather visible chunks of the terrain mesh.
        This will traverse the entire quad tree and do frustum-AABB intersection test
        to determine each chunk's visibility. Visible chunk's id will be gathered to 
        the visible chunks array.
    @param isParentInsideFrustum
        To indicate whether this node's parent is completely inside the camera view frustum.
        If set to true, the Frustum-AABB intersection test will be skipped and the node and all its
        children will be considered as visible nodes since they must be inside the frustum as well.
        Otherwise, a Frustum-AABB intersection test is performed to check whether the node is visible.
     */
    void _gatherVisibleTerrainChunksRecursively(const QuadTreeNode& node, const ViewFrustum& viewFrustum,
        bool isParentInsideFrustum, std::vector<int>* visibleChunks);

    /** Gather visible water tiles */
    void _gatherVisibleWaterTilesRecursively(const QuadTreeNode& node, const ViewFrustum& viewFrustum, 
        bool isParentInsideFrustum, std::vector<int>* visibleWaterTiles);

    /**
        Traverse the quad tree to find ray hit points.
      	Perform AABB-Ray intersection on node's AABB to cull unintersected nodes. Perform Triangle-Ray intersection
        test on tile nodes to compute the hit points and add to the array..
     */
    void _raycastOnQuadTreeRecursively(const QuadTreeNode& node, const Ray& ray, std::vector<TerrainRaycastHit>* hits) const;

    /** Calculate world space position by giving tile's location */
    D3DXVECTOR3 _calcTilePositionFromLocation(const Point2d& location) const
    {
        return D3DXVECTOR3(Tile::SIZE * location.y, 0.0f, Tile::SIZE * location.x);
    }

private:
    RenderSystem* mRenderSystem;

    Mesh* mTerrainMesh;
    Mesh* mWaterTileInstancesMesh;
    Effect* mTerrainEffect;
    Effect* mWaterEffect;

    Size2d mDimension;
    std::vector<Tile> mTilesData;

    Size2d mQuadTreeDimension;
    std::vector<QuadTreeNode> mTerrainQuadTreeNodes;
    std::vector<QuadTreeNode> mWaterQuadTreeNodes;

    std::vector<D3DXVECTOR4> mWaterTileInstancePositions;
    std::vector<D3DXVECTOR4> mWaterTileInstanceTexcoords;

    Texture* mBlendTextures[4];
    Texture* mControlTexture;
    Texture* mWaveTextures[2];
};

};

