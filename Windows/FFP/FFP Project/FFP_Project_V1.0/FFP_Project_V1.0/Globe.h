///////////////////////////////////////////////////////////////////////////////
//Globe.h
///////////////////////////////////////////////////////////////////////////////
#ifndef GEOMETRY_GLOBE_H
#define GEOMETRY_GLOBE_H
#include <vector>
#include"SphereOGL.h"
class Globe : public Sphere
{
public:
	// ctor/dtor
	Globe(float radius = 1.0f, int sectorCount = 36, int stackCount = 18);
	~Globe() {}
	// getters/setters
	void Set(float radius, int sectorCount, int stackCount);
	void DrawGlobe() const;
	void DrawAirport(float latitudes, float longitudes);
	void DrawAirport(float latitudes, float longitudes,float* vx,float *vy,float *vz);
	void DrawRoute(float latitude1, float longitude1, float latitude2, float longitude2);
	void GetAirportDetails(float latitude, float longitude, float *Ax, float *Ay, float *Az);
protected:
private:
	float latFactor;
	float longFactor;
};
#endif
