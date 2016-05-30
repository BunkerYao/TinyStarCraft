#include "Precompiled.h"
#include "TerrainModifier.h"

namespace TinyStarCraft
{

TerrainModifier::TerrainModifier(Terrain* terrain)
    : mTerrain(terrain)
{
    _initTileProduceTables();
    // Get data from the terrain.
    mTilesData = mTerrain->getTilesData();
}

void TerrainModifier::hightenGround(const Point2d& location)
{
    _modifyTilesRecursively(location, 1 | 2 | 4 | 8, ETileType::Flat, mTilesData[_toTileIndex(location)].level + 1, 0);
}

void TerrainModifier::lowerGround(const Point2d& location)
{
    // Increase the altitude level of non-flat tiles by 1.
    for (size_t i = 0; i < mTilesData.size(); ++i) {
        if (mTilesData[i].type != ETileType::Flat)
            mTilesData[i].level++;
    }

    _modifyTilesRecursively(location, 1 | 2 | 4 | 8, ETileType::Flat, mTilesData[_toTileIndex(location)].level - 1, 1);

    // Decrease the altitude level of non-flat tiles by 1.
    for (size_t i = 0; i < mTilesData.size(); ++i) {
        if (mTilesData[i].type != ETileType::Flat)
            mTilesData[i].level--;
    }
}

void TerrainModifier::hightenTile(const Point2d& location)
{
    mTilesData[_toTileIndex(location)].level++;
}

void TerrainModifier::lowerTile(const Point2d& location)
{
    mTilesData[_toTileIndex(location)].level--;
}

void TerrainModifier::updateTerrain()
{
    mTerrain->setTilesData(mTilesData);
}

void TerrainModifier::_initTileProduceTables()
{
    // Initialize all produce tiles to undefined.
    mHighteningTileProduceTable.resize(16, std::vector<int>(16, -1));

    // Flat types can be overriden by any other types.
    for (size_t i = 0; i < 16; ++i) {
        mHighteningTileProduceTable[ETileType::Flat][i] = i;
    }

    // When a type meets itself produces itself.
    for (size_t i = 0; i < 16; ++i) {
        mHighteningTileProduceTable[i][i] = i;
    }

    mHighteningTileProduceTable[ETileType::SouthWest][ETileType::NorthWest] = ETileType::WestB;
    mHighteningTileProduceTable[ETileType::SouthWest][ETileType::SouthEast] = ETileType::SouthB;
    mHighteningTileProduceTable[ETileType::SouthA][ETileType::SouthWest] = ETileType::SouthWest;
    mHighteningTileProduceTable[ETileType::WestA][ETileType::SouthWest] = ETileType::SouthWest;
    mHighteningTileProduceTable[ETileType::NorthWest][ETileType::NorthEast] = ETileType::NorthB;
    mHighteningTileProduceTable[ETileType::WestA][ETileType::NorthWest] = ETileType::NorthWest;
    mHighteningTileProduceTable[ETileType::NorthA][ETileType::NorthWest] = ETileType::NorthWest;
    mHighteningTileProduceTable[ETileType::NorthEast][ETileType::SouthEast] = ETileType::EastB;
    mHighteningTileProduceTable[ETileType::NorthA][ETileType::NorthEast] = ETileType::NorthEast;
    mHighteningTileProduceTable[ETileType::EastA][ETileType::NorthEast] = ETileType::NorthEast;
    mHighteningTileProduceTable[ETileType::SouthA][ETileType::SouthEast] = ETileType::SouthEast;
    mHighteningTileProduceTable[ETileType::EastA][ETileType::SouthEast] = ETileType::SouthEast;
    mHighteningTileProduceTable[ETileType::SouthA][ETileType::WestA] = ETileType::SouthWest;
    mHighteningTileProduceTable[ETileType::SouthA][ETileType::NorthA] = ETileType::VShapeSouthToNorth;
    mHighteningTileProduceTable[ETileType::SouthA][ETileType::EastA] = ETileType::SouthEast;
    mHighteningTileProduceTable[ETileType::WestA][ETileType::NorthA] = ETileType::NorthWest;
    mHighteningTileProduceTable[ETileType::WestA][ETileType::EastA] = ETileType::VShapeWestToEast;
    mHighteningTileProduceTable[ETileType::NorthA][ETileType::EastA] = ETileType::NorthEast;
    mHighteningTileProduceTable[ETileType::SouthA][ETileType::VShapeWestToEast] = ETileType::SouthB;
    mHighteningTileProduceTable[ETileType::WestA][ETileType::VShapeSouthToNorth] = ETileType::WestB;
    mHighteningTileProduceTable[ETileType::NorthA][ETileType::VShapeWestToEast] = ETileType::NorthB;
    mHighteningTileProduceTable[ETileType::EastA][ETileType::VShapeSouthToNorth] = ETileType::EastB;
    mHighteningTileProduceTable[ETileType::SouthWest][ETileType::SouthB] = ETileType::SouthB;
    mHighteningTileProduceTable[ETileType::SouthWest][ETileType::WestB] = ETileType::WestB;
    mHighteningTileProduceTable[ETileType::NorthWest][ETileType::NorthB] = ETileType::NorthB;
    mHighteningTileProduceTable[ETileType::NorthWest][ETileType::WestB] = ETileType::WestB;
    mHighteningTileProduceTable[ETileType::NorthEast][ETileType::NorthB] = ETileType::NorthB;
    mHighteningTileProduceTable[ETileType::NorthEast][ETileType::EastB] = ETileType::EastB;
    mHighteningTileProduceTable[ETileType::SouthEast][ETileType::SouthB] = ETileType::SouthB;
    mHighteningTileProduceTable[ETileType::SouthEast][ETileType::EastB] = ETileType::EastB;

    // All *_A types can get overriden by any one of by *_B types.
    for (size_t i = ETileType::SouthA; i <= ETileType::EastA; ++i) {
        for (size_t j = ETileType::SouthB; j <= ETileType::EastB; ++j)
            mHighteningTileProduceTable[i][j] = j;
    }

    // Make the table diagnally symmetrical
    for (size_t i = 0; i < 16; ++i) {
        for (size_t j = 0; j < i; ++j) {
            mHighteningTileProduceTable[i][j] = mHighteningTileProduceTable[j][i];
        }
    }

    mLoweringTileProduceTable.resize(16, std::vector<int>(16, -1));

    // Flat types can be overriden by any other types.
    for (size_t i = 0; i < 16; ++i) {
        mLoweringTileProduceTable[ETileType::Flat][i] = i;
    }

    // When a type meets itself produces itself.
    for (size_t i = 0; i < 16; ++i) {
        mLoweringTileProduceTable[i][i] = i;
    }

    mLoweringTileProduceTable[ETileType::SouthWest][ETileType::NorthWest] = ETileType::WestA;
    mLoweringTileProduceTable[ETileType::SouthWest][ETileType::SouthEast] = ETileType::SouthA;
    mLoweringTileProduceTable[ETileType::SouthWest][ETileType::SouthB] = ETileType::SouthWest;
    mLoweringTileProduceTable[ETileType::SouthWest][ETileType::WestB] = ETileType::SouthWest;
    mLoweringTileProduceTable[ETileType::NorthWest][ETileType::NorthEast] = ETileType::NorthA;
    mLoweringTileProduceTable[ETileType::NorthWest][ETileType::WestB] = ETileType::NorthWest;
    mLoweringTileProduceTable[ETileType::NorthWest][ETileType::NorthB] = ETileType::NorthWest;
    mLoweringTileProduceTable[ETileType::NorthEast][ETileType::SouthEast] = ETileType::EastA;
    mLoweringTileProduceTable[ETileType::NorthEast][ETileType::NorthB] = ETileType::NorthEast;
    mLoweringTileProduceTable[ETileType::NorthEast][ETileType::EastB] = ETileType::NorthEast;
    mLoweringTileProduceTable[ETileType::SouthEast][ETileType::SouthB] = ETileType::SouthEast;
    mLoweringTileProduceTable[ETileType::SouthEast][ETileType::EastB] = ETileType::SouthEast;
    mLoweringTileProduceTable[ETileType::SouthB][ETileType::WestB] = ETileType::SouthWest;
    mLoweringTileProduceTable[ETileType::SouthB][ETileType::EastB] = ETileType::SouthEast;
    mLoweringTileProduceTable[ETileType::WestB][ETileType::NorthB] = ETileType::NorthWest;
    mLoweringTileProduceTable[ETileType::NorthB][ETileType::EastB] = ETileType::NorthEast;
    mLoweringTileProduceTable[ETileType::SouthB][ETileType::VShapeWestToEast] = ETileType::SouthA;
    mLoweringTileProduceTable[ETileType::WestB][ETileType::VShapeSouthToNorth] = ETileType::WestA;
    mLoweringTileProduceTable[ETileType::NorthB][ETileType::VShapeWestToEast] = ETileType::NorthA;
    mLoweringTileProduceTable[ETileType::EastB][ETileType::VShapeSouthToNorth] = ETileType::EastA;
    mLoweringTileProduceTable[ETileType::SouthA][ETileType::SouthWest] = ETileType::SouthA;
    mLoweringTileProduceTable[ETileType::WestA][ETileType::SouthWest] = ETileType::WestA;
    mLoweringTileProduceTable[ETileType::NorthA][ETileType::NorthWest] = ETileType::NorthA;
    mLoweringTileProduceTable[ETileType::WestA][ETileType::NorthWest] = ETileType::WestA;
    mLoweringTileProduceTable[ETileType::NorthA][ETileType::NorthEast] = ETileType::NorthA;
    mLoweringTileProduceTable[ETileType::EastA][ETileType::NorthEast] = ETileType::EastA;
    mLoweringTileProduceTable[ETileType::SouthA][ETileType::SouthEast] = ETileType::SouthA;
    mLoweringTileProduceTable[ETileType::EastA][ETileType::SouthEast] = ETileType::EastA;

    // All *_B types can get overriden by any one of by *_A types.
    for (size_t i = ETileType::SouthA; i <= ETileType::EastA; ++i) {
        for (size_t j = ETileType::SouthB; j <= ETileType::EastB; ++j)
            mLoweringTileProduceTable[i][j] = i;
    }

    // Make the table diagnally symmetrical
    for (size_t i = 0; i < 16; ++i) {
        for (size_t j = 0; j < i; ++j) {
            mLoweringTileProduceTable[i][j] = mLoweringTileProduceTable[j][i];
        }
    }
}

int TerrainModifier::_toTileIndex(const Point2d& location) const
{
    return location.y * mTerrain->getDimension().x + location.x;
}

int TerrainModifier::_getProducetile(ETileType desiredtile, int desiredLevel, ETileType originalTile, int originalLevel,
    int method)
{
    if (method == 0) {
        if (desiredLevel > originalLevel)
            // Higher tile override lower tile.
            return desiredtile;
        else
            return mHighteningTileProduceTable[desiredtile][originalTile];
    }
    else if (method == 1) {
        if (desiredLevel < originalLevel) 
            // Lower tile overrides higher tile.
            return desiredtile;
        else 
            return mLoweringTileProduceTable[desiredtile][originalTile];
    }
    
    // Should never get here.
    return 0;
}

void TerrainModifier::_modifyTilesRecursively(const Point2d& location, unsigned char extendDirections, ETileType desiredTile, 
    int altitudeLevel, int method)
{
    const int tileIndex = _toTileIndex(location);

    if (method == 0) {
        // If we are hightening the ground, stop if altitude is lower than the original altitude in
        // this tile.
        if (altitudeLevel < mTilesData[tileIndex].level)
            return;
    }
    else if (method == 1) {
        // If we are lowering the ground, stop if altitude is higher than the original altitude
        // in this tile.
        if (altitudeLevel > mTilesData[tileIndex].level)
            return;
    }

    int produceTile = _getProducetile(desiredTile, altitudeLevel, mTilesData[tileIndex].type, mTilesData[tileIndex].level, method);
    if (produceTile == -1) {
        // If the produce of this two tile types is undefined, then highten/lower this tile as well.
        produceTile = 0;
        extendDirections = 1 | 2 | 4 | 8;
        altitudeLevel = (method == 0) ? altitudeLevel + 1 : altitudeLevel - 1;
    }

    // Update the tiles data and altitude level data.
    mTilesData[tileIndex].type = (ETileType)produceTile;
    mTilesData[tileIndex].level = altitudeLevel;

    if (method == 0)
        // Neighbor altitude decrease in highten method.
        altitudeLevel--;
    else if (method == 1) 
        // Neighbor altitude increase in lower method.
        altitudeLevel++;

    // Mask values for 8 directions.
    static const int directions[] = { 1, 2, 4, 8, 8 | 1, 1 | 2, 2 | 4, 4 | 8 };
    // Location deltas for 8 directions.
    static const Point2d locationDeltas[] = { {0, 1}, {-1, 0}, {0, -1}, {1, 0}, {1, 1}, {-1, 1}, {-1, -1}, {1, -1} };
    // Neighbor tiles for hightening and lowering method.
    static ETileType neighborTiles[2][8] =
    {
        // Hightening method
        {
            ETileType::SouthWest,
            ETileType::NorthWest,
            ETileType::NorthEast,
            ETileType::SouthEast,
            ETileType::SouthA,
            ETileType::WestA,
            ETileType::NorthA,
            ETileType::EastA
        },
        // Lowering method
        {
            ETileType::NorthEast,
            ETileType::SouthEast,
            ETileType::SouthWest,
            ETileType::NorthWest,
            ETileType::NorthB,
            ETileType::EastB,
            ETileType::SouthB,
            ETileType::WestB
        }
    };
    
    // Recursively modify neibghbor tiles at direction 1, 2, 4, 8
    unsigned char flags = 0;
    Rectd terrainRect = Rectd::makeRect(Point2d::ZERO(), mTerrain->getDimension());
    for (int i = 0; i < 4; ++i) {
        Point2d nextLocation = location + locationDeltas[i];
        if ((extendDirections & directions[i]) && terrainRect.isPointInside(nextLocation)) {
            _modifyTilesRecursively(nextLocation, directions[i], neighborTiles[method][i], altitudeLevel, method);
            flags |= directions[i];
        }
    }

    // Recursively modify neibghbor tiles at corner directions.
    for (int i = 4; i < 8; ++i) {
        if ((directions[i] & flags) == directions[i]) {
            Point2d nextLocation = location + locationDeltas[i];
            _modifyTilesRecursively(nextLocation, directions[i], neighborTiles[method][i], altitudeLevel, method);
        }
    }
}

}
