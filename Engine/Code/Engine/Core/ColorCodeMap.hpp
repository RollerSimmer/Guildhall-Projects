#pragma once

#include "Engine/Renderer/Rgba.hpp"
#include <vector>

const int COLOR_CODE_MAP_BAD_INDEX=-1;

template<class T>
class ColorCodeMap
	{
	private: 
		std::vector<Rgba> m_colorKeys;
		std::vector<T> m_dataEntries;
	public:

		ColorCodeMap<T>()
			: m_colorKeys()
			, m_dataEntries()
			{

			}

		void add(const Rgba& colorkey,const T& dataentry)
			{
			if (!doesMapContainColorKey(colorkey))
				{
				m_colorKeys.push_back(colorkey);
				m_dataEntries.push_back(dataentry);
				}
			}

		T* lookupKey(const Rgba& colorKey)
			{
			int ikey=getIndexOfClosestColor(colorKey);
			if(ikey==COLOR_CODE_MAP_BAD_INDEX || ikey>(int)m_dataEntries.size())
				return nullptr;
			else
				return &m_dataEntries[ikey];
			}

		bool doesMapContainColorKey(const Rgba& colorkey)
			{
			bool result=getDataIndexByKey(colorkey)!=COLOR_CODE_MAP_BAD_INDEX;
			return result;
			}

		int getDataIndexByKey(const Rgba& searchColorKey)
			{
			int result=COLOR_CODE_MAP_BAD_INDEX;
			for (int ikey = 0; ikey < (int)m_colorKeys.size() ; ikey++)
				{
				Rgba existingColorKey=m_colorKeys[ikey];
				if (existingColorKey==searchColorKey)
					{
					result=ikey;
					break;
					}
				}
			return result;
			}

		int getIndexOfClosestColor(const Rgba& color)
			{
			const int INITIAL_LOW_COLOR_DISTANCE=255*4;
			int lowdist=INITIAL_LOW_COLOR_DISTANCE;
			int indexOfClosestColor=COLOR_CODE_MAP_BAD_INDEX;
			for (int ikey = 0; ikey < (int)m_colorKeys.size() ; ikey++)
				{
				Rgba& curkey=m_colorKeys[ikey];
				int curdist=color.calcTaxiDistFrom(curkey);
				if (curdist<lowdist)
					{
					lowdist=curdist;
					indexOfClosestColor=ikey;
					}
				}
			int result=indexOfClosestColor;
			return result;
			}

		bool isEmpty()
			{
			bool result = m_colorKeys.empty() || m_dataEntries.empty();
			return result;
			}

		void clear()
			{
			if(m_colorKeys.size()>0)
				m_colorKeys.clear();
			if(m_dataEntries.size()>0)
				m_dataEntries.clear();
			}
	};