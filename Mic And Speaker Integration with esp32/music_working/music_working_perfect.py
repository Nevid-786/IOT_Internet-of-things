import serial
import numpy as np
import time
import soundfile as sf
from scipy.signal import resample

PORT = "COM3"
BAUD = 921600
TARGET_SR = 16000
CHUNK = 256

ser = serial.Serial(PORT, BAUD)
time.sleep(2)

audio, sr = sf.read("voice.wav")

if len(audio.shape) > 1:
    audio = audio[:, 0]

if sr != TARGET_SR:
    audio = resample(audio, int(len(audio) * TARGET_SR / sr))
    
audio = audio * 4.0   # 2x louder
audio = np.clip(audio, -1.0, 1.0)
audio = (audio * 32767).astype(np.int16)
# audio = np.clip(audio, -1.0, 1.0)
# audio = (audio * 32767).astype(np.int16)

print("Original SR:", sr)
print("Streaming...")

start = time.perf_counter()

for i in range(0, len(audio), CHUNK):

    block = audio[i:i + CHUNK]

    if len(block) < CHUNK:
        block = np.pad(block, (0, CHUNK - len(block)))

    ser.write(block.tobytes())

    next_time = start + ((i + CHUNK) / TARGET_SR)

    while time.perf_counter() < next_time:
        pass

print("Done")
ser.close()