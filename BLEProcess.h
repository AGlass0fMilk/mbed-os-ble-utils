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
#include "pretty_printer.h"

#include <events/mbed_events.h>
#include "platform/Callback.h"
#include "platform/NonCopyable.h"
#include "platform/mbed_toolchain.h"

#include "ble/BLE.h"
#include "Gap.h"
#include "gap/AdvertisingDataParser.h"
#include "ble/common/FunctionPointerWithContext.h"

static const uint16_t MAX_ADVERTISING_PAYLOAD_SIZE = 50;

/**
 * Handle initialization and shutdown of the BLE Instance.
 * It will also run the  event queue and call your post init callback when everything is up and running.
 */
class BLEProcess : private mbed::NonCopyable<BLEProcess>, public ble::Gap::EventHandler
{
public:
    /**
     * Construct a BLEProcess from an event queue and a ble interface.
     * Call start() to initiate ble processing.
     */
    BLEProcess(events::EventQueue &event_queue, BLE &ble_interface);

    virtual ~BLEProcess();

    /**
     * Initialize the ble interface, configure it and start advertising.
     */
    void start();

    /**
     * Close existing connections and stop the process.
     */
    void stop();

    /**
     * Subscription to the ble interface initialization event.
     *
     * @param[in] cb The callback object that will be called when the ble interface is initialized.
     */
    void on_init(mbed::Callback<void(BLE&, events::EventQueue&)> cb) {
        _post_init_cb = cb;
    }

    /**
     * Set callback for a succesful connection.
     *
     * @param[in] cb The callback object that will be called when we connect to a peer
     */
    MBED_DEPRECATED("BLEProcess::on_connect is deprecated. Instead, see BLEProcess::add_gap_event_handler")
    void on_connect(mbed::Callback<void(BLE&, events::EventQueue&, const ble::ConnectionCompleteEvent &event)> cb) {
        _post_connect_cb = cb;
    }

    /**
     * Allow the application to register Gap::EventHandler instances to the internal ChainableGapEventHandler
     * @param[in] eh Gap::EventHandler instance to add to event handler chain
     */
    //void add_gap_event_handler();

    /** Name we advertise as. */
    virtual const char* get_device_name()
    {
        static const char name[] = "BleProcess";
        return name;
    }

protected:
    /**
     * Sets up adverting payload and start advertising.
     * This function is invoked when the ble interface is initialized.
     */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *event);

    /**
     * Start the gatt client process when a connection event is received.
     * This is called by Gap to notify the application we connected
     */
    void onConnectionComplete(const ble::ConnectionCompleteEvent &event) override;

    /**
     * Stop the gatt client process when the device is disconnected then restart advertising.
     * This is called by Gap to notify the application we disconnected
     */
    void onDisconnectionComplete(const ble::DisconnectionCompleteEvent &event) override;

    /** Restarts main activity */
    void onAdvertisingEnd(const ble::AdvertisingEndEvent &event);

    /**
     * Start advertising or scanning. Triggered by init or disconnection.
     */
    virtual void start_activity();

    /**
     * Start the advertising process; it ends when a device connects.
     */
    void start_advertising();

    /**
     * Schedule processing of events from the BLE middleware in the event queue.
     */
    void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *event);

protected:
    events::EventQueue &_event_queue;
    BLE &_ble;
    ble::Gap &_gap;

    uint8_t _adv_buffer[MAX_ADVERTISING_PAYLOAD_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;

    ble::advertising_handle_t _adv_handle = ble::LEGACY_ADVERTISING_HANDLE;

    mbed::Callback<void(BLE&, events::EventQueue&)> _post_init_cb;
    mbed::Callback<void(BLE&, events::EventQueue&, const ble::ConnectionCompleteEvent &event)> _post_connect_cb;
};

#endif /* _BLEPROCESS_H_ */
