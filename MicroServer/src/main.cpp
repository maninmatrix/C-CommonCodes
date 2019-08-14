#include "GameLogic.h"
#include "BaseMsg.h"
#include "RedisServer.h"
int main(int argc, char *argv[])
{
	InitializeLog(CONFIG_FILE, NULL);
	__log(_DEBUG, "main", "**************** staring%s v0.0.0.1 ****************\r\n", SERVER_NAME);
	IGameProvider *lpGameProvider = &MainLogic::Instance();

	if (0 == lpGameProvider->Start())
	{
		pause();
	}

	delete lpGameProvider;

	__log(_DEBUG, "main", "%s has been exit !\r\n", argv[0]);

	return 0;
}
