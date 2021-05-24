import time
import csv
from paho.mqtt import client as mqtt_client

class PUBLISH_CSV:
    def __init__(self, topic, qos, server, port):
       self.topic = topic
       self.qos = qos
       self.server = server
       self.port = port
       #self.on_publish( 
    def handle_csv_file(self):
        with open("/home/dpan/.sens_data/realtime_sens.csv", "r") as sens:
            sens_data = sens.readlines()[-1]
        return sens_data
    def publish_to_client(self):
        client = mqtt_client.Client()
        #client.is_callback = self.is_callback(client, userdata, mid)
        client.connect(self.server, self.port, keepalive=60, bind_address="")
        client.loop_start()
        while True:
            (rc, mid) = client.publish(self.topic, str(self.handle_csv_file()), self.qos)
            if rc == 0:
                logging.info('published OK')
            else:
                logging.info('published fail')
            time.sleep(10)
    def is_callback(self, client, userdata, mid):
        logging.info("callback: "+str(mid))

def main():
    pub_data = PUBLISH_CSV("poc/sens", 0, "localhost", 1883)
    data_str = pub_data.publish_to_client()

if __name__ == '__main__':
    import logging
    logging.basicConfig(level=logging.INFO)
    main()

