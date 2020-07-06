import sys
import time
import serial
import serial.tools.list_ports
from PyQt5.QtGui import QRegExpValidator, QDoubleValidator, QPalette
from PyQt5.QtWidgets import QApplication, QCheckBox, QLabel, QLineEdit, QMainWindow, QPushButton, QRadioButton, QStyleFactory,QWidget
from PyQt5.QtCore import Qt, QRegExp

# Looks to see what COM# the Arduino is using. #COM# can also be found under Device Manager.
arduino_ports = [
    p.device
    for p in serial.tools.list_ports.comports()
    if 'Arduino' in p.description  
]
if not arduino_ports:
    raise IOError("No Arduino found. Replug in USB cable and try again.")
serialcomm  = serial.Serial(arduino_ports[0], baudrate = 9600, timeout = 1)
serialcomm.close()
serialcomm.open()
time.sleep(3) #allows for Arduino to boot up


def Start():
    m = Window()
    m.show()
    #print("Start")
    #time.sleep(5) #
    return m

class Window(QMainWindow):
    def __init__(self):
        #QWidget.__init__(self)
        super(QMainWindow,self).__init__()
        self.left = 100
        self.top = 100
        self.width = 400
        self.height = 300
        self.initUI()
    
    def initUI(self):
        #Enables LED1 and LED2 to be on or off (PWM 10, PWM 11)
        self.label = QLabel("Ignore This",self)
        self.label.move(50,50)
        
        self.rbtn1 = QCheckBox("LED1",self)
        self.rbtn1.setChecked(False)        
        self.rbtn1.type = "LED 1"
        self.rbtn1.toggled.connect(self.input_type)
        self.rbtn1.move(50,80)

        self.rbtn2 = QCheckBox("LED2",self)
        self.rbtn2.setChecked(False)        
        self.rbtn2.type = "LED 2"
        self.rbtn2.toggled.connect(self.input_type)
        self.rbtn2.move(50,100)

        
        #Brightness
        self.label = QLabel("Brightness (0 to 255)",self)
        self.label.move(200,50)

        self.label = QLabel("LED 1",self)
        self.label.move(200,80)

        self.label = QLabel("LED 2",self)
        self.label.move(200,120)                

        #Gains are only Doubles
        #self.onlyDouble = QDoubleValidator()

        #Input is only integers from 0-255 using regex
        regex = QRegExp("^([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$")
        regex_validator = QRegExpValidator(regex,self)

        self.textbox1 = QLineEdit(self)
        self.textbox1.move(240, 80)
        self.textbox1.resize(100,30)
        self.textbox1.setValidator(regex_validator)

        self.textbox2 = QLineEdit(self)
        self.textbox2.move(240, 120)
        self.textbox2.resize(100,30)
        self.textbox2.setValidator(regex_validator)


        self.button = QPushButton('Confirm Brightness', self)
        self.button.move(200,160)
        self.button.clicked.connect(self.pid_values)
        self.show()

        #How the window opens
        #self.setGeometry(self.left, self.top, self.width, self.height)
        self.setFixedSize(self.width,self.height)
        self.setWindowTitle("Controlling Mulitple LEDS")

    def input_type(self):
        checkBox = self.sender()     
        if checkBox.isChecked() == True:
            print(f"{checkBox.type} is ON")
        elif checkBox.isChecked() == False:
            print(f"{checkBox.type} is OFF")

    def pid_values(self):
        textboxValue1 = self.textbox1.text()
        textboxValue2 = self.textbox2.text()
        print(f"LED 1 Brightness:{textboxValue1}        LED 2 Brightness:{textboxValue2}")
        bytes_textboxValue1 = str.encode("a" + textboxValue1)
        bytes_textboxValue2 = str.encode("b" + textboxValue2)
        serialcomm.write(bytes_textboxValue1)
        serialcomm.write(bytes_textboxValue2)
"""         print("a" + textboxValue1)
        print("b" + textboxValue1) """
#app = QApplication(sys.argv)

if __name__ == "__main__":
    app = QApplication(sys.argv) #it's fine to either use this, or in line 7
    app.setStyle("Fusion")
    screen = Window()
    screen.show()
    sys.exit(app.exec_())
    serialcomm.close()
