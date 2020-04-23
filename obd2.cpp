#include "OBD2.h"
#include "array";
using namespace std;

//ссылаться через тип дабы небыло конфилктов с глобальными переменными
ADAPTER_STATUS OBD2::Adapter_Status = ADAPTER_STATUS::Adapter_NO;
OBD_STATES OBD2::m_state = OBD_STATES::OBD_DISCONNECTED;
ECU_ANSWER OBD2::Ecu_Answer = ECU_ANSWER::ANSWER_NO;
ECU_STATUS OBD2::Ecu_Status = ECU_STATUS::ECU_Communication_NO;
unsigned long Last_Resiver_Time; // Время последнего принятого байта
unsigned long Time_Errors;			// Время получения первого сообщения об ошибке
extern int StatusFAN;

array<ErrorCode, 59> Codes_Errors{
	 ErrorCode{{0x01, 0x02}, "Низкий уровень сигнала датчика массового расхода воздуха"},
	 {{0x01, 0x03}, "Высокий уровень сигнала датчика массового расхода воздуха"},
	 {{0x01, 0x12}, "Низкий уровень сигнала датчика температуры"},
	 {{0x01, 0x13}, "Высокий уровень сигнала датчика температуры"},
	 {{0x01, 0x15}, "Неверный сигнал датчика температуры охлаждающей"},
	 {{0x01, 0x17}, "Низкий уровень сигнала датчика температуры охлаждающей"},
	 {{0x01, 0x18}, "Высокий уровень сигнала датчика температуры охлаждающей"},
	 {{0x01, 0x22}, "Низкий уровень сигнала датчика положения дроссельной заслонки"},
	 {{0x01, 0x23}, "Высокий уровень сигнала датчика положения дроссельной заслонки"},
	 {{0x01, 0x30}, "Неверный сигнал датчика кислорода"},
	 {{0x01, 0x32}, "Высокий уровень сигнала датчика кислорода"},
	 {{0x01, 0x34}, "Отсутствие сигнала датчика кислорода"},
	 {{0x02, 0x01}, "Обрыв цепи управления форсункой 1-го цилиндра"},
	 {{0x01, 0x02}, "Обрыв цепи управления форсункой 2-го цилиндра"},
	 {{0x02, 0x03}, "Обрыв цепи управления форсункой 3-го цилиндра"},
	 {{0x02, 0x04}, "Обрыв цепи управления форсункой 4-го цилиндра"},
	 {{0x02, 0x61}, "Замыкание на массу цепи управления форсункой 1-го цилиндра"},
	 {{0x02, 0x62}, "Замыкание на источник питания цепи управления форсункой 1-го цилиндра"},
	 {{0x02, 0x64}, "Замыкание на массу цепи управления форсункой 2-го цилиндра"},
	 {{0x02, 0x65}, "Замыкание на источник питания цепи управления форсункой 2-го цилиндра"},
	 {{0x02, 0x67}, "Замыкание на массу цепи управления форсункой 3-го цилиндра"},
	 {{0x02, 0x68}, "Замыкание на источник питания цепи управления форсункой 3-го цилиндра"},
	 {{0x02, 0x70}, "Замыкание на массу цепи управления форсункой 4-го цилиндра"},
	 {{0x02, 0x71}, "Замыкание на источник питания цепи управления форсункой 4-го цилиндра"},
	 {{0x03, 0x27}, "Низкий уровень сигнала датчика детонации"},
	 {{0x03, 0x28}, "Высокий уровень сигнала датчика детонации"},
	 {{0x03, 0x35}, "Неверный сигнал датчика положения коленчатого валаа"},
	 {{0x03, 0x36}, "Ошибка датчика положения коленчатого вала"},
	 {{0x04, 0x44}, "Замыкание на источник питания или обрыв цепи управления клапаном продувки адсорбера"},
	 {{0x04, 0x45}, "Замыкание на массу цепи управления клапаном продувки адсорбера"},
	 {{0x04, 0x80}, "Неисправная цепь управления реле вентилятора охлаждения"},
	 {{0x05, 0x00}, "Неверный сигнал датчика скорости автомобиля"},
	 {{0x05, 0x03}, "Прерывающийся сигнал датчика скорости автомобиля"},
	 {{0x05, 0x06}, "Низкие обороты холостого хода"},
	 {{0x05, 0x07}, "Высокие обороты холостого хода"},
	 {{0x05, 0x60}, "Неверное напряжение бортовой сети"},
	 {{0x05, 0x61}, "Повышенное напряжение бортовой сети"},
	 {{0x05, 0x62}, "Пониженное напряжение бортовой сети"},
	 {{0x06, 0x01}, "Ошибка контрольной суммы ПЗУ"},
	 {{0x06, 0x03}, "Ошибка внешнего ОЗУ"},
	 {{0x06, 0x04}, "Ошибка внутреннего ОЗУ"},
	 {{0x06, 0x07}, "Неверный сигнал канала детонации контроллера"},
	 {{0x11, 0x02}, "Низкое сопротивление нагревателя датчика кислорода"},
	 {{0x11, 0x15}, "Неисправная цепь управления нагревом датчика кислорода"},
	 {{0x11, 0x23}, "Аддитивная составляющая коррекции по воздуху состава смеси превышает порог. Состав - богатый"},
	 {{0x11, 0x24}, "Аддитивная составляющая коррекции по воздуху состава смеси превышает порог. Состав - бедный"},
	 {{0x11, 0x28}, "Мультипликативная составляющая коррекции состава смеси превышает порог. Состав - богатый"},
	 {{0x11, 0x36}, "Мультипликативная составляющая коррекции состава смеси превышает порог. Состав - бедный"},
	 {{0x11, 0x37}, "Аддитивная составляющая коррекции по топливу превышает порог. Состав - богатый"},
	 {{0x11, 0x40}, "Аддитивная составляющая коррекции по топливу превышает порог. Состав - бедный"},
	 {{0x15, 0x00}, "Неверный сигнал датчика массового расхода воздуха"},
	 {{0x15, 0x01}, "Обрыв цепи управления реле электробензонасоса"},
	 {{0x15, 0x02}, "Замыкание на массу цепи управления реле электробензонасоса"},
	 {{0x15, 0x09}, "Замыкание на источник питания цепи управления реле электробензонасоса"},
	 {{0x15, 0x13}, "Перегрузка цепи управления регулятором холостого хода"},
	 {{0x15, 0x14}, "Замыкание на массу цепи управления регулятором холостого хода"},
	 {{0x15, 0x70}, "Обрыв цепи управления регулятором холостого хода"},
	 {{0x16, 0x02}, "Неверный сигнал иммобилизатора"},
	 {{0x16, 0x89}, "Ошибочные значения кодов в памяти ошибок контроллера"}};

void OBD2::Fan(int status = 0)
{
	if (status == 0)
	{
		while (millis() - Last_Resiver_Time < ISO_Request_response_Delay)
		{
			;
		}
		RequestCommands(bosh_mp7_Control[0], 8);
		Delay_ms(ISO_Request_response_Delay);
		ECU_Resiver_(8);
	}
	else
	{
		while (millis() - Last_Resiver_Time < ISO_Request_response_Delay)
		{
			;
		}
		RequestCommands(bosh_mp7_Control[1], 8);
		Delay_ms(ISO_Request_response_Delay);
		ECU_Resiver_(8);
		if (Ecu_Answer == ANSWER_OK)
		{
			//if (OBDBuffer[3] == bosh_mp7_Control[1][9]) {
			//	//Расшифровка полученых сообщений 83 f1 11 54 00 00
			//	Serial.printf("Fan is OK!!!\n");
			//}
			//else
			//{
			//	StatusFAN = 0;
			//	Serial.printf("Fan is NOT OK!!!\n");
			//}
		}
	}
}

void OBD2::AdapterSearch()
{
	int rCnt = 0, i = 0;
	uint8_t c;

	Adapter_Status = Adapter_NO;

	Communication_Init();

	if (m_state == OBD_CONNECTED)
	{
		while (millis() - Last_Resiver_Time < ISO_Request_response_Delay)
		{
			;
		}
		RequestCommands(bosh_mp7_0, 5);
		Delay_ms(ISO_Request_response_Delay);
		//Wait recive byte ECU
		unsigned long timeout = millis() + OBD_TIMEOUT_LONG;
		//Serial.printf("<<  IN EXO ADAPTER: ");
		while (millis() <= timeout)
		{
			if (Ecu_Answer == ANSWER_OK && millis() - Last_Resiver_Time >= ISO_Response_Byte_Delay)
			{
				break;
			}
			if (OBDUART.available())
			{
				Adapter_Status = Adapter_OK;
				Ecu_Answer = ANSWER_OK;
				Last_Resiver_Time = millis() - 1;
				break;
			}
			Last_Resiver_Time = millis() - 1;
		}
	}
	if (Adapter_Status == Adapter_NO)
		Serial.println(" --NO-- ");
	else
		Serial.println(" --OK-- ");
}

void OBD2::Start_diagnostic()
{
	unsigned long start_Time;

	//Serial.printf("Start_diagnostic - Ecu_Status  =  %i\n", Ecu_Status);
	if (Ecu_Status == ECU_Communication_NO)
	{
		//start_Time = millis();
		//Serial.printf("Time StartCommunication = %u \n", start_Time);
		StartCommunication();
		//Serial.printf("Time StartCommunication END = %u \n", millis() - start_Time);
		if (Ecu_Status == ECU_Communication_OK)
			TesterPresent();
	}
	//Serial.printf("After diagnostic - Ecu_Status  =  %i\n", Ecu_Status);
}

void OBD2::SetupKLine()
{
	// Setup serial pins
	pinMode(K_OUT, OUTPUT);
	pinMode(K_IN, INPUT);
}

// terminate communication channel
void OBD2::OBDUART_end()
{
	m_state = OBD_DISCONNECTED;
	OBDUART.end();
}

void OBD2::OBDUART_init()
{
	//long baudrates[] = { 115200, 38400, 10400 };
	//OBDUART.begin(baudrates[2]);
	m_state = OBD_DISCONNECTED;
	OBDUART.begin(10400);
	while (!OBDUART)
	{
		;
	}
	//Проверка порта

	unsigned long detectedBaudRate = OBDUART.baudRate();
	if (detectedBaudRate)
	{
		//Serial.printf("Detected baudrate is %lu\n", detectedBaudRate);
		//PrintTextColor(0, 80, 1, "Detected baudrate", ST7735_WHITE);
		m_state = OBD_CONNECTED;
	}
	else
	{
		Serial.println("No baudrate detected, OBDUART will not work!");
		//PrintTextColor(0, 80, 1, "No baudrate detected", ST7735_WHITE);
	}
}

void OBD2::Communication_Init()
{
	int count = 0;
	uint8_t c;
	uint8_t rCnt = 0;

	OBDUART_end();

	Delay_ms(500);
	SetupKLine();
	Delay_ms(100);

	//Serial.println("This is the KWP2000 <Fast Init> sequence.");
	digitalWrite(K_OUT, HIGH);
	Delay_ms(300);
	digitalWrite(K_OUT, LOW);
	Delay_ms(25);
	digitalWrite(K_OUT, HIGH);
	Delay_ms(24);

	OBDUART_init();
}

// initialize OBD-II connection
void OBD2::StartCommunication()
{
	int count = 0;
	unsigned long start_Time, TimeOperation;

	Ecu_Status = ECU_Communication_NO;
	/*AdapterSearch();
	if (Adapter_Status == Adapter_OK)
	{*/
	while (count <= 8)
	{
		Communication_Init();
		if (m_state == OBD_CONNECTED)
		{
			//start_Time = millis();
			//TimeOperation = start_Time;
			RequestCommands(bosh_mp7_2, 5);
			//Serial.printf("bosh_mp7_2 sizeof = %i \n", sizeof(bosh_mp7_2));
			//Serial.printf("RequestCommands(bosh_mp7_2) = %u \n", millis() - TimeOperation);
			//TimeOperation = millis();
			Delay_ms(ISO_Request_response_Delay);
			//Serial.printf("ISO_Request_response_Delay = %u \n", millis() - TimeOperation);
			//Wait recive byte ECU
			//TimeOperation = millis();
			ECU_Resiver();
			//ECU_Resiver_(5);
			//Serial.printf("ECU_Resiver() = %u \n", millis() - TimeOperation);
			if (Ecu_Answer == ANSWER_OK)
			{
				Serial.printf("OBDBuffer[8] == %X \n", OBDBuffer[8]);
				if (OBDBuffer[8] == bosh_mp7_2[5])
				{
					Ecu_Status = ECU_Communication_OK;
					break;
				}
			}
			Serial.println("");
		}
		count++;
		//Serial.printf("Time Start communication = %u \n", millis() - start_Time);
	}
	//}
}

void OBD2::ReadEcuIdentification()
{
	//убедитесь, что прошло не менее 50 мс с момента последнего ответа
	while (millis() - Last_Resiver_Time < ISO_Request_response_Delay)
	{
		;
	}
	RequestCommands(bosh_mp7_3, 6);
	Delay_ms(ISO_Request_response_Delay);
	//Wait recive byte ECU
	ECU_Resiver();
	if (Ecu_Answer == ANSWER_OK)
	{
		if (OBDBuffer[9] == bosh_mp7_3[6])
		{
		}
	}
}

void OBD2::TesterPresent()
{
	int count = 0;

	while (count < 2)
	{
		//убедитесь, что прошло не менее 50 мс с момента последнего ответа
		while (millis() - Last_Resiver_Time < ISO_Request_response_Delay)
		{
			;
		}
		RequestCommands(bosh_mp7_4, 5);
		Delay_ms(ISO_Request_response_Delay);
		//Wait recive byte ECU
		ECU_Resiver();
		if (Ecu_Answer == ANSWER_OK)
		{
			if (OBDBuffer[9] == bosh_mp7_4[5])
			{
				;
				//break;
			}
		}
		count++;
	}
}

void OBD2::InputOutputControlByLocalIdentifier()
{

	//убедитесь, что прошло не менее 50 мс с момента последнего ответа
	while (millis() - Last_Resiver_Time < ISO_Request_response_Delay)
	{
		;
	}
	RequestCommands(bosh_mp7_6, 7);
	Delay_ms(ISO_Request_response_Delay);
	//Wait recive byte ECU
	ECU_Resiver();
	if (Ecu_Answer == ANSWER_OK)
	{
		if (OBDBuffer[8] == bosh_mp7_3[7])
		{
		}
	}
}

void OBD2::readMemoryByAddress(uint8_t Address_Memory)
{
}

void OBD2::ReadDataByLocalIdentifier()
{
	int rCnt = 0;
	int count = 0;
	uint8_t c;
	unsigned long TimeOperation;

	// Zero the response buffer up to maxLen
	for (uint8_t i = 0; i < 255; i++)
	{
		OBDBuffer[i] = 0;
	}
	//убедитесь, что прошло не менее 50 мс с момента последнего ответа
	/*while (count < 6)
	{*/
	while (millis() - Last_Resiver_Time < ISO_Request_response_Delay)
	{
		;
	}
	RequestCommands(bosh_mp7_13, 6);
	Delay_ms(ISO_Request_response_Delay);
	//Wait recive byte ECU
	//TimeOperation = millis();
	//ECU_Resiver();
	ECU_Resiver_(6);
	//Serial.printf("ReadDataByLocalIdentifier >> ECU_Resiver() = %u \n", millis() - TimeOperation);
	if (Ecu_Answer == ANSWER_OK)
	{
		if (OBDBuffer[3] == bosh_mp7_13[6])
		{
			if (OBDBuffer[5] > 0)
			{ // Опрос и сброс ошибок
				if (Time_Errors == 0)
				{
					Time_Errors = millis() + 600000;
				}
				if (millis() - Time_Errors > 0)
				{
					RequestError();
					ResetError();
					Time_Errors = 0;
				}
			}
			//				KWP				      00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41
			//                              00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41
			//TE = OBDBuffer[14] - 40; //	A6 F1 11 61 01 00 00 00 00 00 00 00 34 00 7F 93 90 00 00 00 00 57 EA 5E 00 00 82 00 00 28 64 80 F7 96 00 00 01 00 00 00 00 9B
			uint16_t temp = 0;

			V = 5.2 + OBDBuffer[14] * 0.05; //23(21) байт   Напряжение в сети
			U = 5.2 + OBDBuffer[25] * 0.05; //23(21) байт   Напряжение в сети

			TE = OBDBuffer[15] * 0.75 - 48; //15(13) байт   Температура охлаждающей жидкости в градусах

			RPM = OBDBuffer[24] * 10; //24 байт   Обороты двигателя 0

			Speed = OBDBuffer[17]; //22(20) байт   Скорость 0
		}
		else
		{
			Erors_ECU_Answer++;
		}
	}
	else
	{
		Erors_ECU_Answer++;
	}
	count++;
	//}
}

void OBD2::StopCommunication()
{

	int count = 0;

	while (count < 8)
	{
		//убедитесь, что прошло не менее 50 мс с момента последнего ответа
		while (millis() - Last_Resiver_Time < ISO_Request_response_Delay)
		{
			;
		}
		RequestCommands(bosh_mp7_14, 5);
		Delay_ms(ISO_Request_response_Delay);
		//Wait recive byte ECU
		ECU_Resiver();
		if (Ecu_Answer == ANSWER_OK)
		{
			if (OBDBuffer[8] == bosh_mp7_14[5])
			{
				//Serial.printf("Stop >> %X \n", OBDBuffer[8]);
				Ecu_Status = ECU_Communication_NO;
				Delay_ms(MAXSENDTIME); //после выполнения запроса StopCommunication Service:	TIdle = P3min = 	100 ms
				break;
			}
		}
		count++;
	}
}

void OBD2::RequestError()
{

	//unsigned long TimeOperation;

	//убедитесь, что прошло не менее 50 мс с момента последнего ответа
	while (millis() - Last_Resiver_Time < ISO_Request_response_Delay)
	{
		;
	}
	RequestCommands(bosh_mp7_Request_Errors, 8); // запрос ошибок
	Delay_ms(ISO_Request_response_Delay);
	//Wait recive byte ECU
	//TimeOperation = millis();
	ECU_Resiver_(8);
	//Serial.printf("ReadDataByLocalIdentifier >> ECU_Resiver() = %u \n", millis() - TimeOperation);
	if (Ecu_Answer == ANSWER_OK)
	{
		if (OBDBuffer[3] == bosh_mp7_Request_Errors[8])
		{
			//Расшифровка полученых сообщений 85 f1 11 58 01 04 44 84
			//Количество ошибок
			if (OBDBuffer[4] == 0x00)
				Serial.printf("ERRORS NO");
			else
			{
				//Serial.printf("ERRORS >> %X%X", OBDBuffer[6], OBDBuffer[7]);
				for (auto i = 1; i < OBDBuffer[4]; i++)
				{
					for (auto errorCode = Codes_Errors.begin(); errorCode != Codes_Errors.end(); ++errorCode)
					{
						auto code = errorCode->Code;
						bool isError = (code[0] & OBDBuffer[3 + 2 * i]) & (code[1] & OBDBuffer[4 + 2 * i]); //5-6-1  7-8-2  9-10-3
						if (isError)
						{
							Serial.printf("ERRORS %i = %s\n", errorCode->message);
						}
					}
				}
			}
		}
	}
	else
		Serial.printf("Request_Errors >> ANSWER_NO\n");
}

void OBD2::ResetError()
{
	//unsigned long TimeOperation;

	//убедитесь, что прошло не менее 50 мс с момента последнего ответа
	while (millis() - Last_Resiver_Time < ISO_Request_response_Delay)
	{
		;
	}
	RequestCommands(bosh_mp7_Reset_Errors, 7); // запрос ошибок
	Delay_ms(ISO_Request_response_Delay);
	//Wait recive byte ECU
	//TimeOperation = millis();
	ECU_Resiver_(7);
	//Serial.printf("bosh_mp7_Reset_Errors >> ECU_Resiver() = %u \n", millis() - TimeOperation);
	if (Ecu_Answer == ANSWER_OK)
	{
		if (OBDBuffer[3] == bosh_mp7_Reset_Errors[7])
		{
			//Расшифровка полученых сообщений 83 f1 11 54 00 00
			Serial.printf("Reset ALL Errors Ecu\n");
		}
	}
	else
		Serial.printf("Reset_Errors >> ANSWER_NO\n");
}

void OBD2::RequestCommands(const uint8_t *request, size_t len)
{

	Serial.print(">> OUT: ");
	for (size_t i = 0; i < len; i++)
	{
		OBDUART.write(request[i]);
		Delay_ms(ISO_Request_Byte_Delay);
		Serial.printf("%02X ", request[i]);
	}
	//Serial.println("");
}

void OBD2::ECU_Resiver()
{
	int rCnt = 0;
	uint8_t c;

	// Zero the response buffer up to maxLen
	for (uint8_t i = 0; i < 255; i++)
	{
		OBDBuffer[i] = 0;
	}

	Ecu_Answer = ANSWER_NO;
	Serial.printf("<<  IN: ");
	Last_Resiver_Time = 0;
	unsigned long timeout = millis() + OBD_TIMEOUT_LONG;
	//Serial.printf("Time OUT = %u \n", timeout);
	while (millis() <= timeout)
	{
		if (Ecu_Answer == ANSWER_OK && millis() - Last_Resiver_Time >= ISO_Response_Byte_Delay)
		{
			break;
		}
		if (OBDUART.available())
		{
			c = OBDUART.read();
			Serial.printf("%X ", c);
			OBDBuffer[rCnt] = c;
			rCnt++;
			Ecu_Answer = ANSWER_OK;
			Last_Resiver_Time = millis() - 1;
			//Serial.printf("Last_Resiver_Time = %u  \n", Last_Resiver_Time);
		}
	}
	if (rCnt == 0)
	{
		Serial.print("ERROR: resiver timeout");
	}
	else
	{
		Serial.println("");
	}
}

void OBD2::ECU_Resiver_(size_t len)
{
	int rCnt = 0;
	uint8_t c;

	// Zero the response buffer up to maxLen
	for (uint8_t i = 0; i < 255; i++)
	{
		OBDBuffer[i] = 0;
	}

	Ecu_Answer = ANSWER_NO;
	Serial.printf("<<  IN: ");
	Last_Resiver_Time = 0;
	unsigned long timeout = millis() + OBD_TIMEOUT_LONG;
	//Serial.printf("Time OUT = %u \n", timeout);
	while (millis() <= timeout)
	{
		if (Ecu_Answer == ANSWER_OK && millis() - Last_Resiver_Time >= ISO_Response_Byte_Delay)
		{
			break;
		}
		if (OBDUART.available())
		{
			c = OBDUART.read();
			//Serial.printf("%X ", c);
			//Отсекаем эхо запроса
			if (rCnt >= len)
			{
				OBDBuffer[rCnt - len] = c;
				Serial.printf("%X ", c);
			}
			rCnt++;
			Ecu_Answer = ANSWER_OK;
			Last_Resiver_Time = millis() - 1;
			//Serial.printf("Last_Resiver_Time = %u  \n", Last_Resiver_Time);
		}
	}
	if (rCnt == 0)
	{
		Ecu_Answer = ANSWER_NO;
		Serial.println("ERROR: resiver timeout");
	}
	else
	{
		if (rCnt == len)
		{
			Ecu_Answer = ANSWER_NO;
			Serial.println("ERROR: EXO K-Line");
		}
		else
		{
			Serial.println("");
			//проверка суммы полученношо сообщения
			/*if (!_CRC(OBDBuffer, rCnt - len))
			{
				Ecu_Answer = ANSWER_NO;
				Serial.println("ERROR: ShekSum answer");
			}*/
		}
	}
}

void OBD2::Delay_ms(uint32_t ms)
{

	uint32_t startTime;

	startTime = millis();
	while ((millis() - startTime) < ms)
	{
	}
}

byte OBD2::checkErrorMessage(const char *buffer)
{
	const char *errmsg[] = {"UNABLE", "ERROR", "TIMEOUT", "NO DATA"};
	for (byte i = 0; i < sizeof(errmsg) / sizeof(errmsg[0]); i++)
	{
		if (strstr(buffer, errmsg[i]))
			return i + 1;
	}
	return 0;
}

bool OBD2::_CRC(uint8_t *data, size_t len)
{
	uint8_t crc = 0, i = 0;

	for (i = 0; i < len; i++)
	{
		crc += data[i];
	}

	if (crc == data[len])
		return true;

	return false;
}
