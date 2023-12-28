#ifndef REQUEST_MANAGER_H
#define REQUEST_MANAGER_H

#include "errorwrappers.h"
#include "parserequest.h"

#define DEBUG
#define MAX_LEN_REQUEST 4098

void ManageRequest(int sock);

#endif