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

#ifndef _BLEPROCESS_H_
#define _BLEPROCESS_H_

#include <stdint.h>
#include <events/mbed_events.h>

#include "platform/Callback.h"
#include "platform/NonCopyable.h"
#include "platform/mbed_toolchain.h"

#include "ble/BLE.h"
#include "Gap.h"
#include "gap/ChainableGapEventHandler.h"

/** Configuration preprocessors */

#ifndef MBED_CONF_BLE_PROCESS_PREFERRED_TX_PHY
#define MBED_CONF_BLE_PROCESS_PREFERRED_TX_PHY LE_1M
#endif

#ifndef MBED_CONF_BLE_PROCESS_PREFERRED_RX_PHY
#define MBED_CONF_BLE_PROCESS_PREFERRED_RX_PHY LE_1M
#endif

/**
 * Handle initialization and shutdown of the BLE Instance.
 * It will also run the  event queue and call your post init callback when everything is up and running.
 */
class BLEProcess : private mbed::NonCopyable<BLEProcess>, public ble::Gap::EventHandler
{

public:

    using post_init_cb_t = mbed::Callback<void(BLE::InitializationCompleteCallbackContext*)>;

public:
    /**
     * Construct a BLEProcess from an event queue and a ble interface.
     * Call start() to initiate ble processing.
     * @param[in] event_queue Event Queue to process BLE events on
     * @param[in] ble_interface BLE interface to use
     */
    BLEProcess(events::EventQueue &event_queue, BLE &ble_interface);

    virtual ~BLEProcess();

    /**
     * Initialize the ble interface, configure it and start the process.
     */
    ble_error_t start();

    /**
     * Stop the process.
     */
    void stop();

    /**
     * Subscription to the ble interface initialization event.
     *
     * @param[in] cb The callback object that will be called when the ble interface is initialized.
     */
    void on_init(post_init_cb_t cb) {
        _post_init_cb = cb;
    }

    /** Name we advertise as. */
    virtual const char* get_device_name()
    {
        static const char name[] = "MbedBLE";
        return name;
    }

    ChainableGapEventHandler& gap_event_chain() {
        return _gap_eh;
    }

protected:

    /**
     * This function is invoked when the ble interface is initialized.
     */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *event);

    /**
     * Schedule processing of events from the BLE middleware in the event queue.
     */
    void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *event);

    /**
     * Finish initialization process
     * This method must be implemented by the subclass process
     */
    virtual void finish_initialization(void);

    /**
     * Must be called by the subclass after initialization is finished.
     */
    void initialization_done(ble_error_t err);

protected:

    events::EventQueue &_event_queue;
    BLE &_ble;
    ChainableGapEventHandler _gap_eh;

    /** Application post init callback */
    post_init_cb_t _post_init_cb;


};

#endif /* _BLEPROCESS_H_ */
