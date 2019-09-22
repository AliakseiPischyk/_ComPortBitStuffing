#include <iostream>
#include <conio.h>
#include "COM.h"
#include "BitStuffer.h"

#define BUFSIZE 128
#define BAUDRATE 115200


int main()
{
	BitStuffer bit_stuffer;
	
	try {
		std::string port_name;
		std::cout << "Enter port name" << std::endl;
		std::cin >> port_name;

		Baud speed;
		std::cout << "Enter baudrate" << std::endl;
		std::cin >> speed;

		COM com_port(port_name, speed);

		while (true)
		{
			system("cls");
			std::cout << "Port: " << port_name << std::endl;
			std::cout << "1.Recieve" << std::endl;
			std::cout << "2.Send" << std::endl;
			std::cout << "3.Exit" << std::endl;

			rewind(stdin);
			switch (_getch() - '0')
			{
			case 1:
			{
				uint8_t buff[BUFSIZE];
				ZeroMemory(buff, BUFSIZE);

				com_port.Read(buff, BUFSIZE);
				std::cout << std::endl << "Recieved data: " << buff << std::endl;
				auto decoded = bit_stuffer.CodeBytes(reinterpret_cast<char*>(buff), BitStufferWorkStrategy::DECODE);

				std::cout << std::endl << "encoded data: " << decoded << std::endl;
				std::cout << "Press any key to continue" << std::endl;
				system("pause");
				break;
			}
			case 2:
			{
				std::string buff;

				std::cout << "Enter data" << std::endl;
				std::getline(std::cin, buff);
				auto coded = bit_stuffer.CodeBytes(buff, BitStufferWorkStrategy::CODE);
				std::cout << "coded data" <<coded<< std::endl;
				com_port.Write(reinterpret_cast<const uint8_t*>(coded.c_str()), coded.length());
				std::cout << "Press any key to continue" << std::endl;
				system("pause");
				break;
			}
			case 3:
			{
				com_port.Disconnect();
				return 0;
			}
			default:
			{
				continue;
			}
			}
		}

		com_port.Disconnect();

		return 0;
	}
	catch (const ComPortException& e) {
		std::cerr << e.what();
		system("pause");
		return 0;
	}
}