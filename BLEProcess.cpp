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
#include "ble_logging.h"
#include "assert.h"

#define TRACE_GROUP "BleP"

BLEProcess::BLEProcess(events::EventQueue &event_queue, BLE &ble_interface)  :
_event_queue(event_queue),
_ble(ble_interface)
{
}

BLEProcess::~BLEProcess() {
    stop();
}

ble_error_t BLEProcess::start() {
    tr_info("ble process started.");

    if (_ble.hasInitialized()) {
        tr_error("the ble instance (%p) has already been initialized", (void*)(&_ble));
        return BLE_ERROR_ALREADY_INITIALIZED;
    }

    /* handle gap events */
    _gap_eh.addEventHandler(this);
    _ble.gap().setEventHandler(&_gap_eh);

    /* This will inform us of all events so we can schedule their handling
     * using our event queue */
    _ble.onEventsToProcess(
        makeFunctionPointer(this, &BLEProcess::schedule_ble_events)
    );

    ble_error_t error = _ble.init(
        this, &BLEProcess::on_init_complete
    );

    if (error) {
        ble_log_error(error, "Error returned by BLE::init");
    }

    return error;

}

void BLEProcess::stop() {
    if (_ble.hasInitialized()) {
        _ble.shutdown();
        tr_info("Ble process stopped.");
    }
}

void BLEProcess::on_init_complete(BLE::InitializationCompleteCallbackContext *event) {

    if (event->error) {
        ble_log_error(event->error, "Error during initialization");
        return;
    }

    tr_info("BLE instance (%p) initialized", (void*)(&_ble));

    /* Set the preferred PHY as per the configuration */
    ble::phy_set_t rx_phys(/* 1M */ false, /* 2M */ false, /* coded */ false);
    ble::phy_set_t tx_phys(/* 1M */ false, /* 2M */ false, /* coded */ false);

    /* Note: 2M and coded phy communication will only take place if both peers support it */

    /* RX Phy */
    if(MBED_CONF_BLE_PROCESS_PREFERRED_RX_PHY == ble::phy_t::LE_2M) {
        /** Assert that the 2M phy is supported */
        assert(_ble.gap().isFeatureSupported(ble::controller_supported_features_t::LE_2M_PHY));
        rx_phys.set_2m(true);
    } else if(MBED_CONF_BLE_PROCESS_PREFERRED_RX_PHY == ble::phy_t::LE_CODED) {
        /** Assert that the coded phy is supported */
        assert(_ble.gap().isFeatureSupported(ble::controller_supported_features_t::LE_CODED_PHY));
        rx_phys.set_coded(true);
    } else {
        /* LE_1M is the default */
        rx_phys.set_1m(true);
    }

    /* TX Phy */
    if(MBED_CONF_BLE_PROCESS_PREFERRED_TX_PHY == ble::phy_t::LE_2M) {
        /** Assert that the 2M phy is supported */
        assert(_ble.gap().isFeatureSupported(ble::controller_supported_features_t::LE_2M_PHY));
        tx_phys.set_2m(true);
    } else if(MBED_CONF_BLE_PROCESS_PREFERRED_TX_PHY == ble::phy_t::LE_CODED) {
        /** Assert that the coded phy is supported */
        assert(_ble.gap().isFeatureSupported(ble::controller_supported_features_t::LE_CODED_PHY));
        tx_phys.set_coded(true);
    } else {
        /* LE_1M is the default */
        tx_phys.set_1m(true);
    }

    ble_error_t error = _ble.gap().setPreferredPhys(&tx_phys, &rx_phys);

    if (error) {
        ble_log_error(error, "GAP::setPreferredPhys failed");
    }

    finish_initialization();

}

//void BLEProcess::onConnectionComplete(
//        const ble::ConnectionCompleteEvent &event) {
//    if (event.getStatus() == BLE_ERROR_NONE) {
//        tr_info("Connected to: ");
//        print_address(event.getPeerAddress());
//        if (_post_connect_cb) {
//            _post_connect_cb(_ble, _event_queue, event);
//        }
//    } else {
//        tr_error("Failed to connect");
//        start_activity();
//    }
//}
//
//void BLEProcess::onDisconnectionComplete(
//        const ble::DisconnectionCompleteEvent &event) {
//    tr_info("Disconnected.");
//    start_activity();
//}
//
//void BLEProcess::onAdvertisingEnd(const ble::AdvertisingEndEvent &event) {
//    start_activity();
//}
//
//void BLEProcess::start_activity() {
//    _event_queue.call([this]() { start_advertising(); });
//}
//
//void BLEProcess::start_advertising() {
//    ble_error_t error;
//
//    if (_gap.isAdvertisingActive(_adv_handle)) {
//        /* we're already advertising */
//        return;
//    }
//
//    ble::AdvertisingParameters adv_params(
//        ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
//        ble::adv_interval_t(ble::millisecond_t(40))
//    );
//
//    error = _gap.setAdvertisingParameters(_adv_handle, adv_params);
//
//    if (error) {
//        tr_error("_ble.gap().setAdvertisingParameters() failed");
//        return;
//    }
//
//    _adv_data_builder.clear();
//    _adv_data_builder.setFlags();
//    _adv_data_builder.setName(get_device_name());
//
//    /* Set payload for the set */
//    error = _gap.setAdvertisingPayload(
//        _adv_handle, _adv_data_builder.getAdvertisingData()
//    );
//
//    if (error) {
//        print_error(error, "Gap::setAdvertisingPayload() failed");
//        return;
//    }
//
//    error = _gap.startAdvertising(_adv_handle, ble::adv_duration_t(ble::millisecond_t(4000)));
//
//    if (error) {
//        print_error(error, "Gap::startAdvertising() failed");
//        return;
//    }
//
//    tr_info("Advertising as \"%s\"", get_device_name());
//}

void BLEProcess::schedule_ble_events(BLE::OnEventsToProcessCallbackContext *event) {
    _event_queue.call(mbed::callback(&event->ble, &BLE::processEvents));
}

void BLEProcess::finish_initialization(void) {
    initialization_done(BLE_ERROR_NONE);
}

void BLEProcess::initialization_done(ble_error_t err) {

    ble_log_local_mac_address(_ble);

    struct BLE::InitializationCompleteCallbackContext event = {
            .ble = _ble,
            .error = err
    };

    if (_post_init_cb) {
        _post_init_cb(&event);
    }
}




