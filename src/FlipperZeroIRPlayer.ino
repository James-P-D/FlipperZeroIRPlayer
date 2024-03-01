#include "M5Cardputer.h"
#include "M5GFX.h"
#include "SPI.h"
#include "SD.h"
#include "IRremote.hpp"

#define SD_SPI_SCK_PIN  40
#define SD_SPI_MISO_PIN 39
#define SD_SPI_MOSI_PIN 14
#define SD_SPI_CS_PIN   12
#define DISABLE_CODE_FOR_RECEIVER  // Disables restarting receiver after each
                                   // send. Saves 450 bytes program memory and
                                   // 269 bytes RAM if receiving functions are
                                   // not used.
#define SEND_PWM_BY_TIMER
#define IR_TX_PIN       44

#define MAX_RAW_SIZE    1000
#define NAME_STR        "name: "
#define TYPE_STR        "type: "
#define PROTOCOL_STR    "protocol: "
#define ADDRESS_STR     "address: "
#define COMMAND_STR     "command: "
#define FREQUENCY_STR   "frequency: "
#define DUTY_CYCLE_STR  "duty_cycle: "
#define DATA_STR        "data: "

IRsend irsend;
M5Canvas canvas(&M5Cardputer.Display);
String prompt = "> ";

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextSize(0.4);
    M5Cardputer.Display.drawRect(0, 0, M5Cardputer.Display.width(), M5Cardputer.Display.height() - 28, WHITE);
    M5Cardputer.Display.setTextFont(&fonts::FreeMono18pt7b);

    M5Cardputer.Display.fillRect(0, M5Cardputer.Display.height() - 4, M5Cardputer.Display.width(), 4, WHITE);

    canvas.setTextFont(&fonts::FreeMono18pt7b);
    canvas.setTextSize(0.4);
    canvas.createSprite(M5Cardputer.Display.width() - 8, M5Cardputer.Display.height() - 36);
    canvas.setTextScroll(true);
    canvas.println(
      "Enter path to Flipper Zero\n" 
      ".IR file and press <ENTER>");
    canvas.pushSprite(4, 4);
    M5Cardputer.Display.drawString(prompt, 4, M5Cardputer.Display.height() - 24);

    SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
    if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000)) {
        canvas.println("Card failed, or not present");
        return;
    }

    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
        canvas.println("No SD card attached");
        return;
    }

    IrSender.begin(DISABLE_LED_FEEDBACK);  // Start with IR_SEND_PIN as send pin
    IrSender.setSendPin(IR_TX_PIN);
}

byte nibble(char c) {
  if ((c >= '0') && (c <= '9')) {
      return c - '0';
  }

  if ((c >= 'a') && (c <= 'f')) {
      return c - 'a' + 10;
  }

  if ((c >= 'A') && (c <= 'F')) {
      return c - 'A' + 10;
  }

  return 0;  // Not a valid hexadecimal character
}


bool string_to_bytes(String s, byte *a){
    if (s.length() != 11) {      
        return false;
    }

    for (int i=0; i < s.length(); i++) {
        int byte_index = i / 3;
        int nibble_index = i % 3;
        if (nibble_index == 0) {
            a[byte_index] = nibble(s[i]);
        } else if (nibble_index == 1) {
            a[byte_index] << 4;
            a[byte_index] += nibble(s[i]);
        } // Don't do anything if 'nibble_index' is 2 since this is a space
    }

    return true;
}

void send_parsed(String ir_name, String ir_address, String ir_command) {   
    byte address_bytes[4];
    byte command_bytes[4];
    if (!string_to_bytes(ir_address, address_bytes)) {
        canvas.println("ERROR: cannot parse " + ir_address);
        return;
    }

    if (!string_to_bytes(ir_command, command_bytes)) {
        canvas.println("ERROR: cannot parse " + ir_command);
        return;
    }

    uint16_t address_final = 0;
    address_final = address_bytes[0];
    address_final <<= 8;
    address_final += address_bytes[1];
    
    uint16_t command_final = 0;
    command_final = command_bytes[0];
    command_final <<= 8;
    command_final += command_bytes[1];
  
    canvas.println("name:" + ir_name);
    canvas.println("addr:" + ir_address);
    canvas.println("cmd: " + ir_command);

    IrSender.sendNEC(address_final, command_final, 0);
    delay(100);

    canvas.pushSprite(4, 4);
}

void send_raw(String ir_name, String ir_frequency, String ir_duty_cycle, String ir_data) {
    canvas.println("ERROR: Sorry, not implemented yet!");
    canvas.pushSprite(4, 4);
    return;
  
    unsigned int raw_data[MAX_RAW_SIZE];
    int raw_data_size = 0;
    String item = "";
    for(int i = 0; i < ir_data.length(); i++) {
        char ch = ir_data[i];
        if (i == MAX_RAW_SIZE) {
            canvas.println("ERROR: Raw data too long");
            canvas.pushSprite(4, 4);
            return;
        }
        
        if ((ch == ' ') || (i == (ir_data.length()-1))) {
            if (item.length() > 0) {
                raw_data[i] = item.toInt();           
                item = "";
                raw_data_size++;
            }
        } else {
            item += String(ch);
        }
    }

    if (raw_data_size == 0) {
        canvas.println("ERROR: no raw data to send");
        canvas.pushSprite(4, 4);
    } else {
        //irsend.sendRaw(raw_data, raw_data_size, 38); // 38 = kHz

        int khz = 38; // 38kHz carrier frequency for the NEC protocol
        unsigned int irSignal[] = {9000, 4500, 560, 560, 560, 560, 560, 1690, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 560, 560, 560, 560, 560, 560, 1690, 560, 560, 560, 560, 560, 560, 560, 560, 560, 1690, 560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 39416, 9000, 2210, 560}; //AnalysIR Batch Export (IRremote) - RAW
        //irsend.sendRaw(irSignal, sizeof(irSignal) / sizeof(irSignal[0]), khz); //Note the approach used to automatically calculate the size of the array.
         
        canvas.println("name:" + ir_name);
        canvas.println("freq:" + ir_frequency);
        canvas.println("dcyc:" + ir_duty_cycle);
        canvas.println("data:" + String(raw_data[0]) + " [..] " + String(raw_data[raw_data_size-1]));
        canvas.pushSprite(4, 4);
    }
}

void loop() {
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

            for (auto i : status.word) {
                prompt += i;
            }

            if (status.del) {
                prompt.remove(prompt.length() - 1);
            }

            if (status.enter) {                
                prompt.remove(0, 2);
                //prompt = "/ir/off.ir";
                //prompt = "/ir/raw.ir";

                File file = SD.open(prompt);
                if (!file) {
                    canvas.println("Failed to open file for reading: " + prompt);
                } else {
                    canvas.println("Successfully read from " + prompt);
                    String line_read = "";
                    String ir_name = "";
                    String ir_type = "";
                    String ir_protocol = "";
                    String ir_address = "";
                    String ir_command = "";
                    String ir_frequency = "";
                    String ir_duty_cycle = "";
                    String ir_data = "";
                    
                    while (file.available()) {
                        auto c = file.read();
                        if ((c == 0x0D) || (c == 0x0A)) {
                            if (line_read.length() > 0) {
                                if (line_read.startsWith(NAME_STR)) {
                                    line_read.remove(0, 6);
                                    ir_name = line_read;
                                } else if (line_read.startsWith(TYPE_STR)) {
                                    line_read.remove(0, 6);
                                    ir_type = line_read;
                                } else if (line_read.startsWith(PROTOCOL_STR)) {
                                    line_read.remove(0, 10);
                                    ir_protocol = line_read;
                                } else if (line_read.startsWith(ADDRESS_STR)) {
                                    line_read.remove(0, 9);
                                    ir_address = line_read;
                                } else if (line_read.startsWith(COMMAND_STR)) {
                                    line_read.remove(0, 9);
                                    ir_command = line_read;
                                    if (ir_type == "parsed") {
                                        send_parsed(ir_name, ir_address, ir_command);
                                    } else {
                                        canvas.println("ERROR. " + ir_type + " but using 'command'");
                                    }
                                } else if (line_read.startsWith(FREQUENCY_STR)) {
                                    line_read.remove(0, 11);
                                    ir_frequency = line_read;
                                } else if (line_read.startsWith(DUTY_CYCLE_STR)) {
                                    line_read.remove(0, 12);
                                    ir_duty_cycle = line_read;
                                } else if (line_read.startsWith(DATA_STR)) {
                                    line_read.remove(0, 6);
                                    ir_data = line_read;
                                    if (ir_type == "raw") {
                                        send_raw(ir_name, ir_frequency, ir_duty_cycle, ir_data);
                                    } else {
                                        canvas.println("ERROR. " + ir_type + " but using 'data'");
                                    }
                                }
                                line_read = "";
                            }  
                        } else {
                            line_read += (char)c;
                        }
                    }
                    canvas.println("Complete!");
                    file.close();
                }
                
                canvas.pushSprite(4, 4);
                prompt = "> ";
            }

            M5Cardputer.Display.fillRect(0, M5Cardputer.Display.height() - 28, M5Cardputer.Display.width(), 25, BLACK);

            M5Cardputer.Display.drawString(prompt, 4, M5Cardputer.Display.height() - 24);
        }
    }
}
