from bluepy.btle import Scanner, ScanEntry, DefaultDelegate
from struct import unpack
from time import time, sleep
from datetime import datetime, timezone
from collections import deque, defaultdict
from threading import Thread
from queue import Queue, Empty
from uuid import UUID

MY_SERVICE_BASE_UUID = UUID('f96d0000-1139-4e07-8ccf-d28be904fc0f')
# Address of BLE device to connect to.
BLE_ADDRESS = "e8:6c:a2:e3:c6:65"

class ScanDelegate(DefaultDelegate):
    def __init__(self, q):
        DefaultDelegate.__init__(self)
        self.q = q

    def handleDiscovery(self, dev, isNewDev, isNewData):
        # Length(1) + DATA(N)
        #             AD Type(1) + AD Data(n)
        #                          Service UUID(16) + Data(m)
        if isNewDev:
            if dev.addr == BLE_ADDRESS:
                logging.info('Discoverd device %s', dev.addr)
                logging.info('RSSI= %s', dev.rssi)            
            else:
                return None
        elif isNewData:
            if dev.addr == BLE_ADDRESS:
                logging.info('Received new data from %s', dev.addr)
            else:
                return None
        service_data = dev.getValue(ScanEntry.SERVICE_DATA_128B)
        if service_data:
            data = service_data[16:]
            n = len(data)//2
            if n>0:
                r_data = bytes(reversed(service_data[:16]))
                service_uuid = UUID(bytes=r_data[:2]+b'\x00\x00'+r_data[4:16])
                service_uuid16 = unpack('H', r_data[2:4])[0]
                if service_uuid == MY_SERVICE_BASE_UUID:
                    fields = unpack('nh', data)
                    logging.info('fields: %s', fields)
                    logger.info('{0} {1} {2}'.format(dev.addr, service_uuid16, fields))
                    self.q.put(({
                        'measurement':'sensordata',
                        'tags':{'address':dev.addr, 'service_uuid16':service_uuid16},
                        'fields':{'field{i}':v for i,v in enumerate(fields)},
                        'time':datetime.now(timezone.utc).strftime('%Y-%m-%dT%H:%M:%S.%f')},
                        'long_term_storage_policy'))

def main():
    q = Queue()
    #t = Thread(target=influxdb_writer, args=(db,q))
    #t.start()

    scanner = Scanner().withDelegate(ScanDelegate(q))
    while True:
        try:
            devices = scanner.scan(10)
        except (KeyboardInterrupt, SystemExit):
            break
        except Exception as e:
            logger.exception(e)
    #q.put((None,None))
    #t.join()

if __name__ == '__main__':
    #import argparse
    import logging

    logging.basicConfig(level=logging.INFO)
    logger = logging.getLogger('scanner')
    logger.setLevel(logging.INFO)
    main()
