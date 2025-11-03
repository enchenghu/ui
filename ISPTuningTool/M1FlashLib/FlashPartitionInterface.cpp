#include "FlashPartitionInterface.h"
#include "FlashPartitionBootLoader.h"
#include "FlashPartitionDcpc.h"
#include "FlashPartitionDdrSetting.h"
#include "FlashPartitionHwFw.h"
#include "FlashPartitionIspPatch.h"
#include "FlashPartitionKernel.h"
#include "FlashPartitionMoveData.h"
#include "FlashPartitionOsd.h"
#include "FlashPartitionSensorSetting.h"

FlashPartitionFactory::FlashPartitionFactory()
    : _partitionMap({
        { PART_ARM_BOOT, std::make_shared<FlashPartitionCreator<FlashPartitionBootLoader>>() },
        { PART_DCPC, std::make_shared<FlashPartitionCreator<FlashPartitionDcpc>>() },
        { PART_DDR_SETTING, std::make_shared<FlashPartitionCreator<FlashPartitionDdrSetting>>() },
        { PART_GENERAL_HW_SETTING, std::make_shared<FlashPartitionCreator<FlashPartitionHwSetting>>() },
        { PART_FW_SETTING, std::make_shared<FlashPartitionCreator<FlashPartitionFwSetting>>() },
        { PART_FW_RECOVER, std::make_shared<FlashPartitionCreator<FlashPartitionFirmwareRecover>>() },
        { PART_FW_PATCH, std::make_shared<FlashPartitionCreator<FlashPartitionPatch>>() },
        { PART_LINUX_KERNEL, std::make_shared<FlashPartitionCreator<FlashPartitionKernel>>() },
        { PART_MOVE_DATA, std::make_shared<FlashPartitionCreator<FlashPartitionMoveData>>() },
        { PART_OSD, std::make_shared<FlashPartitionCreator<FlashPartitionOsd>>() },
        { PART_SENSOR_SETTING, std::make_shared<FlashPartitionCreator<FlashPartitionSensorSetting>>() },
    })
{
}