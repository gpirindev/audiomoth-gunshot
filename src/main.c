/****************************************************************************
 * main.c
 * openacousticdevices.info
 * June 2017
 *****************************************************************************/

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "audioMoth.h"

#include <math.h>
#include "arm_const_structs.h"
#include "predictor.h"
#include "weights.h"
#include "arm_nnfunctions.h"
#include "arm_nnsupportfunctions.h"

/* Sleep and LED constants */

#define DEFAULT_WAIT_INTERVAL               1

#define WAITING_LED_FLASH_INTERVAL          2
#define WAITING_LED_FLASH_DURATION          10

#define LOW_BATTERY_LED_FLASHES             10

#define SHORT_LED_FLASH_DURATION            100
#define LONG_LED_FLASH_DURATION             500

/* Useful time constants */

#define SECONDS_IN_MINUTE                   60
#define SECONDS_IN_HOUR                     (60 * SECONDS_IN_MINUTE)
#define SECONDS_IN_DAY                      (24 * SECONDS_IN_HOUR)

/* SRAM buffer constants */

#define EXTERNAL_SRAM_SIZE_IN_SAMPLES       (AM_EXTERNAL_SRAM_SIZE_IN_BYTES / 2)
#define NUMBER_OF_BUFFERS                   8
#define NUMBER_OF_SAMPLES_IN_BUFFER         RECORDING_LENGTH/2
#define NUMBER_OF_SAMPLES_IN_DMA_TRANSFER   1024
#define NUMBER_OF_BUFFERS_TO_SKIP           1
#define NUMBER_OF_OVERLAP_SAMPLES			256
#define NN_INPUT_TO_COPY_LEN				31*19
#define NN_INPUT_TO_WRITE_LEN				32*19

/* Analysis time constants */

#define DETECTION_THRESHOLD                 60
#define TIMESTAMP_LENGTH 					15
#define MAX_ANALYSIS_TIME					600
#define RESULT_LENGTH						5

/* WAV header constant */

#define PCM_FORMAT                          1
#define RIFF_ID_LENGTH                      4
#define LENGTH_OF_ARTIST                    32
#define LENGTH_OF_COMMENT                   256

/* USB configuration constant */

#define MAX_START_STOP_PERIODS              5

/* DC filter constant */

#define DC_BLOCKING_FACTOR                  0.995f

/* Useful macros */

#define FLASH_LED(led, duration) { \
    AudioMoth_set ## led ## LED(true); \
    AudioMoth_delay(duration); \
    AudioMoth_set ## led ## LED(false); \
}

#define RETURN_ON_ERROR(fn) { \
    bool success = (fn); \
    if (success != true) { \
        if (configSettings->enableBatteryCheck ) { \
            AudioMoth_disableBatteryMonitor(); \
        } \
        FLASH_LED(Both, LONG_LED_FLASH_DURATION) \
        return SDCARD_WRITE_ERROR; \
    } \
}

#define SAVE_SWITCH_POSITION_AND_POWER_DOWN(duration) { \
    *previousSwitchPosition = switchPosition; \
    AudioMoth_powerDownAndWake(duration, true); \
}

#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

/* Recording state enumeration */

typedef enum {RECORDING_OKAY, SWITCH_CHANGED, SDCARD_WRITE_ERROR, BATTERY_CHECK} AM_recordingState_t;

/* WAV header */

#pragma pack(push, 1)

typedef struct {
    char id[RIFF_ID_LENGTH];
    uint32_t size;
} chunk_t;

typedef struct {
    chunk_t icmt;
    char comment[LENGTH_OF_COMMENT];
} icmt_t;

typedef struct {
    chunk_t iart;
    char artist[LENGTH_OF_ARTIST];
} iart_t;

typedef struct {
    uint16_t format;
    uint16_t numberOfChannels;
    uint32_t samplesPerSecond;
    uint32_t bytesPerSecond;
    uint16_t bytesPerCapture;
    uint16_t bitsPerSample;
} wavFormat_t;

typedef struct {
    chunk_t riff;
    char format[RIFF_ID_LENGTH];
    chunk_t fmt;
    wavFormat_t wavFormat;
    chunk_t list;
    char info[RIFF_ID_LENGTH];
    icmt_t icmt;
    iart_t iart;
    chunk_t data;
} wavHeader_t;

#pragma pack(pop)

static wavHeader_t wavHeader = {
    .riff = {.id = "RIFF", .size = 0},
    .format = "WAVE",
    .fmt = {.id = "fmt ", .size = sizeof(wavFormat_t)},
    .wavFormat = {.format = PCM_FORMAT, .numberOfChannels = 1, .samplesPerSecond = 0, .bytesPerSecond = 0, .bytesPerCapture = 2, .bitsPerSample = 16},
    .list = {.id = "LIST", .size = RIFF_ID_LENGTH + sizeof(icmt_t) + sizeof(iart_t)},
    .info = "INFO",
    .icmt = {.icmt.id = "ICMT", .icmt.size = LENGTH_OF_COMMENT, .comment = ""},
    .iart = {.iart.id = "IART", .iart.size = LENGTH_OF_ARTIST, .artist = ""},
    .data = {.id = "data", .size = 0}
};

void setHeaderDetails(uint32_t sampleRate, uint32_t numberOfSamples) {

    wavHeader.wavFormat.samplesPerSecond = sampleRate;
    wavHeader.wavFormat.bytesPerSecond = 2 * sampleRate;
    wavHeader.data.size = 2 * numberOfSamples;
    wavHeader.riff.size = 2 * numberOfSamples + sizeof(wavHeader_t) - sizeof(chunk_t);

}

void setHeaderComment(uint32_t currentTime, int8_t timezone, uint8_t *serialNumber, uint32_t gain, AM_batteryState_t batteryState, bool batteryVoltageLow, bool switchPositionChanged) {

    time_t rawtime = currentTime + timezone * SECONDS_IN_HOUR;

    struct tm *time = gmtime(&rawtime);

    /* Format artist field */

    char *artist = wavHeader.iart.artist;

    sprintf(artist, "AudioMoth %08X%08X", (unsigned int)*((uint32_t*)serialNumber + 1), (unsigned int)*((uint32_t*)serialNumber));

    /* Format comment field */

    char *comment = wavHeader.icmt.comment;

    sprintf(comment, "Recorded at %02d:%02d:%02d %02d/%02d/%04d (UTC", time->tm_hour, time->tm_min, time->tm_sec, time->tm_mday, 1 + time->tm_mon, 1900 + time->tm_year);

    comment += 36;

    if (timezone < 0) sprintf(comment, "%d", timezone);

    if (timezone > 0) sprintf(comment, "+%d", timezone);

    if (timezone < 0 || timezone > 0) comment += 2;

    if (timezone < -9 || timezone > 9) comment += 1;

    sprintf(comment, ") by %s at gain setting %d while battery state was ", artist, (unsigned int)gain);

    comment += 74;

    if (batteryState == AM_BATTERY_LOW) {

        sprintf(comment, "less than 3.6V.");

        comment += 15;

    } else if (batteryState >= AM_BATTERY_FULL) {

        sprintf(comment, "greater than 4.9V.");

        comment += 18;

    } else {

        batteryState += 35;

        sprintf(comment, "%01d.%01dV.", batteryState / 10, batteryState % 10);

        comment += 5;

    }

    if (batteryVoltageLow || switchPositionChanged) {

        sprintf(comment, " Recording cancelled before completion due to ");

        comment += 46;

        if (batteryVoltageLow) {

            sprintf(comment, "low battery voltage.");

        } else if (switchPositionChanged) {

            sprintf(comment, "change of switch position.");

        }

    }

}

/* USB configuration data structure */

#pragma pack(push, 1)

typedef struct {
    uint16_t startMinutes;
    uint16_t stopMinutes;
} startStopPeriod_t;

typedef struct {
    uint32_t time;
    uint8_t gain;
    uint8_t clockDivider;
    uint8_t acquisitionCycles;
    uint8_t oversampleRate;
    uint32_t sampleRate;
    uint8_t sampleRateDivider;
    uint16_t sleepDuration;
    uint16_t recordDuration;
    uint8_t enableLED;
    uint8_t activeStartStopPeriods;
    startStopPeriod_t startStopPeriods[MAX_START_STOP_PERIODS];
    int8_t timezone;
    uint8_t enableBatteryCheck;
    uint8_t disableBatteryLevelDisplay;
} configSettings_t;

#pragma pack(pop)

configSettings_t defaultConfigSettings = {
    .time = 0,
    .gain = 2,
    .clockDivider = 4,
    .acquisitionCycles = 16,
    .oversampleRate = 1,
    .sampleRate = 256000,
    .sampleRateDivider = 32,
    .sleepDuration = 0,
    .recordDuration = 60,
    .enableLED = 1,
    .activeStartStopPeriods = 0,
    .startStopPeriods = {
        {.startMinutes = 60, .stopMinutes = 120},
        {.startMinutes = 300, .stopMinutes = 420},
        {.startMinutes = 540, .stopMinutes = 600},
        {.startMinutes = 720, .stopMinutes = 780},
        {.startMinutes = 900, .stopMinutes = 960}
    },
    .timezone = 0,
    .enableBatteryCheck = 0,
    .disableBatteryLevelDisplay = 0
};

uint32_t *previousSwitchPosition = (uint32_t*)AM_BACKUP_DOMAIN_START_ADDRESS;

uint32_t *timeOfNextRecording = (uint32_t*)(AM_BACKUP_DOMAIN_START_ADDRESS + 4);

uint32_t *durationOfNextRecording = (uint32_t*)(AM_BACKUP_DOMAIN_START_ADDRESS + 8);

configSettings_t *configSettings = (configSettings_t*)(AM_BACKUP_DOMAIN_START_ADDRESS + 12);

/* DC filter variables */

static int8_t bitsToShift;

static int32_t previousSample;
static int32_t previousFilterOutput;

/* SRAM buffer variables */

static volatile uint32_t writeBuffer;
static volatile uint32_t writeBufferIndex;
static volatile uint32_t readBuffer;

static int16_t* write_buffers[NUMBER_OF_BUFFERS];
static int16_t* read_buffers[NUMBER_OF_BUFFERS];

/* Neural network weights */

static const uint16_t fft_length = FFT_LENGTH;
static const q7_t dense1_wt[DENSE1_IN_DIM*DENSE1_OUT_DIM] = DENSE1_WT;
static const q7_t dense1_bias[DENSE1_OUT_DIM] = DENSE1_BIAS;
static const q7_t dense2_wt[DENSE2_IN_DIM*DENSE2_OUT_DIM] = DENSE2_WT;
static const q7_t dense2_bias[DENSE2_OUT_DIM] = DENSE2_BIAS;
static const q7_t dense3_wt[DENSE3_IN_DIM*DENSE3_OUT_DIM] = DENSE3_WT;
static const q7_t dense3_bias[DENSE3_OUT_DIM] = DENSE3_BIAS;
static const q7_t dense4_wt[DENSE4_IN_DIM*DENSE4_OUT_DIM] = DENSE4_WT;
static const q7_t dense4_bias[DENSE4_OUT_DIM] = DENSE4_BIAS;

/* CNN weights */
/*
static const q7_t conv1_wt[CONV1_WT_NUM] = CONV1_WT;
static const q7_t conv1_bias[CONV1_OUT_CH] = CONV1_BIAS;
static const q7_t conv2_wt[CONV2_WT_NUM] = CONV2_WT;
static const q7_t conv2_bias[CONV2_OUT_CH] = CONV2_BIAS;
static const q7_t dense_l1_wt[DENSE_L1_IN_DIM*DENSE_L1_OUT_DIM] = DENSE_L1_WT;
static const q7_t dense_l1_bias[DENSE_L1_OUT_DIM] = DENSE_L1_BIAS;
static const q7_t dense_l2_wt[DENSE_L2_IN_DIM*DENSE_L2_OUT_DIM] = DENSE_L2_WT;
static const q7_t dense_l2_bias[DENSE_L2_OUT_DIM] = DENSE_L2_BIAS;
*/

/* Recording state */

static volatile bool switchPositionChanged;

/* DMA buffers */

static int16_t primaryBuffer[NUMBER_OF_SAMPLES_IN_DMA_TRANSFER];
static int16_t secondaryBuffer[NUMBER_OF_SAMPLES_IN_DMA_TRANSFER];

/* Current recording file name */

static char fileName[20];
static char resultsFileName[20];

/* Firmware version and description */

static uint8_t firmwareVersion[AM_FIRMWARE_VERSION_LENGTH] = {1, 2, 1};

static uint8_t firmwareDescription[AM_FIRMWARE_DESCRIPTION_LENGTH] = "AudioMoth-Firmware-Basic";

/* Function prototypes */

static void flashLedToIndicateBatteryLife(void);
static void filter(int16_t *source, int16_t *dest, uint8_t sampleRateDivider, uint32_t size);
static void scheduleRecording(uint32_t currentTime, uint32_t *timeOfNextRecording, uint32_t *durationOfNextRecording);
static AM_recordingState_t makeRecording(uint32_t currentTime, uint32_t recordDuration, bool enableLED, AM_batteryState_t batteryState);

/* Main function */

int main(void) {

    /* Initialise device */

    AudioMoth_initialise();

    AudioMoth_setupSWOForPrint();

	memcpy(configSettings, &defaultConfigSettings, sizeof(configSettings_t));

    uint32_t currentTime;

    AudioMoth_getTime(&currentTime, NULL);

    *timeOfNextRecording = currentTime;

    *durationOfNextRecording = configSettings->recordDuration;


    /* Make recording if appropriate */

    bool enableLED = 1;

	/* Make recording is battery check is disabled or enabled and okay */

	AM_recordingState_t recordingState = RECORDING_OKAY;

	AM_batteryState_t batteryState = AudioMoth_getBatteryState();

	recordingState = makeRecording(currentTime, *durationOfNextRecording, enableLED, batteryState);

}

/* Time zone handler */

inline void AudioMoth_timezoneRequested(int8_t *timezone) {

    *timezone = configSettings->timezone;

}


/* AudioMoth interrupt handlers */

inline void AudioMoth_handleSwitchInterrupt() {

    switchPositionChanged = true;

}

inline void AudioMoth_handleMicrophoneInterrupt(int16_t sample) { }

inline void AudioMoth_handleDirectMemoryAccessInterrupt(bool isPrimaryBuffer, int16_t **nextBuffer) {

    int16_t *source = secondaryBuffer;

    if (isPrimaryBuffer) source = primaryBuffer;

    /* Update the current buffer index and write buffer */

    filter(source, write_buffers[writeBuffer] + writeBufferIndex, configSettings->sampleRateDivider, NUMBER_OF_SAMPLES_IN_DMA_TRANSFER);

    writeBufferIndex += NUMBER_OF_SAMPLES_IN_DMA_TRANSFER / configSettings->sampleRateDivider;

    if (writeBufferIndex == NUMBER_OF_SAMPLES_IN_BUFFER) {

        writeBufferIndex = 0;

        writeBuffer = (writeBuffer + 1) & (NUMBER_OF_BUFFERS - 1);

    }

}

/* AudioMoth USB message handlers */

inline void AudioMoth_usbFirmwareVersionRequested(uint8_t **firmwareVersionPtr) {

    *firmwareVersionPtr = firmwareVersion;

}

inline void AudioMoth_usbFirmwareDescriptionRequested(uint8_t **firmwareDescriptionPtr) {

    *firmwareDescriptionPtr = firmwareDescription;

}

inline void AudioMoth_usbApplicationPacketRequested(uint32_t messageType, uint8_t *transmitBuffer, uint32_t size) {

    /* Copy the current time to the USB packet */

    uint32_t currentTime;

    AudioMoth_getTime(&currentTime, NULL);

    memcpy(transmitBuffer + 1, &currentTime, 4);

    /* Copy the unique ID to the USB packet */

    memcpy(transmitBuffer + 5, (uint8_t*)AM_UNIQUE_ID_START_ADDRESS, AM_UNIQUE_ID_SIZE_IN_BYTES);

    /* Copy the battery state to the USB packet */

    AM_batteryState_t batteryState = AudioMoth_getBatteryState();

    memcpy(transmitBuffer + 5 + AM_UNIQUE_ID_SIZE_IN_BYTES, &batteryState, 1);

    /* Copy the firmware version to the USB packet */

    memcpy(transmitBuffer + 6 + AM_UNIQUE_ID_SIZE_IN_BYTES, firmwareVersion, AM_FIRMWARE_VERSION_LENGTH);

    /* Copy the firmware description to the USB packet */

    memcpy(transmitBuffer + 6 + AM_UNIQUE_ID_SIZE_IN_BYTES + AM_FIRMWARE_VERSION_LENGTH, firmwareDescription, AM_FIRMWARE_DESCRIPTION_LENGTH);

}

inline void AudioMoth_usbApplicationPacketReceived(uint32_t messageType, uint8_t* receiveBuffer, uint8_t *transmitBuffer, uint32_t size) {

    /* Copy the USB packet contents to the back-up register data structure location */

    memcpy(configSettings, receiveBuffer + 1, sizeof(configSettings_t));

    /* Copy the back-up register data structure to the USB packet */

    memcpy(transmitBuffer + 1, configSettings, sizeof(configSettings_t));

    /* Set the time */

    AudioMoth_setTime(configSettings->time, 0);

}

/* Remove DC offset from the microphone samples */

static void filter(int16_t *source, int16_t *dest, uint8_t sampleRateDivider, uint32_t size) {

    int32_t filteredOutput;
    int32_t scaledPreviousFilterOutput;

    int index = 0;

    for (int i = 0; i < size; i += sampleRateDivider) {

        int32_t sample = 0;

        for (int j = 0; j < sampleRateDivider; j += 1) {

            sample += (int32_t)source[i + j];

        }

        if (bitsToShift > 0) sample <<= bitsToShift;

        if (bitsToShift < 0) sample >>= -bitsToShift;

        scaledPreviousFilterOutput = (int32_t)(DC_BLOCKING_FACTOR * previousFilterOutput);

        filteredOutput = sample - previousSample + scaledPreviousFilterOutput;

        if (filteredOutput > INT16_MAX) {

            dest[index++] = INT16_MAX;

        } else if (filteredOutput < INT16_MIN) {

            dest[index++] = INT16_MIN;

        } else {

            dest[index++] = (int16_t)filteredOutput;

        }

        previousFilterOutput = filteredOutput;

        previousSample = sample;

    }

}

/* Save recording to SD card */

static AM_recordingState_t makeRecording(uint32_t currentTime, uint32_t recordDuration, bool enableLED, AM_batteryState_t batteryState) {

    /* Initialise buffers */

    writeBuffer = 0;
    readBuffer = 0;

    writeBufferIndex = 0;

	q7_t* mid_buffer = (q7_t*) AM_EXTERNAL_SRAM_START_ADDRESS;
    read_buffers[0] = (q15_t*) (mid_buffer + DNN_TOTAL_BUFFER_SIZE);
    write_buffers[0] = read_buffers[0] + NUMBER_OF_OVERLAP_SAMPLES;

    for (int i = 1; i < NUMBER_OF_BUFFERS; i += 1) {
        read_buffers[i] = read_buffers[i - 1] + NUMBER_OF_SAMPLES_IN_BUFFER;
        write_buffers[i] = read_buffers[i] + NUMBER_OF_OVERLAP_SAMPLES;
    }

    float32_t fft_input[FFT_LENGTH] = {};
	float32_t fft_output[FFT_LENGTH] = {};
	float32_t hamming[FFT_LENGTH];
	for(int i = 0; i < fft_length; i++){
	  hamming[i] = 0.54f - 0.46f*(arm_cos_f32(2*M_PI*i/(fft_length-1)));
	}

	q7_t results[MAX_ANALYSIS_TIME];
    q7_t output[NN_OUTPUT_SIZE];
	float32_t spectrogram_buffer[NN_INPUT_SIZE];
	q7_t feature_map[NN_INPUT_SIZE];

    /* Calculate the bits to shift */

    bitsToShift = 0;

    uint16_t oversampling = configSettings->oversampleRate * configSettings->sampleRateDivider;

    while (oversampling > 16) {
        oversampling >>= 1;
        bitsToShift -= 1;
    }

    while (oversampling < 16) {
        oversampling <<= 1;
        bitsToShift += 1;
    }

    /* Calculate recording parameters */

    uint32_t numberOfSamplesInHeader = sizeof(wavHeader) >> 1;

    uint32_t numberOfSamples = configSettings->sampleRate / configSettings->sampleRateDivider * recordDuration;

    /* Enable the battery monitor */

    if (configSettings->enableBatteryCheck) {

        AudioMoth_enableBatteryMonitor();

        AudioMoth_setBatteryMonitorThreshold(AM_BATTERY_LOW);

    }

    /* Initialise microphone for recording */

    AudioMoth_enableExternalSRAM();

    AudioMoth_enableMicrophone(configSettings->gain, configSettings->clockDivider, configSettings->acquisitionCycles, configSettings->oversampleRate);

    AudioMoth_initialiseDirectMemoryAccess(primaryBuffer, secondaryBuffer, NUMBER_OF_SAMPLES_IN_DMA_TRANSFER);

    AudioMoth_startMicrophoneSamples(configSettings->sampleRate);

    /* Initialise file system and open a new file */
   
    if (enableLED) {

        AudioMoth_setRedLED(true);

    }

    RETURN_ON_ERROR(AudioMoth_enableFileSystem());

    /* Open a file with the current local time as the name */

    time_t rawtime = currentTime + configSettings->timezone * SECONDS_IN_HOUR;

    struct tm *time = gmtime(&rawtime);

    sprintf(fileName, "%04d%02d%02d_%02d%02d%02d.WAV", 1900 + time->tm_year, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);
    sprintf(resultsFileName, "%04d%02d%02d_%02d%02d%02d.TXT", 1900 + time->tm_year, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);

    RETURN_ON_ERROR(AudioMoth_openFile(fileName));

    if (enableLED) {

        AudioMoth_setRedLED(false);

    }

    /* Termination conditions */

    switchPositionChanged = false;

    bool batteryVoltageLow = false;

    /* Main record loop */

    uint32_t samplesWritten = 0;

    uint32_t buffersProcessed = 0;

    uint32_t analysisIndex = 0;

    readBuffer = writeBuffer;

    while (samplesWritten < numberOfSamples + numberOfSamplesInHeader && !switchPositionChanged && !batteryVoltageLow) {

        while (readBuffer != writeBuffer && samplesWritten < numberOfSamples + numberOfSamplesInHeader && !switchPositionChanged && !batteryVoltageLow) {

            /* Light LED during SD card write if appropriate */

            if (enableLED) {

                AudioMoth_setRedLED(true);

            }

            /* Write the appropriate number of bytes to the SD card */

            uint32_t numberOfSamplesToWrite = 0;

            if (buffersProcessed >= NUMBER_OF_BUFFERS_TO_SKIP) {

                numberOfSamplesToWrite = MIN(numberOfSamples + numberOfSamplesInHeader - samplesWritten, NUMBER_OF_SAMPLES_IN_BUFFER);

            }

            /* Analyze sample */

			if(buffersProcessed < NUMBER_OF_BUFFERS_TO_SKIP){
				wave_to_spectrogram(fft_input, fft_output, read_buffers[readBuffer]+NUMBER_OF_OVERLAP_SAMPLES, (int32_t) NUMBER_OF_SAMPLES_IN_BUFFER, spectrogram_buffer, feature_map, hamming, (uint16_t) FFT_LENGTH);
			}
			else {
				wave_to_spectrogram(fft_input, fft_output, read_buffers[readBuffer], (int32_t) NUMBER_OF_SAMPLES_IN_BUFFER + NUMBER_OF_OVERLAP_SAMPLES, spectrogram_buffer, feature_map + NN_INPUT_TO_COPY_LEN, hamming, (uint16_t) FFT_LENGTH);
				forward_propagate_q7_dnn(feature_map, mid_buffer, (results+analysisIndex), dense1_wt, dense1_bias,
						dense2_wt, dense2_bias, dense3_wt, dense3_bias, dense4_wt, dense4_bias);
				memcpy(feature_map, feature_map+NN_INPUT_TO_WRITE_LEN, NN_INPUT_TO_COPY_LEN);

				analysisIndex++;

				if(results[analysisIndex] > DETECTION_THRESHOLD){
					AudioMoth_setGreenLED(true);

					AudioMoth_delay(500);

					AudioMoth_setGreenLED(false);
				}
			}
			if(readBuffer == NUMBER_OF_BUFFERS-1){
				memcpy(read_buffers[0], read_buffers[readBuffer]+NUMBER_OF_SAMPLES_IN_BUFFER, NUMBER_OF_OVERLAP_SAMPLES*2);
			}

            /* Write the recording to the SD card*/

            RETURN_ON_ERROR(AudioMoth_writeToFile(write_buffers[readBuffer], 2 * numberOfSamplesToWrite));

            /* Increment buffer counters */

            readBuffer = (readBuffer + 1) & (NUMBER_OF_BUFFERS - 1);

            samplesWritten += numberOfSamplesToWrite;

            buffersProcessed += 1;

            /* Clear LED */

            AudioMoth_setRedLED(false);

        }

        /* Enable the battery monitor */

        if (configSettings->enableBatteryCheck) {

            AudioMoth_enableBatteryMonitor();

            AudioMoth_setBatteryMonitorThreshold(AM_BATTERY_LOW);

        }

        /* Sleep until next DMA transfer is complete */

        AudioMoth_sleep();

    }

    if (configSettings->enableBatteryCheck ) {

        AudioMoth_disableBatteryMonitor();

    }

    /* Initialise the WAV header */

    samplesWritten = MAX(numberOfSamplesInHeader, samplesWritten);

    setHeaderDetails(configSettings->sampleRate / configSettings->sampleRateDivider, samplesWritten - numberOfSamplesInHeader);

    setHeaderComment(currentTime, configSettings->timezone, (uint8_t*)AM_UNIQUE_ID_START_ADDRESS, configSettings->gain, batteryState, batteryVoltageLow, switchPositionChanged);

    /* Write the header */

    AudioMoth_setRedLED(true);

    RETURN_ON_ERROR(AudioMoth_seekInFile(0));

    RETURN_ON_ERROR(AudioMoth_writeToFile(&wavHeader, sizeof(wavHeader)));

    /* Close the file */

    RETURN_ON_ERROR(AudioMoth_closeFile());

    /* Write the results */

    RETURN_ON_ERROR(AudioMoth_openFile(resultsFileName));

    for(int i =0; i<analysisIndex; i++){
    	char res[RESULT_LENGTH];
    	sprintf(res, "%04d,", results[i]);
    	RETURN_ON_ERROR(AudioMoth_writeToFile(res, RESULT_LENGTH));
    }

    RETURN_ON_ERROR(AudioMoth_closeFile());

    if (enableLED) {

        AudioMoth_setRedLED(false);

    }

    /* Return with state */

    if (batteryVoltageLow) return BATTERY_CHECK;

    if (switchPositionChanged) return SWITCH_CHANGED;

    return RECORDING_OKAY;

}

static void scheduleRecording(uint32_t currentTime, uint32_t *timeOfNextRecording, uint32_t *durationOfNextRecording) {

    /* Check number of active state stop periods */

    if (configSettings->activeStartStopPeriods > MAX_START_STOP_PERIODS) {

        configSettings->activeStartStopPeriods = MAX_START_STOP_PERIODS;

    }

    /* No active periods */

    if (configSettings->activeStartStopPeriods == 0) {

        *timeOfNextRecording = UINT32_MAX;

        *durationOfNextRecording = configSettings->recordDuration;

        return;

    }

    /* Calculate the number of seconds of this day */

    time_t rawtime = currentTime;

    struct tm *time = gmtime(&rawtime);

    uint32_t currentSeconds = SECONDS_IN_HOUR * time->tm_hour + SECONDS_IN_MINUTE * time->tm_min + time->tm_sec;

    /* Check each active start stop period */

    uint32_t durationOfCycle = configSettings->recordDuration + configSettings->sleepDuration;

    for (uint32_t i = 0; i < configSettings->activeStartStopPeriods; i += 1) {

        startStopPeriod_t *period = configSettings->startStopPeriods + i;

        /* Calculate the start and stop time of the current period */

        uint32_t startSeconds = SECONDS_IN_MINUTE * period->startMinutes;

        uint32_t stopSeconds = SECONDS_IN_MINUTE * period->stopMinutes;

        /* Calculate time to next period or time to next start in this period */

        if (currentSeconds < startSeconds) {

            *timeOfNextRecording = currentTime + (startSeconds - currentSeconds);

            *durationOfNextRecording = MIN(configSettings->recordDuration, stopSeconds - startSeconds);

            return;

        } else if (currentSeconds < stopSeconds) {

            uint32_t cycles = (currentSeconds - startSeconds + durationOfCycle) / durationOfCycle;

            uint32_t secondsFromStartOfPeriod = cycles * durationOfCycle;

            if (secondsFromStartOfPeriod < stopSeconds - startSeconds) {

                *timeOfNextRecording = currentTime + (startSeconds - currentSeconds) + secondsFromStartOfPeriod;

                *durationOfNextRecording = MIN(configSettings->recordDuration, stopSeconds - startSeconds - secondsFromStartOfPeriod);

                return;

            }

        }

    }

    /* Calculate time until first period tomorrow */

    startStopPeriod_t *firstPeriod = configSettings->startStopPeriods;

    uint32_t startSeconds = SECONDS_IN_MINUTE * firstPeriod->startMinutes;

    uint32_t stopSeconds = SECONDS_IN_MINUTE * firstPeriod->stopMinutes;

    *timeOfNextRecording = currentTime + (SECONDS_IN_DAY - currentSeconds) + startSeconds;

    *durationOfNextRecording = MIN(configSettings->recordDuration, stopSeconds - startSeconds);

}

static void flashLedToIndicateBatteryLife(void){

    uint32_t numberOfFlashes = LOW_BATTERY_LED_FLASHES;

    AM_batteryState_t batteryState = AudioMoth_getBatteryState();

    /* Set number of flashes according to battery state */

    if (batteryState > AM_BATTERY_LOW) {

        numberOfFlashes = (batteryState >= AM_BATTERY_4V6) ? 4 : (batteryState >= AM_BATTERY_4V4) ? 3 : (batteryState >= AM_BATTERY_4V0) ? 2 : 1;

    }

    /* Flash LED */

    for (uint32_t i = 0; i < numberOfFlashes; i += 1) {

        FLASH_LED(Red, SHORT_LED_FLASH_DURATION)

        if (numberOfFlashes == LOW_BATTERY_LED_FLASHES) {

            AudioMoth_delay(SHORT_LED_FLASH_DURATION);

        } else {

            AudioMoth_delay(LONG_LED_FLASH_DURATION);

        }

    }

}

