#include "GuidInterface.h"

// 2025.4.李
IGuidInterface::IGuidInterface()
{
	create_guid(&Guid);
}