# TECHIN515_Lab4

# Magic Wand: Gesture Recognition with ESP32 + Edge Impulse

This project implements a gesture-controlled magic wand using an ESP32 microcontroller, an MPU6050 IMU sensor, and Edge Impulse for real-time gesture classification. The system supports three gestures mapped to spells: Fire Bolt (Z), Reflect Shield (O), and Healing Spell (V).

## 🛠 Hardware Components

- ESP32 Dev Board (e.g., XIAO_ESP32C3)
- MPU6050 Accelerometer & Gyroscope Sensor
- LED (for visual feedback)
- Perf board with soldered connections
- Battery (for standalone operation)
- Wires and basic soldering tools
- Custom enclosure for the wand

## 🔌 Wiring Instructions

| MPU6050 Pin | ESP32 Pin       |
|-------------|------------------|
| VCC         | 3.3V             |
| GND         | GND              |
| SDA         | GPIO21 (I2C SDA) |
| SCL         | GPIO22 (I2C SCL) |

Ensure all connections are stable. For best results, solder the connections onto a perf board to avoid loose wires.

## 📈 Data Collection Instructions

1. Upload `gesture_capture.ino` to your ESP32 board.
2. Run the following command:python process_gesture_data.py –gesture “Z” –person “your_name”
3. Perform each gesture consistently. Collect at least 150 samples per gesture (Z, O, V).
4. Data will be saved under `/dataset/<label>/`.

## 🧠 Model Training with Edge Impulse

1. Create a new project at [Edge Impulse](https://studio.edgeimpulse.com/).
2. Upload your data under "Data acquisition", using auto-split for training/testing.
3. Design your impulse:
- Input: Time series (x, y, z)
- Processing block: Spectral Features
- Learning block: Neural Network (Classifier)
- Parameters: Window size = 98 ms, Stride = 1 ms, Frequency = 100 Hz
4. Training configuration:
- 100 epochs
- Learning rate: 0.0005
- Batch size: 32
- Network: Dense(40) → Dense(20) → Dense(10)
5. Evaluate using Live Classification and Model Testing.
6. Deploy via "Arduino Library" and extract to `/edge-impulse-export/`.

## 🚀 Uploading and Testing

1. Open `wand.ino`.
2. Include the exported model header.
3. Adjust output pins and gesture handling as needed.
4. Upload to ESP32 and monitor predictions over Serial (115200 baud) or via LEDs.

## 🎬 Demo Video

Watch the live demo here: [[Demo Video Link](https://drive.google.com/drive/folders/1uvAmPBLtT0bdmchEe4QJf39zpsE4dv-S?dmr=1&ec=wgc-drive-hero-goto)]

## 🛠 Dependencies

### Arduino Libraries:
- Adafruit MPU6050
- Adafruit Sensor
- Wire

### Python:
- Python ≥ 3.8  
- Install packages:pip install -r requirements.txt

## 🧪 Troubleshooting

- **Sensor not detected?** Check power and I2C connections. Use soldering if needed.
- **No data received?** Confirm correct COM port and baud rate (115200).
- **Unreliable prediction?** Add more training samples, tweak impulse settings, or tune model layers.
