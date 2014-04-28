#ifndef HEXMATH_H_
#define HEXMATH_H_

typedef struct
{
	int x;
	int y;
} OffCoord;

typedef struct
{
	int x;
	int y;
} AxCoord;

typedef struct
{
	int x;
	int y;
	int z;
} CubeCoord;

typedef struct
{
	float x;
	float y;
} FloatCoord;

// Distance calculations
int AxDist (AxCoord origin, AxCoord target);
int OffDistAx (OffCoord origin, OffCoord target);
int OffDistCube (OffCoord origin, OffCoord target);
int CubeDist (CubeCoord origin, CubeCoord target);

// Conversion calculations
AxCoord CubeToAx (CubeCoord cube);
AxCoord OffToAx (OffCoord off);

CubeCoord AxToCube (AxCoord ax);
CubeCoord OffToCube (OffCoord off);

OffCoord AxToOff (AxCoord ax);
OffCoord CubeToOff (CubeCoord cube);



// Azimuth calculations
float AzimuthAx (AxCoord origin, AxCoord target);
float AzimuthCube (CubeCoord origin, CubeCoord target);
float AzimuthOff (OffCoord origin, OffCoord target);

// Hex rounding
AxCoord RoundAx (float x, float y, float z);
CubeCoord RoundCube (float x, float y, float z);
OffCoord RoundOff (float x, float y, float z);

// Find coordinate at a given hex
FloatCoord AxHexIs (AxCoord in);
FloatCoord CubeHexIs (CubeCoord in);
FloatCoord OffHexIs (OffCoord in);

// Find hex at a given coordinate
AxCoord AxHexAt (float x, float y);
CubeCoord CubeHexAt (float x, float y);
OffCoord OffHexAt (float x, float y);

// Hexes along a line between two coordinates
void HexesBetweenAx (AxCoord origin, AxCoord target);
void HexesBetweenCube (CubeCoord origin, CubeCoord target);
void HexesBetweenOff (OffCoord origin, OffCoord target);

#endif /* HEXMATH_H_ */
