#pragma once

#include "Utilities/Point2.h"
#include "Utilities/Size2.h"
#include "Terrain.h"

namespace TinyStarCraft
{

class Terrain;

/**
  	A helper class to modify terrain geometry.
 */
class TerrainModifier
{
public:
    /**
      	Constructor
    @param terrain
        A terrain to be modified. The terrain must be initialized before being modified.
     */
    explicit TerrainModifier(Terrain* terrain);

    /** 
        Increase a tile's altitude level by 1. 
    @remarks
        This operation will perform a elevation logic algorithm which will change neighbor tiles to
        generate continuous elevations.
     */
    void hightenGround(const Point2d& location);

    /**
      	Decrease a tile's altitude level by 1.
    @remarks
        This operation will perform a elevation logic algorithm which will change neighbor tiles to
        generate continuous elevations.
     */
    void lowerGround(const Point2d& location);

    /**
      	Increase one tile's altitude level by 1.
     */
    void hightenTile(const Point2d& location);

    /**
      	Decrease one tile's altitude level by 1.
     */
    void lowerTile(const Point2d& location);

    /**
        Update the terrain to apply modification.
     */
    void updateTerrain();

private:
    /** Transform a location to tile index in one dimensional array. */
    int _toTileIndex(const Point2d& location) const;

    /** Initialize the produce tables. */
    void _initTileProduceTables();

    /**
        Get a produce tile by giving two tile types and their altitude level.
    @param method
        Set to 0 means this is a hightening operation; Set to 1 means this is a lowering operation.
     */
    int _getProducetile(ETileType desiredtile, int desiredLevel, ETileType originalTile, int originalLevel, int method);

    /**
      	Called in hightenGround/lowerGround functions.
        Perform elevation logic which need expansively modification on neighbor tiles.
    @param directionMask 
        Masks to tell the function the expanding directions. Masks are:
        1 - Positive Y, 2 - Negative X, 4 - Negative Y, 8 - Positive X
        Near directions combined togeter makes diagnal direction.
    @param method
        Set to 0 means this is a hightening operation; Set to 1 means this is a lowering operation.
     */
    void _modifyTilesRecursively(const Point2d& location, unsigned char directionMask, ETileType desiredTile, int altitudeLevel,
        int method);

private:
    Terrain* mTerrain;
    
    // Modifier's data.
    // Modifications are stored in these data arrays. These data will be applied
    // to terrain when TerrainModifier::updateTerrain is called.
    std::vector<Tile> mTilesData;

    typedef std::vector<std::vector<int>> ProduceTable;
    // This two tables defines what type of tile to produce when certain two
    // types of tile meet. For hightening and lowering method there are two different
    // rules.
    ProduceTable mHighteningTileProduceTable;
    ProduceTable mLoweringTileProduceTable;
};

}

