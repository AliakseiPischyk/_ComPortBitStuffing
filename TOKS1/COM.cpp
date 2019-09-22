#include "COM.h"
#include <exception>

void COM::Disconnect()
{
	if (_handle != 0) {
		CloseHandle(_handle);
		_handle = NULL;
	}
}

COM::COM(const std::string_view& port, const Baud speed) {
	_handle = CreateFile(
		port.data(),
		GENERIC_READ | GENERIC_WRITE,
		0, // dwShareMode
		NULL, // lpSecurityAttributes
		OPEN_EXISTING, // dwCreationDispostion. OPEN_EXISTING	Opens the file. The function fails if the file does not exist.
		FILE_ATTRIBUTE_NORMAL, // dwFlagsAndAttributes. FILE_ATTRIBUTE_NORMAL	The file has no other attributes set. This attribute is valid only if used alone.
		NULL); //hTemplateFile. Ignored; as a result, CreateFile does not copy the extended attributes to the new file.

	if (_handle == INVALID_HANDLE_VALUE) {
		_handle = NULL;
		throw ComPortException();
	}

	SetupComm(_handle, BUFSIZ, BUFSIZ);

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = MAXDWORD;
	CommTimeOuts.ReadTotalTimeoutMultiplier = MAXDWORD;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 0;
	if (!SetCommTimeouts(_handle, &CommTimeOuts)) {
		_handle = 0;
		throw ComPortException("Timeout setting error");
	}

	DCB dcb;
	memset(&dcb, 0, sizeof(dcb));
	dcb.DCBlength = sizeof(DCB);
	GetCommState(_handle, &dcb);
	dcb.BaudRate = DWORD(speed);
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;

	dcb.StopBits = ONESTOPBIT;
	dcb.fAbortOnError = TRUE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fRtsControl = RTS_CONTROL_TOGGLE;
	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;
	dcb.fInX = dcb.fOutX = FALSE;
	dcb.XonChar = 0;
	dcb.XoffChar = uint8_t(0xff);
	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.XonLim = 128;
	dcb.XoffLim = 128;

	if (!SetCommState(_handle, &dcb)) {
		CloseHandle(_handle);
		_handle = NULL;
		throw ComPortException("Error occured while setting port parameters");
	}
}

COM::~COM()
{
	Disconnect();
}

void COM::Write(const uint8_t* buf, const size_t buf_size)
{
	if (_handle == NULL) {
		throw ComPortException("Port is closed"); 
	}

	DWORD feedback;

	if (!WriteFile(_handle, buf, buf_size, &feedback, 0) ||
		feedback != buf_size) {
		CloseHandle(_handle);
		_handle = 0;
		throw ComPortException("Error occured while writing to port");
	}
}

void COM::Read(uint8_t* buf, const size_t buf_size) {
	size_t bytes_to_read_left = buf_size;
	DWORD bytes_read = 0;
	unsigned reading_attempts = 3;

	while (bytes_to_read_left && reading_attempts) {
		if (reading_attempts) reading_attempts--;
		if (!ReadFile(_handle, buf, bytes_to_read_left, &bytes_read, NULL)){
			CloseHandle(_handle);
			_handle = NULL;
			throw ComPortException("Error occured while reading from port");
		}
		bytes_to_read_left -= bytes_read;
		buf += bytes_read;
	}
}