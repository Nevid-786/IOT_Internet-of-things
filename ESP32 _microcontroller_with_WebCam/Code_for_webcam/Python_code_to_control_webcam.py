import cv2
import mediapipe as mp
import numpy as np
import os
import absl.logging
from collections import deque
import serial
import time

# ------------------- Serial Setup -------------------
# Change COM3 to your ESP port (Linux: /dev/ttyUSB0 or /dev/ttyACM0, Mac: /dev/cu.usbserial)
SERIAL_PORT = "COM6"  
BAUD_RATE = 115200

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    time.sleep(2)  # wait for ESP to reset
    print(f"✅ Serial connected to {SERIAL_PORT} at {BAUD_RATE}")
except Exception as e:
    print(f"❌ Serial connection failed: {e}")
    ser = None

# Suppress TensorFlow / Mediapipe warnings
absl.logging.set_verbosity(absl.logging.ERROR)
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

# ------------------- Functions -------------------
def euclidean_dist(p1, p2):
    return np.linalg.norm(np.array(p1) - np.array(p2))

def calculate_ear(landmarks, indices):
    A = euclidean_dist(landmarks[indices[1]], landmarks[indices[5]])
    B = euclidean_dist(landmarks[indices[2]], landmarks[indices[4]])
    C = euclidean_dist(landmarks[0], landmarks[3])
    return (A + B) / (2.0 * C)

def calculate_mar(landmarks, indices):
    A = euclidean_dist(landmarks[13], landmarks[19])
    B = euclidean_dist(landmarks[14], landmarks[18])
    C = euclidean_dist(landmarks[15], landmarks[17])
    D = euclidean_dist(landmarks[12], landmarks[16])
    return (A + B + C) / (2.0 * D)

def calculate_yaw(landmarks):
    left = np.array(landmarks[234])  
    right = np.array(landmarks[454]) 
    nose = np.array(landmarks[1])    
    return (nose[0] - (left[0] + right[0]) / 2) / euclidean_dist(left, right)

def classify_driver_state(ear, mar, yaw):
    if yaw > 0.15:
        return "Left"
    elif yaw < -0.15:
        return "Right"
    if ear < 0.19 and mar > 0.28:
        return "Drowsy"
    return "Alert"

def send_to_esp(state):
    if ser:
        try:
            ser.write((state + "\n").encode())  # send state as string with newline
        except:
            pass

# ------------------- Mediapipe Setup -------------------
mp_face_mesh = mp.solutions.face_mesh
face_mesh = mp_face_mesh.FaceMesh(refine_landmarks=True, max_num_faces=1)

# ------------------- Camera Setup -------------------
cap = None
for i in range(3):
    temp_cap = cv2.VideoCapture(i, cv2.CAP_DSHOW)
    if temp_cap.isOpened():
        cap = temp_cap
        print(f"✅ Camera opened at index {i}")
        break

if cap is None:
    print("❌ No camera detected!")
    exit()

# ------------------- Calibration -------------------
print("⏳ Hold your head straight for calibration (2 seconds)...")
yaw_samples = []
for _ in range(60):
    ret, frame = cap.read()
    if not ret:
        continue
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = face_mesh.process(rgb)
    if results.multi_face_landmarks:
        face_landmarks = results.multi_face_landmarks[0]
        h, w, _ = frame.shape
        landmarks = [(int(pt.x * w), int(pt.y * h)) for pt in face_landmarks.landmark]
        yaw_samples.append(calculate_yaw(landmarks))
yaw_center = np.mean(yaw_samples) if yaw_samples else 0.0
print(f"✅ Calibration done. Center yaw = {yaw_center:.3f}")

# ------------------- State Smoothing -------------------
state_buffer = deque(maxlen=5)

# ------------------- Main Loop -------------------
last_state = None
while True:
    ret, frame = cap.read()
    if not ret:
        print("❌ Failed to grab frame")
        break

    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = face_mesh.process(rgb)

    if results.multi_face_landmarks:
        for face_landmarks in results.multi_face_landmarks:
            h, w, _ = frame.shape
            landmarks = [(int(pt.x * w), int(pt.y * h)) for pt in face_landmarks.landmark]

            ear = calculate_ear(landmarks, [33, 160, 158, 133, 153, 144])
            mar = calculate_mar(landmarks, range(61, 81))
            yaw_raw = calculate_yaw(landmarks)
            yaw = yaw_raw - yaw_center

            raw_state = classify_driver_state(ear, mar, yaw)
            state_buffer.append(raw_state)
            state = max(set(state_buffer), key=state_buffer.count)

            # Send only if state changes (to avoid spamming ESP)
            if state != last_state:
                send_to_esp(state)
                last_state = state
                print(f"📡 Sent to ESP: {state}")

            cv2.putText(frame, f"EAR: {ear:.2f}  MAR: {mar:.2f}  Yaw: {yaw:.2f}", 
                        (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
            cv2.putText(frame, f"State: {state}", 
                        (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 0, 255), 2)

    cv2.imshow("Driver Monitoring", frame)

    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()
if ser:
    ser.close()
