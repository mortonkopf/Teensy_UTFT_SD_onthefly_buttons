
//mon 8th Dec
// using UTFT_Demo_320x240 (C)2012 Henning Karlsen
#include <UTFT.h> //for getting the screen to function
#include <UTouch.h> //for using the touchscreen
#include <UTFT_Buttons.h>//for the screen buttons of course
#include <SdFat.h> //for using the sd card
#define TOUCH_ORIENTATION  LANDSCAPE


//===========for SD card wiring==========//
//MOSI goes to pin 11
//MISO goes to pin 12
//SCLK goes to pin 13
//SS goes to pin 10 

//===========for SD_Fat==================//
SdFile myFile;
#define SD_CS 10 // Set the chip select line to whatever you use
SdFat sd; // set filesystem
SdFile bmpFile; // set filesystem
SdFile fileX;
//===========for sd file button generation====//
#define LIST 64 //this is a fixed number of files to show
char filename[13];//8.3 filename?
int but[LIST]; //number of buttons to make on the fly
char* arrayinfo[LIST]; //place to store initial filenames
String storedNames[LIST];//place to store string of filename
uint16_t counter = 0;
uint16_t listNo =0;
//char charBuf[13];

//==========for bmp display from sd card========//
#define BUFFPIXEL 32

// For 8 bit mode, these connections are required
//  LCD    Teensy3
//  ---    -------
//  DB8       2
//  DB9      14
//  DB10      7
//  DB11      8
//  DB12      6
//  DB13     20
//  DB14     21
//  DB15      5

// For 16 bit mode, these connections are *also* required
//  LCD    Teensy3
//  ---    -------
//  DB0      16
//  DB1      17
//  DB2      19
//  DB3      18
//  DB4       0
//  DB5       1
//  DB6      32  (bottom side)
//  DB7      25  (bottom side)

// These 4 signals can connect to any unused digital pins
#define RS  23
#define WR  22
#define CS  3
#define RST 4
// Remember to change the model parameter to suit your display module!
UTFT myGLCD(SSD1289, RS, WR, CS, RST); 


// Touch Screen
int tchClk = 29;
int tchCS = 30;
int tchDIn = 28;
int tchIRQ = 27;
int tchDout = 26;
UTouch myTouch(tchClk, tchCS, tchDIn, tchDout, tchIRQ);

// Declare which bitmaps we will be using
extern unsigned short  aup[];
extern unsigned short adown[];

// add the buuton init
UTFT_Buttons  myButtons(&myGLCD, &myTouch);

uint16_t arrowState=0;   // current value of the arrow button counter
uint32_t cx, cy;
uint32_t rx[8], ry[8];
uint32_t clx, crx, cty, cby;
float px, py;
int dispx, dispy, centreY, centreX;
uint32_t calx, caly, cals;
// Declare which fonts we will be using
extern uint8_t SmallFont[];
String string;

void setup()
{
//=== Setup the LCD===//
  myGLCD.InitLCD();

//====setup the touchscreen====// 
  myTouch.InitTouch(TOUCH_ORIENTATION);
  dispx=myGLCD.getDisplayXSize();
  dispy=myGLCD.getDisplayYSize();
  centreY=(dispy/2);
  centreX=(dispx/2);
  myTouch.setPrecision(PREC_HI);
  myGLCD.clrScr();
     myGLCD.setFont(SmallFont);
    myButtons.setTextFont(SmallFont);
  delay(1000);
//====setup the sd card==//
  Serial.println("Initializing SD card...");
  if (!
  sd.begin(SD_CS, SPI_HALF_SPEED)) {
    Serial.println("failed!");
    return;
  }
  listFiles();

  
}  

void loop(){

drawButtons();

}
//=========show file names from sd card root==============//
 void listFiles(){

   String string;//immutable storage?
   char name[13];//buffer for button name transfer

  if (!
  sd.begin(SD_CS, SPI_HALF_SPEED)) {
  myGLCD.setColor(200,200,20);
  myGLCD.print("failed!",10,220);
    return;
  }
  myGLCD.print("loading filename ",10,220);
    sd.chdir("/",true);
    sd.vwd()->rewind();
    sd.vwd()->seekSet(0);
  while (fileX.openNext(sd.vwd(), O_READ)) {
    {
      //read all the file names into a storage array
        fileX.getFilename(filename);
        arrayinfo[counter]= filename;
        string = String(arrayinfo[counter]);
        storedNames[listNo]=string;//because strings are immutable?  
      Serial.println(storedNames[listNo]);
      listNo = listNo+1;  
        }
     fileX.close();}
     delay(100);
     myGLCD.print("filenames loaded",10,220);  
  }

////////////////////////
void frontPage(){
      myGLCD.clrScr();
      myButtons.deleteAllButtons();
      drawButtons();
    }
    
//=============================draw the butons===========//
void drawButtons()
{ 
  delay(50);//as a button tap debounce
  myButtons.deleteAllButtons();
  
    int but1, but2, but3, but4, but5, but6, pressed_button,upArrow,downArrow;
    int amount = 8;
    int gridX =125;
    int gridY =15;
    String stringOne;
    char name[13];
  ///////////////////////to bring while into the main page
    int butnumber;
 //////////////////   
  but1 = myButtons.addButton( 10,  10, 90,  30,"test but");//x ccord, y coord, width height
  but2 = myButtons.addButton( 10,  50, 90,  30, "Set time");
  but3 = myButtons.addButton( 10,  90, 40,  30, "loop");
  but4 = myButtons.addButton( 55,  90, 45,  30, "Once");
  but5 = myButtons.addButton( 10, 130, 90, 30, "Clear");
  but6 = myButtons.addButton( 10, 185, 90, 30, "Start");
  myButtons.setButtonColors((0xfff), 0xccc,(0xff),(0xff6),(0xf));//text, inactive, border, highlight, background
 
  myGLCD.setColor(0xfff);
  myGLCD.drawRect(120, 10, 310, 215); //this is the viewing area for the file list
  upArrow = myButtons.addButton( 260,  12, 48,  48, aup);
  downArrow = myButtons.addButton( 260,  165, 48,  48, adown);
   myButtons.drawButtons();
 
 //draw the file touch buttons
    for(int listL=0;listL<8;listL++){
            stringOne=storedNames[arrowState];
            Serial.print(stringOne + "    ");//check value
            Serial.println(arrowState);//check value
            but[arrowState]=  myButtons.addButton( gridX, gridY, 130, 18, " ");
            myButtons.setButtonColors((0xfff), (50,10,10),(0xff),0xff6,0xf);//text, inactive, border, highlight, background
            myButtons.drawButton(but[arrowState]);
            myGLCD.setBackColor(0xf);
            myGLCD.setColor(0xfff);
            myGLCD.print(stringOne,gridX+4,gridY+3);
            gridY =gridY+22; 
            arrowState = arrowState+1;
              }
  
  while(1) 
  {
    if (myTouch.dataAvailable() == true)
    {
      pressed_button = myButtons.checkButtons();

      if (pressed_button==but1){
        myGLCD.print("files available are..", 10, 220);
 }
      if (pressed_button==but2){
        myGLCD.print("Set time             ", 10, 220);
        secondPage();}
      if (pressed_button==but3)
        myGLCD.print("Loop the Image       ", 10, 220);
      if (pressed_button==but4)
        myGLCD.print("Image is one Hit     ", 10, 220);
      if (pressed_button==but5){
        myGLCD.print("Start a clear thing  ", 10, 220);
      arrowState =0;  
      frontPage();}
      if (pressed_button==but6)
        myGLCD.print("Start the countdown  ", 10, 220);   
      if (pressed_button==upArrow){
          if (arrowState>8){
          arrowState=arrowState-16;
          Serial.println(arrowState);
            drawButtons();}
          }
      if (pressed_button==downArrow){delay(50);//using a delay as a debounce effect for the touch button
          if(arrowState < LIST+8){
          arrowState=arrowState;
          Serial.println(arrowState);
            drawButtons();}
      }
            for(int i=0;i<LIST+1;i++){
            if (pressed_button==but[i]){
              storedNames[i+(arrowState-8)].toCharArray(name,13);//convert from string to char for use in the bmp         
      //is the selected file a .bmp file, if yes, show it, if not sy it isnt
      int BMP =storedNames[i+(arrowState-8)].indexOf('BMP');
        if(BMP>0) {
          bmpDraw_scale_to_fit(name); 
         fileX.close();
            }
        else { myGLCD.setColor(200,200,20);
          myGLCD.print("Not a BMP file        ", 10, 220);}
          }//end of button name attachment
        }//end of for LIST length  
 
    //    }
      }
    }
}

//==================second page==========//
void secondPage(){
    myGLCD.clrScr();
    myButtons.deleteAllButtons();
    int but1, pressed_button;
    but1 = myButtons.addButton( 10,  10, 140,  30,"this is a button");//x ccord, y coord, width height
  myButtons.setButtonColors(0xfff, (10,10,10),(0xff),(0xff6),(0xf));//text, inactive, border, highlight, background
  myButtons.drawButtons();
     myGLCD.print("Do something else here", 100, 150);
     while(1) 
  {
    if (myTouch.dataAvailable() == true)
    {
      pressed_button = myButtons.checkButtons();
      if (pressed_button==but1){
      myGLCD.clrScr();
      myButtons.deleteAllButtons();
        frontPage();}
    }
  }
}
  
//====================bmpDraw_scale_to_fit==============================//
void bmpDraw_scale_to_fit(char* filename){
  int but1,pressed_button;  
  int scaler=1;
  int x1, y1;
  
      myGLCD.clrScr();
      myButtons.deleteAllButtons();
        myGLCD.setColor(0,0,0);
          myGLCD.fillRect(0,0,318,224);

 //for full screen touch to return
 but1 = myButtons.addButton( 0, 0, myGLCD.getDisplayXSize()-1,myGLCD.getDisplayYSize()-1," ");//x ccord, y coord, width height
  myButtons.setButtonColors((0xfff), (50,10,10),(0xff),(0xff6),0xf);//text, inactive, border, highlight, background
  myButtons.drawButtons();

  /////////////
  SdFile   bmpFile;
  uint32_t bmpWidth, bmpHeight;   // W+H in pixels
  uint32_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel) //changed from 8 t 32
  uint32_t povbuffer[BUFFPIXEL];  // pixel out buffer - this needs to be 24bit per pixel////////
  uint32_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer // changed from 8 to 32
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  uint32_t w, h, row, col;
  uint32_t r, g, b;
  uint32_t pos = 0;
  uint32_t  povidx = 0;//changed from 8 to 32
  boolean  first = true;

//x1=+5; y1 =dispy-5; //default location to display image from

  // Open requested file on SD card
  bmpFile.open(filename, O_READ);
 progmemPrint(PSTR(filename));
  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    progmemPrint(PSTR("File size: ")); 
    Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    progmemPrint(PSTR("Image Offset: ")); 
    Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    progmemPrint(PSTR("Header size: ")); 
    Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
  
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      progmemPrint(PSTR("Bit Depth: "));
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed
        goodBmp = true; // Supported BMP format -- proceed!

//set the scale of the image
    if (bmpWidth>dispx) {scaler=2;}//half the sie of image - cheap way to fit the image
    if (bmpHeight>dispy) {scaler=2;}

        // BMP rows are padded (if needed) to 4-byte boundary
       rowSize = (bmpWidth * 3 + 3) & ~3;

//centalise the image on x axis (sort of)- make x=0, y =dispy, for bottom corner
    x1=centreX-((bmpWidth*scaler)/2);
    y1=dispy-5;


        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

     for (row=0; row<bmpHeight  /scaler; row++) {       
         if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - (row *scaler)) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + (row) * rowSize;
          if(bmpFile.curPosition() != pos) { // Need seek?
            bmpFile.seekSet(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<bmpWidth; col++) { // For each column...
            // read more pixel data
           if (buffidx >= sizeof(sdbuffer)) { 
             povidx = 0;         
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
           }

            // Convert pixel from BMP to rgb format, push to display
            r = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            b = sdbuffer[buffidx++];
          
          myGLCD.setColor(b,r,g);
       //   myGLCD.drawPixel(y1-col, x1+row/scaler);
    myGLCD.drawPixel(x1+row, y1-col /scaler);
    }

        }
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println("BMP format not recognized.");
  bmpFile.seekSet(pos);
  
   //draw the image first and the button(above) then wait to listen for the button 
  while(1) 
  {
    if (myTouch.dataAvailable() == true)
    {
      pressed_button = myButtons.checkButtons();

      if (pressed_button==but1){
        arrowState=0;
      myGLCD.clrScr();
      myButtons.deleteAllButtons();
       drawButtons();
        }
      }
    }
}


//------- HELPER FUNCTION------//
// Create a 24 bit color value from R,G,B ////these strips are BRG, but you might need to change this
unsigned int Color(byte b/*r*/, byte g, byte /*b*/r)
{
  //Take the lowest 8 bits of each value and append them end to end
return( ((unsigned int)g & 0x1F )<<16 | ((unsigned int)r & 0x1F)<<8 | (unsigned int)b & 0x1F);
  }
//end of helper function

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(SdFile& f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(SdFile& f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

// Copy string from flash to serial port
// Source string MUST be inside a PSTR() declaration!
void progmemPrint(const char *str) {
  char c;
  while(c = pgm_read_byte(str++)) Serial.print(c);
}

// Same as above, with trailing newline
void progmemPrintln(const char *str) {
  progmemPrint(str);
  Serial.println();
}

