/*
 * Copyright (C) 2020 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include <map>
#include <list>
#include <string>
#include <vector>
#include <fstream>
#ifdef __cplusplus
extern "C" {
#endif
#include <errno.h>
#define LOG_TAG "bootcontrolhal"
#include <log/log.h>
#include <hardware/boot_control.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <cutils/properties.h>

#include "bctl_metadata.h"

using namespace std;

static bool validateMetadata(bctl_metadata_t& data) {
    if (data.slot_info[0].magic != BCTL_METADATA_MAGIC || data.slot_info[1].magic != BCTL_METADATA_MAGIC) {
        return false;
    }
    
    return true;
}

static bool readMetadata(bctl_metadata_t& data) {
    std::fstream in(BCTL_METADATA_PARTITION, std::ios::binary | std::ios::in);

    if (in.fail()) {
        return false;
    }

    in.seekg(BCTL_METADATA_OFFSET);

    if (in.fail()) {
        return false;
    }

    in.read(reinterpret_cast<char*>(&data), sizeof(bctl_metadata_t));

    if (!validateMetadata(data))
        return false;

    return !in.eof() && !in.fail();
}

static bool writeMetadata(bctl_metadata_t& data) {
    if (!validateMetadata(data))
        return false;

    // We use std::ios::in | std::ios::out even though we only write so that
    // we don't truncate or append to the file, but rather overwrite the file
    // in the exact place that we want to write the struct to.
    std::fstream out(BCTL_METADATA_PARTITION, std::ios::binary | std::ios::in | std::ios::out);

    if (out.fail()) {
        return false;
    }

    out.seekp(BCTL_METADATA_OFFSET);

    if (out.fail()) {
        return false;
    }

    out.write(reinterpret_cast<const char*>(&data), sizeof(bctl_metadata_t));

    return !out.eof() && !out.fail();
}

void boot_control_init(struct boot_control_module *)
{
}


unsigned get_number_slots(struct boot_control_module *)
{
	return 2;
}

unsigned get_current_slot(struct boot_control_module *)
{
    bctl_metadata_t data;

    if (readMetadata(data)) {
        // This is a clever hack because if slot b is active,
        // is_active will be 0 and if slot a is active, is_active
        // will be 1. In other words, the "not" value of slot A's
        // is_active var lines up to the current active slot index.
        return !data.slot_info[0].is_active;
    }

    return 0;
}


int mark_boot_successful(struct boot_control_module *)
{
    bctl_metadata_t data;
    bool success = false;
    int active_slot;

    if (readMetadata(data)) {
        active_slot = !data.slot_info[0].is_active;
        
        data.slot_info[active_slot].boot_successful = 1;
        data.slot_info[active_slot].tries_remaining = 0;

        if (success)
            if (writeMetadata(data)) {
                return 0;
            } else {
                ALOGE("%s: Failed to write metadata", __func__);
            }
        else {
            ALOGE("%s: No active slot", __func__);
        }
    } else {
        ALOGE("%s: Failed to read metadata", __func__);
    }

    return -1;
}

const char *get_suffix(struct boot_control_module *, unsigned slot)
{
    if (slot < 2) {
        if (slot == 0) {
            return SLOT_SUFFIX_A;
        } else {
            return SLOT_SUFFIX_B;
        }
    }
    // default to slot A
    return SLOT_SUFFIX_A;

}

int set_active_boot_slot(struct boot_control_module *, unsigned slot)
{
    bctl_metadata_t data;
    int slot2 = (slot == 0) ? 1 : 0;

    if (slot < 2) {
        if (readMetadata(data)) {
            data.slot_info[slot].bootable = 1;
            data.slot_info[slot].is_active = 1;
            data.slot_info[slot].boot_successful = 0;
            data.slot_info[slot].tries_remaining = 7;
            
            data.slot_info[slot2].bootable = 1;
            data.slot_info[slot2].is_active = 0;
            data.slot_info[slot2].boot_successful = 0;
            data.slot_info[slot2].tries_remaining = 7;

            if (writeMetadata(data)) {
                return 0;
            } else {
                ALOGE("%s: Failed to write metadata", __func__);
            }
        } else {
            ALOGE("%s: Failed to read metadata", __func__);
        }
    } else {
        ALOGE("%s: Invalid slot", __func__);
    }

    return -1;
}

int set_slot_as_unbootable(struct boot_control_module *, unsigned slot)
{
    bctl_metadata_t data;

    if (slot < 2) {
        if (readMetadata(data)) {
            data.slot_info[slot].bootable = 0;

            if (writeMetadata(data)) {
                return 0;
            } else {
                ALOGE("%s: Failed to write metadata", __func__);
            }
        } else {
            ALOGE("%s: Failed to read metadata", __func__);
        }
    } else {
        ALOGE("%s: Invalid slot", __func__);
    }

    return -1;
}

int is_slot_bootable(struct boot_control_module *, unsigned slot)
{
    bctl_metadata_t data;
    int ret = -1;

    if (slot < 2) {
        if (readMetadata(data)) {
            ret = static_cast<int>(data.slot_info[slot].bootable);
        } else {
            ret = 0;
        }
    }

    return ret;
}

int is_slot_marked_successful(struct boot_control_module *, unsigned slot)
{
    bctl_metadata_t data;
    int ret = -1;

    if (slot < 2) {
        if (readMetadata(data)) {
            ret = static_cast<int>(data.slot_info[slot].boot_successful);
        } else {
            ret = 0;
        }
    } else {
        ret = -1;
    }

    return ret;
}

static hw_module_methods_t boot_control_module_methods = {
	.open = NULL,
};

boot_control_module_t HAL_MODULE_INFO_SYM = {
	.common = {
		.tag = HARDWARE_MODULE_TAG,
		.module_api_version = 1,
		.hal_api_version = 0,
		.id = BOOT_CONTROL_HARDWARE_MODULE_ID,
		.name = "Boot control HAL",
		.author = "Code Aurora Forum",
		.methods = &boot_control_module_methods,
	},
	.init = boot_control_init,
	.getNumberSlots = get_number_slots,
	.getCurrentSlot = get_current_slot,
	.markBootSuccessful = mark_boot_successful,
	.setActiveBootSlot = set_active_boot_slot,
	.setSlotAsUnbootable = set_slot_as_unbootable,
	.isSlotBootable = is_slot_bootable,
	.getSuffix = get_suffix,
	.isSlotMarkedSuccessful = is_slot_marked_successful,
};
#ifdef __cplusplus
}
#endif