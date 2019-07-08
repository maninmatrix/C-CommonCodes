#pragma once

#include "Game/Reconnect.h"

enum {
	RECONNECT_MONSTERNIAN_SERVER
};

class ReconnectService : public Reconnect {
protected:
	virtual bool OnConnected(void *lpcbParam, int iType);
};

