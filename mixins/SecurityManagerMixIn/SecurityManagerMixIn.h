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

#ifndef MBED_OS_BLE_UTILS_MIXINS_SECURITYMANAGERMIXIN_H_
#define MBED_OS_BLE_UTILS_MIXINS_SECURITYMANAGERMIXIN_H_

#include "events/EventQueue.h"
#include "BLE.h"
#include "SecurityManager.h"

/** Configuration preprocessors */

#ifndef MBED_CONF_BLE_SECURITY_MANAGER_MIXIN_ENABLE_BONDING
#define MBED_CONF_BLE_SECURITY_MANAGER_MIXIN_ENABLE_BONDING false
#endif

#ifndef MBED_CONF_BLE_SECURITY_MANAGER_MIXIN_REQUIRE_MITM
#define MBED_CONF_BLE_SECURITY_MANAGER_MIXIN_REQUIRE_MITM false
#endif

#ifndef MBED_CONF_BLE_SECURITY_MANAGER_MIXIN_IO_CAPABILITIES
#define MBED_CONF_BLE_SECURITY_MANAGER_MIXIN_IO_CAPABILITIES IO_CAPS_NONE
#endif

#ifndef MBED_CONF_BLE_SECURITY_MANAGER_MIXIN_ENABLE_SIGNING
#define MBED_CONF_BLE_SECURITY_MANAGER_MIXIN_ENABLE_SIGNING false
#endif

#define BLE_SM_TYPE_PREFIX SecurityManager::
#define BLE_SECURITY_MANAGER_IO_CAPABILITIES BLE_SM_TYPE_PREFIX MBED_CONF_BLE_SECURITY_MANAGER_MIXIN_IO_CAPABILITIES

#if BLE_FEATURE_SECURITY

/**
 * A MixIn for BLEProcess that adds BLE Security Manager features.
 */
template<typename BASE>
class SecurityManagerMixIn : public BASE, public SecurityManager::EventHandler
{
public:

    /**
     * Instantiate a BLEProcess with SecurityManagerMixIn
     * @param[in] eq EventQueue to process BLE events on
     * @param[in] ble BLE instance to use
     * @param[in] sm_db_path (optional) Path to file to store persistent security manager parameters (eg: bonding information)
     * @param[in] passkey (optional) Static 6-digit passkey to use for MITM protection, rather than generating one randomly.
     *
     * @note If no sm_db_path is provided, persistence of bonding information will not be possible
     */
    SecurityManagerMixIn(events::EventQueue &eq, BLE &ble,
            const char* sm_db_path = nullptr, const uint8_t *passkey = nullptr) :
                BASE(eq, ble), _sm_db_path(sm_db_path), _passkey(passkey) { }

    virtual ~SecurityManagerMixIn() { }

protected:

    void initialize_sm(void);

    virtual void finish_initialization() override;

    const char *_sm_db_path;
    const uint8_t *_passkey;

};

#include "SecurityManagerMixIn.tpp"

#endif /* BLE_FEATURE_SECURITY */

#endif /* MBED_OS_BLE_UTILS_MIXINS_SECURITYMANAGERMIXIN_H_ */
