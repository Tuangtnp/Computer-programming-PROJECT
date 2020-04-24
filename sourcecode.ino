#include "PMS.h" 
#include <Wire.h> 
// ใช้เพื่อการสื้อสารกับ I2C
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // เป็นการ set address ของ LCD I2c 0x27 เป็นค่าเริ่มต้น 16 colum 2 row 
PMS pms(Serial); //ประกาศตัว PMS ของ library PMS
PMS::DATA data; //ประกาศตัวแปล data ของ library PMS

int red = A0; //ประกาศตัวแปล ให้ red = analog0
int green = A1; //ประกาศตัวแปล ให้ green = analog1
int blue = A2; //ประกาศตัวแปล ให้ blue = analog2

/*ที่ใช้ analog ก็เพราะว่าจะสามารถลงแสงได้ 0-1023 และ ผสมสีค่อนข้างง่าย
  LED rgb ที่ใช้จะมี common เป็น cathode ต้องต่อลง GND
  ถ้าใช้ digitalWrite(red, LOW); คือดับ
  ถ้าใช้ digitalWrite(red, HIGH); คือติดสูงสุด
*/

void goodLevelLight() { //Green light
  digitalWrite(red, LOW);
  analogWrite(green, 512);
  digitalWrite(blue, LOW);
}

void moderateLevelLight() { //Yellow light
  analogWrite(red, 512);
  analogWrite(green, 200);
  digitalWrite(blue, LOW);
}

void unhealthyLevelLight() { //Orange light
  analogWrite(red, 512);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
}

void hazardousLevelLight() { //Pueple light
  analogWrite(red, 512);
  digitalWrite(green, LOW);
  analogWrite(blue, 512);
}

/*เนื่องจากในแต่ละ 1 colum จะมีช่องสี่เหลี่ยม 5 colum 8 row เราจะสามารถทำ ทำเป็นสัญญาลักษณ์ต่างๆได้
  ตามนี้ ด้วยการเปลี่ยนเลขให้เป็น 1 จะทำให้ช่องไฟติด เช่นถ้าอยากได้รูปหัวใจ
  byte heart[] = {
  B00000,
  B00000, 
  B01010,  # #
  B10101, # # #
  B10001, #   #
  B01010,  # #
  B00100,   #
  B00000
};*/

byte sad[] = { // ทำเป็นรูปหน้าเศร้า
  B00000,     
  B00000,     
  B01010,     // # #
  B00000,
  B01110,     // ###
  B10001,     //#   #
  B10001,     //#   #
  B00000
};

byte smile[] = { //ทำเป็นรูป ยิ้ม
  B00000,
  B00000,
  B01010,     // # #
  B00000,   
  B10001,     //#   #
  B10001,     //#   #
  B01110,     // ###
  B00000
};

byte expressionless[] = {// ทำเป็นรูป หน้านิ่ง
  B00000,    
  B00000, 
  B01010,    // # # 
  B00000,    
  B00000,
  B11111,    //#####
  B00000,
  B00000
};

byte gasmask[] = { //ทำเป็นรูป gasmask 
  B01110,     //  #####
  B11111,     //#########
  B10101,     //## ### ##
  B11111,     //#########
  B01110,     //  #####
  B11011,     //#### ####
  B11011,     //#### ####
  B00000
};

void setup()
{
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  lcd.begin();
  /* ถ้าเอา comment ข้างล่างออกจะปิด blacklight และไฟข้างแต่หน้าจอจะมืดมาก 
    หรือจะเอาตัวปิดที่ข้างหลัง I2C ออกก็จะเป็นการปิดเหมือนกัน*/
  //lcd.noBacklight();
  Serial.begin(9600);
  lcd.setCursor(3,0); // ตั้งตำแหน่งไว้ colum ที่ 3 row 1
  lcd.print("Boosting...");
  delay(1000);
}

void loop()
{
  //ถ้าอ่านค่าได้ให้ทำตามคำสั่งต่อไป
  if (pms.read(data)){
    //SYNTEX lcd.createChar(num, data)
    //สร้างตัว custom character เก็บไว้ใน 0,1,2,3
    //ถ้าอยากให้แสดงจะใช้คำสั่ง lcd.write(num) แล้วใส่ตัวเลขลงไป
    lcd.createChar(0, smile); 
    lcd.createChar(1, expressionless);
    lcd.createChar(2, sad);
    lcd.createChar(3, gasmask);

    // ใช้เพื่อดูค่าใน Serial moniter
    Serial.print("PM 2.5 (ug/m3): "); 
    Serial.println(data.PM_AE_UG_2_5);

    // ถ้าค่าของ PM2.5 น้อยกว่าหรือเท่ากับ 50
    if (data.PM_AE_UG_2_5 <= 50) {
      // ทำการ clear LCD เพื่อไม่ให้มีตัวหนังสือตกค้างเวลาค่าเปลี่ยน
      lcd.clear();
      lcd.setCursor(0, 0); // ตั้งตำแหน่งไว้ที่ colum 0 row 0
      /*ใน library นี้ ถ้าอยากแสดง ค่า PM2.5 ใช้print(data.PM_AE_UG_2_5);
        ถ้าอยากแสดง PM10 data.PM_AE_UG_1_0*
        ถ้าอยากแสดง PM10 data.PM_AE_UG_10_0*/
      lcd.print("PM2.5 :" + String(data.PM_AE_UG_2_5) + " ug/m3");
      //print ค่า PM2.5 ออกมา ตัวอย่าง PM2.5 :45 ug/m3
      lcd.setCursor(0,1); // ตั้งตำแหน่งไว้ที่ colum 0 row 1
      lcd.print("Air:  "); 
      lcd.write(0);
      lcd.print(" Good ");
      lcd.write(0); //print custom character smile
      //ตัวอย่าง Air:  🙂 Good 🙂
      goodLevelLight(); //เรียกใช้ function แสดงไฟสีเขียว
      delay(1000); //delay 1 วิ
    }
    // ถ้าค่าของ PM2.5 อยู่ระหว่าง 51 ถึง 100
    else if (data.PM_AE_UG_2_5 > 50 && data.PM_AE_UG_2_5 <= 100) {
      lcd.clear();  
      lcd.setCursor(0, 0);
      lcd.print("PM2.5 :" + String(data.PM_AE_UG_2_5) + " ug/m3");
      //ตัวอย่าง PM2.5 :85 ug/m3
      lcd.setCursor(0,1);
      lcd.print("Air: Moderate ");
      lcd.write(1);
      //ตัวอย่าง Air: Moderate  😐
      moderateLevelLight(); //เรียกใช้ function แสดงไฟเหลือง
      delay(1000);
     }
    // ถ้าค่าของ PM2.5 อยู่ระหว่าง 101 ถึง 300 
    else if (data.PM_AE_UG_2_5 >= 101 && data.PM_AE_UG_2_5 <= 300) {
      lcd.clear();
      lcd.setCursor(0, 0);
     lcd.print("PM2.5 :" + String(data.PM_AE_UG_2_5) + " ug/m3");
      //ตัวอย่าง PM2.5 :125 ug/m3
      lcd.setCursor(0,1);
      lcd.print("Air: Unhealthy ");
      lcd.write(2);
      //ตัวอย่าง Air: Unhealthy  ☹
      unhealthyLevelLight(); //เรียกใช้ function แสดงไฟแดง
      delay(1000); 
    }
    // ถ้าค่าของ PM2.5 มากว่า 301
    else if (data.PM_AE_UG_2_5 >= 301) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("PM2.5 :" + String(data.PM_AE_UG_2_5) + " ug/m3");
      //ตัวอย่าง PM2.5 :600 ug/m3
      lcd.setCursor(0,1);
      lcd.print("Air: Hazardous ");
      lcd.write(3);
      //ตัวอย่าง Air: Hazardous  (gasmask character)
      hazardousLevelLight(); //เรียกใช้ function แสดงไฟม่วง
      delay(1000);
    }
  } 
}
