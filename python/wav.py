import paho.mqtt.client as mqtt
import pyaudio
import wave

topic = "deviceid/team3/evt/angle"
server = "54.81.240.10"

chunk = 1024


def on_connect(client, userdata, flags, rc):
    print("Connected with RC : " + str(rc))
    client.subscribe(topic)


def on_message(client, userdata, msg):
    global path

    print(msg.topic+" "+str(msg.payload.decode('UTF-8')))
    if int(msg.payload.decode('UTF-8')) == 1:
        path = 'closehh.wav'
        with wave.open(path, 'rb') as f:
            p = pyaudio.PyAudio()
            stream = p.open(format=p.get_format_from_width(f.getsampwidth()),
                            channels=f.getnchannels(),
                            rate=f.getframerate(),
                            output=True)

            data = f.readframes(chunk)
            while data:
                stream.write(data)
                data = f.readframes(chunk)

            stream.stop_stream()
            stream.close()

            p.terminate()
    elif int(msg.payload.decode('UTF-8')) == 2:
        path = 'crash.wav'
        with wave.open(path, 'rb') as f:
            p = pyaudio.PyAudio()
            stream = p.open(format=p.get_format_from_width(f.getsampwidth()),
                            channels=f.getnchannels(),
                            rate=f.getframerate(),
                            output=True)

            data = f.readframes(chunk)
            while data:
                stream.write(data)
                data = f.readframes(chunk)

            stream.stop_stream()
            stream.close()

            p.terminate()
    elif int(msg.payload.decode('UTF-8')) == 3:
        path = 'scare.wav'
        with wave.open(path, 'rb') as f:
            p = pyaudio.PyAudio()
            stream = p.open(format=p.get_format_from_width(f.getsampwidth()),
                            channels=f.getnchannels(),
                            rate=f.getframerate(),
                            output=True)

            data = f.readframes(chunk)
            while data:
                stream.write(data)
                data = f.readframes(chunk)

            stream.stop_stream()
            stream.close()

            p.terminate()
    elif int(msg.payload.decode('UTF-8')) == 4:
        path = 'tom1.wav'
        with wave.open(path, 'rb') as f:
            p = pyaudio.PyAudio()
            stream = p.open(format=p.get_format_from_width(f.getsampwidth()),
                            channels=f.getnchannels(),
                            rate=f.getframerate(),
                            output=True)

            data = f.readframes(chunk)
            while data:
                stream.write(data)
                data = f.readframes(chunk)

            stream.stop_stream()
            stream.close()

            p.terminate()
    elif int(msg.payload.decode('UTF-8')) == 5:
        path = 'tom2.wav'
        with wave.open(path, 'rb') as f:
            p = pyaudio.PyAudio()
            stream = p.open(format=p.get_format_from_width(f.getsampwidth()),
                            channels=f.getnchannels(),
                            rate=f.getframerate(),
                            output=True)

            data = f.readframes(chunk)
            while data:
                stream.write(data)
                data = f.readframes(chunk)

            stream.stop_stream()
            stream.close()

            p.terminate()
    elif int(msg.payload.decode('UTF-8')) == 6:
        path = 'tom3.wav'
        with wave.open(path, 'rb') as f:
            p = pyaudio.PyAudio()
            stream = p.open(format=p.get_format_from_width(f.getsampwidth()),
                            channels=f.getnchannels(),
                            rate=f.getframerate(),
                            output=True)

            data = f.readframes(chunk)
            while data:
                stream.write(data)
                data = f.readframes(chunk)

            stream.stop_stream()
            stream.close()

            p.terminate()
    elif int(msg.payload.decode('UTF-8')) == 7:
        path = 'Kick1.wav'
        with wave.open(path, 'rb') as f:
            p = pyaudio.PyAudio()
            stream = p.open(format=p.get_format_from_width(f.getsampwidth()),
                            channels=f.getnchannels(),
                            rate=f.getframerate(),
                            output=True)

            data = f.readframes(chunk)
            while data:
                stream.write(data)
                data = f.readframes(chunk)

            stream.stop_stream()
            stream.close()

            p.terminate()


client = mqtt.Client()
client.connect(server, 1883, 60)
client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()
