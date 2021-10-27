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
volt=0
freq=0
curr=0
power=0
energy=0
def read_uart():
    global volt,freq,curr,power,energy
    #print("read uart")
    data = ser.read(18)  # read up to 32 bytes
    #print(data)
    if data[0]==255:
        r=data
        v=[r[1],r[2]]
        f=[r[3],r[4]]
        i=[r[5],r[6],r[7],r[8]]
        e=[r[9],r[10],r[11],r[12]]
        p=[r[13],r[14],r[15],r[16]]
        volt_raw=int.from_bytes(v,"big")
        volt=volt_raw/10
        freq_raw=int.from_bytes(f, "big")
        freq=freq_raw/10
        curr_raw=int.from_bytes(i, "big")
        curr=curr_raw/1000
        ener_raw=int.from_bytes(e, "big")
        energy=ener_raw/1000
        power_raw=int.from_bytes(p, "big")
        power=power_raw/10
def send_db():
    #print("send db")
    database.child("Value").child("voltage").set(volt)
    database.child("Value").child("ampere").set(curr)
    database.child("Value").child("frequency").set(freq)
    database.child("Value").child("power").set(power)
    database.child("Value").child("energy").set(energy)
try:
    while True:
        for i in range(0,500):
            read_uart()
        send_db()
except KeyboardInterrupt:
    pass
        
            

