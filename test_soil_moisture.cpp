#include "select_program.h"

#if PROGRAM == TEST_SOIL

#include "mbed.h"


AnalogIn soil_moisture(A2); // Soil Moisture sensor



int main() {

    float moisture;
    printf("\r\nSoil Moisture Sensor Test program");
    printf("\r\n******************\r\n");

    while (1) {
        moisture = soil_moisture.read();
        printf("\rMoisture Level: %.1f%%\n", moisture * 100);

        wait(5);
    }
}


#endif
