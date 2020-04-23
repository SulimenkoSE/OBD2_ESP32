// OBD2UART.h

#ifndef _OBD2UART_h
#define _OBD2UART_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
//#include "WProgram.h"
#endif

#include <string>

#define DEBUG

#define OBD_TIMEOUT_SHORT 100 /* ms */
#define OBD_TIMEOUT_LONG 2000 /* ms */

#define K_OUT 17 // K Output Line
#define K_IN 16  // K Input  Line

#define OBDUART Serial2

// Timings
#define MAXSENDTIME 100									  // 100 ms timeout on KDS comms.
const uint8_t ISO_Request_Byte_Delay = 5;			  // P4 Inter byte time for tester request
const uint8_t ISO_New_tester_request_Delay = 100; // P3 Time between end of ECU responses and start of new tester request
const uint8_t ISO_Request_response_Delay = 50;	  // P2 Time between tester request and ECU response or two ECU responses
const uint8_t ISO_Response_Byte_Delay = 20;		  // P1 Inter byte time for ECU response
// Source and destination adresses, ECU (0x11) & Arduino (0xF1)
const uint8_t ECUaddr = 0x10;
const uint8_t MyAddr = 0xF1;
const uint8_t format = 0x81;

extern uint8_t OBDBuffer[256];

const uint8_t bosh_mp7_0[6] = {0x81, 0x11, 0xF1, 0x82, 0x05, 0x00}; //????
const uint8_t bosh_mp7_1[6] = {0x81, 0x11, 0xF1, 0x81, 0x04, 0x00}; //????

const uint8_t bosh_mp7_2[6] = {0x81, 0x11, 0xF1, 0x81, 0x04, 0xC1};											//  startCommunication C1 >> OBDBuffer[9]
const uint8_t bosh_mp7_3[7] = {0x82, 0x11, 0xF1, 0x1A, 0x80, 0x1E, 0x5A};									//  readEcuIdentification 5A >> OBDBuffer[8]
const uint8_t bosh_mp7_4[6] = {0x81, 0x11, 0xF1, 0x3E, 0xC1, 0x7E};											//  testerPresent 7E >> OBDBuffer[8]         запрос присутствия
const uint8_t bosh_mp7_5[11] = {0x87, 0x11, 0xF1, 0x83, 0x03, 0x00, 0x50, 0x01, 0x14, 0x00, 0x74}; //????
const uint8_t bosh_mp7_6[8] = {0x84, 0x11, 0xF1, 0x30, 0x34, 0x01, 0xEA, 0x70};							//	inputOutputControlByLocalIdentifier 70 >> OBDBuffer[10]
const uint8_t bosh_mp7_7[10] = {0x85, 0x11, 0xF1, 0x23, 0x40, 0x00, 0x00, 0x01, 0xEB, 0x63};			//	readMemoryByAddress 63 >> OBDBuffer[12]
const uint8_t bosh_mp7_8[10] = {0x85, 0x11, 0xF1, 0x23, 0x40, 0x02, 0x00, 0x01, 0xED, 0x63};			//	readMemoryByAddress 63 >> OBDBuffer[12]
const uint8_t bosh_mp7_9[10] = {0x85, 0x11, 0xF1, 0x23, 0x40, 0x03, 0x00, 0x01, 0xEE, 0x63};			//	readMemoryByAddress 63 >> OBDBuffer[12]
const uint8_t bosh_mp7_10[10] = {0x85, 0x11, 0xF1, 0x23, 0x40, 0x04, 0x00, 0x01, 0xEF, 0x63};		//	readMemoryByAddress 63 >> OBDBuffer[12]
const uint8_t bosh_mp7_11[10] = {0x85, 0x11, 0xF1, 0x23, 0x40, 0x05, 0x00, 0x01, 0xF0, 0x63};		//	readMemoryByAddress 63 >> OBDBuffer[12]
const uint8_t bosh_mp7_12[10] = {0x85, 0x11, 0xF1, 0x23, 0x40, 0x07, 0x00, 0x01, 0xF2, 0x63};		//	readMemoryByAddress 63 >> OBDBuffer[12]
const uint8_t bosh_mp7_13[7] = {0x82, 0x11, 0xF1, 0x21, 0x01, 0xA6, 0x61};									//  readDataByLocalIdentifier 61 >> OBDBuffer[9]
const uint8_t bosh_mp7_14[6] = {0x81, 0x11, 0xF1, 0x82, 0x05, 0xC2};											//  stopCommunication C2 >> OBDBuffer[9]

const uint8_t bosh_mp7_15[6] = {0x81, 0x11, 0xF1, 0x82, 0x05, 0x00}; //????
//	CoolingSytemFanRelayOutputControl 30
//Данное значение параметра информирует блок управления, что тестер запрашивает
//прямое управление реле вентилятора системы охлаждения двигателя. 0A
//ReportIOConditions
//Данное значение параметра информирует блок управления,
//что тестер запрашивает условия для управления входом / выходом
//или внутренней переменной определяемой 02

const uint8_t bosh_mp7_16[7] = {0x81, 0x11, 0xF1, 0x30, 0x0A, 0x02, 0xC1};

const uint8_t bosh_mp7_Request_Errors[8] = {0x84, 0x11, 0xF1, 0x18, 0x00, 0xFF, 0x00, 0x58}; // запрос ошибок
const uint8_t bosh_mp7_Reset_Errors[7] = {0x83, 0x11, 0xF1, 0x14, 0x00, 0x00, 0x54};			// запрос стирания ошибок
//Otpravil zapros DTC read
//85 F1 11 58 1 1 70 22 73
//Received message is OK!Checksum is correct!
//DTC is found!
//ERROR 1: P0170

//Управление :
//85 10 F1 30 0A 01 00 00 C1 - реле вентилятора(вкл\выкл)

const uint8_t bosh_mp7_Control[31][9] = {
	 {0x84, 0x11, 0xF1, 0x30, 0x0A, 0x06, 0x00, 0xC6, 0x70},
	 {0x84, 0x11, 0xF1, 0x30, 0x0A, 0x06, 0x01, 0xC7, 0x70}
	 /*{ 0x85, 0x11, 0xF1, 0x30, 0x0A, 0x01, 0x00, 0xC2, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x0B, 0x00, 0x00, 0xC3, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x0B, 0x01, 0x00, 0xC3, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x0C, 0x00, 0x00, 0xC4, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x0C, 0x01, 0x00, 0xC4, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x0D, 0x00, 0x00, 0xC5, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x0D, 0x01, 0x00, 0xC5, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x0E, 0x00, 0x00, 0xC6, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x0E, 0x01, 0x00, 0xC6, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x0F, 0x00, 0x00, 0xC7, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x0F, 0x01, 0x00, 0xC7, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x01, 0x00, 0x00, 0xB8, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x01, 0x01, 0x00, 0xB9, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x02, 0x00, 0x00, 0xB9, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x02, 0x01, 0x00, 0xBA, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x03, 0x00, 0x00, 0xBA, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x03, 0x01, 0x00, 0xBB, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x04, 0x00, 0x00, 0xBB, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x04, 0x01, 0x00, 0xBC, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x05, 0x00, 0x00, 0xBC, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x05, 0x01, 0x00, 0xBD, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x06, 0x00, 0x00, 0xBD, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x06, 0x01, 0x00, 0xBE, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x07, 0x00, 0x00, 0xBE, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x07, 0x01, 0x00, 0xBF, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x08, 0x00, 0x00, 0xBF, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x08, 0x01, 0x00, 0xC0, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x09, 0x00, 0x00, 0xC0, 0x70 },
	{ 0x85, 0x11, 0xF1, 0x30, 0x09, 0x01, 0x00, 0xC1, 0x70 }*/
};

//85 10 F1 30 0C 01 00 00 C3 - лампа диагностики(вкл\выкл)
//85 10 F1 30 09 01 00 00 C0 - Топливное реле - осторожно!
//
//Отредактировано WADIM(2010 - 03 - 19 22:33 : 22)

struct ErrorCode
{
	int Code[2];
	const char *message;
};

//uint8_t ERRORS_KOD[2][100] = {
//	{0x01, 0x02}, "Низкий уровень сигнала датчика массового расхода воздуха",
//	{0x01, 0x03} , "Высокий уровень сигнала датчика массового расхода воздуха",
//	{0x01, 0x12} , "Низкий уровень сигнала датчика температуры"
//};

extern int Erors_ECU_Answer;

extern uint16_t FR_max, FR_min;
extern uint8_t TE, TP, RI, U_MAF, TE_M;
extern uint16_t RPM, RPM_XX, FR, AF, MAF, IT, Fuel_100, Speed;
extern uint8_t U;
extern float V;

typedef enum
{
	Adapter_OK = 0,
	Adapter_NO = 1
} ADAPTER_STATUS;

typedef enum
{
	ANSWER_OK = 0,
	ANSWER_NO = 1,
	ANSWER_Error_Time_OUT = 2
} ECU_ANSWER;

typedef enum
{
	ECU_Communication_OK = 0,
	ECU_Communication_NO = 1
} ECU_STATUS;

// states
typedef enum
{
	OBD_DISCONNECTED = 0,
	OBD_CONNECTED = 1
} OBD_STATES;

class OBD2
{

public:
	//Setup pin Rx Tx fnd Led

	// terminate communication channel
	virtual void SetupKLine();
	virtual void AdapterSearch();
	virtual void Start_diagnostic();
	virtual void ReadDataByLocalIdentifier(); //61 >> OBDBuffer[3]
	virtual void Fan(int status);
	virtual void RequestError();
	virtual void ResetError();
	//статическая переменная типа
	static ADAPTER_STATUS Adapter_Status;
	static OBD_STATES m_state;
	static ECU_ANSWER Ecu_Answer;
	static ECU_STATUS Ecu_Status;

protected:
	virtual void Delay_ms(uint32_t ms);
	virtual byte checkErrorMessage(const char *buffer);
	virtual void OBDUART_end();
	virtual void OBDUART_init();
	virtual void RequestCommands(const uint8_t *request, size_t len);
	virtual void TesterPresent(); //7E >> OBDBuffer[8]
	virtual void Communication_Init();
	virtual void StartCommunication();								 //C1 >> OBDBuffer[9]
	virtual void ReadEcuIdentification();							 //5A >> OBDBuffer[8]
	virtual void InputOutputControlByLocalIdentifier();		 //70 >> OBDBuffer[10]
	virtual void readMemoryByAddress(uint8_t Address_Memory); //63 >> OBDBuffer[12]
	virtual void StopCommunication();								 //C2 >> OBDBuffer[9]
	virtual void ECU_Resiver();
	virtual void ECU_Resiver_(size_t len);
	virtual bool _CRC(uint8_t *data, size_t len);
};
#endif
