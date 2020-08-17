import sys 
import os
import time
from PyQt5.QtGui import QRegExpValidator, QDoubleValidator, QPixmap
from PyQt5.QtWidgets import (QApplication, QPushButton, QWidget, QComboBox, 
QHBoxLayout, QVBoxLayout, QFormLayout, QCheckBox, QGridLayout, QDialog, 
QLabel, QLineEdit, QDialogButtonBox, QFileDialog, QSizePolicy)
from PyQt5.QtCore import Qt, QTimer, QRegExp, QCoreApplication
from pyqtgraph import PlotWidget, plot
import pyqtgraph as pg
from random import randint
import serial
import serial.tools.list_ports
import numpy as np
import psutil
import csv
import qdarkstyle

#Drone: 2 inputs, 4 outputs
#Pro Con: 1 input, 2 outputs

#Fixes Scaling for high resolution monitors
if hasattr(Qt, 'AA_EnableHighDpiScaling'):
    QApplication.setAttribute(Qt.AA_EnableHighDpiScaling, True)
    QCoreApplication.setAttribute(Qt.AA_EnableHighDpiScaling, True)

class SerialComm:
    def __init__(self, port, baudrate, timeout):
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout    

    def serialOpen(self):
        self.ser = serial.Serial(port = self.port,
                                 baudrate = self.baudrate,
                                 timeout = self.timeout)
        return(self.ser)
    
    """
    handshake() method written for now. Will not have functionality yet.
    """
    def handshake(self):
        self.ser.flushInput()
        self.ser.write(b"A")
        print("Handshake request sent")
        response = self.ser.readline().decode().replace('\r\n','')
        
        if(response == "Contact established"):
            print("Handshake success")
        
        else:
            print("Handshake failed")
            self.handshake()

    def readValues(self):
        arduinoData = self.ser.readline().decode().replace('\r\n','').split(",")
        return arduinoData        
    
    def writeValues(self):
        pass


class Dialog1(QDialog):
    def __init__(self, *args, **kwargs):
        super(Dialog1, self).__init__(*args, **kwargs)
        
        self.title = "Settings"
        self.setWindowTitle(self.title)        
        self.setModal(True)
        self.width = 200
        self.height = 200
        self.setFixedSize(self.width, self.height)
        self.initUI()
        
    def initUI(self):
        self.setStyleSheet(qdarkstyle.load_stylesheet())

        mainLayout = QHBoxLayout() 
        leftFormLayout = QFormLayout()
        mainLayout.addLayout(leftFormLayout,100)

        self.port_label = QLabel("Ports:",self)
        
        self.port = QComboBox(self)
        self.port.setFixedWidth(100)
        self.list_port()

        self.baudrate_label = QLabel("Baud Rate:",self)
        self.baudrate = QComboBox(self)
        self.baudrate.setFixedWidth(100)
        #self.baudrate.addItems(["4800","9600","14400"])
        self.baudrate.addItems(["9600","14400"])
        
        self.timeout_label = QLabel("Timeout:",self)

        self.timeout = QLineEdit(self)
        self.timeout.setFixedWidth(100)
        self.timeout.setText("1")

        #For now, it will be 0-255 (FIX THIS IN FUTURE; Timeout in increments of 1s to 255s is weird)
        #regex = QRegExp("^([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$")
        #regex_validator_timeout = QRegExpValidator(regex,self)

        #self.timeout.setValidator(regex_validator_timeout)
        self.timeout.setValidator(QDoubleValidator())
        
        self.samplenum_label = QLabel("Sample #:",self)
        self.samplenum = QLineEdit(self)
        self.samplenum.setFixedWidth(100)
        self.samplenum.setText("25")

        #For now, it will be 0-255 (FIX THIS IN FUTURE; 0 samples makes no sense)
        regex = QRegExp("^([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$")
        regex_validator_samplenum = QRegExpValidator(regex,self)

        self.samplenum.setValidator(regex_validator_samplenum)

        #Ok and cancel button
        buttonBox = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttonBox.accepted.connect(self.accept)
        buttonBox.rejected.connect(self.reject)
       
        leftFormLayout.addRow(self.port_label,self.port)
        leftFormLayout.addRow(self.baudrate_label,self.baudrate)
        leftFormLayout.addRow(self.timeout_label,self.timeout)
        leftFormLayout.addRow(self.samplenum_label,self.samplenum)
        leftFormLayout.addRow(buttonBox)

        self.setLayout(mainLayout)


    def list_port(self): #currently only works with genuine Arduinos due to parsing method
        arduino_ports = [
            p.device
            for p in serial.tools.list_ports.comports()
            if 'Arduino' or 'tty' in p.description  
        ]
        if not arduino_ports:
            raise IOError("No Arduino found. Replug in USB cable and try again.")
        self.port.addItems(arduino_ports)


    def getDialogValues(self):
        if self.exec_() == QDialog.Accepted:
            self.com_value = str(self.port.currentText())
            self.baudrate_value = str(self.baudrate.currentText())
            self.timeout_value = float(self.timeout.text())
            self.samplenum_value = int(self.samplenum.text())
            return([self.com_value, self.baudrate_value, self.timeout_value, self.samplenum_value])

        else:
            print("Settings Menu Closed")
    

class Window(QWidget):
    def __init__(self, *args, **kwargs):
        super(Window, self).__init__(*args, **kwargs)
        
        #Application Title
        self.title = "Drone Control"
        self.setWindowTitle(self.title)

        #Application Size
        self.left = 100
        self.top = 100
        self.width = 1000
        self.height = 700
        self.setGeometry(self.left, self.top, self.width, self.height)
        #self.setFixedSize(self.width,self.height)


        self.initUI()

    def initUI(self):
        """
        Change this to your current relative/absolute path. Will be fixed in final version.
        """
        #self.setStyleSheet(open(r".\Arduino\qdarkstyle\style.qss", "r").read()) #relaitve
        #self.setStyleSheet(open(r"C:\Users\qwert\Desktop\Python VSCode\Arduino\qdarkstyle\style.qss", "r").read()) #absolute
        self.setStyleSheet(qdarkstyle.load_stylesheet())
        
        """
        mainLayout = QHBoxLayout()        
        leftMajor = QGridLayout()
        leftMajor.setSpacing(5)
        rightLayout = QVBoxLayout()
 
        mainLayout.addLayout(leftMajor,20)
        #mainLayout.addLayout(leftFormLayout,20)
        mainLayout.addLayout(rightLayout,150)
        """
        
        mainLayout = QHBoxLayout()
        leftmainLayout = QVBoxLayout()
        leftFormLayout = QFormLayout()
        imageLayout = QHBoxLayout()
        leftmainLayout.addLayout(imageLayout)
        leftmainLayout.addLayout(leftFormLayout)
        rightLayout = QVBoxLayout()
        mainLayout.addLayout(leftmainLayout,20)
        mainLayout.addLayout(rightLayout,150)
        
        """
        self.image = QPixmap('Arduino\CUAtHomeLogo-Horz.jpg') #640 by 267
        self.smallerImage = self.image.scaled(200, 130, Qt.KeepAspectRatio, Qt.FastTransformation)
        self.imageLabel = QLabel(self)
        self.imageLabel.setPixmap(self.smallerImage)         
        """

        self.startbutton = QPushButton("Start",self)
        self.startbutton.setCheckable(False)  
        self.startbutton.clicked.connect(self.startbutton_pushed)
        self.startbutton.resize(100,20)
        self.startbutton.setFixedWidth(100)

        self.stopbutton = QPushButton("Stop",self)
        self.stopbutton.setCheckable(False)  
        self.stopbutton.clicked.connect(self.stopbutton_pushed)
        self.stopbutton.resize(100,20)
        self.stopbutton.setFixedWidth(100)

        self.clearbutton = QPushButton("Clear",self)
        self.clearbutton.setCheckable(False)
        self.clearbutton.clicked.connect(self.clearbutton_pushed)
        self.clearbutton.resize(100,20)
        self.clearbutton.setFixedWidth(100)

        self.savebutton = QPushButton("Save",self)
        self.savebutton.setCheckable(False)
        self.savebutton.clicked.connect(self.savebutton_pushed)
        self.savebutton.resize(100,20)        
        self.savebutton.setFixedWidth(100)

        self.checkBoxShowAll = QCheckBox("Show All Plots", self)
        self.checkBoxShowAll.setChecked(True)
        self.checkBoxShowAll.toggled.connect(self.visibilityAll)

        self.checkBoxHideAll = QCheckBox("Hide All Plots", self)
        self.checkBoxHideAll.setChecked(False)
        self.checkBoxHideAll.toggled.connect(self.hideAll)

        self.checkBoxPlot1 = QCheckBox("Plot 1", self)
        self.checkBoxPlot1.toggled.connect(self.visibility1)
        
        self.checkBoxPlot2 = QCheckBox("Plot 2", self)
        self.checkBoxPlot2.toggled.connect(self.visibility2)

        self.PowerScalingLabel = QLabel("Power Scaling (%)",self)
        self.PowerScalingInput = QLineEdit("",self)
        self.PowerScalingInput.setValidator(QDoubleValidator(0,100,1)) #0-1 as a float FIX THIS

        self.FrequencyLabel = QLabel("Frequency (Hz)",self)
        self.FrequencyInput = QLineEdit("",self)
        self.FrequencyInput.setValidator(QDoubleValidator())

        self.PCheckBox = QCheckBox("P",self)
        self.PCheckBox.setChecked(True)
        self.PCheckBox.toggled.connect(self.PCheckBoxLogic)

        self.PInput = QLineEdit("",self)
        self.PInput.setValidator(QDoubleValidator())

        self.ICheckBox = QCheckBox("I",self)
        self.ICheckBox.setChecked(True)
        self.ICheckBox.toggled.connect(self.ICheckBoxLogic)

        self.IInput = QLineEdit("",self)    
        self.IInput.setValidator(QDoubleValidator())

        self.DCheckBox = QCheckBox("D",self)
        self.DCheckBox.setChecked(True)
        self.DCheckBox.toggled.connect(self.DCheckBoxLogic)
        
        self.DInput = QLineEdit("",self)
        self.DInput.setValidator(QDoubleValidator())

        self.checkBoxShowAll.stateChanged.connect(self.checkbox_logic) 
        self.checkBoxHideAll.stateChanged.connect(self.checkbox_logic) 
        self.checkBoxPlot1.stateChanged.connect(self.checkbox_logic) 
        self.checkBoxPlot2.stateChanged.connect(self.checkbox_logic) 

        self.settings = QPushButton("Settings",self)
        self.settings.clicked.connect(self.settingsMenu)
        #self.settings.setFixedWidth(205)

        self.LabType = QComboBox()
        self.LabType.addItems(["Angle","Speed"])
        self.LabType.activated.connect(self.getLabType)

        self.inputForms = QComboBox()
        self.inputForms.addItems(["Sine","Step"])
        self.inputForms.activated.connect(self.getInput)

        #Creates Plotting Widget        
        self.graphWidgetOutput = pg.PlotWidget()
        self.graphWidgetInput = pg.PlotWidget()
        #state = self.graphWidget.getState()

        #Adds grid lines
        self.graphWidgetOutput.showGrid(x = True, y = True, alpha=None)
        self.graphWidgetInput.showGrid(x = True, y = True, alpha=None)

        #self.graphWidget.setXRange(0, 100, padding=0) #Doesn't move with the plot. Can drag around
        #self.graphWidget.setLimits(xMin=0, xMax=100)#, yMin=c, yMax=d) #Doesn't move with the plot. Cannot drag around

        #self.graphWidget.setYRange(0, 4, padding=0)
        self.graphWidgetOutput.setYRange(-11, 11, padding=0)
        self.graphWidgetOutput.enableAutoRange()
        self.graphWidgetInput.setYRange(-11, 11, padding=0)
        self.graphWidgetInput.enableAutoRange()
        
        #Changes background color of graph
        self.graphWidgetOutput.setBackground((0,0,0))
        self.graphWidgetInput.setBackground((0,0,0))

        #Adds a legend after data starts to plot NOT before
        self.graphWidgetOutput.addLegend()

        #Adds title to graphs
        self.graphWidgetOutput.setTitle("Output", color="w", size="12pt")
        self.graphWidgetInput.setTitle("Input", color="w", size="12pt")

        
        #Positioning the buttons and checkboxes. CURRENT LAYOUT DONT EDIT
        #leftFormLayout.setContentsMargins(70,100,10,10)
        
        #imageLayout.addWidget(self.imageLabel)
        leftFormLayout.addRow(self.startbutton,self.stopbutton)
        leftFormLayout.addRow(self.clearbutton,self.savebutton)
        leftFormLayout.addRow(self.settings)
        leftFormLayout.addRow(self.checkBoxShowAll)
        leftFormLayout.addRow(self.checkBoxHideAll)
        leftFormLayout.addRow(self.checkBoxPlot1)
        leftFormLayout.addRow(self.checkBoxPlot2)
        leftFormLayout.addRow(self.LabType)
        leftFormLayout.addRow(self.inputForms)
        leftFormLayout.addRow(self.PowerScalingLabel,self.PowerScalingInput)
        leftFormLayout.addRow(self.FrequencyLabel,self.FrequencyInput)
        leftFormLayout.addRow(self.PCheckBox,self.PInput)
        leftFormLayout.addRow(self.ICheckBox,self.IInput)
        leftFormLayout.addRow(self.DCheckBox,self.DInput)

        rightLayout.addWidget(self.graphWidgetOutput)
        rightLayout.addWidget(self.graphWidgetInput)

        self.setLayout(mainLayout)


        """
        #leftMajor.addWidget(self.imageLabel,0,0,2,2)
        leftMajor.addWidget(self.startbutton,0,0)

        leftMajor.addWidget(self.stopbutton,0,1)
        leftMajor.addWidget(self.clearbutton,1,0)
        leftMajor.addWidget(self.savebutton,1,1)
        leftMajor.addWidget(self.settings,2,0,1,2)
        leftMajor.addWidget(self.checkBoxShowAll,3,0)
        leftMajor.addWidget(self.checkBoxHideAll,4,0)
        leftMajor.addWidget(self.checkBoxPlot1,5,0)
        leftMajor.addWidget(self.checkBoxPlot2,6,0)
        leftMajor.addWidget(self.inputForms,7,0)
        leftMajor.addWidget(self.AmplitudeLabel,8,0)
        leftMajor.addWidget(self.AmplitudeInput,8,1)
        leftMajor.addWidget(self.FrequencyLabel,9,0)
        leftMajor.addWidget(self.FrequencyInput,9,1)
        leftMajor.addWidget(self.PCheckBox,10,0)
        leftMajor.addWidget(self.PInput,10,1)
        leftMajor.addWidget(self.ICheckBox,11,0)
        leftMajor.addWidget(self.IInput,11,1)
        leftMajor.addWidget(self.DCheckBox,12,0)
        leftMajor.addWidget(self.DInput,12,1)

        rightLayout.addWidget(self.graphWidgetOutput)
        rightLayout.addWidget(self.graphWidgetInput)
        
        self.setLayout(mainLayout)
        """

        #Plot time update settings
        self.timer = QTimer()
        self.timer.setInterval(50) #Changes the plot speed. Defaulted to 50. Can be placed in startbutton_pushed() method
        self.initialState()
        time.sleep(2)
        try:
            self.timer.timeout.connect(self.update)
        except:
            raise Exception("Not Connected")
        #self.show()

    #Checkbox logic
    def checkbox_logic(self, state): 
  
        # checking if state is checked 
        if state == Qt.Checked: 
  
            # if first check box is selected 
            if self.sender() == self.checkBoxShowAll: 
                self.checkBoxHideAll.setChecked(False) 
                self.checkBoxPlot1.setChecked(False)
                self.checkBoxPlot2.setChecked(False)
                #self.checkBoxShow.stateChanged.disconnect(self.uncheck) 
  
            elif self.sender() == self.checkBoxHideAll:
                #self.checkBoxShow.stateChanged.connect(self.uncheck)      
                self.checkBoxShowAll.setChecked(False) 
                self.checkBoxPlot1.setChecked(False) 
                self.checkBoxPlot2.setChecked(False)   
            
            elif self.sender() == self.checkBoxPlot1: 
                self.checkBoxShowAll.setChecked(False) 
                self.checkBoxHideAll.setChecked(False)
                self.checkBoxPlot2.setChecked(False)

            elif self.sender() == self.checkBoxPlot2:
                self.checkBoxShowAll.setChecked(False) 
                self.checkBoxHideAll.setChecked(False)
                self.checkBoxPlot1.setChecked(False)

    #Resets data arrays and establishes serial communcation. Disables itself after clicking
    def startbutton_pushed(self):
        self.initialState() #Reinitializes arrays in case you have to retake data
        self.size = self.serial_values[3] #Value from settings. Windows data
        self.ser = serial.Serial(port = self.serial_values[0], 
                                 baudrate = self.serial_values[1],
                                 timeout = self.serial_values[2])
        self.ser.flushInput()
        self.ser.write(b'A')
        time.sleep(2)
        print("Recording Data")
        self.timer.start()
        #self.timer.setInterval(50)
        self.curve()
        self.startbutton.clicked.disconnect(self.startbutton_pushed)

    #Stops timer and ends serial communication
    def stopbutton_pushed(self):
        self.timer.stop()
        self.ser.close()
        print("y1 zeros:", self.y1_zeros)
        print("y2 zeros:", self.y2_zeros)
        print("y1 full:", self.y1)
        print("y2 full:", self.y2)

    #Resets both plotting windows and reenables Start Button
    def clearbutton_pushed(self):
        self.graphWidgetOutput.clear()
        self.graphWidgetInput.clear()
        self.graphWidgetOutput.enableAutoRange(axis=None, enable=True, x=None, y=None)
        self.startbutton.clicked.connect(self.startbutton_pushed)
    
    #Dumps data into a csv file to a selected path
    def savebutton_pushed(self):
        self.createCSV()
        path = QFileDialog.getSaveFileName(self, 'Save CSV', os.getenv('HOME'), 'CSV(*.csv)')
        if path[0] != '':
            with open(path[0], 'w', newline = '') as csvfile:
                csvwriter = csv.writer(csvfile)
                csvwriter.writerow(self.header)
                csvwriter.writerows(self.data_set)
    
    #Creates csv data
    def createCSV(self):
        self.header = ['x', 'y1', 'y2']
        self.data_set = zip(self.x,self.y1,self.y2)

    #Initilizes lists/arrays
    def initialState(self):
        self.buffersize = 500 #np array size that is used to plot data
        self.step = 0 #Used for repositioning data in plot window to the left

        #Data buffers. What is being plotted in the 2 windows
        self.x1_zeros = np.zeros(self.buffersize+1, float)
        self.y1_zeros = np.zeros(self.buffersize+1, float)
        self.y2_zeros = np.zeros(self.buffersize+1, float)

        #Complete data. What will be written to the csv file
        self.x = list()
        self.y1 = list()
        self.y2 = list()

    def readValues(self):
        arduinoData = self.ser.readline().decode().replace('\r\n','').split(",")
        return arduinoData

    #Initializes data# to have specific attributes
    def curve(self):
        pen1 = pg.mkPen(color = (255, 0, 0), width=1)
        pen2 = pg.mkPen(color = (0, 255, 0), width=1)
        
        self.data1 = self.graphWidgetOutput.plot(pen = pen1, name="Data 1")
        self.data2 = self.graphWidgetOutput.plot(pen = pen2, name="Data 2")

    #Connected to timer to update plot    
    def update(self):
        fulldata = self.readValues()
        #print(fulldata)

        self.step = self.step + 1

        i = int(self.y1_zeros[self.buffersize])
        self.y1_zeros[i] = self.y1_zeros[i+self.size] = float(fulldata[0])
        self.y1_zeros[self.buffersize] = i = (i+1) % self.size
        self.y1.append(fulldata[1])

        j = int(self.y2_zeros[self.buffersize])
        self.y2_zeros[j] = self.y2_zeros[j+self.size] = float(fulldata[1])
        self.y2_zeros[self.buffersize] = j = (j+1) % self.size
        self.y2.append(fulldata[2])

        self.x.append(fulldata[0])

        self.data1.setData(self.y1_zeros[i:i+self.size])
        self.data1.setPos(self.step,0)
        self.data2.setData(self.y2_zeros[j:j+self.size])
        self.data2.setPos(self.step,0)

    def visibilityAll(self):
        showall = self.sender()
        if showall.isChecked() == True:
            self.data1.setVisible(True)
            self.data2.setVisible(True) 

    def hideAll(self):
        disappearall = self.sender()
        if disappearall.isChecked() == True:
            self.data1.setVisible(False)
            self.data2.setVisible(False)

    def visibility1(self):
        test1 = self.sender()
        if test1.isChecked() == True:
            self.data1.setVisible(True)
            self.data2.setVisible(False)
    
    def visibility2(self):
        test2 = self.sender()
        if test2.isChecked() == True:
            self.data2.setVisible(True)
            self.data1.setVisible(False)

    def settingsMenu(self):
        self.settingsPopUp = Dialog1()
        self.settingsPopUp.show()
        #self.settingsPopUp.exec()
        self.serial_values = self.settingsPopUp.getDialogValues()

    def PCheckBoxLogic(self):
        test1 = self.sender()
        if test1.isChecked() == True:
            self.PInput.setEnabled(True)
        elif test1.isChecked() == False:
            self.PInput.setEnabled(False)

    def ICheckBoxLogic(self):
        test1 = self.sender()
        if test1.isChecked() == True:
            self.IInput.setEnabled(True)
        elif test1.isChecked() == False:
            self.IInput.setEnabled(False)

    def DCheckBoxLogic(self):
        test1 = self.sender()
        if test1.isChecked() == True:
            self.DInput.setEnabled(True)
        elif test1.isChecked() == False:
            self.DInput.setEnabled(False)

    def PIDInput(self):
        if self.PInput.text() == "" or self.PCheckBox.checkState() == False:
            self.Pvalue = 0
        else:
            self.Pvalue = self.PInput.text()

        if self.IInput.text() == "" or self.ICheckBox.checkState() == False:
            self.Ivalue = 0
        else:
            self.Ivalue = self.IInput.text()

        if self.DInput.text() == "" or self.DCheckBox.checkState() == False:
            self.Dvalue = 0
        else:
            self.Dvalue = self.DInput.text()
        return([self.Pvalue, self.Ivalue, self.Dvalue])

    def WriteValues(self):
        pass

    #Function that connects output pyqtgraph widget, and the combobox
    def getInput(self):
        self.inputType = str(self.inputForms.currentText())
        pen_input = pg.mkPen(color = (255, 0, 0), width=1)

        if self.inputType == "Sine":
            print("Sine")
            self.graphWidgetInput.clear()
            self.x_input = np.arange(0,10,0.1)
            self.y_input = np.sin(self.x_input)
            self.data_input = self.graphWidgetInput.plot(self.x_input, self.y_input, pen = pen_input)
            self.data_input.setData(self.x_input, self.y_input)  
            self.graphWidgetInput.setYRange(-2, 2, padding=0)

        elif self.inputType == "Step":
            print("Step")
            self.graphWidgetInput.clear()
            self.x_input = np.arange(0,10,0.1)
            self.y_input = np.heaviside(self.x_input,1)
            self.data_input = self.graphWidgetInput.plot(self.x_input, self.y_input, pen = pen_input)
            self.data_input.setData(self.x_input, self.y_input)
            self.graphWidgetInput.setYRange(-2, 2, padding=0)

    def getLabType(self):
        self.inputType = str(self.inputForms.currentText())
        pass

def main():
    app = QApplication(sys.argv)
    app.setStyle("Fusion")
    main = Window()
    main.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()