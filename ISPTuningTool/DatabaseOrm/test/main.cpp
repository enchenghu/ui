#include "../databaseorm.h"
#include <chrono>
#include <crtdbg.h>
#include <direct.h> // Include the <direct.h> header for _getcwd
#include <fstream>
#include <iostream>

int main()
{
	// output current path on windows
	char buffer[256];
	_getcwd(buffer, 256);
	std::cout << buffer << std::endl;
	// check if file exists
	std::ifstream file("M1.sqlite");
	if (!file.good())
	{
		std::cout << "File not found" << std::endl;
		return 1;
	}
	try
	{
		DatabaseOrm::initDatabase("M1.sqlite");
		auto start = std::chrono::high_resolution_clock::now();
		auto end   = std::chrono::high_resolution_clock::now();
		// set seed
		srand(static_cast<unsigned int>(time(NULL)));
		int testCount = 100;
		while (testCount--)
		{
			// tesk all functions
			// std::vector<Register> getAllRegisters();
			{
				start          = std::chrono::high_resolution_clock::now();
				auto registers = DatabaseOrm::getAllRegisters();
				end            = std::chrono::high_resolution_clock::now();
				std::cout << "getAllRegisters() returned " << registers.size() << " registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// std::vector<Register> getRegistersByCameraId(std::shared_ptr<int> cameraId);
			{
				std::shared_ptr<int> cameraId = rand() % 3 == 0 ? nullptr : std::make_shared<int>(rand() % 2);
				start                         = std::chrono::high_resolution_clock::now();
				auto registersByCameraId      = DatabaseOrm::getRegistersByCameraId(cameraId);
				end                           = std::chrono::high_resolution_clock::now();
				std::cout << "getRegistersByCameraId(" << (cameraId == nullptr ? "nullptr" : std::to_string(*cameraId)) << ") returned "
						  << registersByCameraId.size() << " registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// std::vector<Register> getRegisterByModule(Module & module);
			{
				auto modules           = DatabaseOrm::getAllModules();
				int  index             = rand() % modules.size();
				start                  = std::chrono::high_resolution_clock::now();
				auto registersByModule = DatabaseOrm::getRegisterByModule(modules[index]);
				end                    = std::chrono::high_resolution_clock::now();
				std::cout << "getRegisterByModule(" << modules[index].moduleName << ") returned " << registersByModule.size()
						  << " registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
				start = end;
			}
			// std::vector<Register> getRegistersByModuleName(std::string moduleName);
			{
				auto modules               = DatabaseOrm::getAllModules();
				int  index                 = rand() % modules.size();
				start                      = std::chrono::high_resolution_clock::now();
				auto registersByModuleName = DatabaseOrm::getRegistersByModuleName(modules[index].moduleName);
				end                        = std::chrono::high_resolution_clock::now();
				std::cout << "getRegistersByModuleName(" << modules[index].moduleName << ") returned " << registersByModuleName.size()
						  << " registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// std::vector<Register> getRegistersByModuleAndCameraId(Module & module, std::shared_ptr<int> cameraId);
			{
				auto modules                      = DatabaseOrm::getAllModules();
				int  index                        = rand() % modules.size();
				start                             = std::chrono::high_resolution_clock::now();
				auto cameraId                     = rand() % 3 == 0 ? nullptr : std::make_shared<int>(rand() % 2);
				auto registersByModuleAndCameraId = DatabaseOrm::getRegistersByModuleAndCameraId(modules[index], cameraId);
				end                               = std::chrono::high_resolution_clock::now();
				std::cout << "getRegistersByModuleAndCameraId(" << modules[index].moduleName << ", "
						  << (cameraId == nullptr ? "nullptr" : std::to_string(*cameraId)) << ") returned "
						  << registersByModuleAndCameraId.size() << " registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
				start = end;
			}
			// std::vector<Register> getRegistersByModuleNameAndCameraId(std::string moduleName, std::shared_ptr<int> cameraId);
			{
				auto modules  = DatabaseOrm::getAllModules();
				int  index    = rand() % modules.size();
				start         = std::chrono::high_resolution_clock::now();
				auto cameraId = rand() % 3 == 0 ? nullptr : std::make_shared<int>(rand() % 2);
				auto registersByModuleNameAndCameraId =
					DatabaseOrm::getRegistersByModuleNameAndCameraId(modules[index].moduleName, cameraId);
				end = std::chrono::high_resolution_clock::now();
				std::cout << "getRegistersByModuleNameAndCameraId(" << modules[index].moduleName << ", "
						  << (cameraId == nullptr ? "nullptr" : std::to_string(*cameraId)) << ") returned "
						  << registersByModuleNameAndCameraId.size() << " registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
				start = end;
			}
			// Register              getRegisterByRegisterBitInfo(RegisterBitInfo & regBitInfo);
			{
				auto registers = DatabaseOrm::getAllRegisters();
				int  index     = rand() % registers.size();
				start          = std::chrono::high_resolution_clock::now();
				auto registerByRegisterBitInfo =
					DatabaseOrm::getRegisterByRegisterBitInfo(DatabaseOrm::getRegisterBitInfoByRegister(registers[index])[0]);
				end = std::chrono::high_resolution_clock::now();
				std::cout << "getRegisterByRegisterBitInfo(" << registers[index].address << ") returned "
						  << registerByRegisterBitInfo.address << " registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// Register              getRegisterByAddress(int address);
			{
				auto registers         = DatabaseOrm::getAllRegisters();
				int  index             = rand() % registers.size();
				start                  = std::chrono::high_resolution_clock::now();
				auto registerByAddress = DatabaseOrm::getRegisterByAddress(registers[index].address);
				end                    = std::chrono::high_resolution_clock::now();
				std::cout << "getRegisterByAddress(" << registers[index].address << ") returned " << registerByAddress.address
						  << " registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}

			// std::vector<Module> getAllModules();
			{
				start        = std::chrono::high_resolution_clock::now();
				auto modules = DatabaseOrm::getAllModules();
				end          = std::chrono::high_resolution_clock::now();
				std::cout << "getAllModules() returned " << modules.size() << " modules" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// Module              getModuleByRegister(Register & reg);
			{
				auto registers        = DatabaseOrm::getAllRegisters();
				int  index            = rand() % registers.size();
				start                 = std::chrono::high_resolution_clock::now();
				auto moduleByRegister = DatabaseOrm::getModuleByRegister(registers[index]);
				end                   = std::chrono::high_resolution_clock::now();
				std::cout << "getModuleByRegister(" << registers[index].address << ") returned " << moduleByRegister.moduleName
						  << " modules" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// Module              getModuleByRegisterAddress(int regAddress);
			{
				auto registers             = DatabaseOrm::getAllRegisters();
				int  index                 = rand() % registers.size();
				start                      = std::chrono::high_resolution_clock::now();
				auto moduleByRegisterAddrs = DatabaseOrm::getModuleByRegisterAddress(registers[index].address);
				end                        = std::chrono::high_resolution_clock::now();
				std::cout << "getModuleByRegisterAddress(" << registers[index].address << ") returned " << moduleByRegisterAddrs.moduleName
						  << " modules" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// Module              getModuleByRegisterBitInfo(RegisterBitInfo & regBitInfo);
			{
				auto registers = DatabaseOrm::getAllRegisters();
				int  index     = rand() % registers.size();
				start          = std::chrono::high_resolution_clock::now();
				auto moduleByRegister =
					DatabaseOrm::getModuleByRegisterBitInfo(DatabaseOrm::getRegisterBitInfoByRegister(registers[index])[0]);
				end = std::chrono::high_resolution_clock::now();
				std::cout << "getModuleByRegisterBitInfo(" << registers[index].address << ") returned " << moduleByRegister.moduleName
						  << " modules" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// Module              getModuleByCombinedRegister(CombinedRegister & combinedReg);
			{
				auto combinedRegisters        = DatabaseOrm::getAllCombinedRegisters();
				int  index                    = rand() % combinedRegisters.size();
				start                         = std::chrono::high_resolution_clock::now();
				auto moduleByCombinedRegister = DatabaseOrm::getModuleByCombinedRegister(combinedRegisters[index]);
				end                           = std::chrono::high_resolution_clock::now();
				std::cout << "getModuleByCombinedRegister(" << combinedRegisters[index].combinedName << ") returned "
						  << moduleByCombinedRegister.moduleName << " modules" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// Module              getModuleByModuleName(std::string moduleName);
			{
				auto modules            = DatabaseOrm::getAllModules();
				int  index              = rand() % modules.size();
				start                   = std::chrono::high_resolution_clock::now();
				auto moduleByModuleName = DatabaseOrm::getModuleByModuleName(modules[index].moduleName);
				end                     = std::chrono::high_resolution_clock::now();
				std::cout << "getModuleByModuleName(" << modules[index].moduleName << ") returned " << moduleByModuleName.moduleName
						  << " modules" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}

			// std::vector<CombinedRegister> getAllCombinedRegisters();
			{
				start                  = std::chrono::high_resolution_clock::now();
				auto combinedRegisters = DatabaseOrm::getAllCombinedRegisters();
				end                    = std::chrono::high_resolution_clock::now();
				std::cout << "getAllCombinedRegisters() returned " << combinedRegisters.size() << " combined registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// std::vector<CombinedRegister> getCombinedRegistersByCameraId(std::shared_ptr<int> cameraId);
			{
				std::shared_ptr<int> cameraId    = rand() % 3 == 0 ? nullptr : std::make_shared<int>(rand() % 2);
				start                            = std::chrono::high_resolution_clock::now();
				auto combinedRegistersByCameraId = DatabaseOrm::getCombinedRegistersByCameraId(cameraId);
				end                              = std::chrono::high_resolution_clock::now();
				std::cout << "getCombinedRegistersByCameraId(" << (cameraId == nullptr ? "nullptr" : std::to_string(*cameraId))
						  << ") returned " << combinedRegistersByCameraId.size() << " combined registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// std::vector<CombinedRegister> getCombinedRegistersByModuleName(std::string moduleName);
			{
				auto modules                       = DatabaseOrm::getAllModules();
				int  index                         = rand() % modules.size();
				start                              = std::chrono::high_resolution_clock::now();
				auto combinedRegistersByModuleName = DatabaseOrm::getCombinedRegistersByModuleName(modules[index].moduleName);
				end                                = std::chrono::high_resolution_clock::now();
				std::cout << "getCombinedRegistersByModuleName(" << modules[index].moduleName << ") returned "
						  << combinedRegistersByModuleName.size() << " combined registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// std::vector<CombinedRegister> getCombinedRegistersByModuleNameAndCameraId(std::string moduleName, std::shared_ptr<int>
			// cameraId);
			{
				auto modules  = DatabaseOrm::getAllModules();
				int  index    = rand() % modules.size();
				start         = std::chrono::high_resolution_clock::now();
				auto cameraId = rand() % 3 == 0 ? nullptr : std::make_shared<int>(rand() % 2);
				auto combinedRegistersByModuleNameAndCamera =
					DatabaseOrm::getCombinedRegistersByModuleNameAndCameraId(modules[index].moduleName, cameraId);
				end = std::chrono::high_resolution_clock::now();
				std::cout << "getCombinedRegistersByModuleNameAndCameraId(" << modules[index].moduleName << ", "
						  << (cameraId == nullptr ? "nullptr" : std::to_string(*cameraId)) << ") returned "
						  << combinedRegistersByModuleNameAndCamera.size() << " combined registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// std::vector<CombinedRegister> getCombinedRegistersByModule(Module & module);
			{
				auto modules                   = DatabaseOrm::getAllModules();
				int  index                     = rand() % modules.size();
				start                          = std::chrono::high_resolution_clock::now();
				auto combinedRegistersByModule = DatabaseOrm::getCombinedRegistersByModule(modules[index]);
				end                            = std::chrono::high_resolution_clock::now();
				std::cout << "getCombinedRegistersByModule(" << modules[index].moduleName << ") returned "
						  << combinedRegistersByModule.size() << " combined registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// std::vector<CombinedRegister> getCombinedRegistersByModuleAndCameraId(Module & module, std::shared_ptr<int> cameraId);
			{
				auto modules                              = DatabaseOrm::getAllModules();
				int  index                                = rand() % modules.size();
				start                                     = std::chrono::high_resolution_clock::now();
				auto cameraId                             = rand() % 3 == 0 ? nullptr : std::make_shared<int>(rand() % 2);
				auto combinedRegistersByModuleAndCameraId = DatabaseOrm::getCombinedRegistersByModuleAndCameraId(modules[index], cameraId);
				end                                       = std::chrono::high_resolution_clock::now();
				std::cout << "getCombinedRegistersByModuleAndCameraId(" << modules[index].moduleName << ", "
						  << (cameraId == nullptr ? "nullptr" : std::to_string(*cameraId)) << ") returned "
						  << combinedRegistersByModuleAndCameraId.size() << " combined registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// std::vector<CombinedRegister> getCombinedRegisterByRegister(Register & reg);
			{
				auto registers = DatabaseOrm::getAllRegisters();
				int  index     = rand() % registers.size();
				start          = std::chrono::high_resolution_clock::now();
				auto combinedRegistersByRegister =
					DatabaseOrm::getCombinedRegisterByRegister(DatabaseOrm::getRegisterByAddress(registers[index].address));
				end = std::chrono::high_resolution_clock::now();
				std::cout << "getCombinedRegisterByRegister(" << registers[index].address << ") returned "
						  << combinedRegistersByRegister.size() << " combined registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// std::vector<CombinedRegister> getCombinedRegisterByRegisterAddress(int regAddress);
			{
				auto registers                          = DatabaseOrm::getAllRegisters();
				int  index                              = rand() % registers.size();
				start                                   = std::chrono::high_resolution_clock::now();
				auto combinedRegistersByRegisterAddress = DatabaseOrm::getCombinedRegisterByRegisterAddress(registers[index].address);
				end                                     = std::chrono::high_resolution_clock::now();
				std::cout << "getCombinedRegisterByRegisterAddress(" << registers[index].address << ") returned "
						  << combinedRegistersByRegisterAddress.size() << " combined registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// CombinedRegister              getCombinedRegisterByRegisterBitInfo(RegisterBitInfo & regBitInfo);
			{
				auto registers = DatabaseOrm::getAllRegisters();
				int  index     = rand() % registers.size();
				start          = std::chrono::high_resolution_clock::now();
				auto combinedRegisterByRegisterBitInfo =
					DatabaseOrm::getCombinedRegisterByRegisterBitInfo(DatabaseOrm::getRegisterBitInfoByRegister(registers[index])[0]);
				end = std::chrono::high_resolution_clock::now();
				std::cout << "getCombinedRegisterByRegisterBitInfo(" << registers[index].address << ") returned "
						  << combinedRegisterByRegisterBitInfo.combinedName << " combined registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// CombinedRegister getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(std::string moduleName, std::shared_ptr<int>
			// cameraId,
			//                                                                            std::string combinedName);
			{
				auto modules  = DatabaseOrm::getAllModules();
				int  index    = rand() % modules.size();
				start         = std::chrono::high_resolution_clock::now();
				auto cameraId = rand() % 3 == 0 ? nullptr : std::make_shared<int>(rand() % 2);
				auto combinedRegisterByModuleNameAndCameraIdAndCombinedName =
					DatabaseOrm::getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(modules[index].moduleName, cameraId, "test");
				end = std::chrono::high_resolution_clock::now();
				std::cout << "getCombinedRegisterByModuleNameAndCameraIdAndCombinedName(" << modules[index].moduleName << ", "
						  << (cameraId == nullptr ? "nullptr" : std::to_string(*cameraId)) << ", test) returned "
						  << combinedRegisterByModuleNameAndCameraIdAndCombinedName.combinedName << " combined registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// CombinedRegister getCombinedRegisterByModuleAndCameraIdAndCombinedName(Module & module, std::shared_ptr<int> cameraId,
			//                                                                        std::string combinedName);
			{
				auto modules  = DatabaseOrm::getAllModules();
				int  index    = rand() % modules.size();
				start         = std::chrono::high_resolution_clock::now();
				auto cameraId = rand() % 3 == 0 ? nullptr : std::make_shared<int>(rand() % 2);
				auto combinedRegisterByModuleAndCameraIdAndCombinedName =
					DatabaseOrm::getCombinedRegisterByModuleAndCameraIdAndCombinedName(modules[index], cameraId, "test");
				end = std::chrono::high_resolution_clock::now();
				std::cout << "getCombinedRegisterByModuleAndCameraIdAndCombinedName(" << modules[index].moduleName << ", "
						  << (cameraId == nullptr ? "nullptr" : std::to_string(*cameraId)) << ", test) returned "
						  << combinedRegisterByModuleAndCameraIdAndCombinedName.combinedName << " combined registers" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}

			// std::vector<RegisterBitInfo> getAllRegisterBitInfo();
			{
				start                = std::chrono::high_resolution_clock::now();
				auto registerBitInfo = DatabaseOrm::getAllRegisterBitInfo();
				end                  = std::chrono::high_resolution_clock::now();
				std::cout << "getAllRegisterBitInfo() returned " << registerBitInfo.size() << " register bits" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// std::vector<RegisterBitInfo> getRegisterBitInfoByCombinedRegister(CombinedRegister & combinedReg);
			{
				auto combinedRegisters                 = DatabaseOrm::getAllCombinedRegisters();
				int  index                             = rand() % combinedRegisters.size();
				start                                  = std::chrono::high_resolution_clock::now();
				auto registerBitInfoByCombinedRegister = DatabaseOrm::getRegisterBitInfoByCombinedRegister(combinedRegisters[index]);
				end                                    = std::chrono::high_resolution_clock::now();
				std::cout << "getRegisterBitInfoByCombinedRegister(" << combinedRegisters[index].combinedName << ") returned "
						  << registerBitInfoByCombinedRegister.size() << " register bits" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// std::vector<RegisterBitInfo> getRegisterBitInfoByCombinedRegisterId(int combinedId);
			{
				auto combinedRegisters = DatabaseOrm::getAllCombinedRegisters();
				int  index             = rand() % combinedRegisters.size();
				start                  = std::chrono::high_resolution_clock::now();
				auto registerBitInfoByCombinedRegisterId =
					DatabaseOrm::getRegisterBitInfoByCombinedRegisterId(combinedRegisters[index].combinedId);
				end = std::chrono::high_resolution_clock::now();
				std::cout << "getRegisterBitInfoByCombinedRegisterId(" << combinedRegisters[index].combinedId << ") returned "
						  << registerBitInfoByCombinedRegisterId.size() << " register bits" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// std::vector<RegisterBitInfo> getRegisterBitInfoByRegister(Register & reg);
			{
				auto registers                 = DatabaseOrm::getAllRegisters();
				int  index                     = rand() % registers.size();
				start                          = std::chrono::high_resolution_clock::now();
				auto registerBitInfoByRegister = DatabaseOrm::getRegisterBitInfoByRegister(registers[index]);
				end                            = std::chrono::high_resolution_clock::now();
				std::cout << "getRegisterBitInfoByRegister(" << registers[index].address << ") returned "
						  << registerBitInfoByRegister.size() << " register bits" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// std::vector<RegisterBitInfo> getRegisterBitInfoByRegisterAddress(int regAddress);
			{
				auto registers                        = DatabaseOrm::getAllRegisters();
				int  index                            = rand() % registers.size();
				start                                 = std::chrono::high_resolution_clock::now();
				auto registerBitInfoByRegisterAddress = DatabaseOrm::getRegisterBitInfoByRegisterAddress(registers[index].address);
				end                                   = std::chrono::high_resolution_clock::now();
				std::cout << "getRegisterBitInfoByRegisterAddress(" << registers[index].address << ") returned "
						  << registerBitInfoByRegisterAddress.size() << " register bits" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// std::vector<RegisterBitInfo> getRegisterBitInfoByRegisterName(std::string regName);
			{
				auto registerbitinfo               = DatabaseOrm::getAllRegisterBitInfo();
				int  index                         = rand() % registerbitinfo.size();
				start                              = std::chrono::high_resolution_clock::now();
				auto registerBitInfoByRegisterName = DatabaseOrm::getRegisterBitInfoByRegisterName(registerbitinfo[index].registerBitName);
				end                                = std::chrono::high_resolution_clock::now();
				std::cout << "getRegisterBitInfoByRegisterName(" << registerbitinfo[index].registerBitName << ") returned "
						  << registerBitInfoByRegisterName.size() << " register bits" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// std::vector<RegisterBitInfo> getRegisterBitInfoByRegisterAddressAndBitMask(int regAddress, int bitMask);
			{
				auto registerbitinfo                            = DatabaseOrm::getAllRegisterBitInfo();
				int  index                                      = rand() % registerbitinfo.size();
				start                                           = std::chrono::high_resolution_clock::now();
				auto registerBitInfoByRegisterAddressAndBitMask = DatabaseOrm::getRegisterBitInfoByRegisterAddressAndBitMask(
					registerbitinfo[index].registerAddr, registerbitinfo[index].bitMask);
				end = std::chrono::high_resolution_clock::now();
				std::cout << "getRegisterBitInfoByRegisterAddressAndBitMask(" << registerbitinfo[index].registerAddr << ", "
						  << registerbitinfo[index].bitMask << ") returned " << registerBitInfoByRegisterAddressAndBitMask.size()
						  << " register bits" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// RegisterBitInfo              getRegisterBitInfoByRegisterBitName(std::string regBitInfoName);
			{
				auto registerbitinfo = DatabaseOrm::getAllRegisterBitInfo();
				int  index           = rand() % registerbitinfo.size();
				start                = std::chrono::high_resolution_clock::now();
				auto registerBitInfoByRegisterBitName =
					DatabaseOrm::getRegisterBitInfoByRegisterBitName(registerbitinfo[index].registerBitName);
				end = std::chrono::high_resolution_clock::now();
				std::cout << "getRegisterBitInfoByRegisterBitName(" << registerbitinfo[index].registerBitName << ") returned "
						  << registerBitInfoByRegisterBitName.registerBitName << " register bits" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
			// RegisterBitInfo              getRegisterBitInfoByRegisterAndBitMask(Register & reg, int bitMask);
			{
				auto registerbitinfo                     = DatabaseOrm::getAllRegisterBitInfo();
				int  index                               = rand() % registerbitinfo.size();
				start                                    = std::chrono::high_resolution_clock::now();
				auto registerBitInfoByRegisterAndBitMask = DatabaseOrm::getRegisterBitInfoByRegisterAndBitMask(
					DatabaseOrm::getRegisterByAddress(registerbitinfo[index].registerAddr), registerbitinfo[index].bitMask);
				end = std::chrono::high_resolution_clock::now();
				std::cout << "getRegisterBitInfoByRegisterAndBitMask(" << registerbitinfo[index].registerAddr << ", "
						  << registerbitinfo[index].bitMask << ") returned " << registerBitInfoByRegisterAndBitMask.registerBitName
						  << " register bits" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}

			// std::vector<Permission> getAllPermissions();
			{
				start            = std::chrono::high_resolution_clock::now();
				auto permissions = DatabaseOrm::getAllPermissions();
				end              = std::chrono::high_resolution_clock::now();
				std::cout << "getAllPermissions() returned " << permissions.size() << " permissions" << std::endl;
				std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us"
						  << std::endl
						  << std::endl;
			}
		}
	}
	catch (std::system_error &e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}

	return 0;
}