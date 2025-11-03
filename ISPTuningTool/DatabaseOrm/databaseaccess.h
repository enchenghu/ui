#ifndef DATABASEACCESS_H
#define DATABASEACCESS_H

#define SQLITE_HAS_CODEC 1
#include "databaseorm.h"
#include <sqlcipher/sqlite3.h>
#include <sqlite_orm/sqlite_orm.h>

struct ModuleBaseAddr
{
	std::string moduleName;
	int         cameraId;
	int         baseAddr;
};

struct Camera
{
	int         cameraId;
	std::string permissionName;
};

inline void onOpen(sqlite3 *db)
{
	const char *passwd = "Smartsens23";
	sqlite3_key(db, passwd, static_cast<int>(strlen(passwd)));
}

inline auto initStorage(const std::string &path)
{
    auto storage = sqlite_orm::make_storage(path,
        sqlite_orm::make_table("permission",
            sqlite_orm::make_column("permission_name", &DatabaseOrm::Permission::permissionName,
                sqlite_orm::primary_key())),

        sqlite_orm::make_table("register",
            sqlite_orm::make_column("address", &DatabaseOrm::Register::address, sqlite_orm::primary_key()),
            sqlite_orm::make_column("display_name", &DatabaseOrm::Register::displayName),
            sqlite_orm::make_column("rw", &DatabaseOrm::Register::rwInfo),
            sqlite_orm::make_column("default_value", &DatabaseOrm::Register::defaultValue),
            sqlite_orm::make_column("register_description", &DatabaseOrm::Register::registerDescription),
            sqlite_orm::make_column("permission_name", &DatabaseOrm::Register::permissionName),
            sqlite_orm::foreign_key(&DatabaseOrm::Register::permissionName)
                .references(&DatabaseOrm::Permission::permissionName)),

        sqlite_orm::make_table("module",
            sqlite_orm::make_column("module_name", &DatabaseOrm::Module::moduleName, sqlite_orm::primary_key()),
            sqlite_orm::make_column("module_description", &DatabaseOrm::Module::moduleDescription),
            sqlite_orm::make_column("permission_name", &DatabaseOrm::Module::permissionName),
            sqlite_orm::foreign_key(&DatabaseOrm::Module::permissionName)
                .references(&DatabaseOrm::Permission::permissionName)),

        sqlite_orm::make_table("camera",
            sqlite_orm::make_column("camera_id", &Camera::cameraId, sqlite_orm::primary_key()),
            sqlite_orm::make_column("permission_name", &Camera::permissionName),
            sqlite_orm::foreign_key(&Camera::permissionName).references(&DatabaseOrm::Permission::permissionName)),

        sqlite_orm::make_table("module_base_addr", sqlite_orm::make_column("module_name", &ModuleBaseAddr::moduleName),
            sqlite_orm::make_column("camera_id", &ModuleBaseAddr::cameraId),
            sqlite_orm::make_column("base_addr", &ModuleBaseAddr::baseAddr),
            sqlite_orm::primary_key(&ModuleBaseAddr::moduleName, &ModuleBaseAddr::cameraId),
            sqlite_orm::foreign_key(&ModuleBaseAddr::moduleName).references(&DatabaseOrm::Module::moduleName),
            sqlite_orm::foreign_key(&ModuleBaseAddr::cameraId).references(&Camera::cameraId)),

        sqlite_orm::make_table("combined_register",
            sqlite_orm::make_column("combined_id", &DatabaseOrm::CombinedRegister::combinedId,
                sqlite_orm::primary_key().autoincrement()),
            sqlite_orm::make_column("module_name", &DatabaseOrm::CombinedRegister::moduleName),
            sqlite_orm::make_column("camera_id", &DatabaseOrm::CombinedRegister::cameraId),
            sqlite_orm::make_column("combined_name", &DatabaseOrm::CombinedRegister::combinedName),
            sqlite_orm::make_column("display_name", &DatabaseOrm::CombinedRegister::displayName),
            sqlite_orm::make_column("combined_description", &DatabaseOrm::CombinedRegister::combinedDescription),
            sqlite_orm::make_column("permission_name", &DatabaseOrm::CombinedRegister::permissionName),
            sqlite_orm::foreign_key(&DatabaseOrm::CombinedRegister::moduleName)
                .references(&DatabaseOrm::Module::moduleName),
            sqlite_orm::foreign_key(&DatabaseOrm::CombinedRegister::cameraId).references(&Camera::cameraId),
            sqlite_orm::foreign_key(&DatabaseOrm::CombinedRegister::permissionName)
                .references(&DatabaseOrm::Permission::permissionName)),

        sqlite_orm::make_table("register_bit_info",
            sqlite_orm::make_column("register_addr", &DatabaseOrm::RegisterBitInfo::registerAddr),
            sqlite_orm::make_column("bit_mask", &DatabaseOrm::RegisterBitInfo::bitMask),
            sqlite_orm::make_column("register_bit_name", &DatabaseOrm::RegisterBitInfo::registerBitName),
            sqlite_orm::make_column("combined_id", &DatabaseOrm::RegisterBitInfo::combinedId),
            sqlite_orm::make_column("combined_bit_mask", &DatabaseOrm::RegisterBitInfo::combinedBitMask),
            sqlite_orm::make_column("rw", &DatabaseOrm::RegisterBitInfo::rw),
            sqlite_orm::make_column("default_value", &DatabaseOrm::RegisterBitInfo::defaultValue),
            sqlite_orm::make_column("register_bit_description", &DatabaseOrm::RegisterBitInfo::registerBitDescription),
            sqlite_orm::make_column("permission_name", &DatabaseOrm::RegisterBitInfo::permissionName),
            sqlite_orm::primary_key(&DatabaseOrm::RegisterBitInfo::registerAddr,
                &DatabaseOrm::RegisterBitInfo::bitMask),
            sqlite_orm::foreign_key(&DatabaseOrm::RegisterBitInfo::registerAddr)
                .references(&DatabaseOrm::Register::address),
            sqlite_orm::foreign_key(&DatabaseOrm::RegisterBitInfo::combinedId)
                .references(&DatabaseOrm::CombinedRegister::combinedId),
            sqlite_orm::foreign_key(&DatabaseOrm::RegisterBitInfo::permissionName)
                .references(&DatabaseOrm::Permission::permissionName)),

        sqlite_orm::make_table("ui_module",
            sqlite_orm::make_column("module_name", &DatabaseOrm::UiModule::moduleName, sqlite_orm::primary_key()),
            sqlite_orm::make_column("permission_name", &DatabaseOrm::UiModule::permissionName),
            sqlite_orm::foreign_key(&DatabaseOrm::UiModule::permissionName)
                .references(&DatabaseOrm::Permission::permissionName)),

        sqlite_orm::make_table("ui_info", sqlite_orm::make_column("module_name", &DatabaseOrm::UiInfo::moduleName),
            sqlite_orm::make_column("control_name", &DatabaseOrm::UiInfo::controlName),
            sqlite_orm::make_column("control_index", &DatabaseOrm::UiInfo::index),
            sqlite_orm::make_column("combined_name", &DatabaseOrm::UiInfo::combinedName),
            sqlite_orm::make_column("register_address", &DatabaseOrm::UiInfo::registerAddr),
            sqlite_orm::make_column("permission_name", &DatabaseOrm::UiInfo::permissionName),
            sqlite_orm::primary_key(&DatabaseOrm::UiInfo::moduleName, &DatabaseOrm::UiInfo::controlName,
                &DatabaseOrm::UiInfo::index),
            sqlite_orm::foreign_key(&DatabaseOrm::UiInfo::permissionName)
                .references(&DatabaseOrm::Permission::permissionName)),

        sqlite_orm::make_table("ui_info_regmodule",
            sqlite_orm::make_column("ui_module_name", &DatabaseOrm::UiInfoRegModule::uiModuleName),
            sqlite_orm::make_column("ui_control_name", &DatabaseOrm::UiInfoRegModule::uiControlName),
            sqlite_orm::make_column("reg_module_name", &DatabaseOrm::UiInfoRegModule::regModuleName),
            sqlite_orm::make_column("has_camera_id", &DatabaseOrm::UiInfoRegModule::hasCameraId),
            sqlite_orm::foreign_key(&DatabaseOrm::UiInfoRegModule::regModuleName)
                .references(&DatabaseOrm::Module::moduleName)),

        sqlite_orm::make_table("awb_calibration_register",
            sqlite_orm::make_column("awb_register_code", &DatabaseOrm::AWBCalibrationRegister::registerCode),
            sqlite_orm::make_column("module_name", &DatabaseOrm::AWBCalibrationRegister::moduleName),
            sqlite_orm::make_column("combined_name", &DatabaseOrm::AWBCalibrationRegister::combinedName),
            sqlite_orm::primary_key(&DatabaseOrm::AWBCalibrationRegister::registerCode)));

    storage.on_open = onOpen;
    storage.open_forever();
    return storage;
}

using Storage = decltype(initStorage(""));

void initDatabaseInner(std::string databasePath);

class DatabaseAccess
{
public:
	static DatabaseAccess &getInstance();
	static Storage        &getStorage();

protected:
	friend void        initDatabaseInner(std::string);
	static std::string _databasePath;

	static void setDatabasePath(std::string databasePath);

private:
	DatabaseAccess();
	~DatabaseAccess();

	Storage _storage;
};
#endif // DATABASEACCESS_H
