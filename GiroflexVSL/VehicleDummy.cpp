#include "VehicleDummy.h"

extern RwMatrix* (*RwMatrixCreate)(void);
extern RwBool (*RwMatrixDestroy)(RwMatrix* mpMat);
extern RwMatrix* (*RwMatrixTransform)(RwMatrix* matrix, const RwMatrix* transform, RwOpCombineType combineOp);
extern RwMatrix* (*RwMatrixTranslate)(RwMatrix* matrix, const RwV3d* translation, RwOpCombineType combineOp);
extern RpClump* (*RpClumpForAllAtomics)(RpClump* clump, RpAtomicCallBack callback, void* pData);
extern RpGeometry* (*RpGeometryForAllMaterials)(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* pData);

extern char* (*GetFrameNodeName)(RwFrame* frame);

std::vector<RwFrame*> VehicleDummy::m_Frames;
RwMatrix* tempMat = NULL;

void VehicleDummy::CreateTempMatrix()
{
	if (!tempMat) tempMat = RwMatrixCreate();
}

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
CVector VehicleDummy::GetTransformedPosition(CVehicle* vehicle, CVector offset) {
	CreateTempMatrix();

	auto rootFrame = (RwFrame*)vehicle->m_pRwClump->object.parent;
	auto c = rootFrame->child;

	//Log::file << "child= " << GetFrameNodeName(c) << std::endl;

	RwMatrixTransform(tempMat, RwFrameGetMatrix(rootFrame), rwCOMBINEREPLACE);

	if (to_lower(GetFrameNodeName(c)).find("chassis_dummy") != -1)
	{
		RwMatrixTransform(tempMat, RwFrameGetMatrix(c), rwCOMBINEPRECONCAT);
	}

    //prev function:
	//RwMatrixTranslate(tempMat, &offset.ToRwV3d(), rwCOMBINEPRECONCAT);

	//replaced function:
	RwV3d offset_rw = { offset.x, offset.y, offset.z };
	RwMatrixTranslate(tempMat, &offset_rw, rwCOMBINEPRECONCAT);

	return CVector(tempMat->pos.x, tempMat->pos.y, tempMat->pos.z);
}

/*
Takes the world position of a dummy and adds offset from its position
*/
CVector VehicleDummy::GetTransformedDummyPosition(CVehicle* vehicle, RwFrame* dummy, CVector offset) {
	CreateTempMatrix();

	auto rootFrame = (RwFrame*)vehicle->m_pRwClump->object.parent;

	RwMatrixTransform(tempMat, RwFrameGetMatrix(rootFrame), rwCOMBINEREPLACE);

	auto hierarchy = GetFrameHierarchy(dummy, rootFrame);
	for (auto hf : hierarchy) {
		RwMatrixTransform(tempMat, RwFrameGetMatrix(hf), rwCOMBINEPRECONCAT);
	}

	//prev function:
	//RwMatrixTranslate(tempMat, &offset.ToRwV3d(), rwCOMBINEPRECONCAT);

	//replaced function:
	RwV3d offset_rw = { offset.x, offset.y, offset.z };
	RwMatrixTranslate(tempMat, &offset_rw, rwCOMBINEPRECONCAT);

	return CVector(tempMat->pos.x, tempMat->pos.y, tempMat->pos.z);
}

/*
Takes the world position of a dummy (by name) and adds offset from its position
*/
CVector VehicleDummy::GetTransformedDummyPositionByName(CVehicle* vehicle, std::string dummyName, CVector offset) {
	auto frames = VehicleDummy::GetFramesOnVehicle(vehicle);

	auto frame = FindDummy(vehicle, dummyName);

	if (!frame) return CVector(0, 0, 0);

	return VehicleDummy::GetTransformedDummyPosition(vehicle, frame, offset);
}

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

/*
RwMatrix* CloneRwMatrix(RwMatrix* originalMatrix)
{
	if (originalMatrix == NULL)
	{
		// Handle null input matrix
		return NULL;
	}

	// Step 1: Create a new matrix
	RwMatrix* clonedMatrix = RwMatrixCreate();
	if (clonedMatrix == NULL)
	{
		// Handle memory allocation failure
		return NULL;
	}

	// Step 2: Copy the rotational part of the matrix (3x3 matrix)
	clonedMatrix->right = originalMatrix->right;
	clonedMatrix->up = originalMatrix->up;
	clonedMatrix->at = originalMatrix->at;

	// Step 3: Copy the positional part (translation vector)
	clonedMatrix->pos.x = originalMatrix->pos.x;
	clonedMatrix->pos.y = originalMatrix->pos.y;
	clonedMatrix->pos.z = originalMatrix->pos.z;

	// Step 4: Copy the flags (if any)
	clonedMatrix->flags = originalMatrix->flags;

	return clonedMatrix;
}
*/

CVector VehicleDummy::GetDummyOffset_KindaFixed(CVehicle* vehicle, RwFrame* dummy, CVector offset) {
	CreateTempMatrix();

	//set to 0, 0, 0
	auto matrixZero = RwMatrixCreate();
	RwMatrixTransform(tempMat, matrixZero, rwCOMBINEREPLACE);
	RwMatrixDestroy(matrixZero);

	auto rootFrame = (RwFrame*)vehicle->m_pRwClump->object.parent;
	
	//dont transform with root position
	//RwMatrixTransform(tempMat, RwFrameGetMatrix(rootFrame), rwCOMBINEREPLACE);

	auto hierarchy = GetFrameHierarchy(dummy, rootFrame);
	for (auto hf : hierarchy) {
		
		auto mat = RwFrameGetMatrix(hf);
		
		if (to_lower(GetFrameNodeName(hf)).find("chassis_dummy") != -1)
		{
			//POSSIBLE BUG: if your frame is on the chassis_dummy on a bike, it will not apply rotation :(
			continue;
		}

		RwMatrixTransform(tempMat, mat, rwCOMBINEPRECONCAT);
	}

	return CVector(tempMat->pos.x, tempMat->pos.y, tempMat->pos.z);
}