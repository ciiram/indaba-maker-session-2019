#include "select_program.h"

#if PROGRAM == TEST_TEMP

#include "mbed.h"
#include "DHT.h"

DHT sensor(D7, SEN51035P); // Temperature sensor



int main() {
    int err;
    printf("\r\nDHT Test program");
    printf("\r\n******************\r\n");
    wait(1); // wait 1 second for device stable status
    while (1) {
        err = sensor.readData();
        if (err == 0) {
            printf("\rTemperature is %4.2f C \r\n", sensor.ReadTemperature(CELCIUS));
            printf("\rHumidity is %4.2f \r\n", sensor.ReadHumidity());

        } else {
            printf("\r\nErr %i \n",err);
        }

        wait(5);
    }
}


#endif
