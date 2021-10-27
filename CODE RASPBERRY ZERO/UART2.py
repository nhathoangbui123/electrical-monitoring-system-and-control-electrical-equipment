#!usr/local/lib/python3.7/dist-packages/pyrebase
import serial
import pyrebase
config ={
    "apiKey": "AIzaSyCNyYLUIW9bOiBinH08-adRvrobR-m94FQ",
    "authDomain": "homeenergy-430ac.firebaseapp.com",
    "databaseURL": "https://homeenergy-430ac-default-rtdb.firebaseio.com/",
    "storageBucket": "homeenergy-430ac.appspot.com"
}
ser = serial.Serial(
         port = '/dev/ttyUSB0',
         baudrate = 115200,
         parity = serial.PARITY_NONE,
         stopbits = serial.STOPBITS_ONE,
         bytesize = serial.EIGHTBITS,
         timeout=1
)
firebase = pyrebase.initialize_app(config)
database =firebase.database()
d0='0'
d1='0'
d2='0'
d3='0'
threshold=0
def read_db():
    global d0,d1,d2,d3,threshold
    #print("read db")
    dall=database.child("Control").child("All Device").get()
    d0=dall.val()
    dv1=database.child("Control").child("Device 1").get()
    d1=dv1.val()
    dv2=database.child("Control").child("Device 2").get()
    d2=dv2.val()
    dv3=database.child("Control").child("Device 3").get()
    d3=dv3.val()
    nguong=database.child("Control").child("Threshold").get()
    threshold=nguong.val()
def send_uart():
    #print("send uart")
    global d0,d1,d2,d3,threshold
    ser.write(b'F')
    if d0 =='1' :
        ser.write(b'1')
    else:
        ser.write(b'0')
    if d1 =='1' :
        ser.write(b'1')
    else:
        ser.write(b'0')
    if d2 =='1' :
        ser.write(b'1')
    else:
        ser.write(b'0')
    if d3 =='1' :
        ser.write(b'1')
    else:
        ser.write(b'0')
    ser.write(threshold.encode('utf-8'))
    ser.write(b'E')
try:
    while True:
        send_uart()
        read_db()
except KeyboardInterrupt:
    pass