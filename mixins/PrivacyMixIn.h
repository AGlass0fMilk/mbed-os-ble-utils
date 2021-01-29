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

template<typename BASE>
class PrivacyMixIn : public SecurityManagerMixIn<BASE>
{
public:

    PrivacyMixIn(events::EventQueue &event_queue, BLE &ble_interface) : BASE(event_queue, ble_interface) {
    }

    virtual ~PrivacyMixIn() { }

protected:

    virtual void finish_initialization() override;

    virtual void onPrivacyEnabled() override;
};

/* See this: https://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file*/
#include "PrivacyMixIn.tpp"

#endif /* BLE_FEATURE_PRIVACY */

#endif /* MBED_OS_BLE_UTILS_MIXINS_PRIVACYMIXIN_H_ */
