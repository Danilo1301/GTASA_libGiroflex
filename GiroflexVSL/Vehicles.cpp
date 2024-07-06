#include "Vehicles.h"

#include "Log.h"

std::map<int, Vehicle*> Vehicles::m_Vehicles;
std::vector<RenderCorona> Vehicles::m_CoronasToRender;
std::vector<int> Vehicles::m_NewVehiclesRef;

extern int (*GetVehicleRef)(int);
extern uintptr_t* pVehiclePool;

void Vehicles::TryCreateVehicle(int hVehicle, int modelId)
{
	//_vehicleExist.push_back(hVehicle);

	if (HasVehicleHandle(hVehicle)) return;

	//Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicles: Add vehicle " << hVehicle << " (id: " << modelId << ") (" << std::to_string(m_Vehicles.size() + 1) << " total). Modelinfo? " << (ModelInfos::HasModelInfo(modelId) ? "yes" : "no") << std::endl;
	Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicles: Add vehicle " << hVehicle << " (id: " << modelId << ") (" << std::to_string(m_Vehicles.size() + 1) << " total)" << std::endl;

	auto vehicle = m_Vehicles[hVehicle] = new Vehicle(hVehicle, modelId);

    vehicle->Init();
}

bool Vehicles::HasVehicleHandle(int hVehicle)
{
	return m_Vehicles.find(hVehicle) != m_Vehicles.end();
}

Vehicle* Vehicles::GetVehicleByHandle(int hVehicle)
{
	if (!HasVehicleHandle(hVehicle)) return NULL;
	return m_Vehicles.at(hVehicle);
}


Vehicle* Vehicles::GetVehicleByVecIndex(int index)
{
    int i = 0;
    for (auto pair : m_Vehicles)
    {
        if (i == index) return pair.second;
        i++;
    }
    return NULL;
}

void Vehicles::RemoveVehicle(int hVehicle) {
    if (!Vehicles::HasVehicleHandle(hVehicle)) return;

	Vehicle* vehicle = m_Vehicles[hVehicle];

	Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicles: Remove vehicle " << hVehicle << " (id: " << vehicle->modelId << ") (" << std::to_string(m_Vehicles.size() - 1) << " total)" << std::endl;

	m_Vehicles.erase(hVehicle);
	//vehicle->Destroy();
	delete vehicle;
}

void Vehicles::CheckStreamedOutVehicles()
{
    std::vector<int> toRemove;

    for (auto pair : m_Vehicles)
    {
        auto vehicle = pair.second;

        if(vehicle->canBeRemoved) toRemove.push_back(vehicle->hVehicle);
    }

    for (auto hVehicle : toRemove)
    {
        RemoveVehicle(hVehicle);
    }

    /*
    std::vector<int> toRemove;

    for (auto pair : Vehicles::m_Vehicles)
    {
        auto hVehicle = pair.first;

        bool stillExists = false;
        for (auto n : _vehicleExist)
        {
            if (n == hVehicle) {
                stillExists = true;
                continue;
            }
        }

        if (!stillExists)
        {
            toRemove.push_back(hVehicle);
        }
    }

    for (auto hVehicle : toRemove)
    {
        RemoveVehicle(hVehicle);
    }

    _vehicleExist.clear();
    */
}

void Vehicles::Update(int dt)
{
    CheckStreamedOutVehicles();

    //Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicles: " << m_CoronasToRender.size() << " coronas were rendered" << std::endl;
    m_CoronasToRender.clear();

    //Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicles: Update" << std::endl;

    for (auto pair : Vehicles::m_Vehicles)
    {
        auto vehicle = pair.second;
        vehicle->Update(dt);
    }

    std::vector<Vehicle*> vehiclesToRemove;
    for (auto pair : Vehicles::m_Vehicles)
    {
        auto vehicle = pair.second;

        if(vehicle->canBeRemoved) vehiclesToRemove.push_back(vehicle);
    }

    for(auto vehicle : vehiclesToRemove)
    {
        m_Vehicles.erase(vehicle->hVehicle);
        delete vehicle;
    }

    //Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicles: Update end" << std::endl;
}

void Vehicles::RenderBefore(CVehicle* pVehicle)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicles: RenderBefore " << pVehicle << std::endl;

    for(auto pair : m_Vehicles)
    {
        auto vehicle = pair.second;
        if(vehicle->pVehicle != pVehicle) continue;

        vehicle->RenderBefore();
    }
}

void Vehicles::RenderAfter(CVehicle* pVehicle)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicles: RenderAfter " << pVehicle << std::endl;

    for(auto pair : m_Vehicles)
    {
        auto vehicle = pair.second;
        if(vehicle->pVehicle != pVehicle) continue;

        vehicle->RenderAfter();
    }
}

void Vehicles::AddCoronaToRender(RenderCorona corona)
{
    if (corona.color.a == 0) return;
    if (corona.radius <= 0.0f) return;

    m_CoronasToRender.push_back(corona);
}

void Vehicles::TryFindNewVehicles()
{
    //Log::Level(LOG_LEVEL::LOG_BOTH) << "TryFindNewVehicles" << std::endl;

    m_NewVehiclesRef.clear();

    //GET_RANDOM_CAR_IN_SPHERE_NO_SAVE_RECURSIVE
    //https://github.com/AndroidModLoader/GTA_CLEOMod/blob/RUSJJ_CLEO_MOD/cleo4opcodes.cpp

    auto objects = *(GTAVehicleSA**)(*pVehiclePool + 0);
    tByteFlag* flags = *(tByteFlag**)(*pVehiclePool + 4);
    int size = *(int*)(*pVehiclePool + 8);

    for (int i = 0; i < size; ++i)
    {
        if (flags[i].bEmpty) continue;
        auto& ent = objects[i];
        
        //IS_CAR_SIREN_ON
        //https://github.com/AndroidModLoader/GTA_CLEOMod/blob/RUSJJ_CLEO_MOD/cleo4opcodes.cpp
        // 
        //bool sirenOn = *(uint8_t*)((uintptr_t)vehicle + 0x42D + 4) >> 7;
        bool sirenOn = ent.UInt8At(0x42D + 4) >> 7;

        void* driver = (void*)ent.UIntAt(0x460 + 4);

        int ref = GetVehicleRef(ent.AsInt());

        //Log::Level(LOG_LEVEL::LOG_BOTH) << "Found " << ent.AsInt() << " ref " << ref << " siren " << (sirenOn ? "ON" : "OFF") << std::endl;
        //Log::Level(LOG_LEVEL::LOG_BOTH) << "driver " << driver << std::endl;

        for (auto vpair : Vehicles::m_Vehicles)
        {
            auto vehicle = vpair.second;

            if (vehicle->hVehicle != ref) continue;
            
            vehicle->gameSirenState = sirenOn;
            vehicle->pDriver = driver;
        }

        m_NewVehiclesRef.push_back(ref); //important

    }

    for(auto pair : m_Vehicles)
    {
        auto vehicle = pair.second;

        bool stillExists = false;
        for(auto ref : m_NewVehiclesRef)
        {
            if(ref == vehicle->hVehicle)
            {
                stillExists = true;
                break;
            }
        }

        if(!stillExists)
        {
            vehicle->canBeRemoved = true;
        }
    }

    //Log::Level(LOG_LEVEL::LOG_BOTH) << "Found: " << m_NewVehiclesRef.size() << std::endl;
}