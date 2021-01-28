/*
 * Mbed-OS Microcontroller Library
 * Copyright (c) 2020 Embedded Planet
 * Copyright (c) 2020 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 */

#ifndef _BLE_LOGGING_H_
#define _BLE_LOGGING_H_

#include "ble/BLE.h"

/**
 * Log a BLE error
 * @param[in] error Error type to log
 * @param[in] msg Message to prefix error log with
 */
void ble_log_error(ble_error_t error, const char *msg);

/**
 * Log a ble::address_t
 * @param[in] addr Address to log
 */
void ble_log_address(const ble::address_t &addr);

/**
 * Log local device MAC address
 */
void ble_log_local_mac_address();

/**
 * Print the given address into the given buffer.
 * @param[in] buf Buffer to print address into
 * @param[in] addr Address to print into buf
 *
 * @note buf must be at least 19 chars to ensure the whole address fits
 */
void ble_log_sprintf_address(char *buf, const ble::address_t &addr);

/**
 * Convert a ble::phy_t to its corresponding string representation
 * @param[in] phy Phy to convert
 * @retval String representation of phy
 */
const char* phy_to_string(ble::phy_t phy);

#endif /* _BLE_LOGGING_H_ */
