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

#ifndef MBED_OS_BLE_UTILS_MIXINS_PRIVACYMIXIN_H_
#define MBED_OS_BLE_UTILS_MIXINS_PRIVACYMIXIN_H_

#include "events/EventQueue.h"
#include "BLE.h"

#include "SecurityManagerMixIn.h"

#if BLE_FEATURE_PRIVACY

/**
 * A MixIn for BLEProcess that adds BLE privacy features.
 *
 * Privacy requires the Security Manager, and so PrivacyMixIn inherits
 * from the SecurityManagerMixIn class.
 */
template<typename BASE>
class PrivacyMixIn : public SecurityManagerMixIn<BASE>
{
public:

    /**
     * Instantiate a BLEProcess with PrivacyMixIn
     *
     * For parameter documentation, see SecurityManagerMixIn
     */
    PrivacyMixIn(events::EventQueue &eq, BLE &ble,
            const char* sm_db_path = nullptr, const uint8_t *passkey = nullptr) :
                SecurityManagerMixIn<BASE>(eq, ble, sm_db_path, passkey) { }

    virtual ~PrivacyMixIn() { }

protected:

    virtual void finish_initialization() override;

    virtual void onPrivacyEnabled() override;
};

/* See this: https://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file*/
#include "PrivacyMixIn.tpp"

#endif /* BLE_FEATURE_PRIVACY */

#endif /* MBED_OS_BLE_UTILS_MIXINS_PRIVACYMIXIN_H_ */
