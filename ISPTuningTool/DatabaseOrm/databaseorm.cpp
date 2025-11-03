#include "databaseorm.h"
#include "databaseaccess.h"
#include <vector>

namespace DatabaseOrm {
using namespace sqlite_orm;

void initDatabase(std::string databasePath)
{
    try {
        initDatabaseInner(databasePath);
    } catch (...) {
        return;
    }
}

std::vector<Register> getAllRegisters()
{
    try {
        return DatabaseAccess::getStorage().get_all<Register>();
    } catch (...) {
        return {};
    }
}

std::vector<Register> getRegistersByCameraId(std::shared_ptr<int> cameraId)
{
    try {
        if (cameraId == nullptr) {
            return DatabaseAccess::getStorage().get_all<Register>(
                inner_join<RegisterBitInfo>(on(c(&Register::address) == &RegisterBitInfo::registerAddr)),
                inner_join<CombinedRegister>(on(c(&RegisterBitInfo::combinedId) == &CombinedRegister::combinedId)),
                where(is_null(&CombinedRegister::cameraId)));
        }
        return DatabaseAccess::getStorage().get_all<Register>(
            inner_join<RegisterBitInfo>(on(c(&Register::address) == &RegisterBitInfo::registerAddr)),
            inner_join<CombinedRegister>(on(c(&RegisterBitInfo::combinedId) == &CombinedRegister::combinedId)),
            where(c(&CombinedRegister::cameraId) == cameraId));
    } catch (...) {
        return {};
    }
}

std::vector<Register> getRegisterByModule(Module &module)
{
    try {
        return getRegistersByModuleName(module.moduleName);
    } catch (...) {
        return {};
    }
}

std::vector<Register> getRegistersByModuleName(std::string moduleName)
{
    try {
        return DatabaseAccess::getStorage().get_all<Register>(
            inner_join<RegisterBitInfo>(on(c(&Register::address) == &RegisterBitInfo::registerAddr)),
            inner_join<CombinedRegister>(on(c(&RegisterBitInfo::combinedId) == &CombinedRegister::combinedId)),
            where(c(&CombinedRegister::moduleName) == moduleName));
    } catch (...) {
        return {};
    }
}

std::vector<Register> getRegistersByModuleAndCameraId(Module &module, std::shared_ptr<int> cameraId)
{
    try {
        return getRegistersByModuleNameAndCameraId(module.moduleName, cameraId);
    } catch (...) {
        return {};
    }
}

std::vector<Register> getRegistersByModuleNameAndCameraId(std::string moduleName, std::shared_ptr<int> cameraId)
{
    try {
        if (cameraId == nullptr) {
            return DatabaseAccess::getStorage().get_all<Register>(
                inner_join<RegisterBitInfo>(on(c(&Register::address) == &RegisterBitInfo::registerAddr)),
                inner_join<CombinedRegister>(on(c(&RegisterBitInfo::combinedId) == &CombinedRegister::combinedId)),
                where(c(&CombinedRegister::moduleName) == moduleName and is_null(&CombinedRegister::cameraId)));
        }
        return DatabaseAccess::getStorage().get_all<Register>(
            inner_join<RegisterBitInfo>(on(c(&Register::address) == &RegisterBitInfo::registerAddr)),
            inner_join<CombinedRegister>(on(c(&RegisterBitInfo::combinedId) == &CombinedRegister::combinedId)),
            where(c(&CombinedRegister::moduleName) == moduleName and c(&CombinedRegister::cameraId) == cameraId));
    } catch (...) {
        return {};
    }
}
std::vector<Register> getRegistersByUiModuleNameAndCameraId(std::string uiModuleName, std::shared_ptr<int> cameraId)
{
    try {
        return DatabaseAccess::getStorage().get_all<Register>(
            inner_join<RegisterBitInfo>(on(c(&Register::address) == &RegisterBitInfo::registerAddr)),
            inner_join<CombinedRegister>(on(c(&RegisterBitInfo::combinedId) == &CombinedRegister::combinedId)),
            inner_join<UiInfo>(on(c(&CombinedRegister::combinedName) == &UiInfo::combinedName)),
            inner_join<UiInfoRegModule>(on(c(&UiInfoRegModule::regModuleName) == &CombinedRegister::moduleName
                and c(&UiInfoRegModule::uiModuleName) == &UiInfo::moduleName
                and c(&UiInfoRegModule::uiControlName) == &UiInfo::controlName)),
            where(c(&UiInfo::moduleName) == uiModuleName
                and (c(&CombinedRegister::cameraId) == cameraId or is_null(&CombinedRegister::cameraId))));
    } catch (...) {
        return {};
    }
}

Register getRegisterByRegisterBitInfo(RegisterBitInfo &regBitInfo)
{
    try {
        return getRegisterByAddress(regBitInfo.registerAddr);
    } catch (...) {
        return {};
    }
}

Register getRegisterByAddress(uint32_t address)
{
    try {
        return DatabaseAccess::getStorage().get<Register>(address);
    } catch (...) {
        return {};
    }
}

std::vector<Module> getAllModules()
{
    try {
        return DatabaseAccess::getStorage().get_all<Module>();
    } catch (...) {
        return {};
    }
}

Module getModuleByRegister(Register &reg)
{
    try {
        return getModuleByRegisterAddress(reg.address);
    } catch (...) {
        return {};
    }
}

Module getModuleByRegisterAddress(int regAddress)
{
    try {
        auto modules = DatabaseAccess::getStorage().get_all<Module>(
            inner_join<CombinedRegister>(on(c(&CombinedRegister::moduleName) == &Module::moduleName)),
            inner_join<RegisterBitInfo>(on(c(&CombinedRegister::combinedId) == &RegisterBitInfo::combinedId)),
            where(c(&RegisterBitInfo::registerAddr) == regAddress));
        if (modules.size() >= 1) {
            return modules[0];
        }
        return {};
    } catch (...) {
        return {};
    }
}

Module getModuleByRegisterBitInfo(RegisterBitInfo &regBitInfo)
{
    try {
        return getModuleByRegisterAddress(regBitInfo.registerAddr);
    } catch (...) {
        return {};
    }
}

Module getModuleByCombinedRegister(CombinedRegister &combinedReg)
{
    try {
        return getModuleByModuleName(combinedReg.moduleName);
    } catch (...) {
        return {};
    }
}

Module getModuleByModuleName(std::string moduleName)
{
    try {
        return DatabaseAccess::getStorage().get<Module>(where(c(&Module::moduleName) == moduleName));
    } catch (...) {
        return {};
    }
}

std::vector<CombinedRegister> getAllCombinedRegisters()
{
    try {
        return DatabaseAccess::getStorage().get_all<CombinedRegister>();
    } catch (...) {
        return {};
    }
}

std::vector<CombinedRegister> getCombinedRegistersByCameraId(std::shared_ptr<int> cameraId)
{
    try {
        return DatabaseAccess::getStorage().get_all<CombinedRegister>(
            where(c(&CombinedRegister::cameraId) == cameraId));
    } catch (...) {
        return {};
    }
}

std::vector<CombinedRegister> getCombinedRegistersByModuleName(std::string moduleName)
{
    try {
        return DatabaseAccess::getStorage().get_all<CombinedRegister>(
            where(c(&CombinedRegister::moduleName) == moduleName));
    } catch (...) {
        return {};
    }
}

std::vector<CombinedRegister> getCombinedRegistersByModuleNameAndCameraId(std::string moduleName,
    std::shared_ptr<int>                                                              cameraId)
{
    try {
        if (cameraId == nullptr) {
            return DatabaseAccess::getStorage().get_all<CombinedRegister>(
                where(c(&CombinedRegister::moduleName) == moduleName and is_null(&CombinedRegister::cameraId)));
        }
        return DatabaseAccess::getStorage().get_all<CombinedRegister>(
            where(c(&CombinedRegister::moduleName) == moduleName and c(&CombinedRegister::cameraId) == cameraId));
    } catch (...) {
        return {};
    }
}

std::vector<CombinedRegister> getCombinedRegistersByModule(Module &module)
{
    try {
        return getCombinedRegistersByModuleName(module.moduleName);
    } catch (...) {
        return {};
    }
}

std::vector<CombinedRegister> getCombinedRegistersByModuleAndCameraId(Module &module, std::shared_ptr<int> cameraId)
{
    try {
        return getCombinedRegistersByModuleNameAndCameraId(module.moduleName, cameraId);
    } catch (...) {
        return {};
    }
}

std::vector<CombinedRegister> getCombinedRegisterByRegister(Register &reg)
{
    try {
        return getCombinedRegisterByRegisterAddress(reg.address);
    } catch (...) {
        return {};
    }
}

std::vector<CombinedRegister> getCombinedRegisterByRegisterAddress(int regAddress)
{
    try {
        return DatabaseAccess::getStorage().get_all<CombinedRegister>(
            inner_join<RegisterBitInfo>(on(c(&RegisterBitInfo::combinedId) == &CombinedRegister::combinedId)),
            where(c(&RegisterBitInfo::registerAddr) == regAddress));
    } catch (...) {
        return {};
    }
}

std::vector<CombinedRegister> getCombinedRegisterByCombinedName(std::string combinedName)
{
    return std::vector<CombinedRegister>();
}

std::vector<CombinedRegister> getCombinedRegisterByUiModuleNameAndControlNameAndCameraId(std::string uiModuleName,
    std::string controlName, std::shared_ptr<int> cameraId)
{
    try {
        return DatabaseAccess::getStorage().get_all<CombinedRegister>(
            inner_join<UiInfo>(on(c(&CombinedRegister::combinedName) == &UiInfo::combinedName)),
            inner_join<UiInfoRegModule>(on(c(&UiInfoRegModule::regModuleName) == &CombinedRegister::moduleName
                and c(&UiInfoRegModule::uiModuleName) == &UiInfo::moduleName
                and c(&UiInfoRegModule::uiControlName) == &UiInfo::controlName)),
            where(c(&UiInfo::moduleName) == uiModuleName and c(&UiInfo::controlName) == controlName
                and (c(&CombinedRegister::cameraId) == cameraId or is_null(&CombinedRegister::cameraId))));
    } catch (...) {
        return {};
    }
}

CombinedRegister getCombinedRegisterByRegisterBitInfo(RegisterBitInfo &regBitInfo)
{
    try {
        auto combinedRegisters = DatabaseAccess::getStorage().get_all<CombinedRegister>(
            inner_join<RegisterBitInfo>(on(c(&RegisterBitInfo::combinedId) == &CombinedRegister::combinedId)),
            where(c(&RegisterBitInfo::registerAddr) == regBitInfo.registerAddr
                and c(&RegisterBitInfo::bitMask) == regBitInfo.bitMask));
        if (combinedRegisters.size() == 1) {
            return combinedRegisters[0];
        }
        return {};
    } catch (...) {
        return {};
    }
}

CombinedRegister getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(std::string moduleName,
    std::shared_ptr<int> cameraId, std::string combinedName)
{
    try {

        std::vector<CombinedRegister> combinedRegisters;
        if (cameraId == nullptr) {
            combinedRegisters = DatabaseAccess::getStorage().get_all<CombinedRegister>(
                where(c(&CombinedRegister::moduleName) == moduleName
                    and c(&CombinedRegister::combinedName) == combinedName and is_null(&CombinedRegister::cameraId)));
        } else {
            combinedRegisters = DatabaseAccess::getStorage().get_all<CombinedRegister>(
                where(c(&CombinedRegister::moduleName) == moduleName and c(&CombinedRegister::cameraId) == cameraId
                    and c(&CombinedRegister::combinedName) == combinedName));
        }
        if (combinedRegisters.size() == 1) {
            return combinedRegisters[0];
        }
        return {};
    } catch (...) {
        return {};
    }
}

CombinedRegister getCombinedRegisterByModuleAndCameraIdAndCombinedName(Module &module, std::shared_ptr<int> cameraId,
    std::string combinedName)
{
    try {
        return getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(module.moduleName, cameraId, combinedName);
    } catch (...) {
        return {};
    }
}

std::vector<RegisterBitInfo> getAllRegisterBitInfo()
{
    try {
        return DatabaseAccess::getStorage().get_all<RegisterBitInfo>();
    } catch (...) {
        return {};
    }
}

std::vector<RegisterBitInfo> getRegisterBitInfoByCombinedRegister(CombinedRegister &combinedReg)
{
    try {
        return DatabaseAccess::getStorage().get_all<RegisterBitInfo>(
            where(c(&RegisterBitInfo::combinedId) == combinedReg.combinedId));
    } catch (...) {
        return {};
    }
}

std::vector<RegisterBitInfo> getRegisterBitInfoByCombinedRegisterId(int combinedId)
{
    try {
        return DatabaseAccess::getStorage().get_all<RegisterBitInfo>(
            where(c(&RegisterBitInfo::combinedId) == combinedId));
    } catch (...) {
        return {};
    }
}

std::vector<RegisterBitInfo> getRegisterBitInfoByRegister(Register &reg)
{
    try {
        return getRegisterBitInfoByRegisterAddress(reg.address);
    } catch (...) {
        return {};
    }
}

std::vector<RegisterBitInfo> getRegisterBitInfoByRegisterAddress(int regAddress)
{
    try {
        return DatabaseAccess::getStorage().get_all<RegisterBitInfo>(
            where(c(&RegisterBitInfo::registerAddr) == regAddress));
    } catch (...) {
        return {};
    }
}

std::vector<RegisterBitInfo> getRegisterBitInfoByRegisterName(std::string regName)
{
    try {
        return DatabaseAccess::getStorage().get_all<RegisterBitInfo>(
            inner_join<Register>(on(c(&RegisterBitInfo::registerAddr) == &Register::address)),
            where(c(&Register::displayName) == regName));
    } catch (...) {
        return {};
    }
}

std::vector<RegisterBitInfo> getRegisterBitInfoByRegisterAddressAndBitMask(int regAddress, int bitMask)
{
    try {
        return DatabaseAccess::getStorage().get_all<RegisterBitInfo>(
            where(c(&RegisterBitInfo::registerAddr) == regAddress and c(&RegisterBitInfo::bitMask) == bitMask));
    } catch (...) {
        return {};
    }
}

RegisterBitInfo getRegisterBitInfoByRegisterBitName(std::string regBitInfoName)
{
    try {
        auto registerBitInfo = DatabaseAccess::getStorage().get_all<RegisterBitInfo>(
            where(c(&RegisterBitInfo::registerBitName) == regBitInfoName));
        if (registerBitInfo.size() == 1) {
            return registerBitInfo[0];
        }
        return {};
    } catch (...) {
        return {};
    }
}

RegisterBitInfo getRegisterBitInfoByRegisterAndBitMask(Register &reg, int bitMask)
{
    try {
        return DatabaseAccess::getStorage().get<RegisterBitInfo>(
            where(c(&RegisterBitInfo::registerAddr) == reg.address and c(&RegisterBitInfo::bitMask) == bitMask));
    } catch (...) {
        return {};
    }
}

std::vector<Permission> getAllPermissions()
{
    try {
        return DatabaseAccess::getStorage().get_all<Permission>();
    } catch (...) {
        return {};
    }
}

std::vector<UiModule> getAllUiModules()
{
    try {
        return DatabaseAccess::getStorage().get_all<UiModule>();
    } catch (...) {
        return {};
    }
}

std::vector<UiInfo> getAllUiInfo()
{
    try {
        return DatabaseAccess::getStorage().get_all<UiInfo>();
    } catch (...) {
        return {};
    }
}

std::vector<UiInfo> getUiInfoByModuleName(std::string moduleName)
{
    try {
        return DatabaseAccess::getStorage().get_all<UiInfo>(where(c(&UiInfo::moduleName) == moduleName));
    } catch (...) {
        return {};
    }
}

std::vector<UiInfo> getUiInfoByModuleNameAndControlName(std::string moduleName, std::string controlName)
{
    try {
        auto res = DatabaseAccess::getStorage().get_all<UiInfo>(
            where(c(&UiInfo::moduleName) == moduleName and c(&UiInfo::controlName) == controlName));
        // sort by index
        std::sort(res.begin(), res.end(), [](UiInfo &a, UiInfo &b) { return a.index < b.index; });
        return res;
    } catch (...) {
        return {};
    }
}

UiInfo getFirstUiInfoByModuleNameAndControlName(std::string moduleName, std::string controlName)
{
    try {
        auto uiInfos = DatabaseAccess::getStorage().get_all<UiInfo>(
            where(c(&UiInfo::moduleName) == moduleName and c(&UiInfo::controlName) == controlName));
        if (uiInfos.size() == 1) {
            return uiInfos[0];
        }
        return {};
    } catch (...) {
        return {};
    }
}

std::vector<UiInfoRegModule> getAllUiInfoRegModule()
{
    try {
        return DatabaseAccess::getStorage().get_all<UiInfoRegModule>();
    } catch (...) {
        return {};
    }
}

std::vector<UiInfoRegModule> getUiInfoRegModuleByUiModuleNameAndUiControlName(std::string uiModuleName,
    std::string                                                                           uiControlName)
{
    try {
        return DatabaseAccess::getStorage().get_all<UiInfoRegModule>(where(
            c(&UiInfoRegModule::uiModuleName) == uiModuleName and c(&UiInfoRegModule::uiControlName) == uiControlName));
    } catch (...) {
        return {};
    }
}

std::vector<UiInfo> getUiInfoByModuleNameAndControlNameAndControlIndex(std::string moduleName, std::string controlName,
    int controlIndex)
{
    try {
        return DatabaseAccess::getStorage().get_all<UiInfo>(where(c(&UiInfo::moduleName) == moduleName
            and c(&UiInfo::controlName) == controlName and c(&UiInfo::index) == controlIndex));
    } catch (...) {
        return {};
    }
}

std::vector<AWBCalibrationRegister> getAWBRegisterModuleCombine(uint32_t registerCode)
{
    try {
        return DatabaseAccess::getStorage().get_all<AWBCalibrationRegister>(
            where(c(&AWBCalibrationRegister::registerCode) == registerCode));
    } catch (...) {
        return {};
    }
}

std::vector<CombinedRegister> getAWBRegisterCombinedId(std::string moduleName, std::string combinedName, int cameraId)
{
    try {
        if(cameraId<0)
        {
            return DatabaseAccess::getStorage().get_all<CombinedRegister>(
                where(c(&CombinedRegister::moduleName) == moduleName and c(&CombinedRegister::combinedName) == combinedName));
        }
        else
        {
            return DatabaseAccess::getStorage().get_all<CombinedRegister>(
                where(c(&CombinedRegister::moduleName) == moduleName
                    and c(&CombinedRegister::combinedName) == combinedName
                    and c(&CombinedRegister::cameraId) == cameraId));
        }
    } catch (...) {
        return {};
    }
}

std::vector<RegisterBitInfo> getAWBRegisterAddress(uint32_t combinedId)
{
    try {
        return DatabaseAccess::getStorage().get_all<RegisterBitInfo>(
            where(c(&RegisterBitInfo::combinedId) == combinedId));
    } catch (...) {
        return {};
    }
}

} // namespace DatabaseOrm
