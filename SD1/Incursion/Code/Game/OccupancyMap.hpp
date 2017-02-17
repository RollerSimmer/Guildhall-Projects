#pragma once

#include <vector>

//TODO: fix occupancy map so that tanks do not show up on solid tiles

class OccupancyMap
	{
	private:
		std::vector<unsigned char> m_tilebits;
		int m_amtTilesX;
		int m_amtTilesY;
	public:
		OccupancyMap(int widthInTiles,int heightInTiles);
		bool isTileOccupied(int x,int y);
		void setTileOccupancy(int x,int y,bool occupancyState);
		int calcByteIndexInArray(int x,int y);
		void clearAllTiles();
		int calcAmtEntriesPerRow() const;
		void initMap(int widthInTiles,int heightInTiles);
		int calcXByteInRow(int x);
		void clearAndResize(int widthInTiles,int heightInTiles);
	private:
		int calcAmtArrayEntries();
		unsigned char getBitFlag(int x);
	};

