#include "Vehicles.h"

#include "Log.h"
#include "CleoOpcodes.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

std::map<int, Vehicle*> Vehicles::m_Vehicles;

bool Vehicles::HasVehicleHandle(int hVehicle)
{
    return m_Vehicles.find(hVehicle) != m_Vehicles.end();
}

Vehicle* Vehicles::TryCreateVehicle(int hVehicle)
{   
    if(hVehicle == 0)
    {
        Log::Level(eLogLevel::LOG_BOTH) << "Bro, i think your code has some issues (ERROR) (Vehicles)" << std::endl;
        return NULL;
    }

	if (HasVehicleHandle(hVehicle))
    {
        Log::Level(eLogLevel::LOG_BOTH) << "Vehicle already added: " << hVehicle << std::endl;
        return GetVehicleByHandle(hVehicle);
    }

    CVehicle* pVehicle = (CVehicle*)GetVehicleFromRef(hVehicle);

	Log::Level(eLogLevel::LOG_BOTH) << "Vehicles: Add vehicle " << hVehicle << ", ref: " << pVehicle << " (" << std::to_string(m_Vehicles.size() + 1) << " total)" << std::endl;

    auto vehicle = new Vehicle(hVehicle, pVehicle);

	m_Vehicles[hVehicle] = vehicle;

    return vehicle;
}

Vehicle* Vehicles::GetVehicleByHandle(int hVehicle)
{
	if (!HasVehicleHandle(hVehicle)) return NULL;
	return m_Vehicles.at(hVehicle);
}

void Vehicles::RemoveVehicle(int hVehicle)
{
    auto vehicle = m_Vehicles[hVehicle];

    Log::Level(eLogLevel::LOG_BOTH) << "Vehicles: Delete vehicle " << hVehicle << ", ref: " << vehicle->pVehicle << " (" << std::to_string(m_Vehicles.size() - 1) << " total)" << std::endl;

    m_Vehicles.erase(hVehicle);
    delete vehicle;
}

void Vehicles::Update()
{
    std::vector<int> vehicleRefsThatExists;
    auto vehicles = menuVSL->GetVehicles();

    for(auto mvehicle : vehicles)
    {
        auto ref = mvehicle.hVehicle;
        
        if(!HasVehicleHandle(ref))
        {
            auto vehicle = TryCreateVehicle(ref);
        }

        vehicleRefsThatExists.push_back(ref);
    }

    std::vector<int> vehiclesToDelete;
    for(auto p : m_Vehicles)
    {
        auto vehicle = p.second;
        auto ref = vehicle->hVehicle;

        auto it = std::find(vehicleRefsThatExists.begin(), vehicleRefsThatExists.end(), ref);
        if (it == vehicleRefsThatExists.end())
        {
            Log::Level(eLogLevel::LOG_BOTH) << "Could not find vehicle " << ref << std::endl;
            vehiclesToDelete.push_back(vehicle->hVehicle);
        }
    }
    for(auto hVehicle : vehiclesToDelete)
    {
        RemoveVehicle(hVehicle);
    }

    for(auto vehicle : GetVehicles())
    {
        vehicle->Update();
    }
}

std::vector<Vehicle*> Vehicles::GetVehicles()
{
    std::vector<Vehicle*> vehicles;
    for(auto p : m_Vehicles)
    {
        auto vehicle = p.second;

        if(!CAR_DEFINED(vehicle->hVehicle)) continue;

        vehicles.push_back(vehicle);
    }
    return vehicles;
}