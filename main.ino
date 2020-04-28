
/* 
https://www.arduinoshop99.com/product/363/gy-mcu90614-bcc-serial-port-ir-non-contact-infrared-thermometry-module-mlx90614-bcc
 
GY-MCU90614-BCC Serial Port IR Non-Contact Infrared Thermometry Module MLX90614-BCC
Description:
 
Power supply: 3-5v
Current: 5mA
Baud rate: 9600 or 115200
Default auto output frequency: 14HZ
Measurement range:
-40… +125 C degree for ambient temperature
-70… +380 C degree for measurement target temperature
Measuring distance: close range 5-10cm
Size: 21.5 *23 *9.7mm
 
Communication protocol:
 
Serial port command byte:
(1) Serial communication parameters (the default baud rate value 9600bps, can be set by software)
Baud rate:9600 bps  Check bit:N  Data bits:Eight  Stop bit:1
Baud rate:115200 BPS  Check bit:N  Data bits:Eight  Stop bit:1
(2) Module input command,Sent to the GY-MCU90614 Module from External controller(sixteen hex)
 
1.Frame header:0xa5
Instruction format: header+instructions+Check and (8bit) (such as automatic reading of temperature instructions=0xA5+0x45+0xEA)
2.Command instruction:
Continuous output instruction:
0xA5+0x45+0xEA—————-Temperature data (module return data type 0x45)
Query output instruction:
– 0xA5+0x15+0xBATemperature data (module return data type 0x45)
Configuration instruction:(after Power down reset will active)
Baud rate configuration:
0xA5+0xAE+0x53—————9600 (default)
0xA5+0xAF+0x54—————115200
Power on whether automatic transmit temperature data configuration:
0xA5+0x51+0xF6—————Automatic output temperature data after power up(default)
0xA5+0x52+0xF7—————Not automatic output temperature data after power up
 
Communication protocol:
 
Serial port receive:
(1) Serial communication parameters (the default baud rate value 9600 BPS, can be set by software
Baud rate:9600 BPS  Check bit:N  Data bits:Eight  Stop bit:1
Baud rate:115200 BPS  Check bit:N  Data bits:Eight  Stop bit:1
 
(2) Module output format, each frame contains9Bytes (sixteen decimal):
1. Byte0: 0x5A  Header flag
2. Byte1: 0x5A  Header flag
3. Byte2: 0X45  Data type of the frame(0X45: Temperature data)
4. Byte3: 0x04  Data volume (below 4 data make 2 Groups as an example)
5. Byte4: 0x00~0xFF  Data 1 high 8 position
6. Byte5: 0x00~0xFF  Data 1 low 8 position
7. Byte6: 0x00~0xFF  Data 2 high 8 position
8. Byte7: 0x00~0xFF  Data 2 low 8 position
9. Byte8: 0x00~0xFF  Checksum(preceding data accumulate, leaving only low 8 bit)
(3) Data calculation method
Temperature calculation method :
temperature=high 8 position<<8|low 8 bit(The result is 100 multiplied by the actual angle)
Example: send Instruction: A5 45 EA, received One frame data:
<5A- 5A- 45- 04- 0C- 78- 0D- 19- A7 >
Express TO (There is a sign 16 bit, which represents the target temperature):TO=0x0C78/100=31.92C
Express TA (There is a sign 16 bit, indicates the ambient temperature_TO=0x0D19/100=33.53C
 
Usage method:
 
The module is the serial output data, user through the serial port connection, send output instructions, such as 0xA5+0x45+0xEA To the module, the module will be continuous output temperature data; if you want to query output,you can send 0xA5+0x15+0xBA To the module, every time to send, the module will return a temperature data, the query frequency should be lower than 10Hz, if required more than 10Hz continuous output mode, Please  send 0xA5+0x45+0xEA Instruction.

 */

#include <SoftwareSerial.h>
SoftwareSerial serial(10, 9); // sensor TX - D10 , RX - D9
unsigned char Re_buf[11],counter=0;
unsigned char sign=0;
float TO=0,TA=0;


void setup() {
  Serial.begin(9600);
  serial.begin(9600);
  delay(1);
  char packet[3] = {0xA5, 0x45, 0xEA};
  serial.write(packet,sizeof(packet));
}

void loop() {
  unsigned char i=0,sum=0;
  
  if(serial.available()){
    Re_buf[counter]=(unsigned char)serial.read();
    if(counter==0&&Re_buf[0]!=0x5A) return;
    counter++;
    if(counter==9)
    {
       counter=0;
       sign=1;
    }      
  }

  if(sign)
  {
     sign=0;
     for(i=0;i<8;i++)
      sum+=Re_buf[i]; 
     if(sum==Re_buf[i] )       
     {           
        TO=(float)(Re_buf[4]<<8|Re_buf[5])/100;
        Serial.print("TO:");
        Serial.println(TO);  
        TA=(float)(Re_buf[6]<<8|Re_buf[7])/100;
        Serial.print("TA:");
        Serial.println(TA);             
     }
   } 
}
