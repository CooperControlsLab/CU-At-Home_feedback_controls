import sys 
import os
import time
from PyQt5.QtGui import QRegExpValidator, QDoubleValidator, QPixmap
from PyQt5.QtWidgets import (QApplication, QPushButton, QWidget, QComboBox, 
QHBoxLayout, QVBoxLayout, QFormLayout, QCheckBox, QGridLayout, QDialog, 
QLabel, QLineEdit, QDialogButtonBox, QFileDialog, QSizePolicy, QLayout,
QSpacerItem, QStatusBar)
from PyQt5.QtCore import Qt, QTimer, QRegExp, QCoreApplication, QSize
from pyqtgraph import PlotWidget, plot
import pyqtgraph as pg
import serial
import serial.tools.list_ports
import numpy as np
import csv
import qdarkstyle
from QLed import QLed

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
    
    def serialClose(self):
        self.ser.close()

    def serialIsOpen(self):
        self.ser.is_open

    def handshake(self):
        self.ser.flushInput()
        self.ser.write(b"H0,\0")
        print("Handshake request sent")
        response = self.ser.readline().decode().replace('\r\n','') #No need for comma delimiter
        
        if(response == "Contact established"):
            print("Handshake success")
        
        else:
            print("Handshake failed")
            self.handshake()

    def readValues(self):
        self.ser.write(b"R0,\0") #used to call for the next line (Request)
        #current format of received data is b"T23533228,S0.00,A0.00,Q0.00,\0\r\n"
        arduinoData = self.ser.readline().decode().replace('\r\n','').split(",")
        return arduinoData        
    
    #def writeValues(self,P,I,D,Setpoint,LabType,SampleTime,Saturation):
    def writeValues(self,P,I,D,Setpoint,LabType,SampleTime,Saturation):
        Saturation = Saturation.split(",") 
        input2system = f"S0,P{P},I{I},D{D},S1,Z{Setpoint},S2,Y{LabType},S4,T{SampleTime},S5,L{Saturation[0]},U{Saturation[1]},\0"
        print(input2system)
        self.ser.write(str.encode(input2system))
    

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
        self.timeout.setText("0.1")
        timeout_validator = QDoubleValidator(0.0000, 5.0000, 4, notation=QDoubleValidator.StandardNotation)
        self.timeout.setValidator(timeout_validator)
        
        
        regex = QRegExp("([1-9]|[1-9][0-9]|[1-9][0-9][0-9]|1000)") #takes 1-1000 as input
        sampnum_buffernum_validator = QRegExpValidator(regex,self)

        self.samplenum_label = QLabel("Sample Size:",self)
        self.samplenum = QLineEdit(self)
        self.samplenum.setFixedWidth(100)
        self.samplenum.setText("25") #Default is 25
        self.samplenum.setValidator(sampnum_buffernum_validator)

        """
        self.buffernum_label = QLabel("Buffer Size:",self)
        self.buffernum = QLineEdit(self)
        self.buffernum.setFixedWidth(100)
        self.buffernum.setText("500") #Default is 500
        self.buffernum.setValidator(sampnum_buffernum_validator)
        """

        #Ok and cancel button
        buttonBox = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttonBox.accepted.connect(self.accept)
        buttonBox.rejected.connect(self.reject)
       
        leftFormLayout.addRow(self.port_label,self.port)
        leftFormLayout.addRow(self.baudrate_label,self.baudrate)
        leftFormLayout.addRow(self.timeout_label,self.timeout)
        leftFormLayout.addRow(self.samplenum_label,self.samplenum)
        #leftFormLayout.addRow(self.buffernum_label,self.buffernum)
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
            #self.buffernum_value = int(self.buffernum.text())
            return([self.com_value, self.baudrate_value, self.timeout_value, self.samplenum_value])#, self.buffernum_value])

        else:
            print("Settings Menu Closed")
    

class Window(QWidget):
    def __init__(self, *args, **kwargs):
        super(Window, self).__init__(*args, **kwargs)

        self.title = "Motor Control"
        self.setWindowTitle(self.title)

        #Application Size
        self.left = 100
        self.top = 100
        self.width = 1000
        self.height = 700
        self.setGeometry(self.left, self.top, self.width, self.height)

        self.initUI()

    def initUI(self):

        self.setStyleSheet(qdarkstyle.load_stylesheet())


        self.horizontalLayout = QHBoxLayout()
        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setSizeConstraint(QLayout.SetDefaultConstraint)
        self.verticalLayout.setSpacing(6)
        self.gridLayout = QGridLayout()

        self.imageLabel = QLabel()
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.imageLabel.sizePolicy().hasHeightForWidth())
        self.imageLabel.setSizePolicy(sizePolicy)
        self.imageLabel.setMinimumSize(QSize(200, 130))
        self.imageLabel.setMaximumSize(QSize(200, 130))
        self.imageLabel.setPixmap(QPixmap("./Arduino/logo/CUAtHomeLogo-Horz.png").scaled(200, 130, Qt.KeepAspectRatio, Qt.FastTransformation))
        self.verticalLayout.addWidget(self.imageLabel)


        
        self.LEDLabel = QLabel("Arduino Status",self)
        self.LEDLabel.setMinimumSize(QSize(100, 20))
        self.LEDLabel.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.LEDLabel, 0, 0, 1, 1)

        self._led = QLed(self, onColour=QLed.Red, shape=QLed.Circle)
        self._led.clickable = False
        self.gridLayout.addWidget(self._led, 0, 1, 1, 1)
        self._led.value = True
        self._led.setMinimumSize(QSize(20,20))
        self._led.setMaximumSize(QSize(20,20))        



        self.serialOpenButton = QPushButton("Open Serial",self)
        self.serialOpenButton.setCheckable(False)  
        self.serialOpenButton.clicked.connect(self.serialOpenPushed)        
        self.serialOpenButton.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.serialOpenButton, 1, 0, 1, 1)

        self.serialCloseButton = QPushButton("Close Serial",self)
        self.serialCloseButton.setCheckable(False)  
        self.serialCloseButton.clicked.connect(self.serialClosePushed)
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.serialCloseButton.sizePolicy().hasHeightForWidth())
        self.serialCloseButton.setSizePolicy(sizePolicy)
        self.serialCloseButton.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.serialCloseButton, 1, 1, 1, 1)




        self.startbutton = QPushButton("Start Plot",self)
        self.startbutton.setCheckable(False)  
        self.startbutton.clicked.connect(self.startbutton_pushed)        
        self.startbutton.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.startbutton, 2, 0, 1, 1)

        self.stopbutton = QPushButton("Stop Plot",self)
        self.stopbutton.setCheckable(False)  
        self.stopbutton.clicked.connect(self.stopbutton_pushed)
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.stopbutton.sizePolicy().hasHeightForWidth())
        self.stopbutton.setSizePolicy(sizePolicy)
        self.stopbutton.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.stopbutton, 2, 1, 1, 1)
    
        self.clearbutton = QPushButton("Clear Plot",self)
        self.clearbutton.setCheckable(False)
        self.clearbutton.clicked.connect(self.clearbutton_pushed)
        self.clearbutton.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.clearbutton, 3, 0, 1, 1)

        self.savebutton = QPushButton("Save Plot",self)
        self.savebutton.setCheckable(False)
        self.savebutton.clicked.connect(self.savebutton_pushed)
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.savebutton.sizePolicy().hasHeightForWidth())
        self.savebutton.setSizePolicy(sizePolicy)
        self.savebutton.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.savebutton, 3, 1, 1, 1)

        self.settings = QPushButton("Settings",self)
        self.settings.clicked.connect(self.settingsMenu)
        self.settings.setMaximumSize(QSize(300, 20))
        self.gridLayout.addWidget(self.settings, 4, 0, 1, 2)

        self.checkBoxShowAll = QCheckBox("Show All Plots", self)
        self.checkBoxShowAll.setMaximumSize(QSize(100, 20))
        self.checkBoxShowAll.setChecked(True)
        self.checkBoxShowAll.toggled.connect(self.visibilityAll)
        self.gridLayout.addWidget(self.checkBoxShowAll, 5, 0, 1, 1)

        self.checkBoxHideAll = QCheckBox("Hide All Plots", self)
        self.checkBoxHideAll.setChecked(False)
        self.checkBoxHideAll.toggled.connect(self.hideAll)
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.checkBoxHideAll.sizePolicy().hasHeightForWidth())
        self.checkBoxHideAll.setSizePolicy(sizePolicy)
        self.checkBoxHideAll.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.checkBoxHideAll, 5, 1, 1, 1)

        self.checkBoxPlot1 = QCheckBox("Plot 1", self)
        self.checkBoxPlot1.toggled.connect(self.visibility1)
        self.checkBoxPlot1.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.checkBoxPlot1, 6, 0, 1, 1)

        self.checkBoxPlot2 = QCheckBox("Plot 2", self)
        self.checkBoxPlot2.toggled.connect(self.visibility2)
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.checkBoxPlot2.sizePolicy().hasHeightForWidth())
        self.checkBoxPlot2.setSizePolicy(sizePolicy)
        self.checkBoxPlot2.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.checkBoxPlot2, 6, 1, 1, 1)

        self.checkBoxShowAll.stateChanged.connect(self.checkbox_logic) 
        self.checkBoxHideAll.stateChanged.connect(self.checkbox_logic) 
        self.checkBoxPlot1.stateChanged.connect(self.checkbox_logic) 
        self.checkBoxPlot2.stateChanged.connect(self.checkbox_logic)

        self.LabType = QComboBox()
        self.LabType.addItems(["Position","Speed"])
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.LabType.sizePolicy().hasHeightForWidth())
        self.LabType.setSizePolicy(sizePolicy)
        self.LabType.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.LabType, 7, 1, 1, 1)
        self.LabLabel = QLabel("Lab Type")
        self.LabLabel.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.LabLabel, 7, 0, 1, 1)

        """
        self.inputForms = QComboBox()
        self.inputForms.addItems(["Sine","Step"])
        #self.inputForms.activated.connect(self.getInput)
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.inputForms.sizePolicy().hasHeightForWidth())
        self.inputForms.setSizePolicy(sizePolicy)
        self.inputForms.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.inputForms, 7, 1, 1, 1)
        self.inputType = QLabel("Input Type")
        self.inputType.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.inputType, 7, 0, 1, 1)
        """



        self.SetpointLabel = QLabel("Setpoint",self)
        self.SetpointLabel.setMinimumSize(QSize(100, 20))
        self.SetpointLabel.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.SetpointLabel, 8, 0, 1, 1)
        self.SetpointInput = QLineEdit("",self)
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.SetpointInput.sizePolicy().hasHeightForWidth())
        self.SetpointInput.setSizePolicy(sizePolicy)
        self.SetpointInput.setMaximumSize(QSize(100, 20))
        self.SetpointInput.setValidator(QDoubleValidator())
        self.SetpointInput.setText("100")
        self.gridLayout.addWidget(self.SetpointInput, 8, 1, 1, 1)

        self.SaturationLabel = QLabel("Saturation",self)
        self.SaturationLabel.setMinimumSize(QSize(100, 20))
        self.SaturationLabel.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.SaturationLabel, 9, 0, 1, 1)
        self.SaturationInput = QLineEdit("",self)
        self.SaturationInput.setText("-125,125")
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.SaturationInput.sizePolicy().hasHeightForWidth())
        self.SaturationInput.setSizePolicy(sizePolicy)
        self.SaturationInput.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.SaturationInput, 9, 1, 1, 1)

        self.SampleTimeLabel = QLabel("Sample Time",self)
        self.SampleTimeLabel.setMinimumSize(QSize(100, 20))
        self.SampleTimeLabel.setMaximumSize(QSize(100, 20))
        self.gridLayout.addWidget(self.SampleTimeLabel, 10, 0, 1, 1)
        self.SampleTimeInput = QLineEdit("",self)
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.SampleTimeInput.sizePolicy().hasHeightForWidth())
        self.SampleTimeInput.setSizePolicy(sizePolicy)
        self.SampleTimeInput.setMaximumSize(QSize(100, 20))
        self.SampleTimeInput.setText("25")
        self.gridLayout.addWidget(self.SampleTimeInput, 10, 1, 1, 1)
        #self.PowerScalingInput.setValidator(QRegExpValidator(QRegExp("^[0-9][0-9]?$|^100$"))) #0-1 as a float FIX THIS

        PID_validator = QDoubleValidator(0.0000, 50.000, 4, notation=QDoubleValidator.StandardNotation)

        self.PCheckBox = QCheckBox("P",self)
        self.PCheckBox.setMaximumSize(QSize(100, 20))
        self.PCheckBox.setChecked(True)
        self.PCheckBox.toggled.connect(self.PCheckBoxLogic)
        self.gridLayout.addWidget(self.PCheckBox, 11, 0, 1, 1)
        self.PInput = QLineEdit("",self)
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.PInput.sizePolicy().hasHeightForWidth())
        self.PInput.setSizePolicy(sizePolicy)
        self.PInput.setMaximumSize(QSize(100, 20))
        self.PInput.setValidator(PID_validator)
        self.PInput.setText("1")
        self.gridLayout.addWidget(self.PInput, 11, 1, 1, 1)

        self.ICheckBox = QCheckBox("I",self)
        self.ICheckBox.setMaximumSize(QSize(100, 20))
        self.ICheckBox.setChecked(True)
        self.ICheckBox.toggled.connect(self.ICheckBoxLogic)
        self.gridLayout.addWidget(self.ICheckBox, 12, 0, 1, 1)
        self.IInput = QLineEdit("",self)    
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.IInput.sizePolicy().hasHeightForWidth())
        self.IInput.setSizePolicy(sizePolicy)
        self.IInput.setMaximumSize(QSize(100, 20))
        self.IInput.setValidator(PID_validator)
        self.IInput.setText("0")
        self.gridLayout.addWidget(self.IInput, 12, 1, 1, 1)

        self.DCheckBox = QCheckBox("D",self)
        self.DCheckBox.setMaximumSize(QSize(100, 20))
        self.DCheckBox.setChecked(True)
        self.DCheckBox.toggled.connect(self.DCheckBoxLogic)
        self.gridLayout.addWidget(self.DCheckBox, 13, 0, 1, 1)
        self.DInput = QLineEdit("",self)
        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.DInput.sizePolicy().hasHeightForWidth())
        self.DInput.setSizePolicy(sizePolicy)
        self.DInput.setMaximumSize(QSize(100, 20))
        self.DInput.setValidator(PID_validator)
        self.DInput.setText("0")
        self.gridLayout.addWidget(self.DInput, 13, 1, 1, 1)

        self.verticalLayout.addLayout(self.gridLayout)
        spacerItem = QSpacerItem(20, 80, QSizePolicy.Minimum, QSizePolicy.Fixed)
        self.verticalLayout.addItem(spacerItem)

        
        #Fixes some spacing issues on GUI
        self.label = QLabel()
        self.label.setMaximumSize(QSize(200, 130))
        self.label.setText("")
        self.verticalLayout.addWidget(self.label)

        self.horizontalLayout.addLayout(self.verticalLayout)
        self.rightVerticalLayout = QVBoxLayout()

        self.graphWidgetOutput = PlotWidget()
        self.graphWidgetInput = PlotWidget()

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
        self.graphWidgetOutput.setTitle("Response", color="w", size="12pt")
        self.graphWidgetInput.setTitle("PWM Actuation Signal", color="w", size="12pt")

        self.rightVerticalLayout.addWidget(self.graphWidgetOutput)
        self.rightVerticalLayout.addWidget(self.graphWidgetInput)
        self.horizontalLayout.addLayout(self.rightVerticalLayout)

        self.setLayout(self.horizontalLayout)

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

    def serialOpenPushed(self):
        #Try/except statement is to check whether settings menu was opened/changed
        try:
            self.size = self.serial_values[3] #Value from settings. Windows data
            self.serialInstance = SerialComm(self.serial_values[0],self.serial_values[1],self.serial_values[2])
            self.serialInstance.serialOpen()    
        except AttributeError:
            print("Settings menu was never opened")
        #self.serialInstance.handshake()
        
        time.sleep(2)
        
        try:
            self.serialInstance.writeValues(self.PIDInput()["P"],
                                            self.PIDInput()["I"],
                                            self.PIDInput()["D"],
                                            self.getSetpointValue(),
                                            self.getLabType(),
                                            self.getSampleTimeValue(),
                                            self.getSaturationValue()) 
        except AttributeError:
            print("Some fields are empty. Recheck then try again")

        print("Serial open")   
        self.serialOpenButton.clicked.disconnect(self.serialOpenPushed)
        self._led.onColour = QLed.Green

    def serialClosePushed(self):
        print("Serial Close Pressed")
        
        try:
            self.serialInstance.serialClose()
        except:
            print("Something went wrong")

        print("Serial was open. Now closed")
        """
        if self.serialInstance.serialIsOpen() == True:
            self.serialInstance.serialClose()
            print("Serial was open. Now closed")   
        elif self.serialInstance.serialIsOpen() == False:
            print("Serial is already closed")
        """
        self.serialOpenButton.clicked.connect(self.serialOpenPushed)

    #Resets data arrays and establishes serial communcation. Disables itself after clicking
    def startbutton_pushed(self):
        self.initialState() #Reinitializes arrays in case you have to retake data
        print("Recording Data")
        self.timer.start()
        self.curve()
        self.startbutton.clicked.disconnect(self.startbutton_pushed)

    #Stops timer and ends serial communication
    def stopbutton_pushed(self):
        self.timer.stop()
        print("Stopping Data Recording")
        self._led.onColour = QLed.Red


    #Resets both plotting windows and reenables Start Button
    def clearbutton_pushed(self):
        self.graphWidgetOutput.clear()
        self.graphWidgetInput.clear()

        self.graphWidgetOutput.setYRange(-11, 11, padding=0)
        self.graphWidgetOutput.enableAutoRange()
        self.graphWidgetInput.setYRange(-11, 11, padding=0)
        self.graphWidgetInput.enableAutoRange()
        self.startbutton.clicked.connect(self.startbutton_pushed)
        print("Cleared All Graphs")

    #Dumps data into a csv file to a selected path
    def savebutton_pushed(self):
        self.createCSV()
        path = QFileDialog.getSaveFileName(self, 'Save CSV', os.getenv('HOME'), 'CSV(*.csv)')
        if path[0] != '':
            with open(path[0], 'w', newline = '') as csvfile:
                csvwriter = csv.writer(csvfile)
                csvwriter.writerow(self.header)
                csvwriter.writerows(self.data_set)
        print("Saved All Data")

    #Creates csv data
    def createCSV(self):
        self.header = ['time', 'response', 'pwm', 'setpoint']
        self.data_set = zip(self.time,self.y1,self.y2,self.y3)

    #Initilizes lists/arrays
    def initialState(self):
        self.buffersize = 500 #np array size that is used to plot data
        self.step = 0 #Used for repositioning data in plot window to the left

        #Data buffers. What is being plotted in the 2 windows
        self.time_zeros = np.zeros(self.buffersize+1, float)
        self.y1_zeros = np.zeros(self.buffersize+1, float)
        self.y2_zeros = np.zeros(self.buffersize+1, float)
        self.y3_zeros = np.zeros(self.buffersize+1, float)

        #Complete data. What will be written to the csv file
        self.time = list()
        self.y1 = list()
        self.y2 = list()
        self.y3 = list()

        self.getLabTypeAxes()

    '''
    def readValues(self):
        arduinoData = self.ser.readline().decode().replace('\r\n','').split(",")
        return arduinoData
    '''

    #Initializes data# to have specific attributes
    def curve(self):
        pen1 = pg.mkPen(color = (255, 0, 0), width=1)
        pen2 = pg.mkPen(color = (0, 255, 0), width=1)
        pen3 = pg.mkPen(color = (0, 0, 255), width=1)

        self.data1 = self.graphWidgetOutput.plot(pen = pen1, name="Data 1") #Response
        self.data2 = self.graphWidgetOutput.plot(pen = pen2, name="Data 2") #Setpoint
        self.data3 = self.graphWidgetInput.plot(pen = pen3, name="Data 3") #PWM Actuation Signal

    #Connected to timer to update plot. Incoming data is in the form of timestamp,data1,data2...    
    def update(self):
        #fulldata = self.readValues()
        #print(fulldata)
        fulldata = self.serialInstance.readValues()

        self.step = self.step + 1

        """
        New data format reading. Based off of GCode. Since incoming serial data
        is in the form b"T23533228,S0.00,A0.00,Q0.00,\0\r\n", this parsing searches for specific starting letter,
        converts from list to string as the list is length 1, then removes the starting character. 
        Hasn't been implemented due to problem on Arduino end
        # result = [item for item in example if item.startswith('T')][0][1:]
        """


        time_index = int(self.time_zeros[self.buffersize])
        self.time_zeros[time_index] = self.time_zeros[time_index+self.size] = float(fulldata[0])
        self.time_zeros[self.buffersize] = time_index = (time_index+1) % self.size
        self.time.append(fulldata[0])

        i = int(self.y1_zeros[self.buffersize])
        self.y1_zeros[i] = self.y1_zeros[i+self.size] = float(fulldata[1])
        self.y1_zeros[self.buffersize] = i = (i+1) % self.size
        self.y1.append(fulldata[1])

        j = int(self.y2_zeros[self.buffersize])
        self.y2_zeros[j] = self.y2_zeros[j+self.size] = float(fulldata[2])
        self.y2_zeros[self.buffersize] = j = (j+1) % self.size
        self.y2.append(fulldata[2])

        k = int(self.y3_zeros[self.buffersize])
        self.y3_zeros[k] = self.y3_zeros[k+self.size] = float(fulldata[3])
        self.y3_zeros[self.buffersize] = k = (k+1) % self.size
        self.y3.append(fulldata[3])

        self.data1.setData(self.time_zeros[time_index:time_index+self.size],self.y1_zeros[i:i+self.size])
        self.data1.setPos(self.step,0)
        self.data2.setData(self.time_zeros[time_index:time_index+self.size],self.y2_zeros[j:j+self.size])
        self.data2.setPos(self.step,0)
        self.data3.setData(self.time_zeros[time_index:time_index+self.size],self.y3_zeros[k:k+self.size])
        self.data3.setPos(self.step,0)

    #Below 4 change visibility of data# in the curves() method
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

    #Class instance of settings menu. Creates a dialog (popup)
    def settingsMenu(self):
        self.settingsPopUp = Dialog1()
        self.settingsPopUp.show()
        #self.settingsPopUp.exec()
        self.serial_values = self.settingsPopUp.getDialogValues()

    #Changes axes labels depending on what lab is selected
    def getLabTypeAxes(self):
        self.inputType = str(self.LabType.currentText())
        if self.inputType == "Position":
            self.graphWidgetOutput.setLabel('left',"<span style=\"color:white;font-size:16px\">&theta; (°)</span>")
            self.graphWidgetInput.setLabel('left',"<span style=\"color:white;font-size:16px\">Voltage</span>")
            self.graphWidgetOutput.setLabel('bottom',"<span style=\"color:white;font-size:16px\">Time (s)</span>")
            self.graphWidgetInput.setLabel('bottom',"<span style=\"color:white;font-size:16px\">Time (s)</span>")

        elif self.inputType == "Speed":
            self.graphWidgetOutput.setLabel('left',"<span style=\"color:white;font-size:16px\">&omega; (°/s)</span>")
            self.graphWidgetInput.setLabel('left',"<span style=\"color:white;font-size:16px\">Voltage</span>")
            self.graphWidgetOutput.setLabel('bottom',"<span style=\"color:white;font-size:16px\">Time (s)</span>")
            self.graphWidgetInput.setLabel('bottom',"<span style=\"color:white;font-size:16px\">Time (s)</span>")
    
    #GCode format for Labtype (int)
    def getLabType(self):
        self.input = str(self.LabType.currentText())
        if self.inputType == "Position":
            return(0)
        
        elif self.inputType =="Speed":
            return(1)

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

    #GCode format for PID values (float)
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
        return({
                "P": self.Pvalue, 
                "I": self.Ivalue, 
                "D": self.Dvalue
                })

    #GCode format for Setpoint (float?)
    def getSetpointValue(self):
        self.SetpointValue = self.SetpointInput.text()
        if self.SetpointValue == "":
            raise ValueError("Setpoint Field is empty. Enter in a value")
                    
        return(self.SetpointValue)

    #GCode format for Saturation (float?)
    def getSaturationValue(self):
        self.SaturationValue = self.SaturationInput.text()
        if self.SaturationValue == "":
            raise ValueError("Saturation Field is empty. Enter in a comma separated value")
                    
        return(self.SaturationValue)

    #GCode format for Sample Time
    def getSampleTimeValue(self):
        self.SampleTimeValue = self.SampleTimeInput.text()
        if self.SampleTimeValue == "":
            raise ValueError("Sample Time Field is empty. Enter in a a value")

        return(self.SampleTimeValue)

def main():
    app = QApplication(sys.argv)
    app.setStyle("Fusion")
    main = Window()
    main.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    while(1):
        try:
            main()
        except Exception as e:
            print(e)