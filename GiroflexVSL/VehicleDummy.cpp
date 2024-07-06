#include "VehicleDummy.h"

extern RpClump* (*RpClumpForAllAtomics)(RpClump* clump, RpAtomicCallBack callback, void* pData);
extern RpGeometry* (*RpGeometryForAllMaterials)(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* pData);
extern char* (*GetFrameNodeName)(RwFrame* frame);

std::vector<RwFrame*> VehicleDummy::m_Frames;
RwMatrix* tempMat = NULL;

/*
void VehicleDummy::CreateTempMatrix()
{
	if (!tempMat) tempMat = RwMatrixCreate();
}
*/

std::vector<RpAtomic*> VehicleDummy::RpClumpGetAllAtomics(RpClump* clump)
{
	static std::vector<RpAtomic*> atomics;
	atomics.clear();

	RpClumpForAllAtomics(clump, [](RpAtomic* atomic, void* data) {
		atomics.push_back(atomic);
		return atomic;
		}, (void*)((uint32_t)(0)));

	return atomics;
}

std::vector<RpMaterial*> VehicleDummy::RpGeometryGetAllMaterials(RpGeometry* geometry)
{
	static std::vector<RpMaterial*> materials;
	materials.clear();

	RpGeometryForAllMaterials(geometry, [](RpMaterial* material, void* data) {
		materials.push_back(material);
		return material;
		}, 0);

	return materials;
}

std::vector<RwFrame*> VehicleDummy::GetFrameHierarchy(RwFrame* frame, RwFrame* root) {
	std::vector<RwFrame*> hie;
	RwFrame* f = frame;

	while (f != NULL && f != root)
	{
		hie.insert(hie.begin(), f);
		f = RwFrameGetParent(f);
	}
	return hie;
}

std::vector<RwFrame*> VehicleDummy::GetFramesOnVehicle(CVehicle* vehicle) {
	m_Frames.clear();
	FindDummies(vehicle, (RwFrame*)vehicle->m_pRwClump->object.parent);
	return m_Frames;
}

std::string VehicleDummy::GetFrameName(RwFrame* frame)
{
    std::string name = GetFrameNodeName(frame);
    return name;
}


RwFrame* VehicleDummy::FindDummy(CVehicle* vehicle, std::string dummyName) {
	auto frames = VehicleDummy::GetFramesOnVehicle(vehicle);

	for (auto frame : frames)
	{
		std::string frameName = to_lower(GetFrameNodeName(frame));

		if (frameName.compare(to_lower(dummyName)) == 0) {
			return frame;
		}
	}

	return NULL;
}


void VehicleDummy::FindDummies(CVehicle* vehicle, RwFrame* frame) {
	if (!frame)
		return;

	if (RwFrame* nextFrame = frame->child)
		FindDummies(vehicle, nextFrame);

	if (RwFrame* nextFrame = frame->next)
		FindDummies(vehicle, nextFrame);

	if (std::find(m_Frames.begin(), m_Frames.end(), frame) != m_Frames.end())
		return;

	m_Frames.push_back(frame);
}

/*
Takes the vehicle world position and adds offset
*/
/*
CVector VehicleDummy::GetTransformedPosition(CVehicle* vehicle, CVector offset) {
	CreateTempMatrix();

	auto rootFrame = (RwFrame*)vehicle->m_pRwClump->object.parent;
	auto c = rootFrame->child;

	//Log::file << "child= " << GetFrameNodeName(c) << std::endl;

	RwMatrixTransform(tempMat, RwFrameGetMatrix(rootFrame), rwCOMBINEREPLACE);

	if (ToLower(GetFrameNodeName(c)).find("chassis_dummy") != -1)
	{
		RwMatrixTransform(tempMat, RwFrameGetMatrix(c), rwCOMBINEPRECONCAT);
	}

	RwMatrixTranslate(tempMat, &offset.ToRwV3d(), rwCOMBINEPRECONCAT);

	return CVector(tempMat->pos.x, tempMat->pos.y, tempMat->pos.z);
}
*/

/*
Takes the world position of a dummy and adds offset from its position
*/
/*
CVector VehicleDummy::GetTransformedDummyPosition(CVehicle* vehicle, RwFrame* dummy, CVector offset) {
	CreateTempMatrix();

	auto rootFrame = (RwFrame*)vehicle->m_pRwClump->object.parent;

	RwMatrixTransform(tempMat, RwFrameGetMatrix(rootFrame), rwCOMBINEREPLACE);

	auto hierarchy = GetFrameHierarchy(dummy, rootFrame);
	for (auto hf : hierarchy) {
		RwMatrixTransform(tempMat, RwFrameGetMatrix(hf), rwCOMBINEPRECONCAT);
	}

	RwMatrixTranslate(tempMat, &offset.ToRwV3d(), rwCOMBINEPRECONCAT);

	return CVector(tempMat->pos.x, tempMat->pos.y, tempMat->pos.z);
}
*/

/*
Takes the world position of a dummy (by name) and adds offset from its position
*/
/*
CVector VehicleDummy::GetTransformedDummyPositionByName(CVehicle* vehicle, std::string dummyName, CVector offset) {
	auto frames = VehicleDummy::GetFramesOnVehicle(vehicle);

	auto frame = FindDummy(vehicle, dummyName);

	if (!frame) return CVector(0, 0, 0);

	return VehicleDummy::GetTransformedDummyPosition(vehicle, frame, offset);
}
*/

/*
CVector VehicleDummy::GetDummyOffset(CVehicle* vehicle, std::string dummyName) {
	CVector offset = CVector(0, 0, 0);
	auto dummy = FindDummy(vehicle, dummyName);

	if (dummy == NULL) return offset;

	auto rootFrame = (RwFrame*)vehicle->m_pRwClump->object.parent;

	auto hierarchy = GetFrameHierarchy(dummy, rootFrame);
	for (auto hf : hierarchy) {

		auto pos = GetFrameNodePosition(hf);

		offset += pos;
	}

	return offset;
}
*/