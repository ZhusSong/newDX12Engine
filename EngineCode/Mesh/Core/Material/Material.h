// 材质
#pragma once
#include "../../../Core/CoreObject/CoreMinimalObject.h"

class CMaterial :public CCoreMinimalObject
{
public:	CMaterial();

	  void SetBaseColor(const fvector_4d& InBaseColor) { BaseColor = InBaseColor; }

	  FORCEINLINE fvector_4d GetBaseColor()const { return BaseColor; }
private:
	fvector_4d BaseColor;
};