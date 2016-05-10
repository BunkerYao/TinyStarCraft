#include "Precompiled.h"
#include "TerrainBuilder.h"
#include "Terrain.h"

namespace TinyStarCraft
{

TerrainBuilder::TerrainBuilder(const Size2d& dimension)
    : mDimension(dimension)
{
    mCellsData.resize(mDimension.x * mDimension.y, 0);
    mAltitudeLevelData.resize(mDimension.x * mDimension.y, 0);

    _initCellProduceTable();
}

void TerrainBuilder::highten(const Point2d& location)
{
    _modifyCellsRecursively(location, 1 | 2 | 4 | 8, Terrain::FLAT, mAltitudeLevelData[_toCellIndex(location)] + 1, 0);
}

void TerrainBuilder::lower(const Point2d& location)
{
    for (size_t i = 0; i < mCellsData.size(); ++i) {
        if (mCellsData[i] != Terrain::FLAT)
            mAltitudeLevelData[i]++;
    }

    _modifyCellsRecursively(location, 1 | 2 | 4 | 8, Terrain::FLAT, mAltitudeLevelData[_toCellIndex(location)] - 1, 1);

    for (size_t i = 0; i < mCellsData.size(); ++i) {
        if (mCellsData[i] != Terrain::FLAT)
            mAltitudeLevelData[i]--;
    }
}

int TerrainBuilder::_toCellIndex(const Point2d& location) const
{
    return location.y * mDimension.x + location.x;
}

void TerrainBuilder::_initCellProduceTable()
{
    mCellProduceTable.resize(15, std::vector<int>(15, -1));

    for (size_t i = 0; i < 15; ++i) {
        mCellProduceTable[0][i] = i;
    }

    for (size_t i = 0; i < 15; ++i) {
        mCellProduceTable[i][i] = i;
    }

    mCellProduceTable[Terrain::SOUTHWEST][Terrain::NORTHWEST] = Terrain::WEST_B;
    mCellProduceTable[Terrain::SOUTHWEST][Terrain::SOUTHEAST] = Terrain::SOUTH_B;
    mCellProduceTable[Terrain::SOUTH_A][Terrain::SOUTHWEST] = Terrain::SOUTHWEST;
    mCellProduceTable[Terrain::WEST_A][Terrain::SOUTHWEST] = Terrain::SOUTHWEST;
    mCellProduceTable[Terrain::NORTHWEST][Terrain::NORTHEAST] = Terrain::NORTH_B;
    mCellProduceTable[Terrain::WEST_A][Terrain::NORTHWEST] = Terrain::NORTHWEST;
    mCellProduceTable[Terrain::NORTH_A][Terrain::NORTHWEST] = Terrain::NORTHWEST;
    mCellProduceTable[Terrain::NORTHEAST][Terrain::SOUTHEAST] = Terrain::EAST_B;
    mCellProduceTable[Terrain::NORTH_A][Terrain::NORTHEAST] = Terrain::NORTHEAST;
    mCellProduceTable[Terrain::EAST_A][Terrain::NORTHEAST] = Terrain::NORTHEAST;
    mCellProduceTable[Terrain::SOUTH_A][Terrain::SOUTHEAST] = Terrain::SOUTHEAST;
    mCellProduceTable[Terrain::EAST_A][Terrain::SOUTHEAST] = Terrain::SOUTHEAST;
    mCellProduceTable[Terrain::SOUTH_A][Terrain::WEST_A] = Terrain::SOUTHWEST;
    mCellProduceTable[Terrain::SOUTH_A][Terrain::NORTH_A] = Terrain::DOUBLE_NORTHTOSOUTH;
    mCellProduceTable[Terrain::SOUTH_A][Terrain::EAST_A] = Terrain::SOUTHEAST;
    mCellProduceTable[Terrain::WEST_A][Terrain::NORTH_A] = Terrain::NORTHWEST;
    mCellProduceTable[Terrain::WEST_A][Terrain::EAST_A] = Terrain::DOUBLE_WESTTOEAST;
    mCellProduceTable[Terrain::NORTH_A][Terrain::EAST_A] = Terrain::NORTHEAST;
    mCellProduceTable[Terrain::SOUTH_A][Terrain::DOUBLE_WESTTOEAST] = Terrain::SOUTH_B;
    mCellProduceTable[Terrain::WEST_A][Terrain::DOUBLE_NORTHTOSOUTH] = Terrain::WEST_B;
    mCellProduceTable[Terrain::NORTH_A][Terrain::DOUBLE_WESTTOEAST] = Terrain::NORTH_B;
    mCellProduceTable[Terrain::EAST_A][Terrain::DOUBLE_NORTHTOSOUTH] = Terrain::EAST_B;
    mCellProduceTable[Terrain::SOUTHWEST][Terrain::SOUTH_B] = Terrain::SOUTH_B;
    mCellProduceTable[Terrain::SOUTHWEST][Terrain::WEST_B] = Terrain::WEST_B;
    mCellProduceTable[Terrain::NORTHWEST][Terrain::NORTH_B] = Terrain::NORTH_B;
    mCellProduceTable[Terrain::NORTHWEST][Terrain::WEST_B] = Terrain::WEST_B;
    mCellProduceTable[Terrain::NORTHEAST][Terrain::NORTH_B] = Terrain::NORTH_B;
    mCellProduceTable[Terrain::NORTHEAST][Terrain::EAST_B] = Terrain::EAST_B;
    mCellProduceTable[Terrain::SOUTHEAST][Terrain::SOUTH_B] = Terrain::SOUTH_B;
    mCellProduceTable[Terrain::SOUTHEAST][Terrain::EAST_B] = Terrain::EAST_B;

    for (size_t i = Terrain::SOUTH_A; i <= Terrain::EAST_A; ++i) {
        for (size_t j = Terrain::SOUTH_B; j <= Terrain::EAST_B; ++j)
            mCellProduceTable[i][j] = j;
    }

    // Reflect the table in diagonal
    for (size_t i = 0; i < 15; ++i) {
        for (size_t j = 0; j < i; ++j) {
            mCellProduceTable[i][j] = mCellProduceTable[j][i];
        }
    }
}

int TerrainBuilder::_getProduceCell(int desiredCell, int desiredLevel, int originalCell, int originalLevel, int method)
{
    if (method == 0) {
        if (desiredLevel > originalLevel)
            return desiredCell;
        else
            return mCellProduceTable[desiredCell][originalCell];
    }
    else if (method == 1) {
        if (desiredLevel < originalLevel) {
            return desiredCell;
        }
        else {
            // Temprary solution
            if (desiredCell >= Terrain::SOUTH_A && desiredCell <= Terrain::EAST_A)
                desiredCell = Terrain::SOUTH_B + desiredCell - Terrain::SOUTH_A;
            else if (desiredCell >= Terrain::SOUTH_B && desiredCell <= Terrain::EAST_B)
                desiredCell = Terrain::SOUTH_A + desiredCell - Terrain::SOUTH_B;

            if (originalCell >= Terrain::SOUTH_A && originalCell <= Terrain::EAST_A)
                originalCell = Terrain::SOUTH_B + originalCell - Terrain::SOUTH_A;
            else if (originalCell >= Terrain::SOUTH_B && originalCell <= Terrain::EAST_B)
                originalCell = Terrain::SOUTH_A + originalCell - Terrain::SOUTH_B;

            int produceCell = mCellProduceTable[desiredCell][originalCell];

            if (produceCell >= Terrain::SOUTH_A && produceCell <= Terrain::EAST_A)
                produceCell = Terrain::SOUTH_B + produceCell - Terrain::SOUTH_A;
            else if (produceCell >= Terrain::SOUTH_B && produceCell <= Terrain::EAST_B)
                produceCell = Terrain::SOUTH_A + produceCell - Terrain::SOUTH_B;

            return produceCell;
        }
    }
    
    // Should never get here.
    return 0;
}

void TerrainBuilder::_modifyCellsRecursively(const Point2d& location, unsigned char extendDirections, int desiredCell, int altitudeLevel, 
    int method)
{
    const int cellIndex = _toCellIndex(location);

    if (method == 0) {
        if (altitudeLevel < mAltitudeLevelData[cellIndex])
            return;
    }
    else if (method == 1) {
        if (altitudeLevel > mAltitudeLevelData[cellIndex])
            return;
    }

    int produceCell = _getProduceCell(desiredCell, altitudeLevel, mCellsData[cellIndex], mAltitudeLevelData[cellIndex], method);
    if (produceCell == -1) {
        produceCell = 0;
        extendDirections = 1 | 2 | 4 | 8;
        altitudeLevel = (method == 0) ? altitudeLevel + 1 : altitudeLevel - 1;
    }

    mCellsData[cellIndex] = produceCell;
    mAltitudeLevelData[cellIndex] = altitudeLevel;

    if (method == 0)
        altitudeLevel--;
    else if (method == 1) 
        altitudeLevel++;

    static const int directions[] = { 1, 2, 4, 8, 8 | 1, 1 | 2, 2 | 4, 4 | 8 };
    static const Point2d locationDeltas[] = { {0, 1}, {-1, 0}, {0, -1}, {1, 0}, {1, 1}, {-1, 1}, {-1, -1}, {1, -1} };
    static int desiredCells[8];
    if (method == 0) {
        desiredCells[0] = Terrain::SOUTHWEST;
        desiredCells[1] = Terrain::NORTHWEST;
        desiredCells[2] = Terrain::NORTHEAST;
        desiredCells[3] = Terrain::SOUTHEAST;
        desiredCells[4] = Terrain::SOUTH_A;
        desiredCells[5] = Terrain::WEST_A;
        desiredCells[6] = Terrain::NORTH_A;
        desiredCells[7] = Terrain::EAST_A;
    }
    else if (method == 1) {
        desiredCells[0] = Terrain::NORTHEAST;
        desiredCells[1] = Terrain::SOUTHEAST;
        desiredCells[2] = Terrain::SOUTHWEST;
        desiredCells[3] = Terrain::NORTHWEST;
        desiredCells[4] = Terrain::NORTH_B;
        desiredCells[5] = Terrain::EAST_B;
        desiredCells[6] = Terrain::SOUTH_B;
        desiredCells[7] = Terrain::WEST_B;
    }

    unsigned char flags = 0;
    Rectd terrainRect = Rectd::makeRect(Point2d::ZERO(), mDimension);
    for (int i = 0; i < 4; ++i) {
        Point2d nextLocation = location + locationDeltas[i];
        if ((extendDirections & directions[i]) && terrainRect.isPointInside(nextLocation)) {
            _modifyCellsRecursively(nextLocation, directions[i], desiredCells[i], altitudeLevel, method);
            flags |= directions[i];
        }
    }

    for (int i = 4; i < 8; ++i) {
        if ((directions[i] & flags) == directions[i]) {
            Point2d nextLocation = location + locationDeltas[i];
            _modifyCellsRecursively(nextLocation, directions[i], desiredCells[i], altitudeLevel, method);
        }
    }
}

}
