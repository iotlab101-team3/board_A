import pyaudio # pip install pipwin 하고 pipwin install pyaudio
import wave

chunk = 1024

path = 'Tom2.wav' # 가상 환경에서 돌려야함

with wave.open(path, 'rb') as f:
    p = pyaudio.PyAudio()
    stream = p.open(format = p.get_format_from_width(f.getsampwidth()),
                    channels = f.getnchannels(),
                    rate = f.getframerate(),
                    output = True)

    data = f.readframes(chunk)
    while data:
        stream.write(data)
        data = f.readframes(chunk)

    stream.stop_stream()
    stream.close()

    p.terminate()