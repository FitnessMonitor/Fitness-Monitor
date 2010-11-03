#define LCD_LINES           2     // number of visible lines of the display 
#define LCD_DISP_LENGTH    16     // visibles characters per line of the display 
#define LCD_LINE_LENGTH  0x40     // internal line length of the display
#define LCD_START_LINE1  0x00     // DDRAM address of first char of line 1 
#define LCD_START_LINE2  0x40     // DDRAM address of first char of line 2 

#define LCD_PORT         PORTD        // port for the LCD lines
#define LCD_DATA0_PORT   LCD_PORT     // port for 4bit data bit 0
#define LCD_DATA1_PORT   LCD_PORT     // port for 4bit data bit 1
#define LCD_DATA2_PORT   LCD_PORT     // port for 4bit data bit 2
#define LCD_DATA3_PORT   LCD_PORT     // port for 4bit data bit 3
#define LCD_DATA0_PIN    0            // pin for 4bit data bit 0 
#define LCD_DATA1_PIN    1            // pin for 4bit data bit 1 
#define LCD_DATA2_PIN    2            // pin for 4bit data bit 2 
#define LCD_DATA3_PIN    3            // pin for 4bit data bit 3 
#define LCD_RS_PORT      LCD_PORT     // port for RS line        
#define LCD_RS_PIN       4            // pin  for RS line        
#define LCD_RW_PORT      LCD_PORT     // port for RW line        
#define LCD_RW_PIN       5            // pin  for RW line        
#define LCD_E_PORT       LCD_PORT     // port for Enable line    
#define LCD_E_PIN        6            // pin  for Enable line    

/* instruction register bit positions, see HD44780U data sheet */
#define LCD_CLR               0      // DB0: clear display                  
#define LCD_HOME              1      // DB1: return to home position        
#define LCD_ENTRY_MODE        2      // DB2: set entry mode                 
#define LCD_ENTRY_INC         1      // DB1: 1=increment, 0=decrement     
#define LCD_ENTRY_SHIFT       0      // DB2: 1=display shift on           
#define LCD_ON                3      // DB3: turn lcd/cursor on           
#define LCD_ON_DISPLAY        2      // DB2: turn display on                        
#define LCD_FUNCTION          5      // DB5: function set               
#define LCD_FUNCTION_2LINES   3      // DB3: two lines (0->one line)       
#define LCD_CGRAM             6      // DB6: set CG RAM address           
#define LCD_DDRAM             7      // DB7: set DD RAM address           
#define LCD_BUSY              7      // DB7: LCD is busy                  

/* display on/off, cursor on/off, blinking char at cursor position */
#define LCD_DISP_OFF             0x08   // display off                            
#define LCD_DISP_ON              0x0C   // display on, cursor off                                       

/* function set: set interface data length and number of display lines */
#define LCD_FUNCTION_4BIT_1LINE  0x20   // 4-bit interface, single line, 5x7 dots 
#define LCD_FUNCTION_4BIT_2LINES 0x28   // 4-bit interface, dual line,   5x7 dots 

#define LCD_MODE_DEFAULT     ((1<<LCD_ENTRY_MODE) | (1<<LCD_ENTRY_INC) )

extern void lcd_init(uint8_t dispAttr);

extern void lcd_clrscr(void);

extern void lcd_putc(char c);

extern void lcd_puts(const char *s);

extern void lcd_command(uint8_t cmd);

