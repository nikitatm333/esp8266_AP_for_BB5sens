
#ifndef DRG_ESP_FUNC_INCLUDED
#define DRG_ESP_FUNC_INCLUDED

const char* ssid =     "INFRATEST";     //  SSID wi-fi �������
const char* password = "^I={test}.1206"; // ������ �� wi-fi

const char* ap_ssid =     "BBD_";     //  SSID wi-fi �������
const char* ap_password = "password"; // ������ �� wi-fi
#define OTAUSER         "admin"    // ����� ��� ����� � OTA
#define OTAPASSWORD     "admin"    // ������ ��� ����� � ���
#define OTAPATH         "/firmware"// ����, ������� ����� ���������� ����� ip ������ � ��������.
ESP8266WebServer HttpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

#define L 512
float TStorage[L];
int TShead = 0, TStail = 0;


#define KEYS 0x3ED3C2C9
#define BadV 314.1592

#define IN_S_LEN 255
static char in_s[IN_S_LEN];  // for input cmd
static uint8_t in_p;
static char tmps[200]; // for output sprintf

volatile uint8_t DispMode = 0;

String LastRecvd="none yet 1";
String LastErr="none yet 2";
float LastRecvdT = 0.0;

typedef struct tagCmdSetpoint
{
  float T;
  uint32_t signature;
} CmdSetpoint;



typedef struct tagCmdCommon
{
  uint16_t cmd;
  uint16_t reserved;
  uint32_t signature;
  float V;
} CmdCommon;


// TODO: update
enum CMD {cmd_none = 0, cmd_tmeas, cmd_tset, cmd_setp, cmd_seti, cmd_setd, cmd_setpow, cmd_ident, cmd_cutoff, cmd_powerup, cmd_VerboseOn, cmd_VerboseOff, cmd_BridgeOn, cmd_BridgeOff};
//uint8_t *target_addr;

typedef struct tagMyDATA_36
{
  float T[6];
  int16_t CV[4];
  uint8_t status[4]; 
} MyDATA_36;

MyDATA_36 D36;

CmdSetpoint CMDSP;
CmdCommon *CMDC;
uint8_t *target_addr;


size_t CountInString(const char *s, char c)
{
  size_t cnt = 0;

  while (*s)
  {
    if (*s==c)
      cnt++;
    s++;
  }
  return cnt;
}

size_t ReplaceInString(char *s, char from, char to)
{
  size_t cnt = 0;
////////////////////////////crashes!!!!!////////////////
  while (*s)
  {
    if (*s == from)
    {
      cnt++;
      *s = to;      
    }
    s++;
  }
}

static uint32_t table[256];
static int have_table = 0;

void Calc_CRC_Table()
{
  static uint32_t rem;
  static uint8_t octet;
  static uint16_t i, j;

      for (i = 0; i < 256; i++) {
      rem = i;  // remainder from polynomial division 
      for (j = 0; j < 8; j++) {
        if (rem & 1) {
          rem >>= 1;
          rem ^= 0xedb88320;
        } else
          rem >>= 1;
      }
      table[i] = rem;
    }
}

uint32_t rc_crc32(uint32_t crc, uint8_t *buf, size_t len) // http://rosettacode.org/wiki/CRC-32#C
{
  static uint8_t *p, *q;
  static uint8_t octet;
        if (DispMode > 1)
        {
          sprintf(tmps, "CRC32 of len %ld addr %08lX", len, (uint32_t)buf); Serial.print (tmps);
        }
 
  // This check is not thread safe; there is no mutex. 
  if (have_table == 0) {
    // Calculate CRC table. 
    //Calc_CRC_Table();
    have_table = 1;
  }
  crc = ~crc;
  q = buf + len;
  for (p = buf; p < q; p++) {
    octet = *p;  /* Cast to unsigned octet. */
    crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
 
  }
        if (DispMode > 1)
        {
          sprintf(tmps, " result= %08lX", ~crc); Serial.println (tmps);
        }

  return ~crc;
}

void PrintMAC(const uint8_t *m)
{
  int i;

  sprintf(tmps, "%02X:%02X:%02X:%02X:%02X:%02X", m[0], m[1], m[2], m[3], m[4], m[5]);
  Serial.println(tmps);
}


void Dump(const uint8_t *m, uint16_t len)
{
  uint16_t i;

  for (i = 0; i < len; i++)
  {
  Serial.print(*m++, HEX);
  Serial.print(' ');
  }  
}

void OnDataRecv(u8 * mac, u8 *incomingData, u8 len) {
  Data_len = len;
  memcpy(DATA, incomingData, len);
  memcpy(ADDR, mac, 6);
}

void OnDataSent(u8 *mac_addr, u8 status) {
  if (status!=0) 
    Serial.print(status);// == ESP_NOW_SEND_SUCCESS ? "" : "SF ");
//  else
//    Serial.print("S_OK");
}


void Print_DispMode()
{
  Serial.print ("[d]isplay mode =");
  Serial.println (DispMode);
}

#define MaxVerbosityLevel 1

void ChangeVerbosity(uint8_t v)
{
  if (v < MaxVerbosityLevel+1)
  {
    Verbosity = v;
    EEPROM.begin(16); // 16 bytes enough
   // delay(500);
    EEPROM.put(0, Verbosity);
    EEPROM.commit();
    EEPROM.end();
  }
}


#endif
