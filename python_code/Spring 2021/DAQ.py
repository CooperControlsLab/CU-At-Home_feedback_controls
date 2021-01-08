import sys
import os
import time
from pyqtgraph import PlotWidget
import qdarkstyle #has some issues on Apple devices and high dpi monitors
import CoursesDataClass
from SettingsClass import *
from ui_file import Ui_MainWindow
from QLed import QLed
from PyQt5.QtGui import QDoubleValidator, QKeySequence, QPixmap, QRegExpValidator, QIcon, QFont
from PyQt5.QtWidgets import (QApplication, QPushButton, QWidget, QComboBox, 
QHBoxLayout, QVBoxLayout, QFormLayout, QCheckBox, QGridLayout, QDialog, 
QLabel, QLineEdit, QDialogButtonBox, QFileDialog, QSizePolicy, QLayout,
QSpacerItem, QGroupBox, QShortcut, QMainWindow)
import numpy as np
import csv
from itertools import zip_longest

if hasattr(Qt, 'AA_EnableHighDpiScaling'):
    QApplication.setAttribute(Qt.AA_EnableHighDpiScaling, True)
    QCoreApplication.setAttribute(Qt.AA_EnableHighDpiScaling, True)

if hasattr(Qt, 'AA_UseHighDpiPixmaps'):
    QApplication.setAttribute(Qt.AA_UseHighDpiPixmaps, True)
    QCoreApplication.setAttribute(Qt.AA_UseHighDpiPixmaps, True)


class Window(QMainWindow):
    def __init__(self, *args, **kwargs):
        super(Window, self).__init__(*args, **kwargs)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.setStyleSheet(qdarkstyle.load_stylesheet())
        self.connections()
        self.graphsettings()
        self.arduinoStatusLed()

    def arduinoStatusLed(self):
        self._led = QLed(self, onColour=QLed.Red, shape=QLed.Circle)
        self._led.clickable = False
        self._led.value = True
        self._led.setMinimumSize(QSize(15,15))
        self._led.setMaximumSize(QSize(15,15))     
        #self.statusBar().addWidget(self._led)
        self.statusLabel = QLabel("Arduino Status:")
        self.statusLabel.setFont(QFont("Calibri",12,QFont.Bold)) 

        self.statusBar().addWidget(self.statusLabel)
        #self.statusBar().reformat()
        self.statusBar().addWidget(self._led)

        self.initialTimer()
        self.initialState()

    def connections(self):
        """
        Menubar
        """
        self.ui.actionStatics.triggered.connect(self.staticsPushed) #
        self.ui.actionBeam.triggered.connect(self.beamPushed) #
        self.ui.actionSound.triggered.connect(self.soundPushed) #
        #self.ui.menubar.triggered.connect(self.soundPushed) # COME BACK TO THIS
        """
        5 Main Buttons
        """
        self.ui.serialOpenButton.clicked.connect(self.serialOpenPushed)  
        self.ui.serialCloseButton.clicked.connect(self.serialClosePushed)
        self.ui.startbutton.clicked.connect(self.startbuttonPushed)        
        self.ui.stopbutton.clicked.connect(self.stopbuttonPushed)
        self.ui.savebutton.clicked.connect(self.savebuttonPushed)
        self.ui.clearbutton.clicked.connect(self.clearbuttonPushed)
        self.ui.settings.clicked.connect(self.settingsMenu)

    def graphsettings(self):
        self.ui.graphWidgetOutput.showGrid(x = True, y = True, alpha=None)
        self.ui.graphWidgetInput.showGrid(x = True, y = True, alpha=None)
        self.ui.graphWidgetOutput.setBackground((0,0,0))
        self.ui.graphWidgetInput.setBackground((0,0,0))
        self.legendOutput = self.ui.graphWidgetOutput.addLegend()
        self.legendInput = self.ui.graphWidgetInput.addLegend()

        '''
        self.timer = QTimer()
        self.timer.setInterval(50) #Changes the plot speed. Defaulted to 50. Can be placed in startbuttonPushed() method
        self.initialState()
        time.sleep(2)
        try:
            self.timer.timeout.connect(self.updatePlot)
        except AttributeError:
            print("Something went wrong")
        '''
    def initialTimer(self):
        self.timer = QTimer()
        self.timer.setInterval(500) #Changes the plot speed. Defaulted to 50 ms. Can be placed in startbuttonPushed() method
        time.sleep(2)
        try: 
            self.timer.timeout.connect(self.updatePlot)
        except AttributeError:
            print("Something went wrong")

    def staticsPushed(self):
        print('statics')
        self.course = "Statics"    
        self.setWindowTitle(self.course)

        # Graph settings for specific lab
        self.ui.graphWidgetOutput.setLabel('left',"<span style=\"color:white;font-size:16px\">Voltage (V)</span>")
        self.ui.graphWidgetOutput.setLabel('bottom',"<span style=\"color:white;font-size:16px\">Time (s)</span>")
        self.ui.graphWidgetOutput.setTitle("Voltage????? Might be resistance IDK", color="w", size="12pt")

        self.ui.graphWidgetInput.setLabel('left',"")
        self.ui.graphWidgetInput.setLabel('bottom',"")
        self.ui.graphWidgetInput.setTitle("")

    def beamPushed(self):
        print('beam')
        self.course = "Beam"    
        self.setWindowTitle(self.course)

        # Graph settings for specific lab
        self.ui.graphWidgetOutput.setLabel('left',"<span style=\"color:white;font-size:16px\">Acceleration (m/s^2)</span>")
        self.ui.graphWidgetOutput.setLabel('bottom',"<span style=\"color:white;font-size:16px\">Time (s)</span>")
        self.ui.graphWidgetOutput.setTitle("Acceleration", color="w", size="12pt")

        self.ui.graphWidgetInput.setLabel('left',"")
        self.ui.graphWidgetInput.setLabel('bottom',"")
        self.ui.graphWidgetInput.setTitle("")

    def soundPushed(self):
        print('sound')
        self.course = "Sound"    
        self.setWindowTitle(self.course)

        # Graph settings for specific lab
        self.ui.graphWidgetOutput.setLabel('left',"<span style=\"color:white;font-size:16px\">Speed (m/s)</span>")
        self.ui.graphWidgetOutput.setLabel('bottom',"<span style=\"color:white;font-size:16px\">Time (s)</span>")
        self.ui.graphWidgetOutput.setTitle("Speed", color="w", size="12pt")

        self.ui.graphWidgetInput.setLabel('left',"<span style=\"color:white;font-size:16px\">°C</span>")
        self.ui.graphWidgetInput.setLabel('bottom',"<span style=\"color:white;font-size:16px\">Time (s)</span>")
        self.ui.graphWidgetInput.setTitle("Temperature", color="w", size="12pt")


    def serialOpenPushed(self):
        #Try/except/else/finally statement is to check whether settings menu was opened/changed
        try:
            self.size = self.serial_values[3] #Value from settings. Windows data

            if self.course == "Statics":
                self.serialInstance = CoursesDataClass.StaticsLab(self.serial_values[0],self.serial_values[1],self.serial_values[2])
                
            elif self.course == "Beam":
                self.serialInstance = CoursesDataClass.BeamLab(self.serial_values[0],self.serial_values[1],self.serial_values[2])

            elif self.course == "Sound":
                self.serialInstance = CoursesDataClass.SoundLab(self.serial_values[0],self.serial_values[1],self.serial_values[2])
                self.serialInstance.gcodeLetters = ["T","S","A"]

            self.serialInstance.open() #COME BACK TO THIS. I THINK IT'S WRONG 
            time.sleep(2)
            print("Serial successfully open!")

            if self.serialInstance.is_open() == True:
                self._led.onColour = QLed.Green  
                self.ui.serialOpenButton.clicked.disconnect(self.serialOpenPushed)
                self.ui.serialCloseButton.clicked.connect(self.serialClosePushed)
                self.ui.startbutton.clicked.connect(self.startbuttonPushed)

        except AttributeError:
            print("Settings menu was never opened or Course was never selected in menubar")

        except TypeError:
            print("Settings menu was opened, however OK was not pressed to save values")


    def serialClosePushed(self):
        if self.serialInstance.is_open() == True:
            self.serialInstance.close()
            print("Serial was open. Now closed")   
        elif self.serialInstance.is_open() == False:
            print("Serial is already closed")
        
        try:
            self.ui.serialOpenButton.clicked.connect(self.serialOpenPushed)
        except:
            print("Serial Open button already connected")

        self._led.onColour = QLed.Red

        self.ui.serialCloseButton.clicked.disconnect(self.serialClosePushed)

    def startbuttonPushed(self):
        print("Recording Data")
        self.timer.start()
        self.curve()
        self.ui.startbutton.clicked.disconnect(self.startbuttonPushed)

    def stopbuttonPushed(self):
        self.timer.stop()
        print("Stopping Data Recording")

    def clearbuttonPushed(self):
        self.ui.graphWidgetOutput.clear()
        self.ui.graphWidgetInput.clear()
        #Come back to this
        self.ui.graphWidgetOutput.addLegend()
        self.ui.graphWidgetInput.addLegend()
        #self.graphWidgetOutput.setRange(rect=None, xRange=None, yRange=[-1,100], padding=None, update=True, disableAutoRange=True)
        #self.graphWidgetInput.setRange(rect=None, xRange=None, yRange=[-13,13], padding=None, update=True, disableAutoRange=True)
        self.ui.startbutton.clicked.connect(self.startbuttonPushed)
        self.initialState() #Reinitializes arrays in case you have to retake data
        print("Cleared All Graphs")

    def savebuttonPushed(self):
        self.createCSV(self.course)
        path = QFileDialog.getSaveFileName(self, 'Save CSV', os.getenv('HOME'), 'CSV(*.csv)')
        if path[0] != '':
            with open(path[0], 'w', newline = '') as csvfile:
                csvwriter = csv.writer(csvfile)
                csvwriter.writerows(self.data_set)        
        print("Saved All Data")

    def settingsMenu(self):
        self.settingsPopUp = SettingsClass()
        self.settingsPopUp.show()
        #self.settingsPopUp.exec()
        self.serial_values = self.settingsPopUp.getDialogValues()

    def initialState(self):
        self.buffersize = 500 #np array size that is used to plot data
        self.step = 0 #Used for repositioning data in plot window to the left
        self.time_zeros = np.zeros(self.buffersize+1, float)
        self.y1_zeros = np.zeros(self.buffersize+1, float)
        self.y2_zeros = np.zeros(self.buffersize+1, float)
        self.y3_zeros = np.zeros(self.buffersize+1, float)        

        self.time = ["rat"]
        self.y1 = ["fat"]
        self.y2 = ["gnat"]
        self.y3 = ["cat"]

    def curve(self):
        pen1 = pg.mkPen(color = (255, 0, 0), width=1)
        pen2 = pg.mkPen(color = (0, 255, 0), width=1)
        pen3 = pg.mkPen(color = (0, 255, 255), width=1)

        if self.course == "Statics":
            self.data = self.ui.graphWidgetOutput.plot(pen = pen1, name="Voltage???") 

        elif self.course == "Beam":
            self.data = self.ui.graphWidgetOutput.plot(pen = pen1, name="Acceleration") 

        elif self.course == "Sound":
            self.data1 = self.ui.graphWidgetOutput.plot(pen = pen1, name="Mic 1") 
            self.data2 = self.ui.graphWidgetOutput.plot(pen = pen2, name="Mic 2") 
            self.data3 = self.ui.graphWidgetInput.plot(pen = pen3, name="Temperature")

    def createCSV(self,labtype):
        if labtype == "Statics":
            self.header = ["Time (ms???)", "Voltage???"]
            self.data_set = zip_longest(*[self.time,self.y1], fillvalue="")

        elif labtype == "Beam":
            self.header = ["Time (ms???)", "Acceleration???"]
            self.data_set = zip_longest(*[self.time,self.y1], fillvalue="")

        elif labtype == "Sound":
            self.header = ["Time (ms???)", "Mic 1", "Mic 2", "Temperature (°C)"]
            self.data_set = zip_longest(*[self.time,self.y1,self.y2,self.y3], fillvalue="")

    def updatePlot(self):
        fulldata = self.serialInstance.readValues()
        print(fulldata)
        self.step = self.step + 1
        print(self.serialInstance.gcodeParsing(fulldata))

def main():
    app = QApplication(sys.argv)
    main = Window()
    main.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
