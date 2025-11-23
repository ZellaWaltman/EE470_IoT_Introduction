# relay/relay_node.py
# Relay node: subscribes to ESP pot topic and pushes data into Hostinger MySQL

import paho.mqtt.client as mqtt
import pymysql

# --------------------------------------------
# MQTT SETTINGS
# --------------------------------------------
BROKER_URL = "broker.mqtt-dashboard.com" # MQTT 
BROKER_PORT = 1883 # TCP MQTT port
TOPIC = "testtopic/temp/outTopic/zellas_mqtt/pot" # ESP Potentiometer Topic

# --------------------------------------------
# DATABASE SETTINGS
# --------------------------------------------
# Fill these in with your Hostinger DB details
HOST = "localhost" # MySQL server's IP address/hostname
USER = "u551180265_db_ESP_Program" # MySQL username
PASSWORD = "EE473espDB" # MySQL password
DATABASE = "u551180265_ESP_Program" # SQL DataBase Name

# Sending Potentiometer Values to DB
# --------------------------------------------
# Insert one sensor value into the DB
def push_value_to_db(sensor_value: float):

    # Initialize values for safe closing
    conn = None
    cursor = None

    # Connect to Hostinger Database
    try:
        conn = pymysql.connect(
            host=HOST,
            user=USER,
            password=PASSWORD,
            database=DATABASE,
            cursorclass=pymysql.cursors.Cursor
        )

        # Insert Potentiometer Values using mySQL commands
        cursor = conn.cursor() # Create cursor object to execute SQL commands
        insert_query = "INSERT INTO pot_values (value) VALUES (%s)" # SQL statement, insert new row w/ pot value
        cursor.execute(insert_query, (sensor_value,)) # send SQL command w/ sensor_value replacing %s
        conn.commit() # save inserted row
        print(f"[DB] Inserted value {sensor_value} into pot_values") # Print success message

    # Error Message in case of failure
    except pymysql.MySQLError as err:
        print(f"[DB ERROR] {err}")

    # Close cursor object & Database connection
    finally:
        if cursor is not None:
            cursor.close()
        if conn is not None:
            conn.close()

# --------------------------------------------
# MQTT CALLBACKS
# --------------------------------------------

# DB Connection Confirmation Message
# --------------------------------------------
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(f"[MQTT] Connected to {BROKER_URL}:{BROKER_PORT}")
        client.subscribe(TOPIC)
        print(f"[MQTT] Subscribed to topic: {TOPIC}")
    else:
        print(f"[MQTT] Failed to connect, return code {rc}")

# Get Potentiometer Value from ESP
# --------------------------------------------
def on_message(client, userdata, msg):
    payload = msg.payload.decode().strip() # Remove white space / newline
    print(f"[MQTT] Received from {msg.topic}: {payload}") # Display recieved info

    # ESP sends Vout as string like "1.234"
    try:
        value = float(payload) # Convert payload to float from string
    except ValueError: # Error Message
        print(f"[WARN] Payload is not a float: {payload}")
        return

    print(f"[POT] Vout = {value} V") # Display Potentiometer Value

    # Push Value to DB
    push_value_to_db(value)

# --------------------------------------------
# MAIN
# --------------------------------------------
def main():
    # Simple TCP MQTT client
    client = mqtt.Client()

    client.on_connect = on_connect # On connection, display confirmation
    client.on_message = on_message # On recieved value from ESP, convert & display

    # Display Connecting Message
    print(f"[MQTT] Connecting to broker {BROKER_URL}:{BROKER_PORT} ...")

    # Connect mqtt at HiveMQTT, port 1883, keepalive = 1 min interval
    client.connect(BROKER_URL, BROKER_PORT, 60)

    # loop runs for data to reach DB
    client.loop_forever()

if __name__ == "__main__":
    main()
