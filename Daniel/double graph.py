import sys 
import os
import time
from PyQt5.QtGui import QRegExpValidator
from PyQt5.QtWidgets import (QApplication, QPushButton, QWidget, QComboBox, 
QHBoxLayout, QVBoxLayout, QFormLayout, QCheckBox, QButtonGroup, QDialog, 
QLabel, QLineEdit, QDialogButtonBox)
from PyQt5.QtCore import Qt, QTimer, QRegExp
from pyqtgraph import PlotWidget, plot
import pyqtgraph as pg
from random import randint
import serial
import serial.tools.list_ports
import numpy as np

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
        mainLayout = QHBoxLayout() 
        leftFormLayout = QFormLayout()
        mainLayout.addLayout(leftFormLayout,100)

        self.port_label = QLabel("Ports:",self)
        #self.port_label.move(40,55) #(x,-y) relative to the top left corner of window
        self.port_label.setStyleSheet("font-size:12pt;")
        
        self.port = QComboBox(self)
        self.port.setFixedWidth(100)
        #self.port.resize(self.port.sizeHint())
        #self.port.move(100,50) #(x,-y) relative to the top left corner of window
        self.port.setStyleSheet("font-size:12pt;")
        self.list_port()


        self.baudrate_label = QLabel("Baud Rate:",self)
        self.baudrate_label.setStyleSheet("font-size:12pt;")

        self.baudrate = QComboBox(self)
        self.baudrate.setFixedWidth(100)
        self.baudrate.addItems(["4800","9600","14400"])
        self.baudrate.setStyleSheet("font-size:12pt;")
        
        
        self.timeout_label = QLabel("Timeout:",self)
        self.timeout_label.setStyleSheet("font-size:12pt;")

        self.timeout = QLineEdit(self)
        self.timeout.setFixedWidth(100)
        self.timeout.setStyleSheet("font-size:12pt;")
        self.timeout.setText("1")

        #For now, it will be 0-255 (FIX THIS IN FUTURE; Timeout in increments of 1s to 255s is weird)
        regex = QRegExp("^([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$")
        regex_validator_timeout = QRegExpValidator(regex,self)

        self.timeout.setValidator(regex_validator_timeout)
 
        
        self.samplenum_label = QLabel("Sample #:",self)
        self.samplenum_label.setStyleSheet("font-size:12pt;")

        self.samplenum = QLineEdit(self)
        self.samplenum.setFixedWidth(100)
        self.samplenum.setStyleSheet("font-size:12pt;")
        self.samplenum.setText("100")

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
            if 'Arduino' in p.description  
        ]
        if not arduino_ports:
            raise IOError("No Arduino found. Replug in USB cable and try again.")
        self.port.addItems(arduino_ports)
        #pass

    #def list_port(self):
        #ports = list(serial.tools.list_ports.comports())
        #for p in ports:
            #if "Arduino" in p.description:
                #self.port.addItems(p)
        #print(p)

    def getDialogValues(self):
        if self.exec_() == QDialog.Accepted:
            self.com_value = str(self.port.currentText())
            self.baudrate_value = str(self.baudrate.currentText())
            self.timeout_value = int(self.timeout.text())
            self.samplenum_value = int(self.samplenum.text())

            global ser 
            ser = serial.Serial(self.com_value, baudrate = self.baudrate_value, timeout = self.timeout_value)
            
            global pyqtgraph_samples
            pyqtgraph_samples = self.samplenum_value
            
            print(self.com_value)
            print(self.baudrate_value)
            print(self.timeout_value)
            print(self.samplenum_value)
            return self.baudrate_value, self.timeout_value, self.samplenum_value

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
        self.setStyleSheet("font-size:12pt")
        mainLayout = QHBoxLayout()
        leftFormLayout = QFormLayout()
        rightLayout = QVBoxLayout()
        mainLayout.addLayout(leftFormLayout,20)
        mainLayout.addLayout(rightLayout,150)

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
        #self.savebutton.clicked.connect(self.savebutton_pushed)
        self.savebutton.resize(100,20)        
        self.savebutton.setFixedWidth(100)

        self.checkBoxShowAll = QCheckBox("Show All Plots", self)
        self.checkBoxShowAll.setChecked(True)
        self.checkBoxShowAll.toggled.connect(self.visibility_all)

        self.checkBoxHideAll = QCheckBox("Hide All Plots", self)
        self.checkBoxHideAll.setChecked(False)
        self.checkBoxHideAll.toggled.connect(self.hide_all)

        self.checkBoxPlot1 = QCheckBox("Plot 1", self)
        self.checkBoxPlot1.toggled.connect(self.visibility1)
        
        self.checkBoxPlot2 = QCheckBox("Plot 2", self)
        self.checkBoxPlot2.toggled.connect(self.visibility2)

        self.checkBoxShowAll.stateChanged.connect(self.checkbox_logic) 
        self.checkBoxHideAll.stateChanged.connect(self.checkbox_logic) 
        self.checkBoxPlot1.stateChanged.connect(self.checkbox_logic) 
        self.checkBoxPlot2.stateChanged.connect(self.checkbox_logic) 


        self.settings = QPushButton("Settings",self)
        self.settings.clicked.connect(self.settingsMenu)

        #Buttongroup
        #self.group1 = QButtonGroup()
        #self.group1.addButton(self.showall)
        #self.group1.addButton(self.plot1)
        #self.group1.addButton(self.plot2)
        #self.group1.setId(self.showall, 0)
        #self.group1.setId(self.plot1, 1)
        #self.group1.setId(self.plot2, 2)
            
        self.plot3 = QComboBox()
        self.plot3.addItems(["Sine","Step","Square"])

        #Creates Plotting Widget        
        self.graphWidget = pg.PlotWidget()
        #state = self.graphWidget.getState()

        #Adds grid lines
        self.graphWidget.showGrid(x = True, y = True, alpha=None)
        #self.graphWidget.setXRange(0, 100, padding=0) #Doesn't move with the plot. Can drag around
        #self.graphWidget.setLimits(xMin=0, xMax=100)#, yMin=c, yMax=d) #Doesn't move with the plot. Cannot drag around

        #self.graphWidget.setYRange(0, 4, padding=0)
        self.graphWidget.setYRange(-11, 11, padding=0)
        self.graphWidget.enableAutoRange()

        
        #Changes background color of graph
        #self.graphWidget.setBackground('w')
        self.graphWidget.setBackground((0,0,0))

        #Positioning the buttons and checkboxes
        #leftFormLayout.setContentsMargins(70,100,10,10)
        leftFormLayout.addRow(self.startbutton,self.stopbutton)
        leftFormLayout.addRow(self.clearbutton,self.savebutton)
        leftFormLayout.addRow(self.settings)
        leftFormLayout.addRow(self.checkBoxShowAll)
        leftFormLayout.addRow(self.checkBoxHideAll)
        leftFormLayout.addRow(self.checkBoxPlot1)
        leftFormLayout.addRow(self.checkBoxPlot2)
        leftFormLayout.addRow(self.plot3)
        rightLayout.addWidget(self.graphWidget)

        #Plot time update settings
        self.setLayout(mainLayout)
        self.timer = QTimer()
        self.timer.setInterval(50) #Changes the plot speed
        
        try:
            self.timer.timeout.connect(self.update_plot_data1)
        except:
            raise Exception("Missing 1")

        try:
            self.timer.timeout.connect(self.update_plot_data2)
        except:
            raise Exception("Missing 2")

        try: 
            self.timer.timeout.connect(self.update_plot_data3)
        except:
            raise Exception("Missing 3")

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
    
    #Start Button
    def startbutton_pushed(self):
        self.timer.start()
        self.plotting1()
        self.plotting2()
        self.plotting3()
        self.startbutton.clicked.disconnect(self.startbutton_pushed)

    #Stop Button
    def stopbutton_pushed(self):
        self.timer.stop()

    #Clear Button
    def clearbutton_pushed(self):
        self.graphWidget.clear()
        #self.graphWidget.setXRange(0, 100, padding=0)
        #self.graphWidget.setYRange(0, 4, padding=0)
        #self.graphWidget.setXRange(0, 100, padding=0) #Doesn't move with the plot. Can drag around
        #self.graphWidget.setLimits(xMin=0, xMax=100)#, yMin=c, yMax=d) #Doesn't move with the plot. Cannot drag around
        self.graphWidget.enableAutoRange(axis=None, enable=True, x=None, y=None)
        self.startbutton.clicked.connect(self.startbutton_pushed)

    def update_plot_data1(self):
        self.x1 = self.x1[1:]  
        self.x1.append(self.x1[-1] + 1)  
        self.y1 = self.y1[1:]  
        self.y1.append(randint(-10,10))
        self.data1.setData(self.x1, self.y1)  
    
    def update_plot_data2(self):
        self.x2 = self.x2[1:]  
        self.x2.append(self.x2[-1] + 1)  
        self.y2 = self.y2[1:]   
        self.y2.append(randint(-10,10))
        self.data2.setData(self.x2, self.y2)      

    def update_plot_data3(self):
        self.x3 = self.x2[1:]  
        self.x3.append(self.x2[-1] + 1)  
        self.y3 = self.y2[1:]    
        self.y3.append(np.nan)
        self.data3.setData(self.x3, self.y3) 

    def visibility_all(self):
        showall = self.sender()
        if showall.isChecked() == True:
            self.data1.setVisible(True)
            self.data2.setVisible(True) 

    def hide_all(self):
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

    def plotting1(self):
        self.x1 = list(range(100)) 
        self.y1 = [randint(-10,10) for i in self.x1] 
        pen1 = pg.mkPen(color = (255, 0, 0), width=1)
        self.data1 = self.graphWidget.plot(self.x1, self.y1, pen = pen1)
    
    def plotting2(self):
        self.x2 = list(range(100))
        self.y2 = [randint(-10,10) for i in self.x2]
        pen2 = pg.mkPen(color = (0, 255, 0), width=1)
        self.data2 = self.graphWidget.plot(self.x2, self.y2, pen = pen2)

    def plotting3(self):
        self.x3 = list(range(100))
        self.y3 = [1 for i in self.x3]
        #pen3 = pg.mkPen(color = (12, 65, 64), width=1)
        self.data3 = self.graphWidget.plot(self.x3, self.y3, connect="finite", pen = None)

    def settingsMenu(self):
        self.settingsPopUp = Dialog1()
        self.settingsPopUp.show()
        #self.settingsPopUp.exec()
        self.settingsPopUp.getDialogValues()

def main():
    app = QApplication(sys.argv)
    app.setStyle("Fusion")
    main = Window()
    main.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()