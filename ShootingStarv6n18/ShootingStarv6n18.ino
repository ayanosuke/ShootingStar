//--------------------------------------------------------------------------------
// 九州新幹線　流れ星
// 筑後船小屋駅手前
//    Copyright (c) 2021 Ayanosuke(Maison de DCC)
//    
// 動画の音声を参考にさせていただきました
// https://www.youtube.com/watch?v=20kXB_WHqsc&t=310s
// ～タイムライン～
// 0:23​〜 流れ星新幹線 筑後船小屋到着
// 2:00〜 プロローグ
// 3:45​〜 夜に駆ける/YOASOBI
// 5:42​〜 アイノカタチfeat.HIDE(GReeeeN)/MISIA
// 10:00〜 天体観測/BUMP OF CHICKEN
// 12:00​〜 フィナーレ
// http://maison-dcc.sblo.jp/ http://dcc.client.jp/ http://ayabu.blog.shinobi.jp/
// https://twitter.com/masashi_214
//
// DCC電子工作連合のメンバーです
// https://desktopstation.net/tmi/ https://desktopstation.net/bb/index.php
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//--------------------------------------------------------------------------------

#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
#include "Adafruit_NeoPixel.h"
#include <IRremote.h>
#include "IrCmd_List.h"
#define MAXLED 44
#define LedMaxLevel 50

int ledch;
int RECV_PIN =11;
IRrecv irrecv(RECV_PIN);
decode_results results;


//Task Schedule
unsigned long gPrevious = 0;

unsigned long WAIT_TIMER;
unsigned long SCENE_TIMER;
int volume = 10;

typedef struct {
  char ch;
  char state;
  int wait;
  int nowwait;
  char bright;
  char rled;
  char gled;
  char bled;
  char pos;
} ws2812c;
                          //0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
//unsigned char leaf_tbl[24]={0,0,1,1,2,3,4,5,7,8, 9,10,10, 9, 8, 7, 5, 4, 3, 2, 1, 1, 0, 0}; // なんとなく正規分布
                          //0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21
unsigned char leaf_tbl[24]={0,1,1,2,3,4,5,7,8,9,10,10, 9, 8, 7, 5, 4, 3, 2, 1, 1, 0}; // なんとなく正規分布
                          //0 1 2 3  4 5  6  7 ,8
unsigned char leaf_tbl2[9]={0,2,5,9,10,9, 5, 2,  0}; // なんとなく正規分布


char rbuf[48];
char gbuf[48];
char bbuf[48];

unsigned long Timer1 = 0;
unsigned long Timer2 = 0;

ws2812c ch[MAXLED+1];
Adafruit_NeoPixel pixels(MAXLED, 3, NEO_GRB + NEO_KHZ800);

SoftwareSerial mySoftwareSerial(0, 1); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

void setup() {
//  Serial.begin(115200);

  // ソフトウエアシリアル通信レートセット:
  mySoftwareSerial.begin(9600);  
  myDFPlayer.begin(mySoftwareSerial);
  myDFPlayer.volume(volume);  //Set volume value. From 0 to 30
  
//  // Serial.println("setup()");   
  pixels.begin();
  
//  pinMode(13,OUTPUT);
  
  for(ledch = 0 ; ledch <= MAXLED ; ledch ++){
    ch[ledch].ch = 0;
    ch[ledch].state = 0;
    ch[ledch].wait = ledch*8;
    ch[ledch].nowwait = ch[ledch].wait;
    ch[ledch].bright = 0;
    ch[ledch].rled = 0;
    ch[ledch].gled = 0;
    ch[ledch].bled = 0;
  }
  show();

  //Reset task
  gPrevious = millis();
  
//  // Serial.println("loop()");   
  irrecv.enableIRIn();
}


void loop() {
  static int state = 2;//300;
  static unsigned long SceneTimer = 0;

  if(irrecv.decode(&results)){
    irrecv.resume(); // Receive the next value
//Serial.println(results.value,HEX);  
    switch(results.value){
      case IRCMD_NEC_FUNC1:
      case IRCMD_SONYTV_FUNC1:
      case IRCMD_SONYMD_FUNC1:
                              rinit();
                              show();
                              myDFPlayer.pause();
                              //myDFPlayer.stopAdvertise(); 
                              state = 200;  // 消灯
                              break;
      case IRCMD_NEC_FUNC2: 
      case IRCMD_SONYTV_FUNC2:
      case IRCMD_SONYMD_FUNC2:
                              myDFPlayer.pause();
                              state = 0;
                              break;
      case IRCMD_NEC_FUNC3:
      case IRCMD_SONYTV_FUNC3:
      case IRCMD_SONYMD_FUNC3:
                              myDFPlayer.pause();
                              state = 2;
                              break;  
      case IRCMD_NEC_FUNC4:
      case IRCMD_SONYTV_FUNC4:
      case IRCMD_SONYMD_FUNC4:
                              myDFPlayer.pause();
                              state = 16;
                              break;  
      case IRCMD_NEC_FUNC5:
      case IRCMD_SONYTV_FUNC5:
      case IRCMD_SONYMD_FUNC5:
                              myDFPlayer.pause();
                              state = 34;
                              break;
      case IRCMD_NEC_FUNC6:
      case IRCMD_SONYTV_FUNC6:
      case IRCMD_SONYMD_FUNC6:
                              myDFPlayer.pause();
                              state = 44;
                              break;

      case IRCMD_NEC_FUNC7:
      case IRCMD_SONYTV_FUNC7:
      case IRCMD_SONYMD_FUNC7:
                              myDFPlayer.pause();
                              state = 60;
                              break;


      case IRCMD_NEC_SPEEDUP:
//      case IRCMD_SONYTV_SPEEDUP:  //  error: previously used here
      case IRCMD_SONYMD_SPEEDUP:
                               volume ++;
                               if(volume >=20)
                                volume = 20;
                               myDFPlayer.volume(volume);
                               break;
      case IRCMD_NEC_SPEEDDOWN:
//    case IRCMD_SONYTV_SPEEDDOWN:  // error: previously used here
      case IRCMD_SONYMD_SPEEDDOWN:
                               volume --;
                               if(volume < 0)
                                volume = 0;
                               myDFPlayer.volume(volume);
                               break;
    
#if 0
      case IRCMD_NEC_SPEEDUP: 
      case IRCMD_SONYTV_SPEEDUP:
      case IRCMD_SONYMD_SPEEDUP:
                              state = 100;
                              break;
      case IRCMD_NEC_SPEEDDOWN:
      case IRCMD_SONYTV_SPEEDDOWN:
      case IRCMD_SONYMD_SPEEDDOWN:
                              state = 101;
                              break;
      case IRCMD_NEC_FUNC9:
      case IRCMD_SONYTV_FUNC9:
      case IRCMD_SONYMD_FUNC9:
                              state = 102;
                              break;
#endif
      defautl:
              break;
    
    }
  }
 
  switch(state){
    case 0: // 流れる虹LED初期設定
            WAIT_TIMER = 20;
            SCENE_TIMER = 0;
            myDFPlayer.loop(1);
            scene1(1);
            state = 1;
            break;

    case 1: // 流れる虹LED実行
            if(scene1(0) == 1){
            }
            break;

            //--------------------------------------------------
            // 2:00​- プロローグ
            //--------------------------------------------------            
    case 2: // 全LEDフェードアウト初期設定
            WAIT_TIMER = 50;
            SCENE_TIMER = 5000;
            scene2(1);
            state = 3;
            break;
    case 3: // 全LEDフェードアウト実行
            if(scene2(0) == 1){
              state = 4;
            }
            break;

            // Prologue：プロローグ
    case 4: // RED LEAF ROTETO初期設定 4.1回くらい回る設定
            WAIT_TIMER = 90;
            SCENE_TIMER = 14000;

            myDFPlayer.play(2); // 2:00​- プロローグ   
            scene3(1);
            state = 5;
            break;
    case 5: // RED LEAF ROTETO実行
            if(scene3(0) == 1){
              state = 6;
            }
            break;

    case 6: // 2:14 WAIT LAZER
            WAIT_TIMER = 80;
            SCENE_TIMER = 12000;
            scene3_5(1);
            state = 7;
            break;
    case 7: // WHITE LAZER実行
            if(scene3_5(0) == 1){
              state = 8;
            }
            break;

    case 8: // 2:28 赤点滅 + WAIT LAZER
            WAIT_TIMER = 20;
            SCENE_TIMER = 15000;
            scene3_6(1);
            state = 9;
            break;
    case 9: // 赤点滅 + WHITE LAZER実行
            if(scene3_6(0) == 1){
              state = 10;
            }
            break;

    case 10: // 2:42 BLUERED LEAF ROTETO初期設定
            WAIT_TIMER = 80;
            SCENE_TIMER = 28000;
            scene4(1);
            state = 11;
            break;
    case 11: // RED LEAF ROTETO実行
            if(scene4(0) == 1){
              state = 12;
            }
            break;
            
    case 12: // 3:10 白点灯赤青全体交互初期設定
            WAIT_TIMER = 20;
            SCENE_TIMER = 40000;
            scene5(1);
            state = 13;
            break;
    case 13: //白点灯赤青全体交互実行 
            if(scene5(0) == 1){
              state = 16;
            }
            break;

#if 0 // scene5()に入っている
    case 14: // 3:40 赤点滅
            WAIT_TIMER = 20;
            SCENE_TIMER = 5000;
            state = 15;
            break;
    case 15: // 3:40 赤点滅

            if(scene6() == 1){
              state = 16;
            }
            break;
#endif

            //--------------------------------------------------
            // 3:45​- ♪夜に駆ける/YOASOBI
            //--------------------------------------------------   
    case 16: // 3:45​- ♪夜に駆ける/YOASOBI

            rinit();
            show();myDFPlayer.play(3); 
            SCENE_TIMER = 8000;
            scene7_0(1);
            state = 17;
            break;
    case 17:
             if(scene7_0(0) == 1){ 
                state = 18;
             }    
             break;

    case 18: // 3:53 ピンク点滅初期設
            WAIT_TIMER = 200;
            SCENE_TIMER = 10000;
            scene7(1);
            state = 19;
            break;
    case 19: // 3:53 ピンク点滅実行
            if(scene7(0) == 1){
              state = 20;
            }
            break;
    
    case 20: // 3:59 ピンク、青、中央からLeafbuf
            WAIT_TIMER = 100;
            SCENE_TIMER = 13000;
            scene14(1);
            state = 21;
            break;
    case 21: // 3:59 ピンク、青、Leafbuf実行
            if(scene14(0) == 1){
              state = 22;
            }
            break;   
 
    
    case 22: // 4:17 ピンク、白　全体交互
            WAIT_TIMER = 100;
            SCENE_TIMER = 14000;
            scene9(1);
            state = 23;
            break;
    case 23: // 4:17 ピンク、白　全体交互実行
            if(scene9(0) == 1){
              state = 24;
            }
            break;   

    case 24: // 4:30 紫が細かく流れる
            WAIT_TIMER = 100;
            SCENE_TIMER = 15000;
            scene10(1);
            state = 25;
            break;
    case 25: // 4:30 紫が細かく流れる
            if(scene10(0) == 1){
              state = 26;
            }
            break;   

    case 26: // 4:45 紫が中央から左右に流れる
            WAIT_TIMER = 100;
            SCENE_TIMER = 14000;
            state = 27;
            break;
    case 27: // 4:45 紫が中央から左右に流れる
            if(scene11() == 1){
              state = 28;
            }
            break;  
    
    case 28: // 4:58 青全体点滅→フェードアウト消灯
            WAIT_TIMER = 1;
            SCENE_TIMER = 10000;
            state++;
            break;
    case 29: // 4:58 青全体点滅→フェードアウト消灯
            if(scene12() == 1){
              state++;
            }
            break;  

    case 30: //5:00 白全点灯、中央からピンク、緑、LEAF＿回転
            WAIT_TIMER = 100;
            SCENE_TIMER = 29000;
            scene13(1);
            state++;
            break;  
    case 31: //5:00 白全点灯、中央からピンク、緑、LEAF＿回転
            if(scene13(0) == 1){
              state ++;
            }
            break;  
    

    case 32: // 5:28 中央からピンク、青、LEAF＿回転
            WAIT_TIMER = 80;
            SCENE_TIMER = 14000;
            scene14(1);
            state ++;
            break;
    case 33: // 5:28 中央からピンク、青、LEAF＿回転
            if(scene14(0) == 1){
              state ++;
            }
            break;  

            //--------------------------------------------------
            // 5:42​- ♪アイノカタチfeat.HIDE(GReeeeN)/MISIA
            //--------------------------------------------------   
    case 34: // 5:42 中央から先は赤、徐々に黄色？LEAF＿回転
            myDFPlayer.play(4);  
            WAIT_TIMER = 200;
            SCENE_TIMER = 55000;
            scene15(1);
            state ++;
            break;
    case 35: // 5:42 中央から先は赤、徐々に黄色？LEAF＿回転
            if(scene15(0) == 1){
              state ++;
            }
            break;  


    case 36:  // 6:39 全体黄色点灯
            WAIT_TIMER = 80;
            SCENE_TIMER = 22000;
            state ++;
            break;
    case 37: // 6:39 全体黄色点灯
            if(scene16() == 1){
              state ++;
            }
            break;  

    case 38:  // 7:13 全体紅葉色、中央から、赤・もみじ色で流れる消えないように
            WAIT_TIMER = 200;
            SCENE_TIMER = 65000;
            scene15(1);
            state ++;
            break;
    case 39: // 7:13 全体紅葉色、中央から、赤・もみじ色で流れる
            if(scene15(0) == 1){
              state ++;
            }
            break;  

    case 40: // 青、中央からLeafbuf
            WAIT_TIMER = 100;
            SCENE_TIMER = 25000;
            scene17(1);
            state ++; 
            break;
    case 41: // 青、Leafbuf実行
            if(scene17(0) == 1){
              state++;
            }
            break;              

    case 42:  // 8:50 全体紫、赤、緑・赤
            WAIT_TIMER = 80;
            SCENE_TIMER = 65000;
            state ++;
            break;
    case 43: // 8:50 全体紫、赤、緑・赤
            if(scene19() == 1){
              state ++;
            }
            break;
              
            //--------------------------------------------------
            // 10:00​- ♪天体観測/BUMP OF CHICKEN
            //--------------------------------------------------   
    case 44:  //9:44 全体黄色、白。花火、消灯
            myDFPlayer.play(5);  
            WAIT_TIMER = 80;
            SCENE_TIMER = 27000;
            scene20(1);
            state ++;
            break;
    case 45: // 9:44 全体黄色、白。花火、消灯
            if(scene20(0) == 1){
              state ++;
            }
            break;  

    case 46:  // 10:27 全体青、しばらくしたら、中央から青・白で流れる。
            WAIT_TIMER = 200;
            SCENE_TIMER = 30000;
            state ++;
            break;
    case 47: // 10:27 全体青、しばらくしたら、中央から青・白で流れる。
            if(scene21() == 1){
              state ++;
            }
            break;  

    case 48:  // 10:50 中央から青で流れて、白の点も流れる
            WAIT_TIMER = 200;
            SCENE_TIMER = 30000;
            scene22(1); 
            state ++;
            break;
    case 49: // 10:50 中央から青で流れて、白の点も流れる
            if(scene22(0) == 1){
              state ++;
            }
            break;  

    case 50:  // 11:03 中央から赤・黄で流れる。
            WAIT_TIMER = 200;
            SCENE_TIMER = 20000;
            scene15(1);
            state ++;
            break;
    case 51: // 11:03 中央から赤・黄で流れる。
            if(scene15(0) == 1){
              state ++;
            }
            break;  
            

    case 52:  // 11:25 全体赤点滅。全体七色変化点灯
            WAIT_TIMER = 200;
            SCENE_TIMER = 30000;
            state ++;
            break;
    case 53: // 11:25 全体赤点滅。全体七色変化点灯
            if(scene24() == 1){
              state++;
            }
            break;  



    case 54:  // 11:50 全体七色点灯、中央から流れる　列車移動開始
            WAIT_TIMER = 200;
            SCENE_TIMER = 15000;
            state ++;
           break;
    case 55: /// 11:50 全体七色点灯、中央から流れる　列車移動開始
            if(scene25() == 1){
              state = 200;
            }
            break;  

    case 60: // 流れる虹LED初期設定
            WAIT_TIMER = 20;
            SCENE_TIMER = 0;
            scene1(1);
            state = 61;
            break;

    case 61: // 流れる虹LED実行
            if(scene1(0) == 1){
            }
            break;



#if 0
    case 100: // 雫
            if(scene30() == 1){
              state = 200;
            }
            break;  
    case 101: // たまる雫
            if(scene31() == 1){
              state = 200;
            }
            break;  
    case 102: // ボール物理法則
            if(scene32() == 1){
              state = 200;
            }
            break;  


 
    case 300://
          rinit();
          show();
          break;
#endif  
    default:
            break;
  }
}

// --------------------------------------------------------------------------------
// 流れる虹LED
// --------------------------------------------------------------------------------
int scene1( int startinit ){
  enum{
    ST_INIT = 0,
    ST_ROTATE_RAINBOW,
    ST_WAIT,
    ST_END,
  };

  static char state = ST_INIT;

  if(startinit  == 1){
    state = ST_INIT;
  }
    
  switch(state){
    case ST_INIT:
            // Serial.println("ST_ROTE");  
            rinit();
            for(ledch = 0 ; ledch <= MAXLED ; ledch ++){
              ch[ledch].ch = 0;
              ch[ledch].state = 0;
              ch[ledch].wait = ledch*8;
              ch[ledch].nowwait = ch[ledch].wait;
              ch[ledch].bright = 0;
              ch[ledch].rled = 0;
              ch[ledch].gled = 0;
              ch[ledch].bled = 0;
            }
            show();
            Timer1 = millis();
            state = ST_ROTATE_RAINBOW;

            break;         
 
    case ST_ROTATE_RAINBOW:
            for(ledch = 0 ; ledch <= MAXLED-1 ; ledch++){ 
              chCont(ledch);
              pixels.setPixelColor(ledch, pixels.Color(ch[ledch].rled, ch[ledch].gled, ch[ledch].bled));
            }
            pixels.show();
            Timer2 = millis();
            state = ST_WAIT;
            break;

    case ST_WAIT:
            if(SCENE_TIMER != 0 ){
              if ( (millis() - Timer1) >= SCENE_TIMER){
                state = ST_END;
                break; 
              }
            }
            if ( (millis() - Timer2) >= WAIT_TIMER){
              state = ST_ROTATE_RAINBOW; 
            }

            break;
    case ST_END:
            return 1;
            break;

    default:
            break;
  }
  return 0;
}

// --------------------------------------------------------------------------------
// 全LEDフェードアウト処理
// --------------------------------------------------------------------------------
int scene2( int initstate ){
  enum{
    ST_INIT = 0,
    ST_FADEOUT,
    ST_WAIT,
    ST_END,
  };

  static char state = ST_INIT;

  if(initstate == 1){
    state = ST_INIT;
  }
    
  switch(state){
    case ST_INIT:
//            rinit();
            Timer1 = millis();
            Timer2 = Timer1;
            state = ST_FADEOUT;
// Serial.println("ST_INIT");  
            break;
            
    case ST_FADEOUT:
            for(ledch = 0 ; ledch <= MAXLED-1 ; ledch++){   
              ch[ledch].rled--;
              if(ch[ledch].rled<=0)
                ch[ledch].rled=0;
      
              ch[ledch].gled--;
              if(ch[ledch].gled<=0)
                ch[ledch].gled=0;
    
              ch[ledch].bled--;
              if(ch[ledch].bled<=0)
                ch[ledch].bled=0;

              pixels.setPixelColor(ledch, pixels.Color(ch[ledch].rled, ch[ledch].gled, ch[ledch].bled));
            }
            pixels.show();
            state = ST_WAIT;
            break;
            
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_FADEOUT;
            } 

            break;
    case ST_END:
            return 1;
            break;

    default:
            break;
  }
  return 0;
}

// --------------------------------------------------------------------------------
// 赤leaf回転
// --------------------------------------------------------------------------------
int scene3( int initstate ){
  enum{
    ST_INIT = 0,
    ST_REDLEAFROTE,
    ST_WAIT,
    ST_END,
  };

  static char state = ST_INIT;

  if(initstate == 1){
    state = ST_INIT;
  }
    
  switch(state){
    case ST_INIT:
            Timer1 = millis();
            Timer2 = Timer1;
            state = ST_REDLEAFROTE;
// Serial.println("ST_INIT");  
            break;
    
    case ST_REDLEAFROTE:
            chCont3();
            for(ledch = 0 ; ledch <= MAXLED-1 ; ledch++){   
              pixels.setPixelColor(ledch, pixels.Color(ch[ledch].rled, ch[ledch].gled, ch[ledch].bled));
            }
            pixels.show();
            state = ST_WAIT;
            break;
            
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_REDLEAFROTE;
            } 

            break;
    case ST_END:
            return 1;
            break;

    default:
            break;
  }
  return 0;
}


// --------------------------------------------------------------------------------
// 白レーザー
// --------------------------------------------------------------------------------
int scene3_5( int initstate){
  enum{
    ST_INIT = 0,
    ST_RUN,
    ST_WAIT,
    ST_OFF,
    ST_OFFWAIT,
    ST_END,
  };

  static char state = ST_INIT;
  static char oncnt = 0;
  static char offcnt = 0;
  int i;

  if(initstate == 1){
    state = ST_INIT;
  }
  switch(state){
    case ST_INIT:
            Timer1 = millis();
            Timer2 = Timer1;
            ledch = 0;
            oncnt = 0;
            offcnt = 0;
            rinit();
            state = ST_RUN;
// Serial.println("ST_INIT");  
            break;
    
    case ST_RUN:
            ch[1].rled = LedMaxLevel;
            ch[1].gled = LedMaxLevel;
            ch[1].bled = LedMaxLevel;
            show();
            for(i=21;i>=2;i--){     // 変数ローテート
              ch[i*2-1]=ch[i*2-3];
//              // Serial.println(i);  
            }
            oncnt ++;
            if(oncnt >= 20){
              oncnt = 0;
              state = ST_OFF;
            } else {
              state = ST_WAIT;
            }
            break;
            
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_RUN;
            } 

            break;
    case ST_OFF:
            ch[1].rled = 0;
            ch[1].gled = 0;
            ch[1].bled = 0;
            show();
            for(i=21;i>=2;i--){     // 変数ローテート
              ch[i*2-1]=ch[i*2-3];
            }
            offcnt ++;
            if(offcnt >= 40){
              offcnt = 0;
              Timer2 = millis();
              state = ST_RUN;
            } else {
              Timer2 = millis();
              state = ST_OFFWAIT;
            }
            break;
    case ST_OFFWAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_OFF;
            } 

            break;          
    case ST_END:
            return 1;
            break;

    default:
            break;
  }
  return 0;
}


// --------------------------------------------------------------------------------
// 2:28 赤点滅 + 白レーザー
// --------------------------------------------------------------------------------
int scene3_6( int initstate ){
  enum{
    ST_INIT = 0,
    ST_RUN,
    ST_WAIT,
    ST_OFF,
    ST_OFFWAIT,
    ST_END,
  };

  static char state = ST_INIT;
  static char oncnt = 0;
  static char offcnt = 0;
  int i;
  static char pos[6];
  static char laserpos[44];
  static char wa = 0;

  if(initstate == 1){
    state = ST_INIT;
  }
  
  switch(state){
    case ST_INIT:
            Timer1 = millis();
            Timer2 = Timer1;
            ledch = 0;
            oncnt = 0;
            offcnt = 0;
            rinit();
            state = ST_RUN;
// Serial.println("ST_INIT");  
            break;
    
    case ST_RUN:
            for(i=0;i<=5;i++){
              pos[i]=(char)random(2);
                for(ledch = i*8 ; ledch <= i*8+7 ; ledch++){   
                  ch[ledch].rled = pos[i] ? LedMaxLevel/4 :0;
                  ch[ledch].gled = 0;
                  ch[ledch].bled = 0;
                }
            }
            laserpos[0] = LedMaxLevel;

            wa++;
            if(wa>=4){
              for(i=21;i>=1;i--){     // 変数ローテート
                ch[i*2].rled = ch[i*2].gled = ch[i*2].bled = laserpos[i];  
                laserpos[i]=laserpos[i-1];
              }
            wa=0;
            } else {
                for(i=21;i>=1;i--){     // 変数ローテート
                  ch[i*2].rled = ch[i*2].gled = ch[i*2].bled = laserpos[i];
              }
            }
            show();
            
            oncnt ++;
            if(oncnt >= 100){
              oncnt = 0;
              wa=0;
              state = ST_OFF;
            } else {
              state = ST_WAIT;
            }
            break;
            
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_RUN;
            } 

            break;
    case ST_OFF:
            for(i=0;i<=5;i++){
              pos[i]=(char)random(2);
                for(ledch = i*8 ; ledch <= i*8+7 ; ledch++){   
                  ch[ledch].rled = pos[i] ? LedMaxLevel/2 :0;
                  ch[ledch].gled = 0;
                  ch[ledch].bled = 0;
                }
            }
            laserpos[0] = 0;
            wa++;
            if(wa>=4){
              for(i=21;i>=1;i--){     // 変数ローテート
                ch[i*2].rled = ch[i*2].gled = ch[i*2].bled = laserpos[i];  
                laserpos[i]=laserpos[i-1];
              }
              wa=0;
            }else{
              for(i=21;i>=1;i--){     // 変数ローテート
                ch[i*2].rled = ch[i*2].gled = ch[i*2].bled = laserpos[i];  
              }
            }
            show();
            offcnt ++;
            if(offcnt >= 100){
              offcnt = 0;
              Timer2 = millis();
              state = ST_RUN;
            } else {
              Timer2 = millis();
              state = ST_OFFWAIT;
            }
            break;
    case ST_OFFWAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_OFF;
            } 

            break;          
    case ST_END:
            return 1;
            break;

    default:
            break;
  }
  return 0;
}







// --------------------------------------------------------------------------------
// 2:42 赤青leaf回転
// --------------------------------------------------------------------------------
int scene4( int initstate ){
  enum{
    ST_INIT = 0,
    ST_REDBLUELEAFROTE,
    ST_WAIT,
    ST_END,
  };

  static char state = ST_INIT;
  if(initstate == 1){
    state = ST_INIT;  
  }
  switch(state){
    case ST_INIT:
            Timer1 = millis();
            Timer2 = Timer1;
            state = ST_REDBLUELEAFROTE;
// Serial.println("ST_INIT");  
            break;
    
    case ST_REDBLUELEAFROTE:
            chCont4();
            show();
            state = ST_WAIT;
            break;
            
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_REDBLUELEAFROTE;
            } 

            break;
    case ST_END:
            return 1;
            break;

    default:
            break;
  }
  return 0;
}



void show(void){
  for(ledch = 0 ; ledch <= MAXLED-1 ; ledch++){   
    pixels.setPixelColor(ledch, pixels.Color(ch[ledch].rled, ch[ledch].gled, ch[ledch].bled));
   }
   pixels.show();  
}



// --------------------------------------------------------------------------------
// 3:10 白点灯赤青全体交互
// --------------------------------------------------------------------------------
int scene5( int initstate ){
  enum{
    ST_INIT = 0,
    ST_RUN,
    ST_WAIT,
    ST_END,
  };

  static char state = ST_INIT;

  if(initstate == 1){
    state = ST_INIT;
  }
    
  switch(state){
    case ST_INIT:
            Timer1 = millis();
            Timer2 = Timer1;
            state = ST_RUN;
// Serial.println("scene5()");  
            break;
    
    case ST_RUN:
            chCont5();
            show();
            state = ST_WAIT;
            break;
            
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_RUN;
            } 

            break;
    case ST_END:
            return 1;
            break;

    default:
            break;
  }
  return 0;
}

// --------------------------------------------------------------------------------
// 3:40 赤点滅
// --------------------------------------------------------------------------------
int scene6(void){
  enum{ 
    ST_INIT = 0,
    ST_RUN,
    ST_WAIT,
    ST_WHITE,
    ST_WHITEWAIT,
    ST_FADEOUT,
    ST_END,
  };
  static long sel;
  static char pos[6];
  static char state = ST_INIT;
  int i;
    static int bright = 0;
    
  switch(state){
    case ST_INIT:
            Timer1 = millis();
            Timer2 = Timer1;
            state = ST_RUN;
// Serial.println("scene6()");  
            break;
    
    case ST_RUN:
            for(i=0;i<=5;i++){
              pos[i]=(char)random(2);          
//// Serial.println((unsigned char)pos[i]);  
                for(ledch = i*8 ; ledch <= i*8+7 ; ledch++){   
                  ch[ledch].rled = pos[i] ? LedMaxLevel/2 :0;
                  ch[ledch].gled = 0;
                  ch[ledch].bled = 0;
                }
            }
            for(ledch = 0 ; ledch <= MAXLED-1 ; ledch++){   
              pixels.setPixelColor(ledch, pixels.Color(ch[ledch].rled, ch[ledch].gled, ch[ledch].bled));
            }
            pixels.show();  
            state = ST_WAIT;
            break;

    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_WHITE;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_RUN;
            } 
            break;

    case ST_WHITE: // 白点灯後5秒保持
          allled(LedMaxLevel,LedMaxLevel,LedMaxLevel);
#if 0
          for(ledch = 0 ; ledch <= MAXLED-1  ; ledch++){   
            ch[ledch].rled = LedMaxLevel;
            ch[ledch].gled = LedMaxLevel; 
            ch[ledch].bled = LedMaxLevel;
          }
          show();
#endif
          bright = LedMaxLevel;
          Timer2 = 2000; // 5秒
          gPrevious = millis();
          state = ST_WHITEWAIT;
          break;
    case ST_WHITEWAIT:
          if ( (millis() - gPrevious) >= Timer2){
            state = ST_FADEOUT; 
          }
          break;
    case ST_FADEOUT:
          allled(bright,bright,bright);
#if 0
          for(ledch = 0 ; ledch <= MAXLED-1  ; ledch++){   
            ch[ledch].rled = bright;
            ch[ledch].gled = bright;
            ch[ledch].bled = bright;          
          }
          show();
#endif
          bright--;
          if(bright<=0){
            state = ST_END;
          }
          break;

            
    case ST_END:
            return 1;
            break;

    default:
            break;
  }
  return 0;
}

// --------------------------------------------------------------------------------
// 音楽間奏
// --------------------------------------------------------------------------------
int scene7_0( int initstate ){
  enum{
    ST_INIT = 0,
    ST_WAIT,
    ST_END,
  };

  static char state = ST_INIT;

  if(initstate==1){
    state = ST_INIT;
  }
    
  switch(state){
    case ST_INIT:
            rinit();
            Timer1 = millis();
            state = ST_WAIT;
// Serial.println("scene7_0()");  
            break;
    
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
            }  
            break;
    case ST_END:
            return 1;
            break;

    default:
            break;
  }
  return 0;
}



// --------------------------------------------------------------------------------
// 3:53 ピンク点滅(13回)
// --------------------------------------------------------------------------------
int scene7( int initstate ){
  enum{
    ST_INIT = 0,
    ST_RUN,
    ST_PWAIT,
    ST_BWAIT,
    ST_BBRNK,
    ST_BBWAIT,
    ST_BB2WAIT,
    ST_END,
  };
  static int loopcnt = 0;
  static int bright = 0;
  static long timm = 0;

  static char state = ST_INIT;

  if(initstate == 1){
    state = ST_INIT;  
  }
  switch(state){
    case ST_INIT:
            Timer1 = millis();
            Timer2 = millis();
            loopcnt = 0;
            bright = 0;
            timm = 0;
            state = ST_RUN;
            break;
    
    case ST_RUN:// ピンク点灯
          allled(LedMaxLevel,0,LedMaxLevel);
          timm = 200; // 500ms
          state = ST_PWAIT;
          Timer2 = millis();
          break;
          
    case ST_PWAIT:
          if ( (millis() - Timer2) >= WAIT_TIMER){
            allled(0,0,0);// ピンク消灯
            Timer2 = millis();
            state = ST_BWAIT;
          }  
          break;
            
    case ST_BWAIT:
          if ( (millis() - Timer2) >= WAIT_TIMER){
            loopcnt ++;
            if(loopcnt>=14){
              loopcnt = 0;
              state = ST_BBRNK;
              break;
            }
            state = ST_RUN;
          }
          break; 
                
    case ST_BBRNK: // 青点滅
          allled(0,0,LedMaxLevel);
          timm = 100; // 100ms
          state = ST_BBWAIT;
          Timer2 = millis();
          break;

    case ST_BBWAIT:
          if ( (millis() - Timer2) >= timm){
            allled(0,0,0);// 青消灯
            Timer2 = millis();
            state = ST_BB2WAIT;
          }  
          break;
    case ST_BB2WAIT: // 青点滅
          if ( (millis() - Timer2) >= timm){
            loopcnt ++;
            if(loopcnt>=10){
              loopcnt = 0;
              state = ST_END;
              break;
            }
            Timer2 = millis();
            state = ST_BBRNK;
          }
          break;
    case ST_END:
           
            return 1;
            break;

    default:
            break;
  }
  return 0;
}

// --------------------------------------------------------------------------------
// 3:59 ピンク、青、Leafbuf
// --------------------------------------------------------------------------------
int scene8( int initstate ){
  enum{
    ST_INIT = 0,
    ST_RUN,
    ST_WAIT,
    ST_END,
  };

  static unsigned char tblcnt = 0;
  static int waitcnt = 0;
  static char even=0;
  static char state = ST_INIT;

  if(initstate==1){
    state = ST_INIT;  
  }
  switch(state){
    case ST_INIT:
            Timer1 = millis();
            Timer2 = millis();
            rinit();
            tblcnt = 0;
            state = ST_RUN;

// Serial.println("scene8()");  
            break;
    
    case ST_RUN:
          hrote();
            // 端のLEDにleaf_tblを代入 
          if(tblcnt < 20){
            float cal;
            cal = LedMaxLevel * (float)leaf_tbl[tblcnt] / 10.0;
            ch[21].rled = rbuf[21]=(char)cal;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=LedMaxLevel;

            ch[22].rled = rbuf[22]=(char)cal;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=LedMaxLevel;

          } else {
            ch[21].rled = rbuf[21]=0;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=0;         

            ch[22].rled = rbuf[22]=0;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=0;
          }

          if(even==1){  // 速度を落とす為1/2
            tblcnt++;
            even = 0;
          } else {
            even=1;
          }
          if(tblcnt >= 20){
            tblcnt=0;
            state = 2;
          }
          break;
    case 2: // から送り
          hrote();
          waitcnt++;
          if(waitcnt>5){
            waitcnt = 0;
            state = 3;    
          }
          break;

    case 3: // 1-23 LEDにbufの内容をシフトして書き込み
          rote();
            // 中央のLEDにleaf_tblを代入
          if(tblcnt < 24){
            float cal;
            cal = LedMaxLevel * (float)leaf_tbl[tblcnt] / 10.0;
            ch[0].rled = rbuf[0]=LedMaxLevel;
            ch[0].gled = gbuf[0]=(char)cal;
            ch[0].bled = bbuf[0]=LedMaxLevel;
          }
          
          if(even==1){
            tblcnt++;
            even = 0;
          } else {
            even=1;
          }
          if(tblcnt >= 20){
            tblcnt=0;
            state = 4;
          }
          break;
    case 4: // から送り
          rote();
          waitcnt++;
          if(waitcnt>10){
            waitcnt = 0;
            state = 3;    
          }
          break;


    default:
          break;
  }
}








#if 0


            show();
            state = ST_WAIT;  
            break;
            
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  


            break;
    case ST_END:
            return 1;
            break;

    default:
            break;
  }
  return 0;
}

#endif

void allled(int r,int g,int b){
  for(ledch = 0 ; ledch <= MAXLED-1  ; ledch++){   
    ch[ledch].rled = r;
    ch[ledch].gled = g; 
    ch[ledch].bled = b;
   }
   show();  
}

// --------------------------------------------------------------------------------
// 4:17 ピンク、白　全体交互
// --------------------------------------------------------------------------------
int scene9( int initstate ){
  enum{
    ST_INIT = 0,
    ST_RUN,
    ST_FADE,
    ST_FADE2,
    ST_WAIT,
    ST_END,
  };
  static int state = ST_INIT;
  static int bright = 0;
  if(initstate == 1){
    state = ST_INIT;
  }

  switch(state){
    case ST_INIT:
          bright = 0;
          state = ST_RUN;
          break;

    case ST_RUN: // ピンク点灯
          allled(LedMaxLevel,0,LedMaxLevel);
          Timer1 = millis();
          Timer2 = millis();

          state = ST_FADE;
          break;

    case ST_FADE: // ピンクから白へ遷移
          if ( (millis() - Timer2) >= WAIT_TIMER){
            Timer2 = millis();
            bright++;
//Serial.println(bright);
            allled(LedMaxLevel,bright,LedMaxLevel);
//            allled(LedMaxLevel,LedMaxLevel,LedMaxLevel);
            if(bright>=LedMaxLevel){
              state = ST_FADE2;
              break;
            }
          }
          if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
          } 
          break;
    case ST_FADE2: // 白からピンクへ遷移
          if ( (millis() - Timer2) >= WAIT_TIMER){
            Timer2 = millis();
            bright--;
//Serial.println(bright);
            if(bright<=0){
              state = ST_FADE;
              break;
            }
            allled(LedMaxLevel,bright,LedMaxLevel);
          }
          if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
          } 
          break;
    case ST_END:
            return 1;
            break;
    default:
            break;
  }
  return 0;
}

// --------------------------------------------------------------------------------
// 4:30 紫が細かく流れる
// --------------------------------------------------------------------------------
int scene10( int initstate ){
  enum{
    ST_INIT = 0,
    ST_RUN,
    ST_WAIT,
    ST_END,
  };
  static unsigned char tblcnt = 0;
  static int waitcnt = 0;
  static int state = ST_INIT;

  if(initstate == 1){
    state = ST_INIT;
  }
  
  switch(state){
    case ST_INIT:
          Timer1 = millis();
          Timer2 = Timer1;
          state = ST_RUN;
          break;
    case ST_RUN: // 1-23 LEDにbufの内容をシフトして書き込み
          rote();
#if 0
          for(ledch = 43 ; ledch >= 1 ; ledch--){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch-1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch-1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch-1];
          }  
#endif
            // 先頭のLEDにleaf_tblを代入
           if(tblcnt <= 8){
            float cal;
            cal = LedMaxLevel * (float)leaf_tbl2[tblcnt] / 10.0;
            rbuf[0]=(char)cal;
            gbuf[0] = 0;
            bbuf[0] = (char)cal;
           } else {
              rbuf[0] = 0;
              gbuf[0] = 0;
              bbuf[0] = 0;
           }
            show();
           tblcnt++;
          if(tblcnt >= 13){
            tblcnt=0;
          }
          state = ST_WAIT;
          break;

    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_RUN;
            } 

            break;
        case ST_END:
            return 1;
            break;
    default:
          break;
  }
  return 0;
}


// --------------------------------------------------------------------------------
// 4:45 紫が中央から左右に流れる
// --------------------------------------------------------------------------------
int scene11( void ){
  enum{
    ST_INIT = 0,
    ST_RUN,
    ST_WAIT,
    ST_END,
  };
  static unsigned char tblcnt = 0;
  static int waitcnt = 0;
  static int state = ST_INIT;
  
  switch(state){
    case ST_INIT:
          Timer1 = millis();
          Timer2 = Timer1;
          state = ST_RUN;
//  // Serial.println("ST_RUN");  
       
          break;
    case ST_RUN: // 1-23 LEDにbufの内容をシフトして書き込み
          hrote();
#if 0
          for(ledch = 0 ; ledch <= 21 ; ledch++){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch+1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch+1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch+1];
          }  
          for(ledch = 43 ; ledch >= 22 ; ledch--){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch-1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch-1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch-1];
          }  
#endif
            // 中央のLEDにleaf_tblを代入
          if(tblcnt <= 8){
            float cal;
            cal = LedMaxLevel * (float)leaf_tbl2[tblcnt] / 10.0;
            ch[21].rled = rbuf[21]=(char)cal;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=(char)cal;

            ch[22].rled = rbuf[22]=(char)cal;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=(char)cal;

          } else {
            ch[21].rled = rbuf[21]=0;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=0;            

            ch[22].rled = rbuf[22]=0;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=0;
          }
         show();
          tblcnt++;
          if(tblcnt >= 13){
            tblcnt=0;
          }
         state = ST_WAIT;
//           // Serial.println("ST_WAIT");  
          break;
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_RUN;
            } 

            break;
        case ST_END:
 //         // Serial.println("ST_END");  
            return 1;
            break;
    default:
          break;
  }
    return 0;
}

// --------------------------------------------------------------------------------
// 4:58 青全体点滅→フェードアウト消灯
// --------------------------------------------------------------------------------
int scene12( void ){
  enum{
    ST_INIT = 0,
    ST_RUN,
    ST_1,
    ST_2,
    ST_3,
    ST_4,
    ST_5,
    ST_6,
    ST_WAIT,
    ST_FADE,
    ST_END,
  };
  static int cnt = 0;
  static int waitcnt = 0;
  static int state = ST_INIT;
  static char OnOff = 0;
  char pos=0;
  
  switch(state){
    case ST_INIT:
          Timer1 = millis();
          Timer2 = Timer1;
          state = ST_RUN;
//  // Serial.println("ST_RUN");  
       
          break;
    case ST_RUN:
          OnOff = (char)random(2);
          pos = (char)random(1,6+1);
//// Serial.print((unsigned char)OnOff);
//// Serial.print(",");  
//// Serial.println((unsigned char)pos);  
       
          switch(pos){
            case 1: state = ST_1;break;
            case 2: state = ST_2;break;
            case 3: state = ST_3;break;             
            case 4: state = ST_4;break;
            case 5: state = ST_5;break;
            case 6: state = ST_6;break;
            default:break;   
          }
          break;
          
    case ST_1:
          for(ledch = 0 ; ledch <= 5 ; ledch++){   
            ch[ledch].rled = 0;
            ch[ledch].gled = 0; 
            if(OnOff == 1){
              ch[ledch].bled = LedMaxLevel;
            } else {
              ch[ledch].bled = 0;
            }
          }
          show();
          state = ST_WAIT;
          break;

    case ST_2:
          for(ledch = 6 ; ledch <= 13; ledch++){   
            ch[ledch].rled = 0;
            ch[ledch].gled = 0; 
            if(OnOff == 1){
              ch[ledch].bled = LedMaxLevel;
            } else {
              ch[ledch].bled = 0;
            }
          }
          show();
          state = ST_WAIT;
          break;

    case ST_3:
          for(ledch = 14 ; ledch <= 21 ; ledch++){   
            ch[ledch].rled = 0;
            ch[ledch].gled = 0; 
            if(OnOff == 1){
              ch[ledch].bled = LedMaxLevel;
            } else {
              ch[ledch].bled = 0;
            }
          }
          show();
          state = ST_WAIT;
          break;
          
    case ST_4:
          for(ledch = 22 ; ledch <= 29 ; ledch++){   
            ch[ledch].rled = 0;
            ch[ledch].gled = 0; 
            if(OnOff == 1){
              ch[ledch].bled = LedMaxLevel;
            } else {
              ch[ledch].bled = 0;
            }
          }
          show();
          state = ST_WAIT;
          break;

    case ST_5:
          for(ledch = 30 ; ledch <= 37 ; ledch++){   
            ch[ledch].rled = 0;
            ch[ledch].gled = 0; 
            if(OnOff == 1){
              ch[ledch].bled = LedMaxLevel;
            } else {
              ch[ledch].bled = 0;
            }
          }
          show();
          state = ST_WAIT;
          break;

    case ST_6:
          for(ledch = 38 ; ledch <= 43 ; ledch++){   
            ch[ledch].rled = 0;
            ch[ledch].gled = 0; 
            if(OnOff == 1){
              ch[ledch].bled = LedMaxLevel;
            } else {
              ch[ledch].bled = 0;
            }
          }
          show();
          state = ST_WAIT;
          break;
    
    
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  

//            if ( (millis() - Timer2) >= WAIT_TIMER){
//              Timer2 = millis();
              state = ST_RUN;
//            } 
            cnt++;
            if(cnt > 300){
              state = ST_FADE;
            }
            break;

    case ST_FADE:
          for(ledch = 0 ; ledch <= 43 ; ledch++){   
              ch[ledch].bled = 0;
          }
          show();
          state = ST_END;
          break;            
        
        
        case ST_END:
 //         // Serial.println("ST_END");  
            return 1;
            break;
    default:
          break;
  }
    return 0;
}


// --------------------------------------------------------------------------------
// 5:00 白全点灯、中央からピンク、緑、LEAF＿回転
// --------------------------------------------------------------------------------
int scene13( int initstate ){
  enum{
    ST_INIT = 0,
    ST_FAST,
    ST_RUN,
    ST_WAIT,
    ST_END,
  };

  static int NowLevel = 0;
  static int state = ST_INIT;
  static unsigned char tblcnt = 0;
  static char NowCol=0;
  float cal;
  
  if(initstate == 1){
    state = ST_INIT;
  }
            
  switch(state){
    case ST_INIT:
            state = ST_FAST;
            break;
    case ST_FAST:
          for(ledch = 0 ; ledch <= 43 ; ledch++){   
            ch[ledch].rled = NowLevel;
            ch[ledch].gled = 0; 
            ch[ledch].bled = NowLevel;
            rbuf[ledch]=NowLevel;
            gbuf[ledch]=0;
            bbuf[ledch]=NowLevel;            
          }
          show();
          NowLevel++;
          delay(10);
          if(NowLevel>LedMaxLevel){
            state = ST_RUN;
          Timer1 = millis();
          Timer2 = Timer1;
            break;
          }
          break;

    case ST_RUN: // 1-23 LEDにbufの内容をシフトして書き込み
          hrote();
#if 0
          for(ledch = 0 ; ledch <= 21 ; ledch++){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch+1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch+1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch+1];
          }  
          for(ledch = 43 ; ledch >= 22 ; ledch--){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch-1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch-1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch-1];
          }  
#endif
            // 中央のLEDにleaf_tblを代入
          if(tblcnt <= 20){

            cal = LedMaxLevel * (float)leaf_tbl[tblcnt] / 10.0;
 // Serial.println((unsigned char)NowCol);  
            if(NowCol == 0){
              ch[21].rled = rbuf[21]=(char)cal;
              ch[21].gled = gbuf[21]=0;
              ch[21].bled = bbuf[21]=(char)cal;
              ch[22].rled = rbuf[22]=(char)cal;
              ch[22].gled = gbuf[22]=0;
              ch[22].bled = bbuf[22]=(char)cal;
            } else {
              ch[21].rled = rbuf[21]=0;
              ch[21].gled = gbuf[21]=(char)cal;
              ch[21].bled = bbuf[21]=0;
              ch[22].rled = rbuf[22]=0;
              ch[22].gled = gbuf[22]=(char)cal;
              ch[22].bled = bbuf[22]=0;              
            }
          } else {
            ch[21].rled = rbuf[21]=0;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=0;         

            ch[22].rled = rbuf[22]=0;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=0;

          }
         show();
          tblcnt++;
          if(tblcnt >= 20){
            if(NowCol == 0 ){
              NowCol = 1;
            } else {
              NowCol = 0;
            }
            tblcnt=0;
          }
         state = ST_WAIT;
//           // Serial.println("ST_WAIT");  
          break;
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_RUN;
            } 

            break;
        case ST_END:
 //         // Serial.println("ST_END");  
            return 1;
            break;
    default:
          break;
  }
    return 0;
}

// --------------------------------------------------------------------------------
// 5:28 中央からピンク、青、LEAF＿回転
// --------------------------------------------------------------------------------
int scene14( int initstate ){
  enum{
    ST_INIT = 0,
    ST_FAST,
    ST_RUN,
    ST_WAIT,
    ST_END,
  };

  static int NowLevel = 0;
  static int state = ST_INIT;
  static unsigned char tblcnt = 0;
  static char NowCol=0;
            float cal;

  if(initstate == 1){
    state = ST_INIT;
  }
  
  switch(state){
    case ST_INIT:
            state = ST_FAST;
            break;
    case ST_FAST:
          for(ledch = 0 ; ledch <= 43 ; ledch++){   
            ch[ledch].rled = NowLevel;
            ch[ledch].gled = 0; 
            ch[ledch].bled = NowLevel;
            rbuf[ledch]=NowLevel;
            gbuf[ledch]=0;
            bbuf[ledch]=NowLevel;            
          }
          show();
          NowLevel++;
          delay(10);
          if(NowLevel>LedMaxLevel){
            state = ST_RUN;
          Timer1 = millis();
          Timer2 = Timer1;
            break;
          }
          break;

    case ST_RUN: // 1-23 LEDにbufの内容をシフトして書き込み
          hrote();
#if 0
          for(ledch = 0 ; ledch <= 21 ; ledch++){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch+1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch+1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch+1];
          }  
          for(ledch = 43 ; ledch >= 22 ; ledch--){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch-1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch-1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch-1];
          }  
#endif
            // 中央のLEDにleaf_tblを代入
          if(tblcnt <= 20){

            cal = LedMaxLevel * (float)leaf_tbl[tblcnt] / 10.0;
 // Serial.println((unsigned char)NowCol);  
            if(NowCol == 0){
              ch[21].rled = rbuf[21]=(char)cal;
              ch[21].gled = gbuf[21]=0;
              ch[21].bled = bbuf[21]=(char)cal;
              ch[22].rled = rbuf[22]=(char)cal;
              ch[22].gled = gbuf[22]=0;
              ch[22].bled = bbuf[22]=(char)cal;
            } else {
              ch[21].rled = rbuf[21]=0;
              ch[21].gled = gbuf[21]=0;
              ch[21].bled = bbuf[21]=(char)cal;
              ch[22].rled = rbuf[22]=0;
              ch[22].gled = gbuf[22]=0;
              ch[22].bled = bbuf[22]=(char)cal;              
            }
          } else {
            ch[21].rled = rbuf[21]=0;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=0;         

            ch[22].rled = rbuf[22]=0;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=0;

          }
         show();
          tblcnt++;
          if(tblcnt >= 20){
            if(NowCol == 0 ){
              NowCol = 1;
            } else {
              NowCol = 0;
            }
            tblcnt=0;
          }
         state = ST_WAIT;
//           // Serial.println("ST_WAIT");  
          break;
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_RUN;
            } 

            break;
        case ST_END:
 //         // Serial.println("ST_END");  
            return 1;
            break;
    default:
          break;
  }
    return 0;
}

// --------------------------------------------------------------------------------
// 5:42 中央から先は赤、徐々に黄色？LEAF＿回転
// --------------------------------------------------------------------------------
int scene15( int initstate ){
  enum{
    ST_INIT = 0,
    ST_FAST,
    ST_RUN,
    ST_WAIT,
    ST_END,
  };

  static int NowLevel = 0;
  static int state = ST_INIT;
  static unsigned char tblcnt = 0;
  static char NowCol=0;
  float cal;
  static float cntr=0;
  static char cntr_F=0;

  if(initstate == 1){
    state = ST_INIT;
  }
  
  switch(state){
    case ST_INIT:
          state = ST_RUN;
//          rinit();
          NowLevel = 0;
          tblcnt =0;
          NowCol=0;
          cntr =0;
          cntr_F=0;
          Timer1 = millis();
          Timer2 = millis();
          break;
          
    case ST_RUN: // 1-23 LEDにbufの内容をシフトして書き込み
          hrote();
#if 0
          for(ledch = 0 ; ledch <= 21 ; ledch++){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch+1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch+1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch+1];
          }  
          for(ledch = 43 ; ledch >= 22 ; ledch--){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch-1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch-1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch-1];
          }  
#endif
            // 中央のLEDにleaf_tblを代入
          if(tblcnt <= 20){

            cal = LedMaxLevel * (float)leaf_tbl[tblcnt] / 10.0;
            ch[21].rled = rbuf[21]=(char)cal;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=0;
            ch[22].rled = rbuf[22]=(char)cal;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=0;
          } else {
            ch[21].rled = rbuf[21]=0;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=0;         

            ch[22].rled = rbuf[22]=0;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=0;

          }

          for(ledch = 0 ; ledch <= 43 ; ledch++){
            float cm,cn;
            cm = rbuf[ledch]*(cntr/20.0);   
            cn = rbuf[ledch]*(cntr/40.0);   

            ch[ledch].gled =cm;
            ch[ledch].bled = cn;

          }  

          
         show();
          tblcnt++;
          if(tblcnt >= 40){
            tblcnt=0;
          }
          if(cntr_F==0){
            cntr = cntr + 1;
            if(cntr>=20){
              cntr_F=1;
              cntr=20;
            }
          } else {
            cntr = cntr - 1;
            if(cntr<=0){
              cntr = 0;
              cntr_F=0;
            }
          }
          
         state = ST_WAIT;
//           // Serial.println("ST_WAIT");  
          break;
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_RUN;
            } 

            break;
        case ST_END:

          int lp=0;
          for(lp=0;lp<=100;lp++){
            for(ledch = 0 ; ledch <= 43 ; ledch++){
              ch[ledch].rled++;
              if(ch[ledch].rled >= LedMaxLevel){
                ch[ledch].rled = LedMaxLevel;
              }
              ch[ledch].gled++;
              if(ch[ledch].gled >= LedMaxLevel){
                ch[ledch].gled = LedMaxLevel;
              }
              ch[ledch].bled++;
              if(ch[ledch].bled >= LedMaxLevel /2 ){
                ch[ledch].bled = LedMaxLevel / 2;
              }          
           }
          show();
          delay(100);
          }

            return 1;
            break;
    default:
          break;
  }
    return 0;
}

// --------------------------------------------------------------------------------
// 6:39 全体黄色点灯
// --------------------------------------------------------------------------------
int scene16( void ){
  enum{
    ST_INIT = 0,
    ST_FAST,
    ST_RUN,
    ST_WAIT,
    ST_END,
  };
  static int state = ST_INIT;
  switch(state){
    case ST_INIT:
          state = ST_RUN;
          Timer1 = millis();
          Timer2 = millis();
          break;
          
    case ST_RUN:
          allled(LedMaxLevel,LedMaxLevel,0);
          state = ST_WAIT;
          break;
    case ST_WAIT:
          if ( (millis() - Timer1) >= SCENE_TIMER){
            state = ST_END;
          }
          break;            
    case ST_END:
          return 1;
          break;
    default:
          break;
  }
  return 0;
}

// --------------------------------------------------------------------------------
// 青、LEAF＿回転
// --------------------------------------------------------------------------------
int scene17( int initstate ){
  enum{
    ST_INIT = 0,
    ST_FAST,
    ST_RUN,
    ST_WAIT,
    ST_END,
  };

  static int NowLevel = 0;
  static int state = ST_INIT;
  static unsigned char tblcnt = 0;
  static char NowCol=0;
            float cal;

  if(initstate == 1){
    state = ST_INIT;
  }
  
  switch(state){
    case ST_INIT:
            state = ST_FAST;
            break;
    case ST_FAST:
          for(ledch = 0 ; ledch <= 43 ; ledch++){   
            ch[ledch].rled = 0;
            ch[ledch].gled = 0; 
            ch[ledch].bled = NowLevel;
            rbuf[ledch]=0;
            gbuf[ledch]=0;
            bbuf[ledch]=NowLevel;            
          }
          show();
          NowLevel++;
          delay(10);
          if(NowLevel>LedMaxLevel){
            state = ST_RUN;
          Timer1 = millis();
          Timer2 = millis();
            break;
          }
          break;

    case ST_RUN: // 1-23 LEDにbufの内容をシフトして書き込み
          hrote();
            // 中央のLEDにleaf_tblを代入
          if(tblcnt <= 20){

            cal = LedMaxLevel * (float)leaf_tbl[tblcnt] / 10.0;
 // Serial.println((unsigned char)NowCol);  
              ch[21].rled = rbuf[21]=0;
              ch[21].gled = gbuf[21]=0;
              ch[21].bled = bbuf[21]=(char)cal;
              ch[22].rled = rbuf[22]=0;
              ch[22].gled = gbuf[22]=0;
              ch[22].bled = bbuf[22]=(char)cal;
 
          } else {
            ch[21].rled = rbuf[21]=0;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=0;         

            ch[22].rled = rbuf[22]=0;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=0;

          }
         show();
          tblcnt++;
          if(tblcnt >= 20){
            tblcnt=0;
          }
         state = ST_WAIT;
//           // Serial.println("ST_WAIT");  
          break;
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_RUN;
            } 

            break;
        case ST_END:
 //         // Serial.println("ST_END");  
            return 1;
            break;
    default:
          break;
  }
    return 0;
}

// --------------------------------------------------------------------------------
// 8:30 全体赤、紫、青。中央から左右に流れる、引き続き青が左右にながれる
// --------------------------------------------------------------------------------
int scene18( int initstate){
  enum{
    ST_INIT = 0,
    ST_FAST,
    ST_RUN,
    ST_WAIT,
    ST_END,
  };

  static int NowLevel = 0;
  static int state = ST_INIT;
  static unsigned char tblcnt = 0;
  static char NowCol=0;
  float cal;
  static float cntr=0;
  static char cntr_F=0;

  if(initstate == 1){
    state = ST_INIT;
  }
  
  switch(state){
    case ST_INIT:
    NowLevel = 0;
    tblcnt = 0;
    NowCol=0;
    cntr=0;
    cntr_F=0;
          state = ST_RUN;
          Timer1 = millis();
          Timer2 = millis();
          break;
          
    case ST_RUN: // 1-23 LEDにbufの内容をシフトして書き込み
          hrote();
#if 0
          for(ledch = 0 ; ledch <= 21 ; ledch++){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch+1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch+1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch+1];
          }  
          for(ledch = 43 ; ledch >= 22 ; ledch--){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch-1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch-1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch-1];
          }  
#endif
            // 中央のLEDにleaf_tblを代入
          if(tblcnt <= 20){

            cal = LedMaxLevel * (float)leaf_tbl[tblcnt] / 10.0;
            ch[21].rled = rbuf[21]=0;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=(char)cal;
            ch[22].rled = rbuf[22]=0;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=(char)cal;
          } else {
            ch[21].rled = rbuf[21]=0;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=0;         

            ch[22].rled = rbuf[22]=0;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=0;

          }

     
         show();
          tblcnt++;
          if(tblcnt >= 40){
            tblcnt=0;
          }

          
         state = ST_WAIT;
//           // Serial.println("ST_WAIT");  
          break;
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_RUN;
            } 

            break;
        case ST_END:

          int lp=0;
          for(lp=0;lp<=100;lp++){
            for(ledch = 0 ; ledch <= 43 ; ledch++){
              ch[ledch].rled++;
              if(ch[ledch].rled >= LedMaxLevel){
                ch[ledch].rled = LedMaxLevel;
              }
              ch[ledch].gled--;
              if(ch[ledch].gled <= 1){
                ch[ledch].gled = 1;
              }
              ch[ledch].bled--;
              if(ch[ledch].bled <= 1 ){
                ch[ledch].bled = 1;
              }          
           }
          show();
          delay(100);
          }
           return 1;
            break;
    default:
          break;
  }
    return 0;
}

// --------------------------------------------------------------------------------
// 8:50 全体紫、赤、緑・赤
// --------------------------------------------------------------------------------
int scene19( void ){
  enum{
    ST_INIT = 0,
    ST_FAST,
    ST_RUN,
    ST_WAIT,
    ST_END,
  };

  static int NowLevel = 0;
  static int state = ST_INIT;
  static unsigned char tblcnt = 0;
  static char NowCol=0;
            float cal;
            static float cntr=0;
            static char cntr_F=0;
  switch(state){
    case ST_INIT:
          state = ST_RUN;
          cntr_F=0;
          cntr=0;
          NowCol=0;
          for(ledch = 0 ; ledch <= 43 ; ledch++){   
            ch[ledch].rled = LedMaxLevel;
            ch[ledch].gled = 0;
            ch[ledch].bled = 0;
          }  
          show(); 
          NowCol = LedMaxLevel;
          Timer1 = millis();
          Timer2 = millis();
          break;
          
    case ST_RUN: // 1-23 LEDにbufの内容をシフトして書き込み
          allled(NowCol,LedMaxLevel - NowCol,0);
         if(cntr_F == 0) {
            NowCol--;
            if(NowCol <= 0){
              NowCol = 0;
              cntr_F = 1;         
            }
         } else  if(cntr_F == 1) {
            NowCol++;
            if(NowCol >= LedMaxLevel){
              NowCol = LedMaxLevel;
              cntr_F = 0;         
            }
         }          
         state = ST_WAIT;
//           // Serial.println("ST_WAIT");  
          break;
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_RUN;
            } 

            break;
        case ST_END:
        state = ST_INIT;
           return 1;
            break;
    default:
          break;
  }
    return 0;


}

// for の変数i で合っている？
// --------------------------------------------------------------------------------
// 9:44 全体黄色、白。花火、消灯
// --------------------------------------------------------------------------------
int scene20( int initstate ){
  enum{
    ST_INIT = 0,
    ST_FAST,
    ST_RUN,
    ST_NEXT,
    ST_WAIT,
    ST_END,
  };

  static int NowLevel = 0;
  static int state = ST_INIT;
  static unsigned char tblcnt = 0;
  static char NowCol=0;
  static int i;

  if(initstate == 1){
    state = ST_INIT;
  }
  
  switch(state){
    case ST_INIT:
          state = ST_RUN;
          NowCol=0;
          rinit();
          show();
          Timer1 = millis();
          Timer2 = millis();
          break;
          
    case ST_RUN:
          for(ledch=0;ledch<43;ledch++){
            ch[ledch].rled = NowCol;
            ch[ledch].gled = NowCol;
            ch[ledch].bled = NowCol;              
          }
          show();
          NowCol ++;
          if(NowCol>=LedMaxLevel){
            state = ST_NEXT;
          }
          delay(100);
          break;
    case ST_NEXT:
          for(ledch=0;ledch<43;ledch++){
            ch[ledch].rled = NowCol;
            ch[ledch].gled = NowCol; 
            ch[ledch].bled = NowCol;              
          }
          show();
          NowCol --;
          if(NowCol<=0){
            state = ST_WAIT;
          } 
          delay(50);
          break;
    case ST_WAIT:
             if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }     
          break;
    case ST_END:
          return 1;
          break;
    default:
          break;
  }
  return 0;
}

// --------------------------------------------------------------------------------
// 10:27 全体青、しばらくしたら、中央から青・白で流れる。
// --------------------------------------------------------------------------------
int scene21( void ){
  enum{
    ST_INIT = 0,
    ST_FAST,
    ST_RUN,
    ST_NEXT,
    ST_WAIT,
    ST_END,
  };

  static int NowLevel = 0;
  static int state = ST_INIT;
  static unsigned char tblcnt = 0;
  static char NowCol=0;
  static int i;
              float cal;
  switch(state){
    case ST_INIT:
          rinit();
          show();
          Timer1 = millis();
          Timer2 = millis();
          state = ST_RUN;
          break;
    
    case ST_RUN:
          allled(0,0,NowCol);
          NowCol ++;
          if(NowCol>=LedMaxLevel){
            state = ST_NEXT;
          }
          delay(100);
          break;
    case ST_NEXT:// 1-23 LEDにbufの内容をシフトして書き込み
          for(ledch = 0 ; ledch <= 9 ; ledch++){ //21->19   
            ch[ledch*2].rled = rbuf[ledch*2] = rbuf[ledch*2+2];
            ch[ledch*2].gled = gbuf[ledch*2] = gbuf[ledch*2+2];
            ch[ledch*2].bled = bbuf[ledch*2] = bbuf[ledch*2+2];
          }  
          for(ledch = 9 ; ledch >= 0 ; ledch--){  //22->24
            ch[ledch*2+25].rled = rbuf[ledch*2+25] = rbuf[ledch*2+25-2];
            ch[ledch*2+25].gled = gbuf[ledch*2+25] = gbuf[ledch*2+25-2];
            ch[ledch*2+25].bled = bbuf[ledch*2+25] = bbuf[ledch*2+25-2];

          }  
            // 中央のLEDにleaf_tblを代入
          if(tblcnt <= 3){

            cal = LedMaxLevel;
            ch[20].rled = rbuf[20]=(char)cal;
            ch[20].gled = gbuf[20]=(char)cal;
            ch[20].bled = bbuf[20]=(char)cal;
            ch[23].rled = rbuf[23]=(char)cal;
            ch[23].gled = gbuf[23]=(char)cal;
            ch[23].bled = bbuf[23]=(char)cal;
          } else {
            ch[20].rled = rbuf[20]=0;
            ch[20].gled = gbuf[20]=0;
            ch[20].bled = bbuf[20]=LedMaxLevel;      

            ch[23].rled = rbuf[23]=0;
            ch[23].gled = gbuf[23]=0;
            ch[23].bled = bbuf[23]=LedMaxLevel;
          }
         show();
          tblcnt++;
          if(tblcnt >= 20){
            tblcnt=0;
          }

          
         state = ST_WAIT;
//           // Serial.println("ST_WAIT");   
          break;
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_NEXT;
            } 

            break;
    case ST_END:
          state = ST_INIT;
          return 1;
          break;
    default:
          break;
  }
  return 0;

}

// --------------------------------------------------------------------------------
// 10:50 中央から青で流れて、白の点も流れる
// --------------------------------------------------------------------------------
int scene22( int initstate ){
  enum{
    ST_INIT = 0,
    ST_FAST,
    ST_RUN,
    ST_NEXT,
    ST_WAIT,
    ST_END,
  };

  static int NowLevel = 0;
  static int state = ST_INIT;
  static unsigned char tblcnt = 0;
  static char NowCol=0;
  static int i;
              float cal;

  if(initstate == 1){
    state = ST_INIT;
  }
              
  switch(state){
    case ST_INIT:
          allled(0,0,LedMaxLevel);
          state = ST_FAST;
// Serial.println("ST_RUN");
          break;
    
    case ST_FAST:
          for(ledch = 0 ; ledch <= 21 ; ledch++){  
            ch[ledch*2].rled = LedMaxLevel;
            ch[ledch*2].gled = LedMaxLevel;
            ch[ledch*2].bled = LedMaxLevel;
          }  
          show();
          delay(100);
          for(ledch = 0 ; ledch <= 21 ; ledch++){  
            ch[ledch*2].rled = LedMaxLevel;
            ch[ledch*2].gled = 0;
            ch[ledch*2].bled = LedMaxLevel;
          }  
          show();
          delay(100);
          for(ledch = 0 ; ledch <= 21 ; ledch++){  
            ch[ledch*2].rled = LedMaxLevel;
            ch[ledch*2].gled = LedMaxLevel;
            ch[ledch*2].bled = LedMaxLevel;
          }  
          show();
          delay(100);          

          for(ledch = 0 ; ledch <= 21 ; ledch++){  
            ch[ledch*2].rled = 0;
            ch[ledch*2].gled = 0;
            ch[ledch*2].bled = LedMaxLevel;
          }
          show();

          state = ST_RUN;
          break;

    case ST_RUN: // 1-23 LEDにbufの内容をシフトして書き込み
          hrote();
            // 中央のLEDにleaf_tblを代入
          if(tblcnt <= 20){

            cal = LedMaxLevel * (float)leaf_tbl[tblcnt] / 10.0;
            ch[21].rled = rbuf[21]=0;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=(char)cal;
            ch[22].rled = rbuf[22]=0;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=(char)cal;
          } else {
            ch[21].rled = rbuf[21]=0;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=0;         

            ch[22].rled = rbuf[22]=0;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=0;

          }

     
         show();
          tblcnt++;
          if(tblcnt >= 40){
            tblcnt=0;
          }

          
         state = ST_WAIT;
//           // Serial.println("ST_WAIT");  
          break;
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            if ( (millis() - Timer2) >= WAIT_TIMER){
              Timer2 = millis();
              state = ST_RUN;
            } 

            break;
        case ST_END:
            return 1;
            break;
        default:
            break;
  }
  return 0;
     
}


// --------------------------------------------------------------------------------
// 11:03 中央から赤・黄で流れる。
// --------------------------------------------------------------------------------
int scene23( void ){

}

// --------------------------------------------------------------------------------
// 11:25 全体赤点滅。全体七色変化点灯
// --------------------------------------------------------------------------------
int scene24( void ){
  enum{
    ST_INIT = 0,
    ST_FAST,
    ST_RUN,
    ST_NEXT,
    ST_WAIT,
    ST_END,
  };
  char t2 = 50;
  char t3 = 15;
  static int NowLevel = 0;
  static int state = ST_INIT;
  static unsigned char tblcnt = 0;
  static char cnt=0;
int i;
              float cal;
  switch(state){
    case ST_INIT:
          allled(0,0,LedMaxLevel);
#if 0
          for(ledch = 0 ; ledch <= 43 ; ledch++){   
            ch[ledch].rled = 0;
            ch[ledch].gled = 0;
            ch[ledch]. bled = LedMaxLevel;
          }  
          show();
#endif
          state = ST_FAST;
//// Serial.println("ST_RUN");
          break;
    
    case ST_FAST:
          allled(LedMaxLevel,0,0);
#if 0
          for(ledch = 0 ; ledch <= 43 ; ledch++){  
            ch[ledch].rled = LedMaxLevel;
            ch[ledch].gled = 0;
            ch[ledch].bled = 0;
          }  
          show();
#endif
          delay(t2);
          allled(0,0,0);
#if 0
          for(ledch = 0 ; ledch <= 43 ; ledch++){  
            ch[ledch].rled = 0;
            ch[ledch].gled = 0;
            ch[ledch].bled = 0;
          }  
          show();
#endif
          delay(t2);
          allled(LedMaxLevel,0,LedMaxLevel);
#if 0
          for(ledch = 0 ; ledch <= 43 ; ledch++){  
            ch[ledch].rled = LedMaxLevel;
            ch[ledch].gled = 0;
            ch[ledch].bled = LedMaxLevel;
          }  
          show();
#endif
          delay(t2);
          allled(0,0,0);
#if 0
          for(ledch = 0 ; ledch <= 43 ; ledch++){  
            ch[ledch].rled = 0;
            ch[ledch].gled = 0;
            ch[ledch].bled = 0;
          }  
          show();
#endif
          delay(t2);
          allled(LedMaxLevel,0,LedMaxLevel);
#if 0          
          for(ledch = 0 ; ledch <= 43 ; ledch++){  
            ch[ledch].rled = LedMaxLevel;
            ch[ledch].gled = 0;
            ch[ledch].bled = LedMaxLevel;
          }  
          show();
#endif
          delay(t2);
          allled(0,0,0);
#if 0
          for(ledch = 0 ; ledch <= 43 ; ledch++){  
            ch[ledch].rled = 0;
            ch[ledch].gled = 0;
            ch[ledch].bled = 0;
          }  
          show();
#endif
          delay(t2);   
          allled(LedMaxLevel,0,0);
#if 0       
          for(ledch = 0 ; ledch <= 43 ; ledch++){  
            ch[ledch].rled = LedMaxLevel;
            ch[ledch].gled = 0;
            ch[ledch].bled = 0;
          }
          show();
#endif
          state = ST_RUN;
          break;

    case ST_RUN: // 1-23 LEDにbufの内容をシフトして書き込み
          for(i=0;i<LedMaxLevel;i++){ // 赤→黄
            allled(LedMaxLevel,i,0);
#if 0  
            for(ledch = 0 ; ledch <= 43 ; ledch++){  
              ch[ledch].rled = LedMaxLevel;
              ch[ledch].gled = i;
              ch[ledch].bled = 0;
              }
            show();
#endif
            delay(t3);

          }
          for(i=0;i<LedMaxLevel;i++){ // 黄→緑
            allled(LedMaxLevel-i,LedMaxLevel,0);
#if 0 
            for(ledch = 0 ; ledch <= 43 ; ledch++){  
              ch[ledch].rled = LedMaxLevel-i;
              ch[ledch].gled = LedMaxLevel;
              ch[ledch].bled = 0;
              }
            show();
#endif
            delay(t3);
          }
          for(i=0;i<LedMaxLevel;i++){ // 緑→水
            allled(0,LedMaxLevel,i);
#if 0 
           for(ledch = 0 ; ledch <= 43 ; ledch++){  
             ch[ledch].rled = 0;
             ch[ledch].gled = LedMaxLevel;
             ch[ledch].bled = i;
            }
#endif
            show();
            delay(t3);
          }
          for(i=0;i<LedMaxLevel;i++){ // 水→青
            allled(0,LedMaxLevel-i,LedMaxLevel);
#if 0 
           for(ledch = 0 ; ledch <= 43 ; ledch++){  
             ch[ledch].rled = 0;
             ch[ledch].gled = LedMaxLevel-i;
             ch[ledch].bled = LedMaxLevel;
            }
            show();
#endif
            delay(t3);

          }
          for(i=0;i<LedMaxLevel;i++){ // 青→桃
            allled(i,0,LedMaxLevel);
#if 0 
           for(ledch = 0 ; ledch <= 43 ; ledch++){  
             ch[ledch].rled = i;
             ch[ledch].gled = 0;
             ch[ledch].bled = LedMaxLevel;
            }
            show();
#endif
            delay(t3);
          }
          for(i=0;i<LedMaxLevel;i++){ // 桃→赤
            allled(LedMaxLevel,0,LedMaxLevel-i);
#if 0 
           for(ledch = 0 ; ledch <= 43 ; ledch++){  
             ch[ledch].rled = LedMaxLevel;
             ch[ledch].gled = 0;
             ch[ledch].bled = LedMaxLevel-i;
            }
            show();
#endif
            delay(t3);

          }
          cnt++;
          if(cnt>=4){
            state = ST_END;
          }          
          break;

        case ST_END:
            return 1;
            break;
        default:
            break;
  }
  return 0;
     
}


// --------------------------------------------------------------------------------
// 11:50 全体七色点灯、中央から流れる　列車移動開始
// --------------------------------------------------------------------------------

void rinit(void){
  for(ledch = 0 ; ledch <= 43 ; ledch++){   
    ch[ledch].rled = rbuf[ledch] = 0;
    ch[ledch].gled = gbuf[ledch] = 0; 
    ch[ledch].bled = bbuf[ledch] = 0;
  }
}
void rote(void){
  for(ledch = 43 ; ledch >= 1 ; ledch--){   
    ch[ledch].rled = rbuf[ledch] = rbuf[ledch-1];
    ch[ledch].gled = gbuf[ledch] = gbuf[ledch-1];
    ch[ledch].bled = bbuf[ledch] = bbuf[ledch-1];
  }
}
void hrote(void){
  for(ledch = 0 ; ledch <= 21 ; ledch++){   
    ch[ledch].rled = rbuf[ledch] = rbuf[ledch+1];
    ch[ledch].gled = gbuf[ledch] = gbuf[ledch+1];
    ch[ledch].bled = bbuf[ledch] = bbuf[ledch+1];
  }  
  for(ledch = 43 ; ledch >= 22 ; ledch--){   
    ch[ledch].rled = rbuf[ledch] = rbuf[ledch-1];
    ch[ledch].gled = gbuf[ledch] = gbuf[ledch-1];
    ch[ledch].bled = bbuf[ledch] = bbuf[ledch-1];
  }    
}



int scene25( void ){
  enum{
    ST_INIT = 0,
    ST_RUN,
    ST_RINC,
    ST_GINC,
    ST_BINC,
    ST_WAIT,
    ST_END,
  };
  static unsigned char tblcnt = 0;
  static int waitcnt = 0;
  static int state = ST_INIT;
  static int r,b,g;
  int cj = 10;
    
  switch(state){
    case ST_INIT:
          r = g = 0;
          b=50;
          Timer1 = millis();
          Timer2 = millis();
          state = ST_RUN;
          break;
    case ST_RUN: // 1-23 LEDにbufの内容をシフトして書き込み
          rote();
          show();
          delay(100);
          state = ST_RINC;
          break;

    case ST_RINC:
          rote();
          show();
          delay(100);          
          r=r+cj;
          if(r > LedMaxLevel){
            r = LedMaxLevel;
            b = b-cj;
            if( b < 0){
              b=0;
              state = ST_GINC;
            }
          }
          ch[0].rled=rbuf[0] = r;
          ch[0].gled=gbuf[0] = g;
          ch[0].bled=bbuf[0] = b;
          break;
    case ST_GINC:
          rote();
          show();
          delay(100);
          g=g+cj;
          if(g > LedMaxLevel){
            g = LedMaxLevel;
            r=r-cj;
            if(r < 0){
              r = 0;
              state = ST_BINC;
            }
          }
          ch[0].rled=rbuf[0] = r;
          ch[0].gled=gbuf[0] = g;
          ch[0].bled=bbuf[0] = b;
          break;
    case ST_BINC:
           rote();
          show();
          delay(100);           
           b=b+cj;
          if(b > LedMaxLevel){
            b = LedMaxLevel;
            g=g-cj;
            if(g < 0){
              g = 0;
              state = ST_WAIT;
            }
          }
          ch[0].rled=rbuf[0] = r;
          ch[0].gled=gbuf[0] = g;
          ch[0].bled=bbuf[0] = b;
          break;
    case ST_WAIT:
            if ( (millis() - Timer1) >= SCENE_TIMER){
              state = ST_END;
              break; 
            }  
            state = ST_RINC;

            break;
        case ST_END:
            rinit();
            show();
            return 1;
            break;
    default:
          break;
  }
  return 0;
}



#if 0

// --------------------------------------------------------------------------------
// 雫
// --------------------------------------------------------------------------------
int scene30( void ){
  enum{
    ST_INIT = 0,
    ST_RUN,
    ST_FADE,
    ST_NEXT,
    ST_WAIT,
    ST_WAIT2,
    ST_END,
  };

  float a = 1.0;
  static float y;
  static float x=0.1;
  static float ti;
  static int nowpos=0;
  static int nextwait;

  static int state = ST_INIT;
  
  switch(state){
    case ST_INIT:
                  rinit();
#if 0
                  for(ledch = 0 ; ledch <= 43 ; ledch++){   
                    ch[ledch].rled = rbuf[0]=0;
                    ch[ledch].gled = gbuf[0]=0; 
                    ch[ledch].bled = bbuf[0]=0;
                  }
#endif
                  ch[0].rled = rbuf[0]=LedMaxLevel;
                  ch[0].gled = gbuf[0]=LedMaxLevel;
                  ch[0].bled = bbuf[0]=LedMaxLevel;
                  show();
                  Timer2 = millis();
                  state = ST_RUN;
                  break;
    case ST_RUN:
                  y = a * x;
                  x=x+0.1;
                  ti = 1.0/y;
                  state = ST_WAIT;
                  break;
    case ST_WAIT:
                  if ( (millis() - Timer2) >= ti*10){
                    ch[nowpos].rled = rbuf[nowpos]=0;
                    ch[nowpos].gled = gbuf[nowpos]=0;
                    ch[nowpos].bled = bbuf[nowpos]=0;
                    show();
                    nowpos++;
                    if(nowpos>=45){
                      Timer2 = millis();
                      nextwait = random(1,10);
                      state = ST_WAIT2;
                      break;
                    }
                    state = ST_NEXT;
                    break; 
                 }
                  break;
    case ST_WAIT2:
                  if ( (millis() - Timer2) >= nextwait*100){
                    x=0.1;
                    nowpos=0;
                  ch[nowpos].rled = rbuf[nowpos]=LedMaxLevel;
                  ch[nowpos].gled = gbuf[nowpos]=LedMaxLevel;
                  ch[nowpos].bled = bbuf[nowpos]=LedMaxLevel;
                  show();
                    Timer2 = millis();
                    state = ST_RUN;
                    break;
                  }
                  break;
    case ST_NEXT:

                  ch[nowpos].rled = rbuf[nowpos]=LedMaxLevel;
                  ch[nowpos].gled = gbuf[nowpos]=LedMaxLevel;
                  ch[nowpos].bled = bbuf[nowpos]=LedMaxLevel;
                  show();
                  Timer2 = millis();
                  state = ST_RUN;
                  break;                
    case ST_END:
                  break;
          
  }
}


// --------------------------------------------------------------------------------
// たまる雫
// --------------------------------------------------------------------------------
int scene31( void ){
  enum{
    ST_INIT = 0,
    ST_RUN,
    ST_FADE,
    ST_NEXT,
    ST_WAIT,
    ST_WAIT2,
    ST_END,
  };

  float a = 1.0;
  static float y;
  static float x=0.1;
  static float ti;
  static int nowpos=0;
  static int nextwait;
  static int accumulation = 45;
  static int state = ST_INIT;
  
  switch(state){
    case ST_INIT:
                  rinit();
#if 0
                  for(ledch = 0 ; ledch <= 43 ; ledch++){   
                    ch[ledch].rled = rbuf[0]=0;
                    ch[ledch].gled = gbuf[0]=0; 
                    ch[ledch].bled = bbuf[0]=0;
                  }
#endif
                  ch[0].rled = rbuf[0]=LedMaxLevel;
                  ch[0].gled = gbuf[0]=LedMaxLevel;
                  ch[0].bled = bbuf[0]=LedMaxLevel;
                  show();
                  Timer2 = millis();
                  state = ST_RUN;
                  break;
    case ST_RUN:
                  y = a * x;
                  x=x+0.1;
                  ti = 1.0/y;
                  state = ST_WAIT;
                  break;
    case ST_WAIT:
                  if ( (millis() - Timer2) >= ti*10){
                    ch[nowpos].rled = rbuf[nowpos]=0;
                    ch[nowpos].gled = gbuf[nowpos]=0;
                    ch[nowpos].bled = bbuf[nowpos]=0;
                    show();
                    nowpos++;
                    if(nowpos>=accumulation){
                    ch[nowpos].rled = rbuf[nowpos]=LedMaxLevel;
                    ch[nowpos].gled = gbuf[nowpos]=LedMaxLevel;
                    ch[nowpos].bled = bbuf[nowpos]=LedMaxLevel;                   
                    show();                      
                      
                      accumulation--;
                      if(accumulation<0){
                        accumulation=45;
                      }
                      Timer2 = millis();
                      nextwait = random(1,10);
                      state = ST_WAIT2;
                      break;
                    }
                    state = ST_NEXT;
                    break; 
                 }
                  break;
    case ST_WAIT2:
                  if ( (millis() - Timer2) >= nextwait*100){
                    x=0.1;
                    nowpos=0;
                  ch[nowpos].rled = rbuf[nowpos]=LedMaxLevel;
                  ch[nowpos].gled = gbuf[nowpos]=LedMaxLevel;
                  ch[nowpos].bled = bbuf[nowpos]=LedMaxLevel;
                  show();
                    Timer2 = millis();
                    state = ST_RUN;
                    break;
                  }
                  break;
    case ST_NEXT:

                  ch[nowpos].rled = rbuf[nowpos]=LedMaxLevel;
                  ch[nowpos].gled = gbuf[nowpos]=LedMaxLevel;
                  ch[nowpos].bled = bbuf[nowpos]=LedMaxLevel;
                  show();
                  Timer2 = millis();
                  state = ST_RUN;
                  break;                
    case ST_END:
                  break;
          
  }
}

// --------------------------------------------------------------------------------
// はずむ雫
// --------------------------------------------------------------------------------
int scene32( void ){
  enum{
    ST_INIT = 0,
    ST_RUN,
    ST_FADE,
    ST_NEXT,
    ST_WAIT,
    ST_WAIT2,
    ST_END,
  };

  static float g = 9.8;    // 重力加速度
  float ElapsedTime = 0.005;
  static float y=0;
  static float vy=4;
  static int BounsCnt =0;
  static int state = ST_INIT;
    
  switch(state){
    case ST_INIT:
                  rinit();
#if 0
                  for(ledch = 0 ; ledch <= 43 ; ledch++){   
                    ch[ledch].rled = 0;
                    ch[ledch].gled = 0; 
                    ch[ledch].bled = 0;
                  }
#endif
                  ch[0].rled = LedMaxLevel;
                  ch[0].gled = LedMaxLevel;
                  ch[0].bled = LedMaxLevel;
                  show();
                  y = 0;
                  vy = 2;
                  BounsCnt = 0;
                  state = ST_RUN;
                  break;
    case ST_RUN:
                  ch[(int)y].rled = 0;
                  ch[(int)y].gled = 0;
                  ch[(int)y].bled = 0;
                  
                  y = y + ((vy*ElapsedTime)+( 1/2 * g * ElapsedTime * ElapsedTime)); // 等加速度直線運動の公式

                  ch[(int)y].rled = LedMaxLevel;
                  ch[(int)y].gled = LedMaxLevel;
                  ch[(int)y].bled = LedMaxLevel;
                  show();

                  if((int)y >= 45){     // 床に届いた
                    BounsCnt ++;
                    if(BounsCnt >= 8){  // 8回弾んだら次
                      state = ST_INIT;
                    }
                    y = 44;
                    vy = -vy * 0.7;     // 跳ね返った時の速度を3割減
                  } else{
                    vy = vy + (g*ElapsedTime);
                  }
                  break;
    case ST_END:
                  break;
          
  }
}

#endif




//--------------------------------------------------
// ピンク細かくLEAF回転
//--------------------------------------------------
void chCont10( void ){
  static unsigned char tblcnt = 0;
  static int waitcnt = 0;
  static int state = 0;
  
  switch(state){
    case 0:
          state = 1;
          break;
    case 1: // 1-23 LEDにbufの内容をシフトして書き込み
          rote();
#if 0
          for(ledch = 43 ; ledch >= 1 ; ledch--){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch-1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch-1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch-1];
          }  
#endif
            // 先頭のLEDにleaf_tblを代入
          if(tblcnt < 8){
            float cal;
            cal = LedMaxLevel * (float)leaf_tbl2[tblcnt] / 10.0;
            ch[0].rled = rbuf[0]=(char)cal;
            ch[0].gled = gbuf[0]=0;
            ch[0].bled = bbuf[0]=(char)cal;
          } else {
            ch[0].rled = rbuf[0]=0;
            ch[0].gled = gbuf[0]=0;
            ch[0].bled = bbuf[0]=0;            
          }
          
          tblcnt++;
          if(tblcnt >= 16){
            tblcnt=0;
            state = 2;
          }
          break;
    case 2: // から送り
          rote();
#if 0
          for(ledch = 43 ; ledch >= 1 ; ledch--){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch+1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch+1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch+1];
          } 
#endif
          waitcnt++;
          if(waitcnt>10){
            waitcnt = 0;
            state = 1;    
          }
          break;
    default:
          break;
  }
}



#if 0
// 3:59 ピンク、青、Leafbuf
// 47なので43に直す必要あり
void chCont8( void ){
  static unsigned char tblcnt = 0;
  static int waitcnt = 0;
  static int state = 0;
  static char even=0;
  switch(state){
    case 0:
          rinit();
          state = 1;
          break;
    case 1: // 1-23 LEDにbufの内容をシフトして書き込み
          hrote();
            // 端のLEDにleaf_tblを代入 
          if(tblcnt < 20){
            float cal;
            cal = LedMaxLevel * (float)leaf_tbl[tblcnt] / 10.0;
            ch[21].rled = rbuf[21]=(char)cal;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=LedMaxLevel;

            ch[22].rled = rbuf[22]=(char)cal;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=LedMaxLevel;

          } else {
            ch[21].rled = rbuf[21]=0;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=0;         

            ch[22].rled = rbuf[22]=0;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=0;
          }

          if(even==1){
            tblcnt++;
            even = 0;
          } else {
            even=1;
          }
          if(tblcnt >= 20){
            tblcnt=0;
            state = 2;
          }
          break;
    case 2: // から送り
          hrote();
          waitcnt++;
          if(waitcnt>5){
            waitcnt = 0;
            state = 3;    
          }
          break;

    case 3: // 1-23 LEDにbufの内容をシフトして書き込み
          rote();
            // 中央のLEDにleaf_tblを代入
          if(tblcnt < 24){
            float cal;
            cal = LedMaxLevel * (float)leaf_tbl[tblcnt] / 10.0;
            ch[0].rled = rbuf[0]=LedMaxLevel;
            ch[0].gled = gbuf[0]=(char)cal;
            ch[0].bled = bbuf[0]=LedMaxLevel;
          }
          
          if(even==1){
            tblcnt++;
            even = 0;
          } else {
            even=1;
          }
          if(tblcnt >= 20){
            tblcnt=0;
            state = 4;
          }
          break;
    case 4: // から送り
          rote();
          waitcnt++;
          if(waitcnt>10){
            waitcnt = 0;
            state = 3;    
          }
          break;


    default:
          break;
  }
}

#endif

#if 0
// 3:53 ピンク点滅(13回)
int chCont7( int initstate ){
  static int loopcnt = 0;
  static int bright = 0;
  static int state = 0;
  static float timm = 0;

  if(initstate == 1){
    state = 0;
  }
  
  switch(state){
    case 0:
  // Serial.println("state1");   
          loopcnt = 0;
          bright = 0;
          timm = 0;
          state = 1;
          break;
    case 1: // ピンク点灯
          allled(LedMaxLevel,0,LedMaxLevel);
          for(ledch = 0 ; ledch <= MAXLED-1  ; ledch++){   
            ch[ledch].rled = LedMaxLevel;
            ch[ledch].gled = 0; 
            ch[ledch].bled = LedMaxLevel;
          }
          timm = 200; // 500ms
          gPrevious = millis();
          state = 2;
  // Serial.println("state2");            
          break;
    case 2:
          if ( (millis() - gPrevious) >= timm){
            state = 3; 
  // Serial.println("state3");  
          }
          break;
    case 3: // ピンク消灯
          for(ledch = 0 ; ledch <= MAXLED-1  ; ledch++){   
            ch[ledch].rled = 0;
            ch[ledch].gled = 0; 
            ch[ledch].bled = 0;
          }
          timm = 200; // 500ms
          gPrevious = millis();
          state = 4;
  // Serial.println("state4");            
          break;
    case 4:
          if(loopcnt>=10){
            loopcnt = 0;
            state = 5;
            break;
          }
          if ( (millis() - gPrevious) >= timm){
            state = 1;
            loopcnt++; 
  // Serial.println("state1");  
          }
          break;

    case 5: // 青点滅
          for(ledch = 0 ; ledch <= MAXLED-1  ; ledch=ledch+2){   
            ch[ledch].rled = 0;
            ch[ledch].gled = 0; 
            ch[ledch].bled = LedMaxLevel;
          }
          timm = 50; // 100ms
          gPrevious = millis();
          state = 6;
// Serial.println("state6");            
          break;
    case 6:
          if ( (millis() - gPrevious) >= timm){
            state = 7; 
// Serial.println("state7");  
          }
          break;
    case 7: // 青点滅
          for(ledch = 0 ; ledch <= MAXLED-1  ; ledch=ledch+2){   
            ch[ledch].rled = 0;
            ch[ledch].gled = 0; 
            ch[ledch].bled = 0;
          }
          timm = 50; // 100ms
          gPrevious = millis();
          state = 8;
  // Serial.println("state8");            
          break;
    case 8:
          if ( (millis() - gPrevious) >= timm){
            state = 5;
            loopcnt++; 
  // Serial.println("state5");  
          }
          if(loopcnt>=10){
            state = 9;
            break;
          }
          
          break;
    case 9:
  // Serial.println("state9"); 
          return 1;
          break;
    default:
          break;        
  }
}

#endif

void chCont5( void ){
  static int loopcnt = 0;
  static int bright = 0;
  static int state = 0;
  static float timm = 0;
  
  switch(state){
    case 0:
  // Serial.println("state1");   
          state = 1;
          break;
    case 1: // 白点灯後5秒保持
          for(ledch = 0 ; ledch <= MAXLED-1  ; ledch++){   
            ch[ledch].rled = LedMaxLevel;
            ch[ledch].gled = LedMaxLevel; 
            ch[ledch].bled = LedMaxLevel;
          }
          bright = LedMaxLevel;
          timm = 4000; // 5秒
          gPrevious = millis();
          state = 2;
  // Serial.println("state2");            
          break;
    case 2:
          if ( (millis() - gPrevious) >= timm){
            state = 3; 
  // Serial.println("state3");  
  }
          break;
    case 3:
          for(ledch = 0 ; ledch <= MAXLED-1  ; ledch++){   
            ch[ledch].rled = bright;
            ch[ledch].gled = bright;
            ch[ledch].bled = LedMaxLevel;          
          }
          bright--;
          if(bright<=0){
              // Serial.println("state4");  
            state = 4;
          }
          break;
    case 4:
          for(ledch = 0 ; ledch <= MAXLED-1  ; ledch++){   
            ch[ledch].rled = bright;
            ch[ledch].gled = 0;
            ch[ledch].bled = LedMaxLevel-bright;          
          }
          bright++;
          if(bright>=LedMaxLevel){
            // Serial.println("state5");
            state = 5;
          }
          break;
    case 5:
          for(ledch = 0 ; ledch <= MAXLED-1  ; ledch++){   
            ch[ledch].rled = bright;
            ch[ledch].gled = 0;
            ch[ledch].bled = LedMaxLevel-bright;          
          }
          bright--;

          if(loopcnt>=12){
            loopcnt = 0;
            // Serial.println("state6");  
            state = 6;
            break;
          }
          if(bright<=0){
            loopcnt++;
            // Serial.print(loopcnt);
                        // Serial.print(",");    
            // Serial.println("state4");  
            state = 4;
          }
          break;
    case 6:// 動画 3:40 赤点滅
          for(ledch = 0 ; ledch <= MAXLED-1  ; ledch++){   
            ch[ledch].rled = LedMaxLevel;
            ch[ledch].gled = 0;
            ch[ledch].bled = 0;          
          }
          state = 7;
            // Serial.println("state7");      

          break;
    case 7:
          for(ledch = 0 ; ledch <= MAXLED-1  ; ledch++){   
            ch[ledch].rled = 0;
            ch[ledch].gled = 0;
            ch[ledch].bled = 0;          
          }
          loopcnt++;
          if(loopcnt>=20){
            loopcnt = 0;
            state = 8;
              // Serial.println("state8");  
            break;
          }
            state = 6;
          break;
    case 8:// 動画 3:40 ぱっと白く光る
          for(ledch = 0 ; ledch <= MAXLED-1  ; ledch++){   
            ch[ledch].rled = LedMaxLevel;
            ch[ledch].gled = LedMaxLevel;
            ch[ledch].bled = LedMaxLevel;          
          }
          delay(1000);
          state = 9;    
          break;
    case 9://白から徐々に黒フェードアウト
          for(ledch = 0 ; ledch <= MAXLED-1 ; ledch++){   
            ch[ledch].rled--;
            if(ch[ledch].rled<=0)
              ch[ledch].rled=0;
      
            ch[ledch].gled--;
            if(ch[ledch].gled<=0)
              ch[ledch].gled=0;
    
            ch[ledch].bled--;
            if(ch[ledch].bled<=0)
            ch[ledch].bled=0;
          }
          break;   
    default:
          break;
  }
}


//--------------------------------------------------
// 赤青LEAF回転
//--------------------------------------------------

void chCont4( void ){
  static unsigned char tblcnt = 0;
  static int waitcnt = 0;
  static int state = 0;
  
  switch(state){
    case 0:
          state = 1;
          break;
    case 1: // 1-23 LEDにbufの内容をシフトして書き込み
          for(ledch = 21 ; ledch >= 1 ; ledch--){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch-1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch-1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch-1];
          }  
          for(ledch = 22 ; ledch <= 42 ; ledch++){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch+1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch+1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch+1];
          }  
            // 端のLEDにleaf_tblを代入
          if(tblcnt < 22){
            float cal;
            cal = LedMaxLevel * (float)leaf_tbl[tblcnt] / 10.0;
            ch[0].rled = rbuf[0]=(char)cal;
            ch[0].gled = gbuf[0]=0;
            ch[0].bled = bbuf[0]=0;

            ch[43].rled = rbuf[43]=0;
            ch[43].gled = gbuf[43]=0;
            ch[43].bled = bbuf[43]=(char)cal;

          } else {
            ch[0].rled = rbuf[0]=0;
            ch[0].gled = gbuf[0]=0;
            ch[0].bled = bbuf[0]=0;            

            ch[43].rled = rbuf[43]=0;
            ch[43].gled = gbuf[43]=0;
            ch[43].bled = bbuf[43]=0;
          }
          
          tblcnt++;
          if(tblcnt >= 22){
            tblcnt=0;
            state = 2;
          }
          break;
    case 2: // から送り
          for(ledch = 21 ; ledch >= 1 ; ledch--){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch-1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch-1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch-1];
          }  
          for(ledch = 22 ; ledch <= 42 ; ledch++){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch+1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch+1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch+1];
          }  
          waitcnt++;
          if(waitcnt>10){
            waitcnt = 0;
            state = 1;    
          }
          break;
    default:
          break;
  }
}


//--------------------------------------------------
// 赤LEAF回転
//--------------------------------------------------
void chCont3( void ){
  static unsigned char tblcnt = 0;
  static int waitcnt = 0;
  static int state = 0;
  
  switch(state){
    case 0:
          state = 1;
          break;
    case 1: // 1-23 LEDにbufの内容をシフトして書き込み
          hrote();
#if 0
          for(ledch = 0 ; ledch <= 21 ; ledch++){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch+1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch+1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch+1];
          }  
          for(ledch = 43 ; ledch >= 22 ; ledch--){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch-1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch-1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch-1];
          }  
#endif
            // 中央のLEDにleaf_tblを代入
          if(tblcnt < 22){
            float cal;
            cal = LedMaxLevel * (float)leaf_tbl[tblcnt] / 10.0;
            ch[21].rled = rbuf[21]=(char)cal;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=0;

            ch[22].rled = rbuf[22]=(char)cal;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=0;

          } else {
            ch[21].rled = rbuf[21]=0;
            ch[21].gled = gbuf[21]=0;
            ch[21].bled = bbuf[21]=0;            

            ch[22].rled = rbuf[22]=0;
            ch[22].gled = gbuf[22]=0;
            ch[22].bled = bbuf[22]=0;
          }
          
          tblcnt++;
          if(tblcnt >= 22){
            tblcnt=0;
            state = 2;
          }
          break;
    case 2: // から送り
          hrote();
#if 0
          for(ledch = 0 ; ledch <= 21 ; ledch++){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch+1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch+1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch+1];
          } 

          for(ledch = 43 ; ledch >= 22 ; ledch--){   
            ch[ledch].rled = rbuf[ledch] = rbuf[ledch-1];
            ch[ledch].gled = gbuf[ledch] = gbuf[ledch-1];
            ch[ledch].bled = bbuf[ledch] = bbuf[ledch-1];
          }  
#endif
          waitcnt++;
          if(waitcnt>10){
            waitcnt = 0;
            state = 1;    
          }
          break;
    default:
          break;
  }
}








// --------------------------------------------------------------------------------
// 流れる虹LED
// --------------------------------------------------------------------------------
void chCont( char ich ){
  switch(ch[ich].state){

    case 0: 
            ch[ich].nowwait--;
            if(ch[ich].nowwait <= 0){
              ch[ich].state = 1;
              ch[ich].nowwait = ch[ich].wait;
              break;
            }
            break;
    case 1: // 赤増加、緑LOW、青LOW
            ch[ich].rled ++;
            if(ch[ich].rled >= LedMaxLevel){
              ch[ich].state = 2;
            }
            break;
    case 2: // 赤MAX、緑増加、青LOW
            ch[ich].gled ++;
            if(ch[ich].gled >= LedMaxLevel){
              ch[ich].state = 3;
            }
            break;
    case 3: // 赤減少、緑MAX、青LOW
            ch[ich].rled--;
            if(ch[ich].rled <= 0){
              ch[ich].state = 4;
            }
            break;
    case 4: // 赤LOW、緑MAX、青増加
            ch[ich].bled ++;
            if(ch[ich].bled >= LedMaxLevel){
              ch[ich].state = 5;
            }
            break;
    case 5: // 赤LOW、緑減少、青MAX
            ch[ich].gled --;
            if(ch[ich].gled <= 0){
              ch[ich].state = 6;
            }
            break;
    case 6: // 赤増加、緑LOW、青MAX
            ch[ich].rled ++;
            if(ch[ich].rled >= LedMaxLevel){
              ch[ich].state = 7;
            }
            break;
    case 7: // 赤MAX、赤LOW、青減少
            ch[ich].bled --;
            if(ch[ich].bled <= 0){
              ch[ich].state = 2;
            }
            break;

    default:
            break;
  }
}
