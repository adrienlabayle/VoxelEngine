#pragma once

#include "Chunk.h"

#include <vector>

class TreeMaker
{
public:
	TreeMaker();

	void MakeTree(std::vector<unsigned short>& blocks, int x, int y, int z, int treeType, int hash);

private:
	int DeriveHash(int h, int salt) const;
	float RandomFloat(int h) const;

	void PlaceBranch(std::vector<unsigned short>& blocks, int x, int y, int z, int height, int hash);
	void PlaceLeaves(std::vector<unsigned short>& blocks, int x, int y, int z, int height, int hash);
	void PlaceSplit(std::vector<unsigned short>& blocks, int x, int y, int z, int segmentLength, int depth, int hash);
};