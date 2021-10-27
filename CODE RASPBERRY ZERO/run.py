import time
import _thread
import os

def startprgm(i):
    print "Running thread %d" % i
    if (i == 0):
        time.sleep(0.1)
        print('Running: UART')
        os.system("sudo -H -u pi /usr/bin/python3 /home/pi/UART.py")
    elif (i == 1):
        print('Running: UART2')
        time.sleep(0.1)
        os.system("sudo -H -u pi /usr/bin/python3 /home/pi/UART2.py")
    else:
        i=0
for i in range(2):
    t = threa.Thread(target=startprgm, args=(i,))
    t.start()