import usb.core
import usb.util
import sys
from time import gmtime, strftime
import time

print ("Meteo kezdés",strftime("%Y-%m-%d %H:%M:%S", gmtime()))


# find our device
dev = usb.core.find(idVendor=0x04d8, idProduct=0x0080)

# was it found?
if dev is None:
    raise ValueError('Device not found')
else:
    print ("meteo megvan!")

reattach = False
if dev.is_kernel_driver_active(0):
    reattach = True
    dev.detach_kernel_driver(0)

endpoint_in = dev[0][(0,0)][0]
endpoint_out = dev[0][(0,0)][1]
#print ("endpoint_out",endpoint_out)
#print ("endpoint_in",endpoint_in)

# write the data
msg = b'\x81'

while 1:
    try:
        endpoint_out.write(msg)

        # reading
        print ("Waiting to read...")
        print (endpoint.bEndpointAddress)
        data = dev.read(endpoint_in.bEndpointAddress, 64, 1000)
        DHT11_H = data[0]   # a tobbi helyiertek kimaradt!!
        DHT11_R = data[4]
        BMP180_H = data[8]
        BMP180_P = (data[12]+data[13]*256+data[14]*65536)/100

        print (strftime("%Y-%m-%d %H:%M:%S", gmtime()),
        "DHT t=" , str(DHT11_H) , "C| ",
        "DHT r=", DHT11_R, "%| " ,
        "BMP t=", BMP180_H, "C| " ,
        "BMP p=", BMP180_P, "HPa"
        )
        #print (data)
        time.sleep(10)
    except usb.core.USBError:
        print ("USB error")
    except:
        print ("write failed")
# end while

# This is needed to release interface, otherwise attach_kernel_driver fails
# due to "Resource busy"
usb.util.dispose_resources(dev)

# It may raise USBError if there's e.g. no kernel driver loaded at all
if reattach:
    dev.attach_kernel_driver(0)
