cbuffer Desc
{
	uint4 MatSize;
};
Texture2D<float> InputLeft:register(t0);
Texture2D<float> InputRight:register(t1);
RWTexture2D<float> Out:register(u0);
[numthreads(1, 1, 1)]
void main(uint3 DispatchID:SV_DispatchThreadID)
{
   if (DispatchID.x >=MatSize.w || DispatchID.y >= MatSize.x)
	return;
	float Sum = 0;
	for (uint i = 0; i < MatSize.y; ++i)
	{
		Sum= Sum+InputLeft[uint2(i, DispatchID.y)] * InputRight[uint2(DispatchID.x,i)];
	}
	Out[uint2(DispatchID.x, DispatchID.y)] = Sum;
}