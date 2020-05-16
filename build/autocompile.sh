#! /bin/bash

RED="\033[1;31m";
GRN="\033[1;32m";
YLW="\033[1;33m";
BLU="\033[1;34m";
TRS="\e[0m";

FLAG_VERBOSE=""

PATH_SOURCE="../libraries/indoorino/utils.cpp ../libraries/indoorino/packeTemplate.cpp ../libraries/indoorino/packetStubs.cpp ../libraries/indoorino/packetUtils.cpp ../libraries/indoorino/packetParse.cpp ../libraries/netmodule/netutils.cpp ../libraries/netmodule/netmodule.cpp ../libraries/indoorino/benchmark.cpp"

PATH_BUILD_BOARDS="temp-UNO temp-MEGA temp-DUE temp-ESP8266 temp-D1MINI"
PATH_PROJECT_SKETCH="../sketches/debug_packets/debug_packets.ino ../sketches/sampler/sampler.ino"
PATH_ESP_PROJECT_SKETCH="../sketches/debug_packets/debug_packets.ino ../sketches/sampler/espserver.ino"

FLAG_COMPILE_ARDUINO="1"
FLAG_COMPILE_ESP="1"
FLAG_COMPILE_RAWRUN="1"
FLAG_COMPILE_MODULE="1"

FLAG_TEST_RAWRN="1"
FLAG_TEST_PYMODULE="1"

FILENAME_USER_DEF="../libraries/indoorino/project.h"
FILENAME_TEMP_DIR="/tmp/indoorino/"

test $FLAG_VERBOSE &&
{
    FLAG_VERBOSE_ARDUINO="-verbose -trace"
    FLAG_WALL="-Wall"
} || {
    FLAG_VERBOSE_ARDUINO=""
    FLAG_WALL=""
}

clear
echo -e "\n\n$GRN\tINDOORINO COMPILER START$TRS"

## *** Remove old builds *** ##

for BOARDPATH in $PATH_BUILD_BOARDS; do
    if [ ! -d "./build/$BOARDPATH" ] ; then
        echo -e "\n$GRN\tGenerating new build folder ./build/$BOARDPATH$TRS"
        mkdir -v ./build/$BOARDPATH
    else
        if [ "$(ls -A "./build/$BOARDPATH")" ]; then
            echo -e "\n$YLW\tRemoving old build files from ./build/$BOARDPATH ...$TRS"
            rm -rv ./build/$BOARDPATH/*
        else
            echo -e "\n$GRN\tNo files to remove from ./build/$BOARDPATH$TRS"
        fi
    fi
done

## *** Generating /tmp/indoorino *** ##

if [ ! -d $FILENAME_TEMP_DIR ] ; then
    echo -e "\n$GRN\tGenerating new temp folder ./build/$FILENAME_TEMP_DIR$TRS\n"
    mkdir -v "/tmp/indoorino"
fi
echo -e "\n$YLW\tCopying original file ${FILENAME_TEMP_DIR}project_macros_original$TRS"
cat $FILENAME_USER_DEF>"${FILENAME_TEMP_DIR}project_macros_original"

## *** Generating project header files *** ##

for PROJECT in $PATH_PROJECT_SKETCH; do
    
    STRING="$(echo ${FILENAME_TEMP_DIR}"project_macros_")$(echo $PROJECT | rev | cut -d'/' -f1 | rev | tr -d '.ino')"
    if [ ! -e $STRING ]; then
        
        echo -e "\n$YLW\tGenerating header for $(echo $PROJECT | rev | cut -d'/' -f1 | rev) ... $TRS"
        
        ## *** Parsing original file by lines*** ##
        while IFS= read -r LINE; do
    
            ## *** Check for current project flag *** ##
            TEMP=$(echo $PROJECT | rev | cut -d'/' -f1 | rev | tr -d '.ino' | tr '[:lower:]' '[:upper:]') 
            if [[ $LINE == *"$TEMP"* ]]; then
               
                if [[ $LINE == *"//"* ]]; then
                    echo "$(echo $LINE | tr -d '//')" >> $STRING
                    continue
                fi
            fi

            ## *** Check for other project flag*** ##
            if [[ $LINE == *"INDOORINO_"* ]]; then
                    if [[ $LINE != *"//"* ]]; then
                    echo "//$LINE" >> $STRING
                    continue
                fi
            fi

            ## *** Enabling all debug *** ##
            if [[ $LINE == *"DEBUG_"* ]]; then
                    if [[ $LINE == *"//"* ]]; then
                    echo "$(echo $LINE | tr -d '//')" >> $STRING
                    continue
                fi
            fi
            
            echo "$LINE" >> $STRING

        done < $FILENAME_USER_DEF
    fi
done

trap $(cat "${FILENAME_TEMP_DIR}project_macros_original" > $FILENAME_USER_DEF) INT

#    ________________________________________________________________________
#    |                                                                      |
#    |    BUILDING BOARD PROJECTS                                           |
#    |______________________________________________________________________|
#
    
for PROJECT in $PATH_PROJECT_SKETCH; do

    STRING="$(echo ${FILENAME_TEMP_DIR}"project_macros_")$(echo $PROJECT | rev | cut -d'/' -f1 | rev | tr -d '.ino')"
    cat $STRING > $FILENAME_USER_DEF
        
    #    ________________________________________________________________________
    #    |                                                                      |
    #    |    ARDUINO UNO                                                       |
    #    |______________________________________________________________________|
    #

    test $FLAG_COMPILE_ARDUINO &&
    {

    # /opt/arduino-1.8.12/arduino-builder -dump-prefs -logger=machine -hardware /opt/arduino-1.8.12/hardware -hardware /home/n00b/.arduino15/packages -tools /opt/arduino-1.8.12/tools-builder -tools /opt/arduino-1.8.12/hardware/tools/avr -tools /home/n00b/.arduino15/packages -built-in-libraries /opt/arduino-1.8.12/libraries -libraries /home/n00b/Code/Develop/Indoorino.3.0.6/libraries -fqbn=arduino:avr:uno -ide-version=10812 -build-path /tmp/arduino_build_466183 -warnings=all -build-cache /tmp/arduino_cache_726423 -prefs=build.warn_data_percentage=75 -prefs=runtime.tools.avrdude.path=/home/n00b/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17 -prefs=runtime.tools.avrdude-6.3.0-arduino17.path=/home/n00b/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17 -prefs=runtime.tools.arduinoOTA.path=/home/n00b/.arduino15/packages/arduino/tools/arduinoOTA/1.3.0 -prefs=runtime.tools.arduinoOTA-1.3.0.path=/home/n00b/.arduino15/packages/arduino/tools/arduinoOTA/1.3.0 -prefs=runtime.tools.avr-gcc.path=/home/n00b/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino5 -prefs=runtime.tools.avr-gcc-7.3.0-atmel3.6.1-arduino5.path=/home/n00b/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino5 -verbose /home/n00b/Code/Develop/Indoorino.3.0.6/sketches/debug_packets/debug_packets.ino

        echo -e "\n$GRN\tBUILDING $PROJECT for board UNO\n$TRS"
        arduino-builder -hardware /opt/arduino-1.8.12/hardware/                 \
                        -tools /opt/arduino-1.8.12/hardware/tools/              \
                        -tools /opt/arduino-1.8.12/tools-builder/               \
                        -built-in-libraries /opt/arduino-1.8.12/libraries       \
                        -libraries ../libraries/                                \
                        -fqbn arduino:avr:uno                                   \
                        -build-path ./build/temp-UNO -warnings=all              \
                        $FLAG_VERBOSE_ARDUINO                                   \
                        -prefs=build.warn_data_percentage=75                    \
                        -prefs=runtime.tools.avrdude.path=/home/n00b/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17 \
                        -prefs=runtime.tools.avrdude-6.3.0-arduino17.path=/home/n00b/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17 \
                        -prefs=runtime.tools.arduinoOTA.path=/home/n00b/.arduino15/packages/arduino/tools/arduinoOTA/1.3.0 \
                        -prefs=runtime.tools.arduinoOTA-1.3.0.path=/home/n00b/.arduino15/packages/arduino/tools/arduinoOTA/1.3.0 \
                        -prefs=runtime.tools.avr-gcc.path=/home/n00b/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino5 \
                        -prefs=runtime.tools.avr-gcc-7.3.0-atmel3.6.1-arduino5.path=/home/n00b/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino5 \
                        $PROJECT ||
        {
            echo -e "\n$RED\tERROR buiding $PROJECT for board UNO!$TRS\n\n"
            cat "${FILENAME_TEMP_DIR}""project_macros_original" > $FILENAME_USER_DEF
            exit 1
        }

    }

    #    ________________________________________________________________________
    #    |                                                                      |
    #    |    ARDUINO MEGA                                                      |
    #    |______________________________________________________________________|
    #

    test $FLAG_COMPILE_ARDUINO &&
    {
        
    opt/arduino-1.8.12/arduino-builder -dump-prefs -logger=machine -hardware /opt/arduino-1.8.12/hardware -hardware /home/n00b/.arduino15/packages -tools /opt/arduino-1.8.12/tools-builder -tools /opt/arduino-1.8.12/hardware/tools/avr -tools /home/n00b/.arduino15/packages -built-in-libraries /opt/arduino-1.8.12/libraries -libraries /home/n00b/Code/Develop/Indoorino.3.0.6/libraries -fqbn=arduino:avr:mega:cpu=atmega2560 -ide-version=10812 -build-path /tmp/arduino_build_466183 -warnings=all -build-cache /tmp/arduino_cache_726423 -prefs=build.warn_data_percentage=75 -prefs=runtime.tools.avrdude.path=/home/n00b/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17 -prefs=runtime.tools.avrdude-6.3.0-arduino17.path=/home/n00b/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17 -prefs=runtime.tools.arduinoOTA.path=/home/n00b/.arduino15/packages/arduino/tools/arduinoOTA/1.3.0 -prefs=runtime.tools.arduinoOTA-1.3.0.path=/home/n00b/.arduino15/packages/arduino/tools/arduinoOTA/1.3.0 -prefs=runtime.tools.avr-gcc.path=/home/n00b/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino5 -prefs=runtime.tools.avr-gcc-7.3.0-atmel3.6.1-arduino5.path=/home/n00b/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino5 -verbose /home/n00b/Code/Develop/Indoorino.3.0.6/sketches/debug_packets/debug_packets.ino

            
        echo -e "\n$GRN\tBUILDING $PROJECT for board MEGA2560\n$TRS"
        arduino-builder -hardware /opt/arduino-1.8.12/hardware/                 \
                        -tools /opt/arduino-1.8.12/hardware/tools/              \
                        -tools /opt/arduino-1.8.12/tools-builder/               \
                        -built-in-libraries /opt/arduino-1.8.12/libraries       \
                        -libraries ../libraries/                                \
                        -fqbn arduino:avr:mega:cpu=atmega2560                   \
                        -build-path ./build/temp-MEGA -warnings=all             \
                        $FLAG_VERBOSE_ARDUINO                                   \
                        -prefs=build.warn_data_percentage=75                    \
                        -prefs=runtime.tools.avrdude.path=/home/n00b/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17 \
                        -prefs=runtime.tools.avrdude-6.3.0-arduino17.path=/home/n00b/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17 \
                        -prefs=runtime.tools.arduinoOTA.path=/home/n00b/.arduino15/packages/arduino/tools/arduinoOTA/1.3.0 \
                        -prefs=runtime.tools.arduinoOTA-1.3.0.path=/home/n00b/.arduino15/packages/arduino/tools/arduinoOTA/1.3.0 \
                        -prefs=runtime.tools.avr-gcc.path=/home/n00b/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino5 \
                        -prefs=runtime.tools.avr-gcc-7.3.0-atmel3.6.1-arduino5.path=/home/n00b/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino5 \
                        $PROJECT ||
        {
            echo -e "\n$RED\tERROR buiding $PROJECT for board MEGA2560!$TRS\n\n"
            cat "${FILENAME_TEMP_DIR}""project_macros_original" > $FILENAME_USER_DEF
            exit 1
        }

    }

    #    ________________________________________________________________________
    #    |                                                                      |
    #    |    ARDUINO DUE                                                       |
    #    |______________________________________________________________________|
    #

    test $FLAG_COMPILE_ARDUINO &&
    {
        
    # /opt/arduino-1.8.12/arduino-builder -dump-prefs -logger=machine -hardware /opt/arduino-1.8.12/hardware -hardware /home/n00b/.arduino15/packages -tools /opt/arduino-1.8.12/tools-builder -tools /opt/arduino-1.8.12/hardware/tools/avr -tools /home/n00b/.arduino15/packages -built-in-libraries /opt/arduino-1.8.12/libraries -libraries /home/n00b/Code/Develop/Indoorino.3.0.6/libraries -fqbn=arduino:sam:arduino_due_x_dbg -ide-version=10812 -build-path /tmp/arduino_build_466183 -warnings=all -build-cache /tmp/arduino_cache_726423 -prefs=build.warn_data_percentage=75 -prefs=runtime.tools.arm-none-eabi-gcc.path=/home/n00b/.arduino15/packages/arduino/tools/arm-none-eabi-gcc/4.8.3-2014q1 -prefs=runtime.tools.arm-none-eabi-gcc-4.8.3-2014q1.path=/home/n00b/.arduino15/packages/arduino/tools/arm-none-eabi-gcc/4.8.3-2014q1 -prefs=runtime.tools.bossac.path=/home/n00b/.arduino15/packages/arduino/tools/bossac/1.6.1-arduino -prefs=runtime.tools.bossac-1.6.1-arduino.path=/home/n00b/.arduino15/packages/arduino/tools/bossac/1.6.1-arduino -verbose /home/n00b/Code/Develop/Indoorino.3.0.6/sketches/debug_packets/debug_packets.ino

        echo -e "\n$GRN\tBUILDING $PROJECT for board DUE\n$TRS"
        arduino-builder -hardware /opt/arduino-1.8.12/hardware/                         \
                        -hardware /home/n00b/.arduino15/packages                        \
                        -tools /home/n00b/.arduino15/packages/arduino/hardware/tools/   \
                        -tools /opt/arduino-1.8.12/tools-builder/                       \
                        -tools /home/n00b/.arduino15/packages                           \
                        -built-in-libraries /opt/arduino-1.8.12/libraries               \
                        -fqbn arduino:sam:arduino_due_x_dbg                             \
                        -libraries ../libraries/                                        \
                        -build-path ./build/temp-DUE -warnings=all                      \
                        $FLAG_VERBOSE_ARDUINO                                           \
                        -prefs=build.warn_data_percentage=75                            \
                        -prefs=runtime.tools.arm-none-eabi-gcc.path=/home/n00b/.arduino15/packages/arduino/tools/arm-none-eabi-gcc/4.8.3-2014q1 \
                        -prefs=runtime.tools.arm-none-eabi-gcc-4.8.3-2014q1.path=/home/n00b/.arduino15/packages/arduino/tools/arm-none-eabi-gcc/4.8.3-2014q1 \
                        -prefs=runtime.tools.bossac.path=/home/n00b/.arduino15/packages/arduino/tools/bossac/1.6.1-arduino \
                        -prefs=runtime.tools.bossac-1.6.1-arduino.path=/home/n00b/.arduino15/packages/arduino/tools/bossac/1.6.1-arduino \
                        $PROJECT ||
        {
            echo -e "\n$RED\tERROR buiding $PROJECT for board DUE!$TRS\n\n"
            cat "${FILENAME_TEMP_DIR}""project_macros_original" > $FILENAME_USER_DEF
            exit 1
        }

    }

done

cat "${FILENAME_TEMP_DIR}project_macros_original" > $FILENAME_USER_DEF

#    ________________________________________________________________________
#    |                                                                      |
#    |      ESP8266 GENERIC                                                 |
#    |______________________________________________________________________|
#

CURRENT_SKETCH="../sketches/debug_packets/debug_packets.ino"

test $FLAG_COMPILE_ESP &&
{
# /opt/arduino-1.8.12/arduino-builder -dump-prefs -logger=machine -hardware /opt/arduino-1.8.12/hardware -hardware /home/n00b/.arduino15/packages -tools /opt/arduino-1.8.12/tools-builder -tools /opt/arduino-1.8.12/hardware/tools/avr -tools /home/n00b/.arduino15/packages -built-in-libraries /opt/arduino-1.8.12/libraries -libraries /home/n00b/Code/Develop/Indoorino.3.0.6/libraries -fqbn=esp8266:esp8266:generic:xtal=80,vt=flash,exception=legacy,ssl=all,ResetMethod=nodemcu,CrystalFreq=26,FlashFreq=40,FlashMode=dout,eesz=1M64,led=2,sdk=nonosdk_190703,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=115200 -ide-version=10812 -build-path /tmp/arduino_build_466183 -warnings=all -build-cache /tmp/arduino_cache_726423 -prefs=build.warn_data_percentage=75 -prefs=runtime.tools.mkspiffs.path=/home/n00b/.arduino15/packages/esp8266/tools/mkspiffs/2.5.0-4-b40a506 -prefs=runtime.tools.mkspiffs-2.5.0-4-b40a506.path=/home/n00b/.arduino15/packages/esp8266/tools/mkspiffs/2.5.0-4-b40a506 -prefs=runtime.tools.mklittlefs.path=/home/n00b/.arduino15/packages/esp8266/tools/mklittlefs/2.5.0-4-69bd9e6 -prefs=runtime.tools.mklittlefs-2.5.0-4-69bd9e6.path=/home/n00b/.arduino15/packages/esp8266/tools/mklittlefs/2.5.0-4-69bd9e6 -prefs=runtime.tools.python3.path=/home/n00b/.arduino15/packages/esp8266/tools/python3/3.7.2-post1 -prefs=runtime.tools.python3-3.7.2-post1.path=/home/n00b/.arduino15/packages/esp8266/tools/python3/3.7.2-post1 -prefs=runtime.tools.xtensa-lx106-elf-gcc.path=/home/n00b/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-4-b40a506 -prefs=runtime.tools.xtensa-lx106-elf-gcc-2.5.0-4-b40a506.path=/home/n00b/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-4-b40a506 -verbose /home/n00b/Code/Develop/Indoorino.3.0.6/sketches/debug_packets/debug_packets.ino

    echo -e "\n$YLW\tBuiling $CURRENT_SKETCH for gerneric ESP8266\n$TRS"
    arduino-builder -hardware /opt/arduino-1.8.12/hardware/                         \
                    -hardware /home/n00b/.arduino15/packages                        \
                    -tools /home/n00b/.arduino15/packages/arduino/hardware/tools/   \
                    -tools /opt/arduino-1.8.12/tools-builder/                       \
                    -tools /home/n00b/.arduino15/packages                           \
                    -built-in-libraries /opt/arduino-1.8.12/libraries               \
                    -fqbn=esp8266:esp8266:generic:xtal=80,vt=flash,exception=legacy,ssl=all,ResetMethod=nodemcu,CrystalFreq=26,FlashFreq=40,FlashMode=dout,eesz=1M64,led=2,sdk=nonosdk_190703,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=115200\
                    -libraries ../libraries/                                        \
                    -build-path ./build/temp-ESP8266                                \
                    $FLAG_VERBOSE_ARDUINO                                                  \
                    -prefs=build.warn_data_percentage=75                            \
                    -prefs=runtime.tools.mkspiffs.path=/home/n00b/.arduino15/packages/esp8266/tools/mkspiffs/2.5.0-4-b40a506 \
                    -prefs=runtime.tools.mkspiffs-2.5.0-4-b40a506.path=/home/n00b/.arduino15/packages/esp8266/tools/mkspiffs/2.5.0-4-b40a506 \
                    -prefs=runtime.tools.mklittlefs.path=/home/n00b/.arduino15/packages/esp8266/tools/mklittlefs/2.5.0-4-69bd9e6 \
                    -prefs=runtime.tools.mklittlefs-2.5.0-4-69bd9e6.path=/home/n00b/.arduino15/packages/esp8266/tools/mklittlefs/2.5.0-4-69bd9e6 \
                    -prefs=runtime.tools.python3.path=/home/n00b/.arduino15/packages/esp8266/tools/python3/3.7.2-post1 \
                    -prefs=runtime.tools.python3-3.7.2-post1.path=/home/n00b/.arduino15/packages/esp8266/tools/python3/3.7.2-post1 \
                    -prefs=runtime.tools.xtensa-lx106-elf-gcc.path=/home/n00b/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-4-b40a506 \
                    -prefs=runtime.tools.xtensa-lx106-elf-gcc-2.5.0-4-b40a506.path=/home/n00b/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-4-b40a506 \
                    $CURRENT_SKETCH ||
    {
        echo -e "\n$RED\tERROR Building $CURRENT_SKETCH for generic ESP8266!$TRS\n\n"
        exit 1
    }

}

#    ________________________________________________________________________
#    |                                                                      |
#    |      ESP8266 WEMOS D1MINI                                            |
#    |______________________________________________________________________|
# 

test $FLAG_COMPILE_ESP &&
{

# /opt/arduino-1.8.12/arduino-builder -dump-prefs -logger=machine -hardware /opt/arduino-1.8.12/hardware -hardware /home/n00b/.arduino15/packages -tools /opt/arduino-1.8.12/tools-builder -tools /opt/arduino-1.8.12/hardware/tools/avr -tools /home/n00b/.arduino15/packages -built-in-libraries /opt/arduino-1.8.12/libraries -libraries /home/n00b/Code/Develop/Indoorino.3.0.6/libraries -fqbn=esp8266:esp8266:d1:xtal=80,vt=flash,exception=legacy,ssl=all,eesz=4M2M,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=921600 -ide-version=10812 -build-path /tmp/arduino_build_466183 -warnings=all -build-cache /tmp/arduino_cache_726423 -prefs=build.warn_data_percentage=75 -prefs=runtime.tools.mkspiffs.path=/home/n00b/.arduino15/packages/esp8266/tools/mkspiffs/2.5.0-4-b40a506 -prefs=runtime.tools.mkspiffs-2.5.0-4-b40a506.path=/home/n00b/.arduino15/packages/esp8266/tools/mkspiffs/2.5.0-4-b40a506 -prefs=runtime.tools.mklittlefs.path=/home/n00b/.arduino15/packages/esp8266/tools/mklittlefs/2.5.0-4-69bd9e6 -prefs=runtime.tools.mklittlefs-2.5.0-4-69bd9e6.path=/home/n00b/.arduino15/packages/esp8266/tools/mklittlefs/2.5.0-4-69bd9e6 -prefs=runtime.tools.python3.path=/home/n00b/.arduino15/packages/esp8266/tools/python3/3.7.2-post1 -prefs=runtime.tools.python3-3.7.2-post1.path=/home/n00b/.arduino15/packages/esp8266/tools/python3/3.7.2-post1 -prefs=runtime.tools.xtensa-lx106-elf-gcc.path=/home/n00b/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-4-b40a506 -prefs=runtime.tools.xtensa-lx106-elf-gcc-2.5.0-4-b40a506.path=/home/n00b/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-4-b40a506 -verbose /home/n00b/Code/Develop/Indoorino.3.0.6/sketches/debug_packets/debug_packets.ino

    echo -e "\n$YLW\tCompiling SERVER for WEMOS D1MINI\n$TRS"
    arduino-builder -hardware /opt/arduino-1.8.12/hardware/                         \
                    -hardware /home/n00b/.arduino15/packages                        \
                    -tools /home/n00b/.arduino15/packages/arduino/hardware/tools/   \
                    -tools /opt/arduino-1.8.12/tools-builder/                       \
                    -tools /home/n00b/.arduino15/packages                           \
                    -built-in-libraries /opt/arduino-1.8.12/libraries               \
                    -fqbn=esp8266:esp8266:d1:xtal=80,vt=flash,exception=legacy,ssl=all,eesz=4M2M,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=921600 \
                    -libraries ../libraries/                                        \
                    -build-path ./build/temp-D1MINI                                 \
                    $FLAG_VERBOSE_ARDUINO                                                  \
                    -prefs=build.warn_data_percentage=75                            \
                    -prefs=runtime.tools.mkspiffs.path=/home/n00b/.arduino15/packages/esp8266/tools/mkspiffs/2.5.0-4-b40a506 \
                    -prefs=runtime.tools.mkspiffs-2.5.0-4-b40a506.path=/home/n00b/.arduino15/packages/esp8266/tools/mkspiffs/2.5.0-4-b40a506 \
                    -prefs=runtime.tools.mklittlefs.path=/home/n00b/.arduino15/packages/esp8266/tools/mklittlefs/2.5.0-4-69bd9e6 \
                    -prefs=runtime.tools.mklittlefs-2.5.0-4-69bd9e6.path=/home/n00b/.arduino15/packages/esp8266/tools/mklittlefs/2.5.0-4-69bd9e6 \
                    -prefs=runtime.tools.python3.path=/home/n00b/.arduino15/packages/esp8266/tools/python3/3.7.2-post1 \
                    -prefs=runtime.tools.python3-3.7.2-post1.path=/home/n00b/.arduino15/packages/esp8266/tools/python3/3.7.2-post1 \
                    -prefs=runtime.tools.xtensa-lx106-elf-gcc.path=/home/n00b/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-4-b40a506 \
                    -prefs=runtime.tools.xtensa-lx106-elf-gcc-2.5.0-4-b40a506.path=/home/n00b/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-4-b40a506 \
                    $CURRENT_SKETCH ||
    {
        echo -e "\n$RED\tERROR Compiling SERVER for WEMOS D1MINI!$TRS\n\n"
        exit 1
    }

}


test $FLAG_COMPILE_RAWRUN &&
{
    echo -e "\n$YLW\tCompiling RAWRUN DESKTOP APP\n$TRS"

    c++ -DRAWRUN $FLAG_WALL -I/usr/include/ -I../libraries/ -pedantic -o rawrun $PATH_SOURCE rawrun.cpp ||
        {
            echo -e "\n$RED\tERROR Compiling RAWRUN DESKTOP APP!$TRS\n\n"
            exit 1
        }

    test $FLAG_TEST_RAWRN && ./rawrun
}

test $FLAG_COMPILE_MODULE &&
{
    echo -e "\n$YLW\tCompiling PYTHON DESKTOP APP$TRS"
    python3.7 setup.py build ||
        {
            echo -e "\n$RED\tERROR Compiling PYTHON DESKTOP APP!$TRS\n\n"
            exit 1
        }

    test $FLAG_TEST_PYMODULE && 
    {
        echo -e "\n$YLW Testing Python Indoorino module$TRS"
        python3.7 module_check.py
    }
}

echo -e "\n$GRN\tALL DONE !$TRS\n\n"
$(cat "${FILENAME_TEMP_DIR}""project_macros_original" > $FILENAME_USER_DEF)



