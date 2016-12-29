#include "pt100.h"

static const int table_pt100[65]={
10000 ,
10390 ,
10779 ,
11167 ,
11554 ,
11940 ,
12324 ,
12708 ,
13090 ,
13471 ,
13851 ,
14229 ,
14607 ,
14983 ,
15358 ,
15733 ,
16105 ,
16477 ,
16848 ,
17217 ,
17586 ,
17953 ,
18319 ,
18684 ,
19047 ,
19410 ,
19771 ,
20131 ,
20490 ,
20848 ,
21205 ,
21561 ,
21915 ,
22268 ,
22621 ,
22972 ,
23321 ,
23670 ,
24018 ,
24364 ,
24709 ,
25053 ,
25396 ,
25738 ,
26078 ,
26418 ,
26756 ,
27093 ,
27429 ,
27764 ,
28098 ,
28430 ,
28762 ,
29092 ,
29421 ,
29749 ,
30075 ,
30401 ,
30725 ,
31049 ,
31371 ,
31692 ,
32012 ,
32330 ,
32648
};

float ResToTmp(float res_arg)
{
	signed char pos;
	signed char i=32;
	signed char	tmpHalf=32;
	int Res=(int)(res_arg*100);
	float dif_res0,dif_res1;
	float result;
	
	if((Res < table_pt100[0])||(Res > table_pt100[63]))
	{
			return -1;
	}
	
	while(tmpHalf)
	{
			tmpHalf >>= 1;
			if(Res > table_pt100[i])
			{
				i = i + tmpHalf;
				pos=1;
			}	
			else
			{
				i = i - tmpHalf;
				pos = -1;
			}
	}
	dif_res0 = (float)(Res - table_pt100[i]);
	dif_res1 = (float)(table_pt100[i+pos] - table_pt100[i]);
	
	result = (dif_res0/dif_res1)*(float)pos;
	result = ((float)i + result)*10;
	
	return result;
}
