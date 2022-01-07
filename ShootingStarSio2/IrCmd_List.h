//--------------------------------------------------------------------------------
// NEC Format (TOSHIBA レグザリモコン)
// SONYTV Format
// SONYMD Format
//--------------------------------------------------------------------------------

#ifndef IRCMD_LIST_H_
#define IRCMD_LIST_H_

#define IRCMD_NEC_POWER		  0x2FD48B7		  // 電源
#define IRCMD_NEC_SPEEDUP	  0x2FD58A7			// 音量+
#define IRCMD_NEC_SPEEDDOWN	0x2FD7887			// 音量-
#define IRCMD_NEC_FUNC1		  0x2FD807F			// ch1
#define IRCMD_NEC_FUNC2		  0x2FD40BF			// ch2
#define IRCMD_NEC_FUNC3		  0x2FDC03F			// ch3
#define IRCMD_NEC_FUNC4		  0x2FD20DF			// ch4
#define IRCMD_NEC_FUNC5		  0x2FDA05F			// ch5
#define IRCMD_NEC_FUNC6		  0x2FD609F			// ch6
#define IRCMD_NEC_FUNC7		  0x2FDE01F			// ch7
#define IRCMD_NEC_FUNC8		  0x2FD10EF			// ch8
#define IRCMD_NEC_FUNC9		  0x2FD906F			// ch9
#define IRCMD_NEC_FUNC10	  0x2FD50AF			// ch10
#define IRCMD_NEC_FUNC11	  0x2FDD02F			// ch11
#define IRCMD_NEC_FUNC12	  0x2FD30CF			// ch12
#define IRCMD_NEC_STOP      0x27DD42B     // stop
#define IRCMD_NEC_PLAY      0x27DB44B     // play


#define IRCMD_SONYTV_POWER     0xA8B4F    // 電源
#define IRCMD_SONYTV_SPEEDUP   0x2FD58A7  // 音量+
#define IRCMD_SONYTV_SPEEDDOWN 0x2FD7887  // 音量-
#define IRCMD_SONYTV_FUNC1     0xB4F      // ch1
#define IRCMD_SONYTV_FUNC2     0x80B4F    // ch2
#define IRCMD_SONYTV_FUNC3     0x40B4F    // ch3
#define IRCMD_SONYTV_FUNC4     0xC0B4F    // ch4
#define IRCMD_SONYTV_FUNC5     0x20B4F    // ch5
#define IRCMD_SONYTV_FUNC6     0xA0B4F    // ch6
#define IRCMD_SONYTV_FUNC7     0x60B4F    // ch7
#define IRCMD_SONYTV_FUNC8     0xE0B4F    // ch8
#define IRCMD_SONYTV_FUNC9     0x10B4F    // ch9
#define IRCMD_SONYTV_FUNC10    0x90B4F    // ch10
#define IRCMD_SONYTV_FUNC11    0x50B4F    // ch11
#define IRCMD_SONYTV_FUNC12    0xD0B4F    // ch12
#define IRCMD_SONYTV_STOP      0xB8B4F    // stop
#define IRCMD_SONYTV_PLAY      0x58B4F    // play


#define IRCMD_SONYMD_POWER     0x5422     // 電源
#define IRCMD_SONYMD_SPEEDUP   0x2422     // 音量+
#define IRCMD_SONYMD_SPEEDDOWN 0x6422     // 音量-
#define IRCMD_SONYMD_FUNC1     0x26       // ch1
#define IRCMD_SONYMD_FUNC2     0x4026     // ch2
#define IRCMD_SONYMD_FUNC3     0x2026     // ch3
#define IRCMD_SONYMD_FUNC4     0x6026     // ch4
#define IRCMD_SONYMD_FUNC5     0x1026     // ch5
#define IRCMD_SONYMD_FUNC6     0x5026     // ch6
#define IRCMD_SONYMD_FUNC7     0x3026     // ch7
#define IRCMD_SONYMD_FUNC8     0x7026     // ch8
#define IRCMD_SONYMD_FUNC9     0x826      // ch9
#define IRCMD_SONYMD_STOP      0x126      // stop
#define IRCMD_SONYMD_PLAY      0x2126     // play


#endif /* DIRCMD_LIST_H_ */
