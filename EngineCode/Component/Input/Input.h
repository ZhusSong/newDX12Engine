﻿#pragma once
#include "../../Core/Engine.h"

// 鼠标代理
DEFINITION_MULTICAST_SINGLE_DELEGATE(FCaptureOnMousesDelegate, void, int, int);
DEFINITION_MULTICAST_SINGLE_DELEGATE(FCaptureOnMousesWheelsDelegate, void, int, int, float);

extern FCaptureOnMousesWheelsDelegate MousesWheelsDelegate;
extern FCaptureOnMousesDelegate MouseDownDelegate;
extern FCaptureOnMousesDelegate MouseUpDelegate;
extern FCaptureOnMousesDelegate MouseMoveDelegate;