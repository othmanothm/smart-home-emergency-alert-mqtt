import os
import requests
from dotenv import load_dotenv
import paho.mqtt.client as mqtt

print("Python script started...")

load_dotenv()

BOT_TOKEN = os.getenv("BOT_TOKEN")
CHAT_ID = os.getenv("CHAT_ID")

MQTT_BROKER = os.getenv("MQTT_BROKER", "broker.hivemq.com")
MQTT_PORT = int(os.getenv("MQTT_PORT", "1883"))
MQTT_TOPIC = os.getenv("MQTT_TOPIC", "othman/home/emergency/alert")

print("BOT_TOKEN loaded:", "YES" if BOT_TOKEN else "NO")
print("CHAT_ID loaded:", CHAT_ID if CHAT_ID else "NO")
print("MQTT_BROKER:", MQTT_BROKER)
print("MQTT_TOPIC:", MQTT_TOPIC)


def send_telegram_message(text):
    if not BOT_TOKEN or not CHAT_ID:
        print("Missing BOT_TOKEN or CHAT_ID in .env")
        return

    url = f"https://api.telegram.org/bot{BOT_TOKEN}/sendMessage"

    payload = {
        "chat_id": CHAT_ID,
        "text": text
    }

    try:
        response = requests.post(url, data=payload, timeout=10)
        print("Telegram response:", response.status_code)
        print(response.text)
    except Exception as error:
        print("Telegram error:", error)


def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT broker")
        client.subscribe(MQTT_TOPIC)
        print("Subscribed to:", MQTT_TOPIC)
    else:
        print("MQTT connection failed, code:", rc)


def on_message(client, userdata, msg):
    message = msg.payload.decode(errors="ignore")
    print("MQTT received:", msg.topic, "->", message)

    telegram_text = "Smart Home Emergency Alert:\n" + message
    send_telegram_message(telegram_text)


print("Creating MQTT client...")
client = mqtt.Client()

client.on_connect = on_connect
client.on_message = on_message

print("Connecting to MQTT broker...")
client.connect(MQTT_BROKER, MQTT_PORT, 60)

print("Waiting for MQTT messages...")
client.loop_forever()