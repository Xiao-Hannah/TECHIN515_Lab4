#include <magic_wand_hannah_inferencing.h> 
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// MPU sensor
Adafruit_MPU6050 mpu;

// LED pins (GPIO)
#define YELLOW_LED 3  // GPIO3
#define GREEN_LED  4  // GPIO4
#define WHITE_LED  9  // GPIO9

// Sampling config
#define SAMPLE_RATE_MS 10
#define CAPTURE_DURATION_MS 1000
#define FEATURE_SIZE EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE

// Data capture state
bool capturing = false;
unsigned long last_sample_time = 0;
unsigned long capture_start_time = 0;
int sample_count = 0;

// Feature buffer
float features[FEATURE_SIZE];

// Required by Edge Impulse classifier
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}

void setup()
{
    Serial.begin(115200);
    Wire.begin(6, 7);  // SDA = GPIO6, SCL = GPIO7

    // LED setup
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(WHITE_LED, OUTPUT);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(WHITE_LED, LOW);

    // MPU init
    Serial.println("Initializing MPU6050...");
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) { delay(10); }
    }

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    Serial.println("MPU6050 ready. Send 'o' to start inference.");
}

void capture_accelerometer_data() {
    if (millis() - last_sample_time >= SAMPLE_RATE_MS) {
        last_sample_time = millis();

        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);

        if (sample_count < FEATURE_SIZE / 3) {
            int idx = sample_count * 3;
            features[idx] = a.acceleration.x;
            features[idx + 1] = a.acceleration.y;
            features[idx + 2] = a.acceleration.z;
            sample_count++;
        }

        if (millis() - capture_start_time >= CAPTURE_DURATION_MS) {
            capturing = false;
            run_inference();
        }
    }
}

void run_inference() {
    ei_impulse_result_t result = { 0 };

    signal_t features_signal;
    features_signal.total_length = FEATURE_SIZE;
    features_signal.get_data = &raw_feature_get_data;

    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false);
    if (res != EI_IMPULSE_OK) {
        Serial.print("Classifier failed: "); Serial.println(res);
        return;
    }

    int max_index = -1;
    float max_value = 0.0;
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        if (result.classification[i].value > max_value) {
            max_value = result.classification[i].value;
            max_index = i;
        }
    }

    Serial.print("Prediction: ");
    Serial.println(ei_classifier_inferencing_categories[max_index]);

    // Reset all LEDs
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(WHITE_LED, LOW);

    // React to detected gesture
    if (strcmp(ei_classifier_inferencing_categories[max_index], "O") == 0) {
        digitalWrite(WHITE_LED, HIGH);
    } 
    else if (strcmp(ei_classifier_inferencing_categories[max_index], "V") == 0) {
        digitalWrite(GREEN_LED, HIGH);
    } 
    else if (strcmp(ei_classifier_inferencing_categories[max_index], "Z") == 0) {
        // Blink green and yellow 3 times
        for (int i = 0; i < 3; i++) {
            digitalWrite(GREEN_LED, HIGH);
            digitalWrite(YELLOW_LED, LOW);
            delay(150);
            digitalWrite(GREEN_LED, LOW);
            digitalWrite(YELLOW_LED, HIGH);
            delay(150);
        }
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(YELLOW_LED, LOW);
    }
}

void loop() {
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        if (cmd == 'o') {
            sample_count = 0;
            capturing = true;
            capture_start_time = millis();
            last_sample_time = millis();
            Serial.println("Starting capture...");
        }
    }

    if (capturing) {
        capture_accelerometer_data();
    }
}