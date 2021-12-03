import paho.mqtt.client as mqtt
import pyaudio
import wave

topic = "deviceid/mj/angle"
server = "3.84.34.84"

chunk = 1024

def on_connect(client, userdata, flags, rc):
    print("Connected with RC : " + str(rc))
    client.subscribe(topic)

def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload.decode('UTF-8')))
    if int(msg.payload.decode('UTF-8')) == 1:
        client.publish("deviceid/jj/cmd/wav", "Tom2.wav")
        path = 'Tom2.wav'
    elif int(msg.payload.decode('UTF-8')) == 2:
        client.publish("deviceid/jj/cmd/wav", "Tom3.wav")
        path = 'Tom3.wav'

client = mqtt.Client()
client.connect(server, 1883, 60)
client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()

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