#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "HexMath.h"


int OffDistCube (OffCoord origin, OffCoord target)
{
	return CubeDist(OffToCube(origin), OffToCube(target));
}

// This is about 8% more efficient than OffDistCube
int OffDistAx (OffCoord origin, OffCoord target)
{
	return AxDist(OffToAx(origin), OffToAx(target));
}

// Determines distance in hexes between two cube coordinates
// Algorithm from www.redblobgames.com/grids/hexagons
int CubeDist (CubeCoord origin, CubeCoord target)
{
	return (abs(origin.x - target.x) +
			abs(origin.y - target.y) +
			abs(origin.z - target.z)) / 2;
}

int AxDist (AxCoord origin, AxCoord target)
{
	int interim_z1 = -origin.x - origin.y;
	int interim_z2 = -target.x - target.y;

	return (abs(origin.x - target.x) +
			abs(origin.y - target.y) +
			abs(interim_z1 - interim_z2)) / 2;
}


// ------------------- Conversions between hex systems --------------------//

// Converts an even-r offset coordinate (x, y) into an axial hex coordinate (x, y)
AxCoord OffToAx (OffCoord off)
{
	AxCoord output;
	output.y = off.y;
	output.x = off.x - (off.y + (off.y & 1)) / 2.0;

	return output;
}

// Converts a cube hex coordinate (x, y, z) into an axial hex coordinate (x, y)
// Algorithm from www.redblobgames.com/grids/hexagons
AxCoord CubeToAx (CubeCoord cube)
{
	AxCoord output;
	output.x = cube.x;
	output.y = cube.z;

	return output;
}

// Converts an axial hex coordinate into a cube coordinate (x, y, z)
CubeCoord AxToCube (AxCoord ax)
{
	CubeCoord output;
	output.x = ax.x;
	output.z = ax.y;
	output.y = -ax.x - ax.y;

	return output;
}

// Converts an even-r offset (x, y) hex coordinate into cube coordinates (x, y, z)
// Algorithm from www.redblobgames.com/grids/hexagons
CubeCoord OffToCube (OffCoord off)
{
	CubeCoord output;
	output.x = off.x - (off.y + (off.y & 1)) / 2.0;
	output.z = off.y;
	output.y = -output.x -output.z;

	return output;
}

// Converts an axial hex coordinate into an even-r offset hex coordinate
OffCoord AxToOff (AxCoord ax)
{
	OffCoord output;
	output.y = ax.y;
	output.x = ax.x + (ax.y + (ax.y & 1)) / 2.0;

	return output;
}

// Converts a cube hex coordinate (x, y, z) into even-r offset (x, y)
// Algorithm from www.redblobgames.com/grids/hexagons
OffCoord CubeToOff (CubeCoord cube)
{
	OffCoord output;
	output.x = cube.x + (cube.z + (cube.z & 1)) / 2.0;
	output.y = cube.z;

	return output;
}

// ------------------- Azimuth calculations --------------------//

// Calculates the normalized azimuth in degrees between two axial coordinates
float AzimuthAx (AxCoord origin, AxCoord target)
{
	float result;
	int deltaX = target.x - origin.x;
	int deltaY = target.y - origin.y;

	// Converts first to an absolute x/y coordinate system
	// Algorithm from www.redblobgames.com/grids/hexagons
	float x = sqrt(3) * (deltaX + (deltaY / 2.0));
	float y = 1.5 * deltaY;


	result = atan2f(x, -y) * (180 / M_PI);
	if (result < 0) result += 360;

	return result;
}

// Calculates the normalized azimuth in degrees between two cubic coordinates
float AzimuthCube (CubeCoord origin, CubeCoord target)
{
	float result;
	int deltaX = target.x - origin.x;
	int deltaY = target.z - origin.z;

	// Converts first to an absolute x/y coordinate system
	// Algorithm from www.redblobgames.com/grids/hexagons
	float x = sqrt(3) * (deltaX + (deltaY / 2.0));
	float y = 1.5 * deltaY;


	result = atan2f(x, -y) * (180 / M_PI);
	if (result < 0) result += 360;

	return result;
}

// Calculates the normalized azimuth in degrees between two offset coordinates
// Uses the even-r offset
float AzimuthOff (OffCoord origin, OffCoord target)
{
	float result;
	int deltaX = target.x - origin.x;
	int deltaY = target.y - origin.y;

	// Converts first to an absolute x/y coordinate system
	// Algorithm from www.redblobgames.com/grids/hexagons
	float x = sqrt(3) * (deltaX - 0.5 * (deltaY & 1));
	float y = 1.5 * deltaY;

	result = atan2f(x, -y) * (180 / M_PI);
	if (result < 0) result += 360;

	return result;
}


// ------------------- Rounding calculations --------------------//

AxCoord RoundAx (float x, float y, float z)
{
	return CubeToAx(RoundCube(x, y, z));
}

// Given a cube coordinate, rounds each component to the nearest integer
// and then makes adjustments to ensure x + y + z = 0.
// Algorithm from www.redblobgames.com/grids/hexagons/
CubeCoord RoundCube (float x, float y, float z)
{
	CubeCoord result;

	result.x = round(x);
	result.y = round(y);
	result.z = round(z);

	float dx = fabs(result.x - x);
	float dy = fabs(result.y - y);
	float dz = fabs(result.z - z);

	if (dx > dy && dx > dz) result.x = -result.y - result.z;
	else if (dy > dz) result.y = -result.x - result.z;
	else result.z = -result.x - result.y;

	return result;
}

OffCoord RoundOff (float x, float y, float z)
{
	return CubeToOff(RoundCube(x, y, z));
}

// ------------------- Finds coordinate at hex --------------------//


// Find coordinate at a given hex
FloatCoord AxHexIs (AxCoord in)
{
	FloatCoord result;

	// Converts first to an absolute x/y coordinate system
	// Algorithm from www.redblobgames.com/grids/hexagons
	result.x = sqrt(3) * (in.x + (in.y / 2.0));
	result.y = 1.5 * in.y;

	return result;
}

FloatCoord CubeHexIs (CubeCoord in)
{
	AxCoord translate;

	translate.x = in.x;
	translate.y = in.z;

	return AxHexIs(translate);
}

FloatCoord OffHexIs (OffCoord in)
{
	FloatCoord result;

	// Converts first to an absolute x/y coordinate system
	// Algorithm from www.redblobgames.com/grids/hexagons
	result.x = sqrt(3) * (in.x - 0.5 * (in.y & 1));
	result.y = 1.5 * in.y;

	return result;
}

// ------------------- Finds hex at calculations --------------------//

float CalcHexAtX (float x, float y)
{
	return (1.0/3.0) * sqrt(3) * x - (1.0/3.0 * y);
}

float CalcHexAtY (float y)
{
	return (2.0/3.0) * y;
}

// Find hex at a given coordinate
AxCoord AxHexAt (float x, float y)
{
	float approxX = CalcHexAtX(x, y);
	float approxZ = CalcHexAtY(y);
	float approxY = -approxX - approxZ;

	return RoundAx(approxX, approxY, approxZ);
}

CubeCoord CubeHexAt (float x, float y)
{
	float approxX = CalcHexAtX(x, y);
	float approxZ = CalcHexAtY(y);
	float approxY = -approxX - approxZ;

	return RoundCube(approxX, approxY, approxZ);
}
OffCoord OffHexAt (float x, float y)
{
	float approxX = CalcHexAtX(x, y);
	float approxZ = CalcHexAtY(y);
	float approxY = -approxX - approxZ;

	return RoundOff(approxX, approxY, approxZ);
}

// ------------------- Finds hex at calculations --------------------//

void HexesBetweenAx (AxCoord origin, AxCoord target)
{
	int i;
	int distance = AxDist(origin, target);

	float dx;
	float dy;
	float progress;

	float coordX;
	float coordY;

	OffCoord result;

	FloatCoord originF = AxHexIs(origin);
	FloatCoord targetF = AxHexIs(target);
	dx = targetF.x - originF.x;
	dy = targetF.y - originF.y;

	// Interpolates at distance + 1 points along the line
	for (i = 0; i <= distance; i++)
	{
		progress = (float)i / (float)distance;

		coordX = (float)originF.x + dx * progress;
		coordY = (float)originF.y + dy * progress;

		result = OffHexAt(coordX, coordY);

		//printf ("Interpolating %d of %d: (%0.2f, %0.2f) is hex (%d, %d)\n",
		//		i, distance, coordX, coordY, result.x, result.y);
	}
}

void HexesBetweenCube (CubeCoord origin, CubeCoord target)
{
	int i;
	int distance = CubeDist(origin, target);

	float dx;
	float dy;
	float progress;

	float coordX;
	float coordY;

	OffCoord result;

	FloatCoord originF = CubeHexIs(origin);
	FloatCoord targetF = CubeHexIs(target);
	dx = targetF.x - originF.x;
	dy = targetF.y - originF.y;

	// Interpolates at distance + 1 points along the line
	for (i = 0; i <= distance; i++)
	{
		progress = (float)i / (float)distance;

		coordX = (float)originF.x + dx * progress;
		coordY = (float)originF.y + dy * progress;

		result = OffHexAt(coordX, coordY);

		//printf ("Interpolating %d of %d: (%0.2f, %0.2f) is hex (%d, %d)\n",
		//		i, distance, coordX, coordY, result.x, result.y);
	}
}

void HexesBetweenOff (OffCoord origin, OffCoord target)
{
	int i;
	int distance = OffDistAx(origin, target);

	float dx;
	float dy;
	float progress;

	float coordX;
	float coordY;

	OffCoord result;

	FloatCoord originF = OffHexIs(origin);
	FloatCoord targetF = OffHexIs(target);
	dx = targetF.x - originF.x;
	dy = targetF.y - originF.y;

	// Interpolates at distance + 1 points along the line
	for (i = 0; i <= distance; i++)
	{
		progress = (float)i / (float)distance;

		coordX = (float)originF.x + dx * progress;
		coordY = (float)originF.y + dy * progress;

		result = OffHexAt(coordX, coordY);

		//printf ("Interpolating %d of %d: (%0.2f, %0.2f) is hex (%d, %d)\n",
		//		i, distance, coordX, coordY, result.x, result.y);
	}

}
