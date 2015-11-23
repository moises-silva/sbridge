#include "mapper.h"
#include "diff.h"
#include "ratedet.h"
#include "scram.h"
#include "slicer.h"
#include "timing.h"
#include "equ.h"
#include "echocan.h"


   //for rates more rapid than 4800 (the data):
 	const unsigned short V32_DiffEncTable_D[16] = {0,1,2,3, 1,0,3,2, 2,3,1,0, 3,2,0,1};
 	const unsigned short V32_DiffDecodTable_D[16] = {0,1,2,3, 1,0,3,2, 3,2,0,1, 2,3,1,0};

   //for 4800 rate (the signaling):
 	const unsigned short V32_DiffEncTable_48[16] = {1,0,3,2, 3,1,2,0, 0,2,1,3, 2,3,0,1};
 	const unsigned short V32_DiffDecodTable_48[16] = {1,0,3,2, 3,1,2,0, 0,2,1,3, 2,3,0,1};

	const unsigned short V32_MAP_96[16]  = {0xFFFF, 0xFDFF, 0xFFFD, 0xFDFD,
								  0x01FF, 0x01FD, 0x03FF, 0x03FD,
								  0xFF01, 0xFF03, 0xFD01, 0xFD03,
								  0x0101, 0x0301, 0x0103, 0x0303};
	
	const short DeMap96_TBL[16] = {3,1,10,11, 2,0,8,9, 5,4,12,14, 7,6,13,15};

   const unsigned short V32_MAP_48[8]  = {0xFDFF ,0x01FD ,0xFF03 ,0x0301 ,0xFDFF ,0xFDFF ,0x0301,0x0301};

   const short V32_DeMap48_TBL[4] = {0, 2, 1, 3};

	const short V32_DeMap96_TBL[16] = {10,0,13,6, 5,15,3,8, 12,7,11,1, 2,9,4,14};

	const short V32_DeMap144_TBL[64] =
   {50,26,47,8,54,30,43,10,  19,59,7,32,17,57,3,34,
    46,9,52,28,45,12,55,31,  6,33,21,61,5,36,16,56,
    48,24,44,13,53,29,41,14, 23,63,4,37,20,60,1,38,
    42,11,49,25,40,15,51,27, 2,35,22,62,0,39,18,58};

	const short V32_ShapeFilter_19_20[] = {
   //rcos with a=0.2 ,length 4*19 :x=-9:9, ph=0.75:-0.25:0  filter=rcos(x+ph,a)
		-41 ,-21 ,159 ,-407 ,818 ,-1493 ,2699 ,-5563 ,29432 ,9628 ,-3752 ,1998 ,-1113 ,588 ,-266 ,79 ,17 ,-53 ,53 ,
 	 	-68 ,0 ,164 ,-470 ,984 ,-1825 ,4172 ,-6386 ,20666 ,20666 ,-6386 ,4172 ,-1825 ,984 ,-470 ,164 ,0 ,-68 ,78 ,
 	 	-53 ,17 ,79 ,-266 ,588 ,-1113 ,1998 ,-3752 ,9628 ,29432 ,-5563 ,2699 ,-1493 ,818 ,-407 ,159 ,-21 ,-41 ,56 ,
 	 	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,32767 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 };


   const short V32_ShapeFilter_21_20[] = {
   //rcos with a=0.2 ,length 4*21 :x=-10:10, ph=0.75:-0.25:0  filter=rcos(x+ph,a)
      56 ,-41 ,-21 ,159 ,-407 ,818 ,-1493 ,2699 ,-5563 ,29432 ,9628 ,-3752 ,1998 ,-1113 ,588 ,-266 ,79 ,17 ,-53 ,53 ,-35 ,
 	 	78 ,-68 ,0 ,164 ,-470 ,984 ,-1825 ,4172 ,-6386 ,20666 ,20666 ,-6386 ,4172 ,-1825 ,984 ,-470 ,164 ,0 ,-68 ,78 ,-57 ,
 	 	53 ,-53 ,17 ,79 ,-266 ,588 ,-1113 ,1998 ,-3752 ,9628 ,29432 ,-5563 ,2699 ,-1493 ,818 ,-407 ,159 ,-21 ,-41 ,56 ,-45 ,
 	 	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,32767 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 };

	const short V32_ShapeFilter_21_25[] = {
   //rcos with a=0.25 ,length 4*21 :x=-10:10, ph=0.75:-0.25:0  filter=rcos(x+ph,a)
		22 ,-55 ,70 ,-26 ,-132 ,484 ,-1150 ,2407 ,-5379 ,29393 ,9514 ,-3508 ,1673 ,-767 ,278 ,-34 ,-58 ,67 ,-39 ,6 ,14 ,
 	 	19 ,-66 ,98 ,-64 ,-111 ,527 ,-1335 ,2838 ,-6082 ,20557 ,20557 ,-6082 ,2838 ,-1335 ,527 ,-111 ,-64 ,98 ,-66 ,19 ,14 ,
 	 	6 ,-39 ,67 ,-58 ,-34 ,278 ,-767 ,1673 ,-3508 ,9514 ,29393 ,-5379 ,2407 ,-1150 ,484 ,-132 ,-26 ,70 ,-55 ,22 ,6 ,
 	 	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,32767 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 };


   const short V32_ShapeFilter_61_25[] = {
   //rcos with a=0.25 ,length 4*61 :x=-30:30, ph=0.75:-0.25:0  filter=rcos(x+ph,a)
      -1 ,1 ,-1 ,0 ,1 ,-2 ,2 ,-1 ,-2 ,4 ,-3 ,1 ,3 ,-7 ,7 ,
 	 	-2 ,-7 ,16 ,-18 ,6 ,22 ,-55 ,70 ,-26 ,-132 ,484 ,-1150 ,2407 ,-5379 ,29393 ,
 	 	9514 ,-3508 ,1673 ,-767 ,278 ,-34 ,-58 ,67 ,-39 ,6 ,14 ,-18 ,12 ,-2 ,-5 ,
 	 	8 ,-5 ,1 ,3 ,-4 ,3 ,-1 ,-1 ,2 ,-2 ,0 ,1 ,-1 ,1 ,0 ,
 	 	-1 ,-1 ,2 ,-2 ,1 ,1 ,-3 ,3 ,-1 ,-2 ,5 ,-5 ,3 ,3 ,-9 ,
 	 	11 ,-5 ,-7 ,20 ,-26 ,14 ,19 ,-66 ,98 ,-64 ,-111 ,527 ,-1335 ,2838 ,-6082 ,
 	 	20557 ,20557 ,-6082 ,2838 ,-1335 ,527 ,-111 ,-64 ,98 ,-66 ,19 ,14 ,-26 ,20 ,-7 ,
 	 	-5 ,11 ,-9 ,3 ,3 ,-5 ,5 ,-2 ,-1 ,3 ,-3 ,1 ,1 ,-2 ,2 ,
 	 	-1 ,-1 ,0 ,1 ,-1 ,1 ,0 ,-2 ,2 ,-1 ,-1 ,3 ,-4 ,3 ,1 ,
 	 	-5 ,8 ,-5 ,-2 ,12 ,-18 ,14 ,6 ,-39 ,67 ,-58 ,-34 ,278 ,-767 ,1673 ,
 	 	-3508 ,9514 ,29393 ,-5379 ,2407 ,-1150 ,484 ,-132 ,-26 ,70 ,-55 ,22 ,6 ,-18 ,16 ,
 	 	-7 ,-2 ,7 ,-7 ,3 ,1 ,-3 ,4 ,-2 ,-1 ,2 ,-2 ,1 ,0 ,-1 ,
 	 	1 ,-1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
 	 	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
 	 	0 ,0 ,0 ,32767 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
 	 	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
 	 	0 ,0 ,0 ,0 };

//	asm("	.sect	\".const\"");
//	asm("	.align 64");
    const short V32_SIN[32] =  // sin(2*pi*300/9600*n); n=0:31
    {
 		0 ,6393 ,12539 ,18204 ,23170 ,27245 ,30273 ,32137 ,
 	 	32767 ,32137 ,30273 ,27245 ,23170 ,18204 ,12539 ,6393 ,
 	 	0 ,-6393 ,-12540 ,-18205 ,-23170 ,-27246 ,-30274 ,-32138 ,
 	 	-32768 ,-32138 ,-30274 ,-27246 ,-23170 ,-18205 ,-12540 ,-6393
 	};

   const short V32_LpfTable[73] =
   // fir2( DCMF_LEN,[0,1500,1740,4800]/4800,[1,1,0,0],kaiser(DCMF_LEN+1,4.2) )
   // DCMF_LEN = 72
   				{
   				 1 ,-4 ,-8 ,-3 ,11 ,20 ,7 ,-25 ,
 					 -42 ,-12 ,49 ,77 ,19 ,-88 ,-131 ,-28 ,
				 	 149 ,210 ,38 ,-239 ,-324 ,-50 ,374 ,490 ,
				 	 61 ,-583 ,-745 ,-71 ,933 ,1187 ,78 ,-1663 ,
				 	 -2216 ,-84 ,4432 ,9054 ,11008 ,9054 ,4432 ,-84 ,
				 	 -2216 ,-1663 ,78 ,1187 ,933 ,-71 ,-745 ,-583 ,
				 	 61 ,490 ,374 ,-50 ,-324 ,-239 ,38 ,210 ,
				 	 149 ,-28 ,-131 ,-88 ,19 ,77 ,49 ,-12 ,
				 	 -42 ,-25 ,7 ,20 ,11 ,-3 ,-8 ,-4 , 1
               };

	const short V32_DownTable[135] = // 5 Tables * 27 Words = 135 Words
/* According to ratefil.m */
   				{
		19 ,-49 ,100 ,-181 ,303 ,-481 ,733 ,-1090 ,1604 ,-2384 ,3717 ,-6658 ,20733 ,20804 ,-6726 ,3782 ,-2443 ,1656 ,-1135 ,771 ,-511 ,326 ,-197 ,111 ,-56 ,24 ,-7 ,
 	 	13 ,-34 ,71 ,-131 ,222 ,-356 ,547 ,-816 ,1201 ,-1778 ,2733 ,-4698 ,11924 ,28108 ,-6334 ,3379 ,-2145 ,1446 ,-991 ,675 ,-450 ,289 ,-177 ,101 ,-52 ,23 ,-7 ,
 	 	4 ,-11 ,24 ,-44 ,77 ,-124 ,192 ,-288 ,425 ,-627 ,953 ,-1585 ,3521 ,32231 ,-2881 ,1434 ,-891 ,596 ,-408 ,279 ,-187 ,121 ,-75 ,44 ,-23 ,11 ,-4 ,
 	 	-3 ,9 ,-20 ,39 ,-69 ,113 ,-177 ,266 ,-393 ,579 ,-872 ,1413 ,-2859 ,32209 ,3543 ,-1606 ,972 ,-644 ,440 ,-301 ,203 ,-133 ,83 ,-49 ,26 ,-13 ,5 ,
 	 	-5 ,19 ,-46 ,91 ,-163 ,270 ,-424 ,643 ,-953 ,1402 ,-2097 ,3326 ,-6279 ,28051 ,11981 ,-4753 ,2785 ,-1825 ,1242 ,-851 ,576 ,-379 ,240 ,-143 ,79 ,-39 ,16
               };

	const short V32_UpTable[162] = // 6 Tables * 27 Words = 162 Words
/* According to ratefil.m */
   				{
				   -6 ,21 ,-49 ,97 ,-173 ,287 ,-451 ,683 ,-1012 ,1487 ,-2219 ,3505 ,-6548 ,27019 ,13471 ,-5195 ,3023 ,-1976 ,1344 ,-921 ,623 ,-411 ,260 ,-156 ,86 ,-43 ,17 ,
			 	 	-4 ,14 ,-32 ,62 ,-109 ,179 ,-280 ,422 ,-623 ,916 ,-1375 ,2209 ,-4354 ,31257 ,6199 ,-2701 ,1618 ,-1068 ,729 ,-499 ,336 ,-220 ,138 ,-82 ,45 ,-21 ,8 ,
			  	 	0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,32763 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
			 	 	7 ,-19 ,41 ,-76 ,130 ,-209 ,322 ,-481 ,708 ,-1045 ,1592 ,-2673 ,6170 ,31287 ,-4382 ,2236 ,-1400 ,939 ,-643 ,438 ,-293 ,190 ,-117 ,68 ,-36 ,16 ,-5 ,
			 	 	14 ,-38 ,79 ,-144 ,244 ,-390 ,597 ,-889 ,1307 ,-1934 ,2978 ,-5146 ,13421 ,27070 ,-6598 ,3552 ,-2262 ,1525 ,-1045 ,711 ,-474 ,304 ,-186 ,106 ,-55 ,24 ,-7 ,
			 	 	20 ,-50 ,101 ,-183 ,306 ,-485 ,738 ,-1095 ,1610 ,-2391 ,3724 ,-6665 ,20741 ,20799 ,-6721 ,3778 ,-2440 ,1653 ,-1133 ,769 ,-509 ,325 ,-197 ,111 ,-56 ,24 ,-7
					};

		const unsigned short V32_ConvEnc_TBL[32] =
		{
			0,14,18,28, 2,12,16,30, 4,10,22,24, 6,8,20,26,
			33,45,55,59, 35,47,53,57, 39,43,49,61, 37,41,51,63
			/*
			0,43,18,57, 4,45,55,30, 33,10,51,24, 39,14,20,61,
			2,41,16,59, 6,47,53,28, 35,8,49,26, 37,12,22,63
			*/
		};

   const short V32_EquSinTbl[] = // q15(sin(pi* [0:1/SIN_TBL_LEN:1]));
		{
       0    ,804   ,1608  ,2410  ,3212  ,4011  ,4808  ,5602  ,
 	 	6393  ,7179  ,7962  ,8739  ,9512  ,10278 ,11039 ,11793 ,
 	 	12539 ,13279 ,14010 ,14732 ,15446 ,16151 ,16846 ,17530 ,
 	 	18204 ,18868 ,19519 ,20159 ,20787 ,21403 ,22005 ,22594 ,
 	 	23170 ,23731 ,24279 ,24811 ,25329 ,25832 ,26319 ,26790 ,
 	 	27245 ,27683 ,28105 ,28510 ,28898 ,29268 ,29621 ,29956 ,
 	 	30273 ,30571 ,30852 ,31113 ,31356 ,31580 ,31785 ,31971 ,
 	 	32137 ,32285 ,32412 ,32521 ,32609 ,32678 ,32728 ,32757 ,
 	 	32767 ,32757 ,32728 ,32678 ,32609 ,32521 ,32412 ,32285 ,
 	 	32137 ,31971 ,31785 ,31580 ,31356 ,31113 ,30852 ,30571 ,
 	 	30273 ,29956 ,29621 ,29268 ,28898 ,28510 ,28105 ,27683 ,
 	 	27245 ,26790 ,26319 ,25832 ,25329 ,24811 ,24279 ,23731 ,
 	 	23170 ,22594 ,22005 ,21403 ,20787 ,20159 ,19519 ,18868 ,
 	 	18204 ,17530 ,16846 ,16151 ,15446 ,14732 ,14010 ,13279 ,
 	 	12539 ,11793 ,11039 ,10278 ,9512  ,8739  ,7962  ,7179  ,
 	 	6393  ,5602  ,4808  ,4011  ,3212  ,2410  ,1608  ,804   ,
 	 	0
      };


		const V32_EQU_CONVERGE_TBL V32_EquConvergeTbl[EQU_CONVERGE_TBL_LEN+1] =
		{
		//  Counter,CoefStart,CoefLen,StepSize,PllKf,PllKp
			{ 60   ,    50   ,   9   ,  22937 ,   0 , 6172},
			{ 120  ,    44   ,  15   ,  14500 ,  70 , 4129},
			{ 240  ,    38   ,  21   ,  10107 ,  70 , 3086},
			{ 450  ,    26   ,  33   ,  6553  ,  55 , 2086},
			{ 900  ,    0    ,  59   ,   2090 ,  35 , 1043},
			{ 2000 ,    0    ,  59   ,   777  ,  35 , 1043},
			{ 32000,    0    ,  59   ,   328  ,  35 , 1043}, 
			{ 32767,    0    ,  59   ,   144  ,  10 , 412},
			{ 32767,    0    ,  59   ,   41   ,   0 , 180}
		};


	const V32_EQU_CONVERGE_TBL V32_EquConvergeResyncTbl[EQU_CONVERGE_RESYNC_TBL_LEN] =
	{
	//  Counter,CoefStart,CoefLen,StepSize,PllKf,PllKp
		{  40  ,     0    ,   59  ,    0   ,   0 , 4172},
		{ 140  ,	 	 0	 	,	 59  ,	 0	  ,   0 , 2086},
		{ 300  ,	 	 0	 	,   59  ,	 0	  ,  35 , 1043},
		{32767 ,	 	 0	 	,   59  ,   6553 ,  55 , 2086}
	};

   const V32_ECH_NORM_TBL V32_EchNormTbl[ECH_MAX_NORM_BITS+1] =
   { {1, 0},

     {4, 1},

     {16, 2}
   };

		const V32_ECH_CONVERGE_TBL V32_EchConvergeTblNear[ECHO_NEAR_CONVERGE_TBL_LEN] =
		{
	//    Counter CoefStrt CoefLen Step
		   {80,  2*20, 2* 8, 16000},
		   {320,  2*16, 2*16, 8200},
		   {850,  2*9, 2*30, 4400},
		   {32767, 2*6,2*36, 3600}
	   };


		const V32_ECH_CONVERGE_TBL V32_EchConvergeTblFar[ECHO_FAR_CONVERGE_TBL_LEN] =
		{
			// Counter,CoefStrt,CoefLen,Step
			{    200  ,  2 *8  ,  2*32 , 3100},
			{   600   ,  2 *0  ,  2*48 , 1550},
			{   32767 ,  2 *0  ,  2*48 , 900},
	//		{   32767 ,  2 *0  ,  2*36 ,    0}      //this line is for the non far exho case.
		};


		const V32_TIMING_CONVERGE_TBL V32_TimingConvergeCallTbl[TIMING_CONVERGE_TBL_LEN] =
		{
		//  Counter,RateShift,RateRound,LocRound,LocShift
			{100   ,   32    ,     0L  ,   0x1  ,   1   },
			{350   ,   32    ,   0x0L  ,   0x2  ,   2   },
			{700   ,   10    ,  0x200L ,   0x10 ,   5   },
			{1200  ,   13    ,  0x1000L ,  0x10 ,   5   },
			{1800  ,   14    ,  0x2000L,   0x20 ,   6   },
			{8000  ,   17    , 0x10000L,   0x20 ,   6   },
			{32767 ,   17    , 0x10000L,   0x20 ,   6   }
		};

/*		const V32_TIMING_CONVERGE_TBL V32_TimingConvergeAnsTbl[TIMING_CONVERGE_TBL_LEN] =
		{
		//  Counter,RateShift,RateRound,LocRound,LocShift
			{100   ,   32    ,   0x0L  ,   0x1  ,   1   },
			{350   ,   32    ,   0x0L  ,   0x2  ,   2   },
			{640   ,   10    ,  0x200L ,   0x8  ,   4   },
			{1000  ,   13    ,  0x1000L,   0x8  ,   4   },
			{4000  ,   16    ,  0x8000L,   0x10 ,   5   },
			{32767 ,   20    , 0x80000L,   0x80 ,   8   }

		};*/

		const V32_TIMING_CONVERGE_TBL V32_TimingConvergeAnsTbl[TIMING_CONVERGE_TBL_LEN] =
		{
		//  Counter,RateShift,RateRound,LocRound,LocShift
			{100   ,   32    ,   0x0L  ,   0x1  ,   1   },
			{350   ,   32    ,   0x0L  ,   0x2  ,   2   },
			{640   ,   10    ,  0x200L ,   0x8  ,   4   },
			{1000  ,   13    ,  0x1000L,   0x8  ,   4   },
			{4000  ,   16    ,  0x8000L,   0x10 ,   5   },
			{8000  ,   17    , 0x10000L,   0x20 ,   6   },
			{32767 ,   20    , 0x80000L,   0x20 ,   6   }

		};


		const V32_TIMING_CONVERGE_TBL V32_TimingConvergeResyncTbl[TIMING_CONVERGE_TBL_LEN] =
		{
		//  Counter,RateShift,RateRound,LocRound,LocShift
			{150   ,	32   , 0x0L	   ,   0x1   ,   1   },
			{300   ,	32   , 0x0L	   ,   0x4   ,   3	 },
			{1000  ,    16   , 0x8000L ,   0x10  ,   5   },
			{1500  ,    16   , 0x8000L ,   0x20  ,   6   },
			{10000 ,    19   , 0x10000L,   0x20  ,   6   },
			{32767 ,    20   , 0x40000L,   0x40  ,   7   }

/*		{500,   32, 1, 0L		,0x1},
		{1000,  32, 4, 0x0L		,0x8},  //320
		{1500,  32, 4, 0x0L		,0x8},
		{2000,  18, 5, 0x20000L	,0x10},
		{15000, 19, 6, 0x40000L	,0x20}, //1500,16,5
		{32767, 20, 8, 0x40000L ,0x80}*/
	};  // Good eia3

   const short V32_PositiveHalfBaudFilter[] =
   	{ 0 ,1 ,-1 ,0 ,0 ,-1 ,1 ,0 ,0 ,1 ,-1 ,0 ,0 ,-1 ,1 ,0 ,
        0 ,1 ,-1 ,0 ,0 ,-1 ,1 ,0 ,0 ,1 ,-1 ,0 ,0 ,-1 ,1 ,0 ,
        0 ,1 ,-1 ,0 ,0 ,-1 ,1 ,0 ,0 ,1 ,-1 ,0 ,0 ,-1 ,1 ,0 };
   const short V32_NegativeHalfBaudFilter[] =
	   { 0 ,-1 ,-1 ,0 ,0 ,1 ,1 ,0 ,0 ,-1 ,-1 ,0 ,0 ,1 ,1 ,0 ,
        0 ,-1 ,-1 ,0 ,0 ,1 ,1 ,0 ,0 ,-1 ,-1 ,0 ,0 ,1 ,1 ,0 ,
        0 ,-1 ,-1 ,0 ,0 ,1 ,1 ,0 ,0 ,-1 ,-1 ,0 ,0 ,1 ,1 ,0 };
   const short V32_DCFilter[] =
	   { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
        1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
        1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };

	const short V32_DCEchoCancellFilter[] = {16384, -32768, 16384};
	const short V32_HalfBaudEchoCancellFilter[] = {16384, 0, 16384};

   const V32_RENEGOTIATE_NOISE_TBL V32_RenegotiateNoiseTbl[5] =
   {
   	{50,50},  //dummy
      {22,22},  //20db
      {11,9},  //23db
      {10,8},  //{6,4}26db
      {3,2}		//30db
   };

   const unsigned short V32_R_WordTbl[4]= {0x09d1, 0x0bd1, 0x0bf1, 0x0bf9};

   const short V32_MaxErrorTbl[4]= {87, 44, 20, 10};  //SNR:19.75, 22.75, 26, 29

	const short V32_ResyncTbl[4]= {160, 80, 40, 22};  //SNR:17, 20, 23, 25.7 
