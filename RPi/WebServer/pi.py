import paho.mqtt.client as mqtt
from paho.mqtt.enums import CallbackAPIVersion


def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")

def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))
    # if msg.topic == "/leds/pi":
    #     if msg.payload == b"ON":
    #         GPIO.output(LED_PIN, GPIO.HIGH)
    #     elif msg.payload == b"OFF":
    #         GPIO.output(LED_PIN, GPIO.LOW)
    #     elif msg.payload == b"TOGGLE":
    #         GPIO.output(LED_PIN, not GPIO.input(LED_PIN))

client = mqtt.Client(callback_api_version=CallbackAPIVersion(2))
client.on_connect = on_connect
client.on_message = on_message