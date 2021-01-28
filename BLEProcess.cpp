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

#include "BLEProcess.h"

#include "mbed_trace.h"

#define TRACE_GROUP "BleP"

BLEProcess::BLEProcess(events::EventQueue &event_queue, BLE &ble_interface)  :
_event_queue(event_queue),
_ble(ble_interface),
_gap(ble_interface.gap()),
_adv_data_builder(_adv_buffer)
{
}

BLEProcess::~BLEProcess() {
    stop();
}

void BLEProcess::start() {
    tr_info("Ble process started.\r\n");

    if (_ble.hasInitialized()) {
        tr_error("Error: the ble instance has already been initialized.\r\n");
        return;
    }

    /* handle gap events */
    _gap.setEventHandler(this);

    /* This will inform us off all events so we can schedule their handling
     * using our event queue */
    _ble.onEventsToProcess(
        makeFunctionPointer(this, &BLEProcess::schedule_ble_events)
    );

    ble_error_t error = _ble.init(
        this, &BLEProcess::on_init_complete
    );

    if (error) {
        tr_error(error, "Error returned by BLE::init.\r\n");
        return;
    }

    // Process the event queue.
    _event_queue.dispatch_forever();

    return;
}

void BLEProcess::stop() {
    if (_ble.hasInitialized()) {
        _ble.shutdown();
        tr_info("Ble process stopped.");
    }
}

void BLEProcess::on_init_complete(
        BLE::InitializationCompleteCallbackContext *event) {
    if (event->error) {
        print_error(event->error, "Error during the initialisation\r\n");
        return;
    }

    tr_info("BLE instance initialized\r\n");

    /* All calls are serialised on the user thread through the event queue */
    start_activity();

    if (_post_init_cb) {
        _post_init_cb(_ble, _event_queue);
    }
}

void BLEProcess::onConnectionComplete(
        const ble::ConnectionCompleteEvent &event) {
    if (event.getStatus() == BLE_ERROR_NONE) {
        tr_info("Connected to: ");
        print_address(event.getPeerAddress());
        if (_post_connect_cb) {
            _post_connect_cb(_ble, _event_queue, event);
        }
    } else {
        tr_error("Failed to connect\r\n");
        start_activity();
    }
}

void BLEProcess::onDisconnectionComplete(
        const ble::DisconnectionCompleteEvent &event) {
    tr_info("Disconnected.\r\n");
    start_activity();
}

void BLEProcess::onAdvertisingEnd(const ble::AdvertisingEndEvent &event) {
    start_activity();
}

void BLEProcess::start_activity() {
    _event_queue.call([this]() { start_advertising(); });
}

void BLEProcess::start_advertising() {
    ble_error_t error;

    if (_gap.isAdvertisingActive(_adv_handle)) {
        /* we're already advertising */
        return;
    }

    ble::AdvertisingParameters adv_params(
        ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
        ble::adv_interval_t(ble::millisecond_t(40))
    );

    error = _gap.setAdvertisingParameters(_adv_handle, adv_params);

    if (error) {
        tr_error("_ble.gap().setAdvertisingParameters() failed\r\n");
        return;
    }

    _adv_data_builder.clear();
    _adv_data_builder.setFlags();
    _adv_data_builder.setName(get_device_name());

    /* Set payload for the set */
    error = _gap.setAdvertisingPayload(
        _adv_handle, _adv_data_builder.getAdvertisingData()
    );

    if (error) {
        print_error(error, "Gap::setAdvertisingPayload() failed\r\n");
        return;
    }

    error = _gap.startAdvertising(_adv_handle, ble::adv_duration_t(ble::millisecond_t(4000)));

    if (error) {
        print_error(error, "Gap::startAdvertising() failed\r\n");
        return;
    }

    tr_info("Advertising as \"%s\"\r\n", get_device_name());
}

void BLEProcess::schedule_ble_events(BLE::OnEventsToProcessCallbackContext *event) {
    _event_queue.call(mbed::callback(&event->ble, &BLE::processEvents));
}
