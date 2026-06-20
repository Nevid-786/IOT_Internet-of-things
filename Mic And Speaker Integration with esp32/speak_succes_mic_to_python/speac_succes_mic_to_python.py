import serial
import numpy as np
from scipy.io.wavfile import write
import speech_recognition as sr
import tempfile
import os
ser = serial.Serial("COM3", 921600)
RATE = 16000
SEC = 3
r = sr.Recognizer()
print("Listening...")
while True:
    raw = ser.read(RATE * SEC * 2)
    audio = np.frombuffer(raw, dtype=np.int16)
    peak = np.max(np.abs(audio))
    print("Volume:", peak)

    if peak < 700:
        print("Silence")
        continue

    # reject clipping
    if peak > 28000:
        print("Audio clipping")
        continue

    temp = tempfile.NamedTemporaryFile(
        delete=False,
        suffix=".wav"
    )

    write(temp.name, RATE, audio)

    try:

        with sr.AudioFile(temp.name) as src:

            data = r.record(src)

            text = r.recognize_google(data)

            print("You said:", text)

    except:
        print("Could not understand")

    finally:

        temp.close()

        os.remove(temp.name)