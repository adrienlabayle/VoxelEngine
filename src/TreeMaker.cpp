#include "TreeMaker.h"

TreeMaker::TreeMaker()
{

}

int TreeMaker::DeriveHash(int h, int salt) const
{
	h ^= salt;
	h *= 0x9e3779b9;
	h ^= (h >> 16);
	h *= 0x85ebca6b;
	h ^= (h >> 13);
	h *= 0xc2b2ae35;
	h ^= (h >> 16);
	return h;
}

float TreeMaker::RandomFloat(int h) const  // Between 0 and 1
{
	return (h & 0xFFFF) / (float)0xFFFF;
}

void TreeMaker::MakeTree(std::vector<unsigned short>& blocks, int x, int y, int z, int treeType, int hash)
{
	switch (treeType)
	{
	case 0:
	{
		// We place the trunk
		int height = ((hash & 3) + 1) * 2;
		if (x >= 0 && x < Chunk::m_XSize && z >= 0 && z < Chunk::m_ZSize)
		{
			for (int dy = y; dy < y + height; dy++)
				blocks[x + Chunk::m_XSize * (dy + Chunk::m_YSize * z)] = 11;  // Trunk
		}

		// We place the leaves
		PlaceLeaves(blocks, x, y + height, z, height, hash >> 2);
		break;
	}
	case 1:
	{
		// We place the trunk
		int height = ((hash & 3) + 1) * 2 + 5;
		if (x >= 0 && x < Chunk::m_XSize && z >= 0 && z < Chunk::m_ZSize)
		{
			for (int dy = y; dy < y + height; dy++)
				blocks[x + Chunk::m_XSize * (dy + Chunk::m_YSize * z)] = 11;  // Trunk
		}
		if ((x + 1) >= 0 && (x + 1) < Chunk::m_XSize && (z + 1) >= 0 && (z + 1) < Chunk::m_ZSize)
		{
			for (int dy = y; dy < y + height; dy++)
				blocks[x + 1 + Chunk::m_XSize * (dy + Chunk::m_YSize * (z + 1))] = 11;  // Trunk
		}
		if ((x + 1) >= 0 && (x + 1) < Chunk::m_XSize && z >= 0 && z < Chunk::m_ZSize)
		{
			for (int dy = y; dy < y + height; dy++)
				blocks[x + 1 + Chunk::m_XSize * (dy + Chunk::m_YSize * z)] = 11;  // Trunk
		}
		if (x >= 0 && x < Chunk::m_XSize && (z + 1) >= 0 && (z + 1) < Chunk::m_ZSize)
		{
			for (int dy = y; dy < y + height; dy++)
				blocks[x + Chunk::m_XSize * (dy + Chunk::m_YSize * (z + 1))] = 11;  // Trunk
		}

		// We place the branches with their leaves
		//int branchPosY = y + height - (((hash >> 2) & 3) + 1);
		int branchPosY = y + (2 * height) / 3;
		PlaceBranch(blocks, x, branchPosY, z, height, hash >> 4);

		// We place the top leaves
		PlaceLeaves(blocks, x, y + height, z, height, hash >> 4);
		break;
	}
	case 2:
	{
		int height = ((hash & 3) + 1) * 3 + 8;

		// We place the trunk which is 2/3 of the total height
		int trunkHeight = (height) / 3;
		for (int dy = y; dy <= y + trunkHeight; dy++)
		{
			if (x >= 0 && x < Chunk::m_XSize && z >= 0 && z < Chunk::m_ZSize)
				blocks[x + Chunk::m_XSize * (dy + Chunk::m_YSize * z)] = 11;
			if ((x + 1) >= 0 && (x + 1) < Chunk::m_XSize && z >= 0 && z < Chunk::m_ZSize)
				blocks[x + 1 + Chunk::m_XSize * (dy + Chunk::m_YSize * z)] = 11;
		}

		// Recursive splitting
		int splitHash = DeriveHash(hash, 224737387);
		PlaceSplit(blocks, x, y + trunkHeight, z, height / 5, 5, splitHash);
		break;
	}
	case 3:
		break;
	default:
		break;
	}
}

void TreeMaker::PlaceBranch(std::vector<unsigned short>& blocks, int x, int y, int z, int height, int hash)
{
	int Xdirection = (hash & 1) * 2 - 1;
	int Ydirection = (hash >> 1) & 1;
	int Zdirection = ((hash >> 2) & 1) * 2 - 1;
	int dx = x;
	int dy = y;
	int dz = z;
	int offsetY = 0;
	for (int i = 0; i < height / 3; i++)
	{
		dx += Xdirection;
		dz += Zdirection;

		offsetY += Ydirection;
		dy += offsetY / 3;

		if (dx >= 0 && dx < Chunk::m_XSize && dz >= 0 && dz < Chunk::m_ZSize && dy >= 0 && dy < Chunk::m_YSize)
			blocks[dx + Chunk::m_XSize * (dy + Chunk::m_YSize * dz)] = 11;
	}
	PlaceLeaves(blocks, dx, dy, dz, std::max(1, height / 2), hash >> 3);
}

void TreeMaker::PlaceLeaves(std::vector<unsigned short>& blocks, int x, int y, int z, int height, int hash)
{
	float baseRadius = std::max(2.0f, height / 2.5f);
	int h1 = DeriveHash(hash, 179424673);
	float rx = baseRadius * (0.85f + ((h1) & 0xFF) / 255.0f * 0.3f);
	float ry = baseRadius * (0.70f + ((h1 >> 8) & 0xFF) / 255.0f * 0.3f);
	float rz = baseRadius * (0.85f + ((h1 >> 16) & 0xFF) / 255.0f * 0.3f);

	int leavesCenterX = x; // no offset cause the trunk is straight, so the top x and z are the same as the bottom one
	int leavesCenterY = y;
	int leavesCenterZ = z;

	int rCeil = (int)std::ceil(std::max({ rx, ry, rz }));  // Take the higher radius of the 3 to define the size of the 'cube' we gonna scan

	float holeFactor = 0.3f + ((h1 >> 24) & 0xFF) / 255.0f * 0.3f;

	for (int dy = -rCeil; dy <= rCeil; dy++)
	{
		for (int dz = -rCeil; dz <= rCeil; dz++)
			for (int dx = -rCeil; dx <= rCeil; dx++)
			{
				float dist = (float)(dx * dx) / (rx * rx) + (float)(dy * dy) / (ry * ry) + (float)(dz * dz) / (rz * rz);
				if (dist > 1.0f) continue; // Check if the current blocks is in the area of the 'sphere'

				// Natural holes on the surface of the 'sphere'
				int leafHash = h1 ^ (dx * 29989 + dy * 30497 + dz * 31337);
				if (RandomFloat(leafHash) > 1.0f - dist * holeFactor) continue;

				int bx = leavesCenterX + dx;
				int by = leavesCenterY + dy;
				int bz = leavesCenterZ + dz;
				int pos = bx + Chunk::m_XSize * (by + Chunk::m_YSize * bz);
				if (bx >= 0 && bx < Chunk::m_XSize && bz >= 0 && bz < Chunk::m_ZSize && by >= 0 && by < Chunk::m_YSize && blocks[pos] == 0)
					blocks[pos] = 12;  // Leaves
			}
	}
}

void TreeMaker::PlaceSplit(std::vector<unsigned short>& blocks, int x, int y, int z, int segmentLength, int depth, int hash)
{
    if (depth == 0 || segmentLength < 2)
    {
        PlaceLeaves(blocks, x, y, z, segmentLength * 2, hash);
        return;
    }

    // Direction
    int dx = ((hash & 1) * 2 - 1);          // -1 or +1 in X
    int dz = (((hash >> 1) & 1) * 2 - 1);   // -1 or +1 in Z
    int dyStep = std::max(1, segmentLength / 3);  // mount 1 bloc every iteration of dyStep

    int cx = x, cy = y, cz = z;
    for (int i = 0; i < segmentLength; i++)
    {
		if (i % dyStep == 0)
		{
			cy++;
			if (cx >= 0 && cx < Chunk::m_XSize && cz >= 0 && cz < Chunk::m_ZSize && cy >= 0 && cy < Chunk::m_YSize)
				blocks[cx + Chunk::m_XSize * (cy + Chunk::m_YSize * cz)] = 11;
		}

		cx += dx;
		cz += dz;

        if (cx >= 0 && cx < Chunk::m_XSize && cz >= 0 && cz < Chunk::m_ZSize && cy >= 0 && cy < Chunk::m_YSize)
            blocks[cx + Chunk::m_XSize * (cy + Chunk::m_YSize * cz)] = 11;
    }

    // Split into 2 daughter branches from the end of the segment
    int hash1 = DeriveHash(hash, 104729);
    int hash2 = DeriveHash(hash, 224737);
    PlaceSplit(blocks, cx, cy, cz, segmentLength - 1, depth - 1, hash1);
    PlaceSplit(blocks, cx, cy, cz, segmentLength - 1, depth - 1, hash2);
}
