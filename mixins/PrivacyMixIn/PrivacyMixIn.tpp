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
void PrivacyMixIn<BASE>::finish_initialization() {
    ble_tr_debug("PrivacyMixIn::finish_initialization() called");
    ble_error_t err;

    /* Initialize the SecurityManager (required for privacy) */
    SecurityManagerMixIn<BASE>::initialize_sm();

    err = this->_ble.gap().enablePrivacy(true);
    if (err) {
        ble_log_error(err, "Error enabling privacy");
        return;
    }

    /* continuation is in onPrivacyEnabled() */
}

template<typename BASE>
void PrivacyMixIn<BASE>::onPrivacyEnabled() {

    ble_tr_info("Privacy Enabled");
    BASE::finish_initialization();

}



