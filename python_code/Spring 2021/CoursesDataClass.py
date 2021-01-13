import sys
import os
import time
import serial
import binascii

class SerialComm:

    def __init__(self, port, baudrate, timeout):
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout    
        self.thread = None
        self._gcodeLetters = None #Private
        self.time = []
        self.ser = serial.Serial(port = self.port,
                                 baudrate = self.baudrate,
                                 timeout = self.timeout)
    '''
    def open(self):
        """
        Opens serial communication
        """
        self.ser = serial.Serial(port = self.port,
                                 baudrate = self.baudrate,
                                 timeout = self.timeout)
        return(self.ser)
    '''

    def open(self):
        """
        Opens serial communication
        """
        self.ser.open()

    def close(self):
        """
        Closes serial communication
        """
        self.ser.close()

    def is_open(self):
        """
        Checks if serial communication is active. Returns boolean
        """
        return(self.ser.is_open)

    def flushInput(self):
        """
        Ensures all data in buffer is sent, then clears buffer
        """
        self.ser.flushInput()

    def readValues(self):
        """
        Current method to send byte (request) so Arduino can send back byte of data.
        Current format of received data is b"T23533228,S0.00,A0.00,Q0.00,\0\r\n"
        T is time 
        S is Setpoint
        A is Response (can be position or velocity)
        Q is PWM Voltage
        """
        #self.ser.write(b"R0,\0") 
        arduinoData = self.ser.readline().decode().replace('\r\n','').split(",")
        return arduinoData

    def requestByte(self):
        """
        Request byte to start and stop serial communication with arduino
        """
        self.ser.write(b"R0,\0") 

    def L(self):
        """
        Request byte to start and stop serial communication with arduino
        """
        self.ser.write(b"L2,\0") 


    def sendInitialRequest(self):
        """
        This should be the first thing sent to Arduino. It is to send a byte of what
        variables should only be sent back
        """
        if self._gcodeLetters is not None:
            a = ','.join(self._gcodeLetters)
            a = a + "\0" #adds null terminator to just the initial request, NOT the _gcodeLetters
            a = a.encode()
            #return binascii.hexlify(a)
            self.ser.write(a)
            return a

    
    def gcodeParsing(self,datastream,hex=False):
        '''
        Returns full list of data from Arduino given the specified gcode letters
        '''
        #result = [_ for _ in input_list if _.startswith(letter)][0][1:]
        result = [_ for _ in datastream if _.startswith(tuple(self._gcodeLetters))]
        #result = [_ for _ in fake_stream if _.startswith(tuple(classType.gcodeLetters))] 
        #result = [_[1:] for _ in result]
        return(result)

    def time(self,datastream):
        """
        "T"
        """
        pass 
    
    @property
    def gcodeLetters(self):
        """
        Getter
        """
        return self._gcodeLetters
    
    @gcodeLetters.setter
    def gcodeLetters(self, var):
        """
        Sets _gcodeLetters attribute as a list of characters to be parsed
        """
        if type(var) is list:
            self._gcodeLetters = var 

        else:
            raise TypeError("Should be set to a list")


    '''
    def dataPlot(self):
        try:
            j = int(self.y2_zeros[self.buffersize])
            self.y2_zeros[j] = self.y2_zeros[j+self.size] = float(self.gcodeParsing("A",fulldata))
            self.y2_zeros[self.buffersize] = j = (j+1) % self.size
            self.y2.append(self.gcodeParsing("A",fulldata))
        except ValueError:
            print("Couldn't parse")
        except IndexError:
            print("Couldn't parse index. Skipping point")
    '''

    def initialLabSelectionSend(self):
        pass

    def initialLabSelectionReceive(self):
        pass

    def labSelection(self,course):
        self.ser.write(f"L{course}".encode())
        '''
        case 1:
            lab = new ProCon();
            break;
        case 2:
            lab = new Statics();
            break;
        case 3:
            lab = new SpeedofSound();
            break;
        case 4:
            lab = new Beam();
            break;
        '''

class StaticsLab(SerialComm):
    def __init__(self, port, baudrate, timeout):
        super().__init__(port, baudrate, timeout)

class BeamLab(SerialComm):
    def __init__(self, port, baudrate, timeout):
        super().__init__(port, baudrate, timeout)

class SoundLab(SerialComm):
    def __init__(self, port, baudrate, timeout):
        super().__init__(port, baudrate, timeout)

class GeneralizedDAQ(SerialComm):
    def __init__(self, port, baudrate, timeout):
        super().__init__(port, baudrate, timeout)

#self.serialInstance.gcodeLetters = ["T","S","A"]

'''
classType = Thermo("COM5",500000,0.1)
classType.gcodeLetters = ("T","Q")#, "\0"]

fake_stream = ['T1', 'S12', 'A1.23', 'Q6.56', '\x00']
fake_stream1 = ['T2', 'S13', 'A3.23', 'Q3.56', '\x00']
fake_stream2 = ['T3', 'S14', 'A153', 'Q4.66', '\x00']

classType  = Thermo("COM5",500000,0.1)
classType.gcodeLetters = ["T","Q"]#, "\0"]
print(classType.sendInitialRequest().decode())
#startswith intakes a tuple
result = [_ for _ in fake_stream if _.startswith(tuple(classType.gcodeLetters))] 
result = [_[1:] for _ in result]
print(result)

classType  = Drones("COM5",500000,0.1)
classType.gcodeLetters = ["S","A"]#, "\0"]
print(classType.sendInitialRequest().decode())
result = [_ for _ in fake_stream1 if _.startswith(tuple(classType.gcodeLetters))] 
result = [_[1:] for _ in result]
print(result)
'''

'''
example  = Thermo("COM5",500000,0.1)
example.gcodeLetters = ["S","A"]
example.serialOpen()
print(example.sendInitialRequest())

def main():
    count = 0
    while count != 20:
        data = example.readValues() 
        print(example.gcodeParsing(data))
        #print(data)
        count = count + 1
    example.serialClose()

if __name__ == '__main__':
    print("Is serial open?", example.serialIsOpen())
    main()
    print("Is serial open?", example.serialIsOpen())
'''