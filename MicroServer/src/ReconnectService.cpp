#include "ReconnectService.h"
#include "GameLogic.h"




bool ReconnectService::OnConnected(void *lpcbParam, int iType) {
	printf("ReconnectService::OnConnected\n");
	long lResult = -1;
	MainLogic *lpFishServer = (MainLogic*)lpcbParam;
	
	printf("ConnectOtherServer\n");
	
	lResult = lpFishServer->ConnectServer("MqProducer", lpFishServer->m_pProducer);
	
	
	

	return (0 == lResult);
}
