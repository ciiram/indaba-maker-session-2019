# Indaba Maker Session 2019

In this session we will develop software to collect data from sensors connected to the [NUCLEO F446RE](https://os.mbed.com/platforms/ST-Nucleo-F446RE/) board. We will also visualise the data collected and perform interpolation using Gaussian process regression.

## Preparation

Before the session, please do the following

1. Install Python 3.  Anaconda is a good choice [https://www.anaconda.com/distribution/](https://www.anaconda.com/distribution/)
1. Sign up for an Mbed account [https://os.mbed.com/account/signup/](https://os.mbed.com/account/signup/).
1. Clone this repository and cd into it
1. Create a [virtual environment](https://docs.python.org/3/tutorial/venv.html)
`python3 -m venv ttn`
1. Activate it
On Linux
`source ttn/bin/activate`
On Windows
`ttn\Scripts\activate.bat`
1. Install the requirements
`pip install -r requirements.txt`
1. Install InfluxDB as shown [here (Linux and Mac)](https://docs.influxdata.com/influxdb/v1.7/introduction/installation/) - [Instructions for Windows](https://docs.google.com/document/d/1_5_oCk_9x3BNI6-9pCWj46zaXa0hJ2BBzWs-c3ZNUz8/edit?usp=sharing)
1. Install software to obtain console output - we will need this to see the output from the microcontrollers
### Software to obtain console output
**Windows**

If you are on Windows, install:

1. [ST Link](http://janjongboom.com/downloads/st-link.zip) - serial driver for the board.
    * Run `dpinst_amd64` on 64-bits Windows, `dpinst_x86` on 32-bits Windows.
1. [Tera term](https://osdn.net/projects/ttssh2/downloads/66361/teraterm-4.92.exe/) - to see debug messages from the board.

**Linux**

If you're on Linux, install:

1. screen - e.g. via `sudo apt install screen`

**MacOS**

Nothing required.




## Firmware Development
We will deploy two programs on the Nucleo board
1. The hardware hello world program *Blinky* that turns an LED on and off.
1. Temperature and humidity sensor

## Data Transmission

Data transmission from the sensors to the application will be via LoRaWAN. This is a long range low power sensor system ideal for the internet of things.

## Hardware requirements
1. NUCLEO-F446RE
1. LoRaWAN Transceiver Shield (Custom made for DSA by ARM!)
1. USB Connector
1. Temperature sensor
1. Soil Moisture Sensor



## Set Up

Follow these instructions from DSA 2018 Nyeri by Jan Jongboon to set up. Refer to the original repo [here](https://github.com/janjongboom/dsa2018-greenhouse-monitor).

1. Go to the [NUCLEO-F446RE](https://os.mbed.com/platforms/ST-Nucleo-F446RE/) platform page and click *Add to your Mbed compiler*.
1. Import the example program into the Arm Mbed Compiler by clicking [this link](https://os.mbed.com/compiler/#import:https://github.com/ciiram/indaba-maker-session-2019).
1. Click *Import*.
1. In the top right corner make sure you selected 'NUCLEO-F446RE'.

    ![Select the correct platform](media/mbed100.png)

This has cloned the repository.

1. Click *Compile*.

    ![Compile](media/mbed4.png)

1. A binary (.bin) file downloads, use drag-and-drop to copy the file to the NODE_F446RE device (like a USB mass storage device).

    **Note:** Here's a [video](https://youtu.be/L5TcmFFD0iw?t=1m25s).

1. When flashing is complete, hit the **RESET** button on the shield.
1. You should notice the led on the nucleo board flashing. We have programmed the board to blink every second.


## Modify Blinky
1. Open `select_program.h`.
1. Note that we have set the program to be compiled here. Later we will change this.

    ```
    #define PROGRAM HELLO_WORLD
    ```

1. Open `hello_world.cpp` and change the value of the constant

   ```
   const float BLINK_PERIOD_S = 1;
   ```

   to a smaller or larger value and recompile the program and drag-and-drop the .bin to the Nucleo board. Confirm that the blinking rate has now changed.


## Temperature and humidity measurement
1. Let's connect up the hardware.
1. Connect red to AVDD, black to GND, yellow to D7.
   ![temperature](media/pinout1_v2.png)
1. On the online compiler, open `select_program.h`.
1. Set:

    ```
    #define PROGRAM TEST_TEMP
    ```
1. Click *Compile*.

    ![Compile](media/mbed4.png)

1. A binary (.bin) file downloads, use drag-and-drop to copy the file to the NODE_F446RE device (like a USB mass storage device).
1. We need to view the program output with temperature and humidity values on the console. In linux we do the following

```
$ ls /dev/ttyACM*
/dev/ttyACM0
```

Then connect to the board using screen:

```
sudo screen /dev/ttyACM0 9600                # might not need sudo if set up lsusb rules properly
```

On Windows

* Unplug your board and plug it back in.
* (Not sure if it configured correctly? Look in 'Device Manager > Ports (COM & LPT)', should list as STLink Virtual COM Port.


The output will look something like this
```
Temperature is 28.00 C
Humidity is 5.00
Temperature is 28.00 C
Humidity is 5.00
Temperature is 28.00 C
Humidity is 5.00

Err 6
```
Sometimes reading the sensor is unsuccessful and the error is reported.

## Soil Moisture Measurement
1. For those with soil moisture sensors, connect the hardware as follows Connect red to 3.3V, black to GND, yellow to A2.
1. For this you will need to create a male-female jumper from the male-male and female-female jumpers provided.
![soil](media/pinout2.png)
1. On the online compiler, open `select_program.h`.
1. Set:

    ```
    #define PROGRAM TEST_SOIL

    ```
1. Compile, flash and view the output on the console
1. If you hold the sensor, the moisture in your palm should cause the percentage to rise. If you can get a wet rag the moisture level will rise even higher.


The output will look something like

```
Soil Moisture Sensor Test program
******************
Moisture Level: 0.0%
Moisture Level: 0.0%
Moisture Level: 36.2%
Moisture Level: 34.3%
Moisture Level: 35.7%
Moisture Level: 36.2%
```


## Data Transmission over LoRa

Follow these instructions from Jan's [repo](https://github.com/janjongboom/dsa2018-greenhouse-monitor)

### Grabbing credentials from The Things Network

We have a LoRaWAN network set up here but you need some credentials to connect to it. Let's grab some credentials from The Things Network.

1. Log in to the [The Things Network console](http://console.thethingsnetwork.org).

    ![console](media/console.png)

1. Use the following credentials:
    * Username: `indaba2019`.
    * Password: `indaba2019`.
1. Click *Applications*.
1. Click on `maker-session`.

    ![console2](media/console2.png)

1. Click *Devices*.
1. Click *Register device*.

On the register device page:

1. First click the *generate* button below 'Device EUI'.

    ![console](media/console4.png)

1. Enter a nice name for your device and click *Register*.

    ![console](media/console3.png)

1. Click **Settings**.

    ![settings](media/ttn20.png)

1. Switch to **ABP**.

    ![settings](media/ttn21.png)

1. Disable (or uncheck) frame counter checks.

    ![frame-counter stuff](media/ttn22.png)

1. Click **Save**.



### Configuring your device
Get the device address, network session key and application session key.

1. Click the **Copy** button next to 'Device Address' to copy to clipboard.

    ![device-address](media/ttn_adr.png)

1. Click the `< >` button of the **Network session key** and **Application session key** values to show the value as C-style array.
1. Click the **Copy** button on the right of the value to copy to clipboard.

Paste these keys into the file `device_addresses.h` in the appropriate sections:

```
static uint32_t DEVADDR = 0x2601112A;
static uint8_t NWKSKEY[] = { 0xD1, 0x8F, 0xB8, 0x4A, 0xB1, 0x1C, 0xAF, 0x3E, 0xBD, 0xC2, 0xB6, 0x84, 0xEF, 0xD4, 0x41, 0xE4 };
static uint8_t APPSKEY[] =  { 0xAF, 0x05, 0x16, 0x6F, 0x17, 0x34, 0xAD, 0xC0, 0x51, 0xD1, 0xE9, 0x7B, 0xF5, 0xFA, 0x33, 0x6E };
```

* Put Device Address on the first line, prefixed with `0x`!
* Put Network Session Key on the second line, don't forget to add `;` at the end.
* Put Application Session Key on the third line, don't forget to add `;` at the end.

1. Connect the temperature (or soil moisture) sensor as you did earlier.
1. Connect the LoRa sheild on top of the Nucleo board.
1. The correct orientation of the LoRa shield is when all the logos are on the top.
1. On the online compiler, open `select_program.h`.
1. Set:

    ```
    #define PROGRAM TEMP_TRANSMIT

    ```

Or

    ```
    #define PROGRAM SOIL_TRANSMIT

    ```
Or

    ```
    #define PROGRAM TEMP_SOIL_TRANSMIT

    ```
 Depending on whether you have the temperature sensor, soil moisture sensor or both.

1. Compile, flash, ...
1. View the output on the console. You get something similar to
```
=======================================
  Temperature and Humidity Sensors
=======================================
Sending every 60 seconds
[DBG ][LSTK]: Initializing MAC layer
[DBG ][LSTK]: Initiating ABP
[DBG ][LSTK]: Frame Counters. UpCnt=0, DownCnt=0
[DBG ][LSTK]: ABP connection OK.
Connection - In Progress ...
Connection - Successful
Ambient Temp=21.000000 Ambient Humi=62.000000
Sending 7 bytes
[INFO][LMAC]: RTS = 7 bytes, PEND = 0, Port: 15
[DBG ][LMAC]: Frame prepared to send at port 15
[DBG ][LMAC]: TX: Channel=1, DR=0
7 bytes scheduled for transmission
[DBG ][LSTK]: Transmission completed
[DBG ][LMAC]: Opening RX1 Window
[DBG ][LMAC]: Opening RX2 Window, Frequency = 869525000
Message Sent to Network Server
Going to sleep!

```
1. You should see the data on the console also appear on TTN in your device under the data tab.


## Writing Data to a Database
TTN does not store data and for us to use the data in any application, we must store it in a database we configure ourselves. We will use an InfluxDB which is well suited to time series data.
We will use the [MQTT protocol](http://mqtt.org/) to transfer data from TTN to our database via a [python SDK](https://github.com/TheThingsNetwork/python-app-sdk) provided by TTN. We will write the data to a local InfluxDB on our machines which we will create.


1.  Run

`python ttn_example.py`

This should create an InfluxDB on your local machine named `indaba_session` and populate it with data whenever your device transmits data. It will also print out messages with the json of the uplink

```
Received uplink from  dev-01
{'time': '2019-08-13T09:44:17.171780715Z', 'fields': {'data_rate': 'SF12BW125', 'rssi': -103.0, 'snr': 4.2, 'Temperature': 19.0, 'Relative Humidity': 67.0}, 'measurement': 'Indaba Session', 'tags': {'sensor': 'dev-01'}}
```

These fields include temperature and humidity as well as radio transmission parameters.

1. Now we can examine the database.

1. Open the database

`influx -precision rfc3339 -database indaba_session`

1. Display the data collected so far
```
Connected to http://localhost:8086 version 1.7.4
InfluxDB shell version: 1.7.4
Enter an InfluxQL query
> SELECT * FROM "Indaba Session"
```

## Data Analysis
We will now get the data into a Jupyter notebook and perform some visualisation and analysis. We will work in the same virtual environment.

1. Activate the environment (On Linux
`source ttn/bin/activate`
On Windows
`ttn\Scripts\activate.bat`)
1. Open the data analysis notebook (data_analysis.ipynb) in this repo and follow the instructions.


## Battery Powered Deployment

1. Take battery holder and place 4 AAA batteries.
1. Connect the jumpers to Vin (red) and GND (black), these are on the left side of the shield

    ![battery](media/pinout3.png)

1. Then remove the USB cable, and remove the shield, and find jumper JP5.

    ![jumper](media/IMG_3523.JPG)

1. Move it from U5V to E5V.
1. Place the shield back.
1. The device should be back online.
1. We'll explore how far we can take the sensor
