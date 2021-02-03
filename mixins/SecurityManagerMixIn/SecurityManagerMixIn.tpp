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

#include "ble_logging.h"

template<typename BASE>
void SecurityManagerMixIn<BASE>::finish_initialization() {
    ble_tr_debug("SecurityManagerMixIn::finish_initialization() called");
    initialize_sm();
    BASE::finish_initialization();
}

template<typename BASE>
void SecurityManagerMixIn<BASE>::initialize_sm() {

    ble_error_t err;

    /* The _sm_db_path will be used to store bonding information but will fallback
     * to storing in memory if file access fails (for example due to lack of a filesystem) */
    err = this->_ble.securityManager().init(
        /* enableBonding */ MBED_CONF_BLE_SECURITY_MANAGER_MIXIN_ENABLE_BONDING,
        /* requireMITM */ MBED_CONF_BLE_SECURITY_MANAGER_MIXIN_REQUIRE_MITM,
        /* iocaps */ BLE_SECURITY_MANAGER_IO_CAPABILITIES,
        /* passkey */ _passkey,
        /* signing */ MBED_CONF_BLE_SECURITY_MANAGER_MIXIN_ENABLE_SIGNING,
        /* dbFilepath */ _sm_db_path
    );

    if (err) {
        ble_log_error(err, "Error during security manager initialization");
        return;
    }

    ble_tr_debug("SecurityManager initialized");

    /** Set this as the SM event handler */
    this->_ble.securityManager().setSecurityManagerEventHandler(this);
}


