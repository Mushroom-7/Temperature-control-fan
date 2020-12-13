#include "dht11.h"//引入头文件dht11
dht11 DHT11;//创建dht11
#define DHT11PIN A0//定义DHT11接入Arduino的管脚为A0
/*******************************************************************************/
#include "U8glib.h"//引入U8g库
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);        //OLED屏定义
#define setFont_L u8g.setFont(u8g_font_7x13)       //大号字体
#define setFont_M u8g.setFont(u8g_font_fixed_v0r)  //中号字体
#define setFont_S u8g.setFont(u8g_font_fixed_v0r)  //小号字体
#define setFont_XL u8g.setFont(u8g_font_fub25n)    //特大号字体

//OLED字模点阵
//字模提取软件可以用PCtoLCD2002或zimo221
const   uint8_t bitmap_s []   U8G_PROGMEM  ={  
0x00,0x00,0x27,0xF8,0x14,0x08,0x14,0x08,0x87,0xF8,0x44,0x08,0x44,0x08,0x17,0xF8,
0x11,0x20,0x21,0x20,0xE9,0x24,0x25,0x28,0x23,0x30,0x21,0x20,0x2F,0xFE,0x00,0x00,/*"湿",0*/
};
const   uint8_t bitmap_d []   U8G_PROGMEM  ={ 
0x01,0x00,0x00,0x80,0x3F,0xFE,0x22,0x20,0x22,0x20,0x3F,0xFC,0x22,0x20,0x22,0x20,
0x23,0xE0,0x20,0x00,0x2F,0xF0,0x24,0x10,0x42,0x20,0x41,0xC0,0x86,0x30,0x38,0x0E,/*"度",1*/
};
const   uint8_t bitmap_h []   U8G_PROGMEM  ={ 
0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,/*":",0*/
};
const   uint8_t bitmap_w []   U8G_PROGMEM  ={  
0x00,0x00,0x23,0xF8,0x12,0x08,0x12,0x08,0x83,0xF8,0x42,0x08,0x42,0x08,0x13,0xF8,
0x10,0x00,0x27,0xFC,0xE4,0xA4,0x24,0xA4,0x24,0xA4,0x24,0xA4,0x2F,0xFE,0x00,0x00,/*"温",0*/
};
const   uint8_t bitmap_du []   U8G_PROGMEM  ={ 
0x01,0x00,0x00,0x80,0x3F,0xFE,0x22,0x20,0x22,0x20,0x3F,0xFC,0x22,0x20,0x22,0x20,
0x23,0xE0,0x20,0x00,0x2F,0xF0,0x24,0x10,0x42,0x20,0x41,0xC0,0x86,0x30,0x38,0x0E,/*"度",1*/
};
const   uint8_t bitmap_t []   U8G_PROGMEM  ={ 
0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,/*":",0*/
};
/*******************************************************************************/
//定义全局变量
double analogVotage;  //模拟电压值
unsigned int dutyCycle;  //占空比
unsigned int tempMin = 5;  //零速温度
unsigned int tempMax = 30 ;  //满速温度
/*******************************************************************************/


void setup()//设置
{
  Serial.begin(9600);     //设置串口波特率为9600//串口监视器中的波特率要和这里的相同，才能在串口监视器中打印想打印的数据或文字内容
/*******************************************************************************/
  //以下信息为DHT11库文件版本信息
  Serial.println("DHT11 TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT11LIB_VERSION);
  Serial.println();
/*******************************************************************************/
  pinMode(9, OUTPUT);   //管脚为D9，发红色光
  pinMode(10, OUTPUT);  //管脚为D10，发绿色光
  pinMode(11, OUTPUT);  //管脚为D11，发蓝色光
}
/*******************************************************************************/




void loop()//循环
{
/*******************************************************************************/
  //记录数据组别
  static int count = 1;
  Serial.println(count++);

  //获取DHT11传感器数据
  int chk = DHT11.read(DHT11PIN);
  
  //DHT11传感器数据校验状态
  Serial.print("Read sensor: ");
  switch (chk)
  {
    case DHTLIB_OK: 
                Serial.println("OK"); 
                break;
    case DHTLIB_ERROR_CHECKSUM: 
                Serial.println("Checksum error"); 
                break;
    case DHTLIB_ERROR_TIMEOUT: 
                Serial.println("Time out error"); 
                break;
    default: 
                Serial.println("Unknown error"); 
                break;
  }
/*******************************************************************************/
  //计算占空比,控制LED_BGCR的颜色（HIGH表示高电平，LOW表示低电平）（注：可以通过调节红绿蓝三原色各自的显示频率来使该LED发出任意颜色的光）
  if (DHT11.temperature < tempMin){
      dutyCycle = 0;
      digitalWrite(9,HIGH); digitalWrite(10,LOW); digitalWrite(11,LOW);//打开主开关后，温度低于阈值，亮红灯
      }
  else if (DHT11.temperature < tempMax){
      dutyCycle = (DHT11.temperature-tempMin)*255/(tempMax-tempMin);
      digitalWrite(9,LOW); digitalWrite(10,HIGH); digitalWrite(11,LOW);//打开主开关后，温度高于阈值，但未满速，亮绿灯
      }
  else {
      dutyCycle = 255;
      digitalWrite(9,LOW); digitalWrite(10,LOW); digitalWrite(11,HIGH);//打开主开关后，风扇满速，亮蓝灯
      }
  analogWrite(5,dutyCycle);  //在管脚D5处产生PWM,使风扇转速随温度变化平滑连续地改变
/*******************************************************************************/
  //在串口监视器中打印DHT11获得的湿度数据
  Serial.print("Humidity : ");
  Serial.print((float)DHT11.humidity,2);//湿度，浮点数，数值精确到小数点后两位
  Serial.println("%");
  //在串口监视器中打印DHT11获得的温度数据
  Serial.print("Temperature (oC): ");
  Serial.println((float)DHT11.temperature, 2);
  //在串口监视器中打印计算得到的占空比
  Serial.print(" Degrees    Duty cycle: ");
  Serial.println(dutyCycle);
  Serial.println("");//Serial.println是换行打印
/*******************************************************************************/
  //OLED显示
  u8g.firstPage();
  do {
    draw();//draw函数（在下面自己定义）//也可以直接把draw函数里的内容放入这里，但用draw函数可以使主体部分的代码看起来更简洁
  } while (u8g.nextPage());
/*******************************************************************************/
  delay(100);//刷新频率//loop函数循环频率
}
/*******************************************************************************/




//设定draw函数
//作用：定义显示在OLED屏上的内容
void draw(void){
  //OLED显示中文（点阵）
  u8g.setColorIndex(1);//颜色选择
  //括号中第一个是x轴，第二个是y轴，第三个是点阵的宽度，要用实际宽度除以8，第四个是点阵的高度（以上单位全是像素）,第五个是这个点阵的名字
  u8g.drawBitmapP ( 0 , 6 , 2 , 16 , bitmap_s);
  u8g.drawBitmapP ( 16 , 6 , 2 , 16 , bitmap_d);
  u8g.drawBitmapP ( 32 , 6 , 1 , 16 , bitmap_h);
  u8g.drawBitmapP ( 0 , 36 , 2 , 16 , bitmap_w);
  u8g.drawBitmapP ( 16 , 36 , 2 , 16 , bitmap_du);
  u8g.drawBitmapP ( 32 , 36 , 1 , 16 , bitmap_t);
/*******************************************************************************/
  //OLED显示的温湿度数据和符号
  setFont_L;//设置字体大小（数字，字母或符号）
  u8g.setPrintPos(45, 20);
  u8g.print((float)DHT11.humidity);
  u8g.setPrintPos(80, 20);
  u8g.print("%");
  u8g.setPrintPos(45, 50);
  u8g.print((float)DHT11.temperature);
  u8g.setPrintPos(80, 50);
  u8g.print(char(176));//温度符号左上角的小o图标
  u8g.setPrintPos(86, 50);
  u8g.print("C");
}
