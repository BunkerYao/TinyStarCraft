#pragma once

#include "Utilities/Point2.h"
#include "Utilities/Size2.h"

namespace TinyStarCraft
{

/**
 *	Terrain builder is a helper class to generate cells data and altitude level data
 *  used to create a terrain.
 */
class TerrainBuilder
{
public:
    /**
     *	Constructor
     *  
     *  @param dimension
     *  Dimension of the terrain to create.
     */
    explicit TerrainBuilder(const Size2d& dimension);

    /** 
     *  Increase a cell's altitude level by 1. 
     *  
     *  @remarks
     *  This operation will perform a elevation logic algorithm which will change neighbor cells to
     *  generate continuous elevations.
     */
    void highten(const Point2d& location);

    /**
     *	Decrease a cell's altitude level by 1.
     *
     *  @remarks
     *  This operation will perform a elevation logic algorithm which will change neighbor cells to
     *  generate continuous elevations.
     */
    void lower(const Point2d& location);

    /** Get terrain dimension. */
    const Size2d& getDimension() const { return mDimension; }

    /** Get cells data. */
    const std::vector<int>& getCellsData() const { return mCellsData; }

    /** Get altitude level data. */
    const std::vector<int>& getAltitudeLevelData() const { return mAltitudeLevelData; }

private:
    /** Transform a location to cell index. */
    int _toCellIndex(const Point2d& location) const;

    /** Initialize the decision table. */
    void _initCellProduceTable();

    /**
     *  Get a produce tile from given two cell types and their altitude level.
     *
     *  @param method
     *  Set to 0 means this is a highten operation; Set to 1 means this is a lower operation.
     */
    int _getProduceCell(int desiredCell, int desiredLevel, int originalCell, int originalLevel, int method);


    void _modifyCellsRecursively(const Point2d& location, unsigned char extendDirections, int desiredCell, int altitudeLevel, int method);

private:
    Size2d mDimension;
    std::vector<int> mCellsData;
    std::vector<int> mAltitudeLevelData;

    typedef std::vector<std::vector<int>> ProduceTable;
    // This table defines rules that decide what type of cell to produce when certain two
    // types of cell meet.
    ProduceTable mCellProduceTable;
};

}

