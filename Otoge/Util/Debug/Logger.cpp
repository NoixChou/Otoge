#include "Logger.h"
#include "../Encoding/EncodingConverter.h"
using namespace std;

int Logger::userCount = 0;

Logger::Logger(const string &moduleName)
{
#ifdef _DEBUG
	if (userCount == 0)
	{
		AllocConsole();

		SetConsoleOutputCP(CP_UTF8);
		auto l_ConOutResult = freopen("CONOUT$", "w", stdout);
		auto l_ConInResult = freopen("CONIN$", "r", stdin);
		if(l_ConOutResult == nullptr || l_ConInResult == nullptr)
		{
			Critical("標準I/Oストリームの取得に失敗しました");
		}
	}
#endif

	userCount++;
	this->ModuleName_ = moduleName;
	//Debug("Logger added, logger count: " + to_string(userCount));
}


Logger::~Logger()
{
	userCount--;
	//Debug("Logger removed, logger count: " + to_string(userCount));

#ifdef _DEBUG
	if (userCount == 0) {
		Log("Remove console");
		FreeConsole();
	}
#endif
}

void Logger::Log(std::string const& message, std::string const& tag)
{
#ifdef _DEBUG
	printf("[%s]", (tag).c_str());
	OutputDebugString(("[" + tag + "] ").c_str());

	if (ModuleName_ != "")
		printf("<%s> ", (ModuleName_).c_str()), OutputDebugString(("<" + ModuleName_ + "> ").c_str());
	printf("%s\n", message.c_str());
	OutputDebugString(message.c_str());
	OutputDebugString("\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
#endif
}

void Logger::LowLevelLog(const std::string& message, const std::string& tag)
{
#ifdef _DEBUG
	printf("[%s]", (tag).c_str());
	OutputDebugString(("[" + tag + "] ").c_str());

	printf("%s\n", message.c_str());
	OutputDebugString(message.c_str());
	OutputDebugString("\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
#endif
}


void Logger::Debug(const string& message)
{
	Log(message, "DEBUG");
}

void Logger::Info(const string& message)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN);
	Log(message, "INFO");
}

void Logger::Warn(const string& message)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
	Log(message, "WARN");
}

void Logger::Error(const string& message)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	Log(message, "ERROR");
}

void Logger::Critical(const string& message)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | BACKGROUND_RED);
	Log(message, "CRITICAL");
}
