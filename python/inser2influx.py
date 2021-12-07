import paho.mqtt.client as mqtt
import requests

topic = "deviceid/team3/evt/angle"
server = "3.84.34.84"

def on_connect(client, userdata, flags, rc):
    print("Connected with RC : " + str(rc))
    client.subscribe(topic)

def on_message(client, userdata, msg):
    if msg.topic == topic.replace("#","angle"):
        angle = float(msg.payload)
        r = requests.post(f'http://3.84.34.84:8086/write?db=sensorDB', data=f'environ,did=esp12 angle={angle}')

client = mqtt.Client()
client.connect(server, 1883, 60)
client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()