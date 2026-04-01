
#include "../../../../../Interface/DirectXDeviceInterface.h"

struct FIMGUIPipeline :public IDirectXDeviceInterface_Struct
{
	FIMGUIPipeline();
	~FIMGUIPipeline();
	void Init(ID3D12DescriptorHeap* InHeap, UINT InOffset);

	void Draw(float DeltaTime);

	void Exit();

protected:
	void Tick(float DeltaTime);
private:
	bool bInitialized = false;
	bool bContextCreated = false;
};