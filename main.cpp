#include <iostream>
#include "main.h"

using namespace std;

// описание класса линейного сенсора (токовый датчик, например)
class LinearSensor
{
public:
	// Поля класса
	// переменные - максимумы и минимумы шкал, электрической и технологической
    float fLinSensElMax, fLinSensElMin, fLinSensTechMin, fLinSensTechMax;
    // смещение шкал, электрической и технологической
    float fShiftEl, fShiftTech;
    float fPriceScale;
    // направление преобразования величин
    int dTypeConversion;
    // искомые величины
    float fLinSensElFind, fLinSensTechFind;

	// Метода класса
	void Initialize(void)
	{
		cout << "Input an electrical scale (min): ";
		cin >> fLinSensElMin;
		cout << "Input an electrical scale (max): ";
		cin >> fLinSensElMax;
//		cout << "ElMin:" << fLinSensElMin << "\n" << "ElMax: " << fLinSensElMax << endl;
		cout << "Input an technical scale (min): ";
		cin >> fLinSensTechMin;
		cout << "Input an technical scale (max): ";
		cin >> fLinSensTechMax;

		// смещение шкал отн-но нуля
		fShiftEl = fLinSensElMin;
		fShiftTech = fLinSensTechMin;

		fPriceScale = (fLinSensTechMax - fLinSensTechMin) / (fLinSensElMax - fLinSensElMin);

		cout << "Input type of conversion (1-2):" << endl;
		cout << "1. Electrical to Technical value." << endl;
		cout << "2. Technical to Electrical value." << endl;
		cin >> dTypeConversion;

		// Проверка корректности введённого значения
		if((dTypeConversion > 2) & (dTypeConversion < 1))
		{
			cout << "Wrong input, sorry.";
			// Завершить выполнение кода!!!
		}

		switch(dTypeConversion)
		{
		case 1:
			{
				cout << "Input electrical value: ";
				cin >> fLinSensElFind;
				if((fLinSensElFind < fLinSensElMin) | (fLinSensElFind > fLinSensElMax))
				{
					cout << "Wrong input!!!" << endl;
					// завершение программы!
				}

				fLinSensTechFind = (fLinSensElFind - fShiftEl) * fPriceScale + fShiftTech;

				cout << "Result: " << fLinSensTechFind << endl;
			}
			break;

		case 2:
			{
				cout << "Input technical value: ";
				cin >> fLinSensTechFind;
				if((fLinSensTechFind < fLinSensTechMin) | (fLinSensTechFind > fLinSensTechMax))
				{
					cout << "Wrong input!!!" << endl;
					// завершение программы!
				}

				cout << "TechMin: " << fLinSensTechMin << "\t" << "TechMax: " << fLinSensTechMax << endl;

				fLinSensElFind = (fLinSensTechFind - fShiftTech) / fPriceScale + fShiftEl;

				cout << "Result: " << fLinSensElFind << endl;
			}
			break;
		}
	}

	// Конструкторы


};

// описание класса нелинейного сенсора (термосопротивления, термопары и т.п.)
class Sensor
{
public:					// Пока что всё сделал публичным, позже необходимо поработать над этим, и что-то убрать в "приват"
	// Поля класса
	int dSensorType;
	int dShift; 						// смещение нуля в градуировочной таблице
	int dTypeConversion;
	// 1 - electrical value to technical value
	// 2 - technical value to electrical value

	int dDataArraySize;
	float fElValue, fTechValue;
	float *pdDataSensor;
	float fResult;

	// Методы базового класса, инициализация класса
	void Initialize(int dSensorType)
	{

		switch(dSensorType)
		{
		case 1:				// Инициализация для ТСМ50
			{
				dShift = -50;
				dDataArraySize = ((sizeof(fTableTSM50200)) / (sizeof(fTableTSM50200[0])));
				pdDataSensor = fTableTSM50200;
			}
			break;

		case 2:
			{
				dShift = -50;
				dDataArraySize = ((sizeof(fTableTSM50180)) / (sizeof(fTableTSM50180[0])));
				pdDataSensor = fTableTSM50180;
			}
			break;

		case 3:
			{
				dShift = -50;
				dDataArraySize = ((sizeof(fTableTSP50600)) / (sizeof(fTableTSP50600[0])));
				pdDataSensor = fTableTSP50600;
			}
			break;
		}

		cout << "Input type of conversion (1-2):" << endl;
		cout << "1. Electrical to Technical value." << endl;
		cout << "2. Technical to Electrical value." << endl;
		cin >> dTypeConversion;

		// Проверка корректности введённого значения
		if((dTypeConversion > 2) & (dTypeConversion < 1))
		{
			cout << "Wrong input, sorry.";
			// Завершить выполнение кода!!!
		}

		switch(dTypeConversion)
		{
		case 1:									// Подразумевается конвертация эл. величины в технологическую
			{
				cout << "Input electrical value: ";
				cin >> fElValue;

				// Проверка введённой эл. величины на вхождение в диапазон градуировочной таблицы
				if((fElValue < pdDataSensor[0]) & (fElValue > pdDataSensor[dDataArraySize - 1]))
				{
					cout << "Wrong input";
					// Завершить выполнение кода далее!!!
				}

				// Отдаём управление приватному методу конвертации величин

				fResult = letElectricalConvert(fElValue);
				cout << "Result = " << fResult << endl;
			}
			break;

		case 2:
			{
				cout << "Input technical value: ";
				cin >> fTechValue;
				// Проверка на вхождение технологического значения в диапазон градуировочной таблицы
				if((fTechValue < (0 - dShift)) & (fTechValue > dDataArraySize))
				{
					cout << "Wrong input";
				}

				// Отдаём управление приватному методу конвертации величин
				fResult = letTechnicalConvert(fTechValue);
				cout << "Result = " << fResult << endl;
			}
			break;
		}
	}

	float letElectricalConvert(float fElValue)
	{
		float k, b, x;

		for(int i=0; i < dDataArraySize; ++i)
		{
			// Подбираем ближайшее эл. значение из градуировочной таблицы
			if((fElValue - pdDataSensor[i]) < 0.1)
			{
				k = (pdDataSensor[i+1] - pdDataSensor[i]) / (float)((i+1 + dShift) - (i + dShift));
//				cout << "k = " << k << endl;
				b = pdDataSensor[i] - k * (i + dShift);
//				cout << "b = " << b << endl;
				x = (fElValue - b) / k;
				break;
			}
		}
		return x;
	}

	float letTechnicalConvert(float fTechValue)
	{
		float y, k, b;

		for(int i=0; i < dDataArraySize; ++i)
		{
			if(((i + dShift) - fTechValue) > 0)
			{
				k = (pdDataSensor[i] - pdDataSensor[i-1]) / (float)((i + dShift) - (i-1 + dShift));
				b = pdDataSensor[i] - k * (i + dShift);
//				cout << "[i] = " << i << endl;
//				cout << "pdDataSensor[i] = " << pdDataSensor[i] << endl;
//				cout << "pdDataSensor[i-1] = " << pdDataSensor[i-1] << endl;
//				cout << "dShift = " << dShift << endl;
//				cout << "k = " << k << endl;
//				cout << "b = " << b << endl;
				y = k * fTechValue + b;
				break;
			}
		}
		return y;
	}

};

//------------------------------- RunFunc -------------------------------//
void RunFunc(int dSensorType)
{
	// Работа с датчиком
	switch(dSensorType)
	{
	case 1:
		{
			Sensor TSM50M;
			// используем метод объявленного объекта
			TSM50M.Initialize(dSensorType);
		}
		break;

	case 2:
		{
			Sensor TSM53M;
			// используем метод объявленного объекта
			TSM53M.Initialize(dSensorType);
		}
		break;

	case 3:
		{
			Sensor TSP50;
			TSP50.Initialize(dSensorType);
		}
		break;

	case 4:
		{
			// обработчик линейных шкал
			LinearSensor EL420;
			EL420.Initialize();
		}
		break;
	}
}

//------------------------------- MAIN -------------------------------//
int main()
{
	int dTypeSensor;
	char cRepeat;

	// выполнение цикла до нажатия клавиши "y"
	do
	{
		cout << "Type of sensors (1-5)." << endl;
		cout << "1. TSM-50M." << endl;
		cout << "2. TSM-53M." << endl;
		cout << "3. TSP-50" << endl;
		cout << "4. Linear grade." << endl;
		cin >> dTypeSensor;

		// Проверка номера типа введённого типа датчика
		if((dTypeSensor < 1) | (dTypeSensor > 4))
		{
			cout << "Out of range, sorry!" << endl;
			return 0;
		}

		// Передача управления ф-ции обработки типа датчика
		RunFunc(dTypeSensor);

		cout << "Repeat? (y)";
		cin >> cRepeat;
	}
	while(cRepeat == ('y' | 'Y'));

	return 0;
}


