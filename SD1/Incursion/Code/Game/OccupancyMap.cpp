#include "Game/OccupancyMap.hpp"

OccupancyMap::OccupancyMap(int widthInTiles, int heightInTiles)
	{
	initMap(widthInTiles,heightInTiles);
	}

bool OccupancyMap::isTileOccupied(int x, int y)
	{
	int ibyte=calcByteIndexInArray(x,y);
	unsigned char xbitflag = getBitFlag(x);
	return (m_tilebits[ibyte] & xbitflag) != 0;
	}

unsigned char OccupancyMap::getBitFlag(int x)
	{
	return (unsigned char)1 << (x & 0x7);
	}

void OccupancyMap::setTileOccupancy(int x, int y, bool occupancyState)
	{
	int ibyte = calcByteIndexInArray(x, y);
	unsigned char bitflag=getBitFlag(x);
	m_tilebits[ibyte] = m_tilebits[ibyte] & ~bitflag;
	if(occupancyState)
		m_tilebits[ibyte] = m_tilebits[ibyte] | bitflag;
	}

int OccupancyMap::calcByteIndexInArray(int x, int y)
	{
	return y*calcAmtEntriesPerRow()+calcXByteInRow(x);
	}

void OccupancyMap::clearAllTiles()
	{
	for (int i = 0; i < (int)m_tilebits.size(); i++)
		{
		m_tilebits[i]=(unsigned char)0;
		}
	}

int OccupancyMap::calcAmtEntriesPerRow() const
	{
	return ((m_amtTilesX + 7) >> 3);
	}

void OccupancyMap::initMap(int widthInTiles, int heightInTiles)
	{
	m_amtTilesX = widthInTiles;
	m_amtTilesY = heightInTiles;
	int amtArrayEntries=calcAmtArrayEntries();
	for (int i = 0; i < amtArrayEntries ; i++)
		{
		m_tilebits.push_back((unsigned char)0);
		}
	}

int OccupancyMap::calcXByteInRow(int x)
	{
	return (x >> 3);
	}

void OccupancyMap::clearAndResize(int widthInTiles,int heightInTiles)
	{
	m_tilebits.clear();
	initMap(widthInTiles,heightInTiles);
	}

int OccupancyMap::calcAmtArrayEntries()
	{
	return calcAmtEntriesPerRow()*m_amtTilesY;
	}
