#ifndef DATABASEORM_H
#define DATABASEORM_H
#include <memory>
#include <string>
#include <vector>

namespace DatabaseOrm
{
struct Permission
{
	std::string permissionName;
};

struct Register
{
	uint32_t    address;
	std::string displayName;
	std::string rwInfo;
	int         defaultValue;
	std::string registerDescription;
	std::string permissionName;
};

struct Module
{
	std::string moduleName;
	std::string moduleDescription;
	std::string permissionName;
};

struct CombinedRegister
{
	int                  combinedId;
	std::string          moduleName;
	std::shared_ptr<int> cameraId;
	std::string          combinedName;
	std::string          displayName;
	std::string          combinedDescription;
	std::string          permissionName;
};

struct RegisterBitInfo
{
	uint32_t    registerAddr;
	int         bitMask;
	std::string registerBitName;
	int         combinedId;
        uint32_t   combinedBitMask;
	std::string rw;
	int         defaultValue;
	std::string registerBitDescription;
	std::string permissionName;
};

struct UiModule
{
	std::string moduleName;
	std::string permissionName;
};

struct UiInfo
{
	std::string                  moduleName;
	std::string                  controlName;
	int                          index;
	std::shared_ptr<std::string> combinedName;
	std::shared_ptr<uint32_t>    registerAddr;
	std::string                  permissionName;
};

struct UiInfoRegModule
{
	std::string uiModuleName;
	std::string uiControlName;
	std::string regModuleName;
	bool        hasCameraId;
};

struct AWBCalibrationRegister
{
    int registerCode;
    std::string moduleName;
    std::string combinedName;
};

void initDatabase(std::string databasePath);

std::vector<Register> getAllRegisters();
std::vector<Register> getRegistersByCameraId(std::shared_ptr<int> cameraId);
std::vector<Register> getRegisterByModule(Module &module);
std::vector<Register> getRegistersByModuleName(std::string moduleName);
std::vector<Register> getRegistersByModuleAndCameraId(Module &module, std::shared_ptr<int> cameraId);
std::vector<Register> getRegistersByModuleNameAndCameraId(std::string moduleName, std::shared_ptr<int> cameraId);
std::vector<Register> getRegistersByUiModuleNameAndCameraId(std::string uiModuleName, std::shared_ptr<int> cameraId);
Register              getRegisterByRegisterBitInfo(RegisterBitInfo &regBitInfo);
Register              getRegisterByAddress(uint32_t address);

std::vector<Module> getAllModules();
Module              getModuleByRegister(Register &reg);
Module              getModuleByRegisterAddress(int regAddress);
Module              getModuleByRegisterBitInfo(RegisterBitInfo &regBitInfo);
Module              getModuleByCombinedRegister(CombinedRegister &combinedReg);
Module              getModuleByModuleName(std::string moduleName);

std::vector<CombinedRegister> getAllCombinedRegisters();
std::vector<CombinedRegister> getCombinedRegistersByCameraId(std::shared_ptr<int> cameraId);
std::vector<CombinedRegister> getCombinedRegistersByModuleName(std::string moduleName);
std::vector<CombinedRegister> getCombinedRegistersByModuleNameAndCameraId(std::string moduleName, std::shared_ptr<int> cameraId);
std::vector<CombinedRegister> getCombinedRegistersByModule(Module &module);
std::vector<CombinedRegister> getCombinedRegistersByModuleAndCameraId(Module &module, std::shared_ptr<int> cameraId);
std::vector<CombinedRegister> getCombinedRegisterByRegister(Register &reg);
std::vector<CombinedRegister> getCombinedRegisterByRegisterAddress(int regAddress);
std::vector<CombinedRegister> getCombinedRegisterByCombinedName(std::string combinedName);
std::vector<CombinedRegister> getCombinedRegisterByUiModuleNameAndControlNameAndCameraId(std::string uiModuleName,
    std::string controlName, std::shared_ptr<int> cameraId);
CombinedRegister              getCombinedRegisterByRegisterBitInfo(RegisterBitInfo &regBitInfo);
CombinedRegister getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(std::string moduleName, std::shared_ptr<int> cameraId,
                                                                           std::string combinedName);
CombinedRegister getCombinedRegisterByModuleAndCameraIdAndCombinedName(Module &module, std::shared_ptr<int> cameraId,
                                                                       std::string combinedName);

std::vector<RegisterBitInfo> getAllRegisterBitInfo();
std::vector<RegisterBitInfo> getRegisterBitInfoByCombinedRegister(CombinedRegister &combinedReg);
std::vector<RegisterBitInfo> getRegisterBitInfoByCombinedRegisterId(int combinedId);
std::vector<RegisterBitInfo> getRegisterBitInfoByRegister(Register &reg);
std::vector<RegisterBitInfo> getRegisterBitInfoByRegisterAddress(int regAddress);
std::vector<RegisterBitInfo> getRegisterBitInfoByRegisterName(std::string regName);
std::vector<RegisterBitInfo> getRegisterBitInfoByRegisterAddressAndBitMask(int regAddress, int bitMask);
RegisterBitInfo              getRegisterBitInfoByRegisterBitName(std::string regBitInfoName);
RegisterBitInfo              getRegisterBitInfoByRegisterAndBitMask(Register &reg, int bitMask);

std::vector<Permission> getAllPermissions();

std::vector<UiModule> getAllUiModules();

std::vector<UiInfo> getAllUiInfo();
std::vector<UiInfo> getUiInfoByModuleName(std::string moduleName);
std::vector<UiInfo> getUiInfoByModuleNameAndControlName(std::string moduleName, std::string controlName);
std::vector<UiInfo> getUiInfoByModuleNameAndControlNameAndControlIndex(std::string moduleName, std::string controlName,
    int controlIndex);
UiInfo              getFirstUiInfoByModuleNameAndControlName(std::string moduleName, std::string controlName);

std::vector<UiInfoRegModule> getAllUiInfoRegModule();
std::vector<UiInfoRegModule> getUiInfoRegModuleByUiModuleNameAndUiControlName(std::string uiModuleName, std::string uiControlName);

std::vector<AWBCalibrationRegister> getAWBRegisterModuleCombine(uint32_t registerCode);
std::vector<CombinedRegister> getAWBRegisterCombinedId(std::string moduleName, std::string combinedName, int cameraId);
std::vector<RegisterBitInfo> getAWBRegisterAddress(uint32_t combinedId);

} // namespace DatabaseOrm
#endif // DATABASEORM_H
