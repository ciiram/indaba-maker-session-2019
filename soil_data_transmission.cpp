#include "select_program.h"

#if PROGRAM == SOIL_TRANSMIT

#include "mbed.h"
#include "mbed.h"
#include "mbed_trace.h"
#include "mbed_events.h"
#include "LoRaWANInterface.h"
#include "SX1276_LoRaRadio.h"
#include "CayenneLPP.h"
#include "lora_radio_helper.h"
#include "standby.h"
#include "DHT.h"
#include "device_addresses.h"


// The port we're sending and receiving on
#define MBED_CONF_LORA_APP_PORT     15

static const int STANDBY_TIME_S = 60;  // standby time in seconds
static const int SENSOR_READ_ATTEMPTS = 3;  // In case of sensor failure,
// we repeat sensing 3 times
static const int SENSOR_WAIT_TIME_MS = 3000;  // slow sensor,
// read no more than once per 2 seconds



// Sensors
static AnalogIn soil_moisture(A2); // Soil Moisture sensor

// EventQueue is required to dispatch events around
static EventQueue ev_queue;

// Constructing Mbed LoRaWANInterface and passing it down the radio object.
static LoRaWANInterface lorawan(radio);

// Application specific callbacks
static lorawan_app_callbacks_t callbacks;

// LoRaWAN stack event handler
static void lora_event_handler(lorawan_event_t event);

// Send a message over LoRaWAN
static void send_message() {
    CayenneLPP payload(50);
    int attempt = 0;

    float moisture = 0.0f;

    moisture = soil_moisture.read();

    payload.addAnalogInput(4, moisture * 100);
    printf("\rMoisture Level: %.1f%%\n", moisture * 100);

    if (payload.getSize() > 0) {
      printf("Sending %d bytes\n", payload.getSize());

      int16_t retcode = lorawan.send(MBED_CONF_LORA_APP_PORT,
                                       payload.getBuffer(),
                                       payload.getSize(),
                                       MSG_UNCONFIRMED_FLAG);

    // for some reason send() ret\urns -1... I cannot find out why,
    // the stack returns the right number. I feel that this is some
    // weird Emscripten quirk
    if (retcode < 0) {
        retcode == LORAWAN_STATUS_WOULD_BLOCK ? printf("send - duty cycle violation\n")
            : printf("send() - Error code %d\n", retcode);

        standby(STANDBY_TIME_S);
      }

      printf("%d bytes scheduled for transmission\n", retcode);
    } else {
      standby(STANDBY_TIME_S);
    }
}


int main() {
    set_time(0);

    printf("\r============================\n");
    printf("\r  Soil Moisture Sensors     \n");
    printf("\r============================\n");

    printf("Sending every %d seconds\n", STANDBY_TIME_S);

    // Enable trace output for this demo,
    // so we can see what the LoRaWAN stack does
    mbed_trace_init();

    if (lorawan.initialize(&ev_queue) != LORAWAN_STATUS_OK) {
        printf("LoRa initialization failed!\n");
        return -1;
    }

    // prepare application callbacks
    callbacks.events = mbed::callback(lora_event_handler);
    lorawan.add_app_callbacks(&callbacks);

    // Disable adaptive data rating
    if (lorawan.disable_adaptive_datarate() != LORAWAN_STATUS_OK) {
        printf("\rdisable_adaptive_datarate failed!\n");
        return -1;
    }

    lorawan.set_datarate(0);  // SF12BW125

    lorawan_connect_t connect_params;
    connect_params.connect_type = LORAWAN_CONNECTION_ABP;

    connect_params.connection_u.abp.dev_addr = DEVADDR;
    connect_params.connection_u.abp.nwk_skey = NWKSKEY;
    connect_params.connection_u.abp.app_skey = APPSKEY;

    lorawan_status_t retcode = lorawan.connect(connect_params);

    if (retcode == LORAWAN_STATUS_OK ||
        retcode == LORAWAN_STATUS_CONNECT_IN_PROGRESS) {
    } else {
        printf("Connection error, code = %d\n", retcode);
        return -1;
    }

    printf("Connection - In Progress ...\r\n");

    // make your event queue dispatching events forever
    ev_queue.dispatch_forever();
}

// This is called from RX_DONE, so whenever a message came in
static void receive_message()  {
    uint8_t rx_buffer[50] = { 0 };
    int16_t retcode;
    retcode = lorawan.receive(MBED_CONF_LORA_APP_PORT, rx_buffer,
                              sizeof(rx_buffer),
                              MSG_UNCONFIRMED_FLAG);

    if (retcode < 0) {
        printf("receive() - Error code %d\n", retcode);
        return;
    }

    printf("Data received on port %d (length %d): ",
           MBED_CONF_LORA_APP_PORT, retcode);

    for (uint8_t i = 0; i < retcode; i++) {
        printf("%02x ", rx_buffer[i]);
    }
    printf("\n");
}

// Event handler
static void lora_event_handler(lorawan_event_t event) {
    switch (event) {
        case CONNECTED:
            printf("Connection - Successful\n");
            ev_queue.call_in(1000, &send_message);
            break;
        case DISCONNECTED:
            ev_queue.break_dispatch();
            printf("Disconnected Successfully\n");
            break;
        case TX_DONE:
            printf("Message Sent to Network Server\n");
            standby(STANDBY_TIME_S);
            break;
        case TX_TIMEOUT:
        case TX_ERROR:
        case TX_CRYPTO_ERROR:
        case TX_SCHEDULING_ERROR:
            printf("Transmission Error - EventCode = %d\n", event);
            standby(STANDBY_TIME_S);
            break;
        case RX_DONE:
            printf("Received message from Network Server\n");
            receive_message();
            break;
        case RX_TIMEOUT:
        case RX_ERROR:
            printf("Error in reception - Code = %d\n", event);
            break;
        case JOIN_FAILURE:
            printf("OTAA Failed - Check Keys\n");
            break;
        default:
            MBED_ASSERT("Unknown Event");
    }
}

#endif
