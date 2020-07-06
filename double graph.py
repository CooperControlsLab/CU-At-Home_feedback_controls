import sys 
import os
import time
from PyQt5.QtWidgets import QApplication, QLabel, QLineEdit, QPushButton, QWidget, QComboBox, QHBoxLayout, QVBoxLayout, QFormLayout, QCheckBox, QStackedWidget
from PyQt5.QtCore import Qt, QTimer
from pyqtgraph import PlotWidget, plot
import pyqtgraph as pg

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
        self.startbutton.clicked.connect(self.startbutton_pushed)
        self.startbutton.resize(100,20)

        self.stopbutton = QPushButton("Stop",self)
        self.stopbutton.clicked.connect(self.stopbutton_pushed)
        self.startbutton.resize(100,20)

        self.plot1 = QCheckBox("Plot 1", self)
        self.plot1.toggled.connect(self.plotting1)
        
        self.plot2 = QCheckBox("Plot 2", self)
        self.plot2.toggled.connect(self.plotting2)

        self.plot3 = QComboBox()
        self.plot3.addItems(["Sine","Step","Square"])
                
        self.graphWidget = pg.PlotWidget()

        #Positioning the buttons and checkboxes
        #leftFormLayout.setContentsMargins(70,100,10,10)
        leftFormLayout.addRow(self.startbutton,self.stopbutton)
        leftFormLayout.addRow(self.plot1)
        leftFormLayout.addRow(self.plot2)
        leftFormLayout.addRow(self.plot3)
        rightLayout.addWidget(self.graphWidget)

        #Plot settings
        self.setLayout(mainLayout)
        self.timer=QTimer()
        self.timer.setInterval(50)
        try:
            self.timer.timeout.connect(self.update_plot_data1)
        except:
            raise IOError("Missing 1")

        try:
            self.timer.timeout.connect(self.update_plot_data2)
        except:
            raise IOError("Missing 2")
        #self.timer.timeout.connect(self.update_plot_data1)
        #self.timer.timeout.connect(self.update_plot_data2)

        #self.show()
    
    #Start Button
    def startbutton_pushed(self):
        self.timer.start()

    #Stop Button
    def stopbutton_pushed(self):
        self.timer.stop()

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

    def plotting1(self):
        test1 = self.sender()
        if test1.isChecked() == True:
            self.x1 = list(range(100)) 
            self.y1 = [0 for i in self.x1] 
            pen1 = pg.mkPen(color=(255, 0, 0))
            self.data1 = self.graphWidget.plot(self.x1, self.y1, pen = pen1)           
        #elif test1.isChecked() == False:
            #self.x1 = []
            #self.y1 = []
            #self.data1 = self.graphWidget.plot(self.x1, self.y1)

    def plotting2(self):
        test2 = self.sender()
        if test2.isChecked() == True:
            self.x2 = list(range(100))
            self.y2 = [2 for i in self.x2]
            pen2 = pg.mkPen(color=(0, 0, 255))
            self.data2 = self.graphWidget.plot(self.x2, self.x2, pen = pen2)
        #elif test2.isChecked() == False:
            #self.x2 = []
            #self.y2 = []
            #self.data2 = self.graphWidget.plot(self.x2, self.y2)

def main():
    app = QApplication(sys.argv)
    app.setStyle("Fusion")
    main = Window()
    main.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()