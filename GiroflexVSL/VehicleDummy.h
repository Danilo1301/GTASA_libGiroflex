#pragma once

#include "pch.h"

class VehicleDummy {
private:
	static std::vector<RwFrame*> m_Frames;
	//static void CreateTempMatrix();
public:

	static std::vector<RpAtomic*> RpClumpGetAllAtomics(RpClump* clump);
	static std::vector<RpMaterial*> RpGeometryGetAllMaterials(RpGeometry* geometry);

	static std::vector<RwFrame*> GetFrameHierarchy(RwFrame* frame, RwFrame* root);

	static std::vector<RwFrame*> GetFramesOnVehicle(CVehicle* vehicle);

    static std::string GetFrameName(RwFrame* frame);

	static RwFrame* FindDummy(CVehicle* vehicle, std::string dummyName);
private:
	static void FindDummies(CVehicle* vehicle, RwFrame* frame);

public:
	//static CVector GetTransformedPosition(CVehicle* vehicle, CVector offset);
	//static CVector GetTransformedDummyPosition(CVehicle* vehicle, RwFrame* dummy, CVector offset);
	//static CVector GetTransformedDummyPositionByName(CVehicle* vehicle, std::string dummyName, CVector offset);
	//static CVector GetDummyOffset(CVehicle* vehicle, std::string dummyName);
};