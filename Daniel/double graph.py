import sys 
import os
import time
from PyQt5.QtWidgets import QApplication, QPushButton, QWidget, QComboBox, QHBoxLayout, QVBoxLayout, QFormLayout, QCheckBox, QButtonGroup
from PyQt5.QtCore import Qt, QTimer
from pyqtgraph import PlotWidget, plot
import pyqtgraph as pg
from random import randint

class Window(QWidget):
    def __init__(self, *args, **kwargs):
        super(Window, self).__init__(*args, **kwargs)
        
        #Application Title
        self.title = "Controlling Multiple Plots"
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

        self.stopbutton = QPushButton("Stop",self)
        self.stopbutton.setCheckable(False)  
        self.stopbutton.clicked.connect(self.stopbutton_pushed)
        self.stopbutton.resize(100,20)

        self.clearbutton = QPushButton("Clear",self)
        self.clearbutton.setCheckable(False)
        self.clearbutton.clicked.connect(self.clearbutton_pushed)
        self.clearbutton.resize(100,20)

        self.savebutton = QPushButton("Save",self)
        self.savebutton.setCheckable(False)
        #self.savebutton.clicked.connect(self.savebutton_pushed)
        self.savebutton.resize(100,20)        

        self.plotall = QCheckBox("All Plots", self)
        self.plotall.setChecked(True)
        self.plotall.toggled.connect(self.visibility_all)

        self.plot1 = QCheckBox("Plot 1", self)
        #self.plot1.setChecked(True)
        self.plot1.toggled.connect(self.visibility1)
        
        self.plot2 = QCheckBox("Plot 2", self)
        #self.plot2.setChecked(True)
        self.plot2.toggled.connect(self.visibility2)

        #Buttongroup
        self.group1 = QButtonGroup()
        self.group1.addButton(self.plotall)
        self.group1.addButton(self.plot1)
        self.group1.addButton(self.plot2)
        self.group1.setId(self.plotall, 0)
        self.group1.setId(self.plot1, 1)
        self.group1.setId(self.plot2, 2)
            
        self.plot3 = QComboBox()
        self.plot3.addItems(["Sine","Step","Square"])

        #Creates Plotting Widget        
        self.graphWidget = pg.PlotWidget()
        #state = self.graphWidget.getState()

        #Adds grid lines
        self.graphWidget.showGrid(x = True, y = True, alpha=None)
        #self.graphWidget.setXRange(0, 100, padding=0) #Doesn't move with the plot. Can drag around
        #self.graphWidget.setLimits(xMin=0, xMax=100)#, yMin=c, yMax=d) #Doesn't move with the plot. Cannot drag around

        self.graphWidget.setYRange(0, 4, padding=0)
        self.graphWidget.enableAutoRange()

        
        #Changes background color of graph
        self.graphWidget.setBackground('w')

        #Positioning the buttons and checkboxes
        #leftFormLayout.setContentsMargins(70,100,10,10)
        leftFormLayout.addRow(self.startbutton,self.stopbutton)
        leftFormLayout.addRow(self.clearbutton,self.savebutton)
        leftFormLayout.addRow(self.plotall)
        leftFormLayout.addRow(self.plot1)
        leftFormLayout.addRow(self.plot2)
        leftFormLayout.addRow(self.plot3)
        rightLayout.addWidget(self.graphWidget)

        #Plot time update settings
        self.setLayout(mainLayout)
        self.timer=QTimer()
        self.timer.setInterval(50) #Changes the plot speed
        
        try:
            self.timer.timeout.connect(self.update_plot_data1)
        except:
            raise Exception("Missing 1")

        try:
            self.timer.timeout.connect(self.update_plot_data2)
        except:
            raise Exception("Missing 2")
        self.timer.timeout.connect(self.update_plot_data1)
        self.timer.timeout.connect(self.update_plot_data2)

        #self.show()
    
    #Start Button
    def startbutton_pushed(self):
        self.timer.start()
        self.plotting1()
        self.plotting2()
       
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

    def update_plot_data1(self):
        self.x1 = self.x1[1:]  
        self.x1.append(self.x1[-1] + 1)  
        self.y1 = self.y1[1:]  
        self.y1.append(self.y1[-1])  
        self.data1.setData(self.x1, self.y1)  
    
    def update_plot_data2(self):
        self.x2 = self.x2[1:]  
        self.x2.append(self.x2[-1] + 1)  
        self.y2 = self.y2[1:]  
        self.y2.append(self.y2[-1])    
        self.data2.setData(self.x2, self.y2)      

    def visibility_all(self):
        testall = self.sender()
        if testall.isChecked() == True:
            self.data1.setVisible(True)
            self.data2.setVisible(True)
        #elif testall.isCheck() == False:
        #    self.data1.setVisible(False)
        #    self.data2.setVisible(False)        

    def visibility1(self):
        test1 = self.sender()
        if test1.isChecked() == True:
            self.data1.setVisible(True)
            self.data2.setVisible(False)       
        #elif test1.isChecked() == False:
        #    self.data1.setVisible(False)

    def visibility2(self):
        test2 = self.sender()
        if test2.isChecked() == True:
            self.data2.setVisible(True)
            self.data1.setVisible(False)       
        #elif test2.isChecked() == False:
        #    self.data2.setVisible(False)
    
    def plotting1(self):
        self.x1 = list(range(100)) 
        self.y1 = [2 for i in self.x1] 
        pen1 = pg.mkPen(color = (255, 0, 0), width=5)
        self.data1 = self.graphWidget.plot(self.x1, self.y1, pen = pen1)
    
    def plotting2(self):
        self.x2 = list(range(100))
        self.y2 = [1 for i in self.x2]
        pen2 = pg.mkPen(color = (0, 0, 255), width=5)
        self.data2 = self.graphWidget.plot(self.x2, self.x2, pen = pen2)

def main():
    app = QApplication(sys.argv)
    app.setStyle("Fusion")
    main = Window()
    main.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()