// AsmJit - Complete JIT Assembler for C++ Language.

// Copyright (c) 2008-2009, Petr Kobalicek <kobalicek.petr@gmail.com>
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

// ----------------------------------------------------------------------------
// WARNING:
// This file was generated by AsmJitLoggerX86X64Dump.py script, do not
// modify it, instead modify the script.
// ----------------------------------------------------------------------------

// We are using sprintf() here.
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif // _MSC_VER

// [Dependencies]
#include "AsmJitLogger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

namespace AsmJit {

// [Constants]

static const char instructionName[] =
  "adc\0"
  "add\0"
  "addpd\0"
  "addps\0"
  "addsd\0"
  "addss\0"
  "addsubpd\0"
  "addsubps\0"
  "amd_prefetch\0"
  "amd_prefetchw\0"
  "and\0"
  "andnpd\0"
  "andnps\0"
  "andpd\0"
  "andps\0"
  "blendpd\0"
  "blendps\0"
  "blendvpd\0"
  "blendvps\0"
  "bsf\0"
  "bsr\0"
  "bswap\0"
  "bt\0"
  "btc\0"
  "btr\0"
  "bts\0"
  "call\0"
  "cbw\0"
  "cdqe\0"
  "clc\0"
  "cld\0"
  "clflush\0"
  "cmc\0"
  "cmova\0"
  "cmovae\0"
  "cmovb\0"
  "cmovbe\0"
  "cmovc\0"
  "cmove\0"
  "cmovg\0"
  "cmovge\0"
  "cmovl\0"
  "cmovle\0"
  "cmovna\0"
  "cmovnae\0"
  "cmovnb\0"
  "cmovnbe\0"
  "cmovnc\0"
  "cmovne\0"
  "cmovng\0"
  "cmovnge\0"
  "cmovnl\0"
  "cmovnle\0"
  "cmovno\0"
  "cmovnp\0"
  "cmovns\0"
  "cmovnz\0"
  "cmovo\0"
  "cmovp\0"
  "cmovpe\0"
  "cmovpo\0"
  "cmovs\0"
  "cmovz\0"
  "cmp\0"
  "cmppd\0"
  "cmpps\0"
  "cmpsd\0"
  "cmpss\0"
  "cmpxchg\0"
  "cmpxchg16b\0"
  "cmpxchg8b\0"
  "comisd\0"
  "comiss\0"
  "cpuid\0"
  "crc32\0"
  "cvtdq2pd\0"
  "cvtdq2ps\0"
  "cvtpd2dq\0"
  "cvtpd2pi\0"
  "cvtpd2ps\0"
  "cvtpi2pd\0"
  "cvtpi2ps\0"
  "cvtps2dq\0"
  "cvtps2pd\0"
  "cvtps2pi\0"
  "cvtsd2si\0"
  "cvtsd2ss\0"
  "cvtsi2sd\0"
  "cvtsi2ss\0"
  "cvtss2sd\0"
  "cvtss2si\0"
  "cvttpd2dq\0"
  "cvttpd2pi\0"
  "cvttps2dq\0"
  "cvttps2pi\0"
  "cvttsd2si\0"
  "cvttss2si\0"
  "cwde\0"
  "daa\0"
  "das\0"
  "dec\0"
  "div\0"
  "divpd\0"
  "divps\0"
  "divsd\0"
  "divss\0"
  "dppd\0"
  "dpps\0"
  "emms\0"
  "enter\0"
  "extractps\0"
  "f2xm1\0"
  "fabs\0"
  "fadd\0"
  "faddp\0"
  "fbld\0"
  "fbstp\0"
  "fchs\0"
  "fclex\0"
  "fcmovb\0"
  "fcmovbe\0"
  "fcmove\0"
  "fcmovnb\0"
  "fcmovnbe\0"
  "fcmovne\0"
  "fcmovnu\0"
  "fcmovu\0"
  "fcom\0"
  "fcomi\0"
  "fcomip\0"
  "fcomp\0"
  "fcompp\0"
  "fcos\0"
  "fdecstp\0"
  "fdiv\0"
  "fdivp\0"
  "fdivr\0"
  "fdivrp\0"
  "femms\0"
  "ffree\0"
  "fiadd\0"
  "ficom\0"
  "ficomp\0"
  "fidiv\0"
  "fidivr\0"
  "fild\0"
  "fimul\0"
  "fincstp\0"
  "finit\0"
  "fist\0"
  "fistp\0"
  "fisttp\0"
  "fisub\0"
  "fisubr\0"
  "fld\0"
  "fld1\0"
  "fldcw\0"
  "fldenv\0"
  "fldl2e\0"
  "fldl2t\0"
  "fldlg2\0"
  "fldln2\0"
  "fldpi\0"
  "fldz\0"
  "fmul\0"
  "fmulp\0"
  "fnclex\0"
  "fninit\0"
  "fnop\0"
  "fnsave\0"
  "fnstcw\0"
  "fnstenv\0"
  "fnstsw\0"
  "fpatan\0"
  "fprem\0"
  "fprem1\0"
  "fptan\0"
  "frndint\0"
  "frstor\0"
  "fsave\0"
  "fscale\0"
  "fsin\0"
  "fsincos\0"
  "fsqrt\0"
  "fst\0"
  "fstcw\0"
  "fstenv\0"
  "fstp\0"
  "fstsw\0"
  "fsub\0"
  "fsubp\0"
  "fsubr\0"
  "fsubrp\0"
  "ftst\0"
  "fucom\0"
  "fucomi\0"
  "fucomip\0"
  "fucomp\0"
  "fucompp\0"
  "fwait\0"
  "fxam\0"
  "fxch\0"
  "fxrstor\0"
  "fxsave\0"
  "fxtract\0"
  "fyl2x\0"
  "fyl2xp1\0"
  "haddpd\0"
  "haddps\0"
  "hsubpd\0"
  "hsubps\0"
  "idiv\0"
  "imul\0"
  "inc\0"
  "int3\0"
  "ja\0"
  "jae\0"
  "jb\0"
  "jbe\0"
  "jc\0"
  "je\0"
  "jg\0"
  "jge\0"
  "jl\0"
  "jle\0"
  "jna\0"
  "jnae\0"
  "jnb\0"
  "jnbe\0"
  "jnc\0"
  "jne\0"
  "jng\0"
  "jnge\0"
  "jnl\0"
  "jnle\0"
  "jno\0"
  "jnp\0"
  "jns\0"
  "jnz\0"
  "jo\0"
  "jp\0"
  "jpe\0"
  "jpo\0"
  "js\0"
  "jz\0"
  "jmp\0"
  "lddqu\0"
  "ldmxcsr\0"
  "lea\0"
  "leave\0"
  "lfence\0"
  "lock\0"
  "maskmovdqu\0"
  "maskmovq\0"
  "maxpd\0"
  "maxps\0"
  "maxsd\0"
  "maxss\0"
  "mfence\0"
  "minpd\0"
  "minps\0"
  "minsd\0"
  "minss\0"
  "monitor\0"
  "mov\0"
  "movapd\0"
  "movaps\0"
  "movbe\0"
  "movd\0"
  "movddup\0"
  "movdq2q\0"
  "movdqa\0"
  "movdqu\0"
  "movhlps\0"
  "movhpd\0"
  "movhps\0"
  "movlhps\0"
  "movlpd\0"
  "movlps\0"
  "movmskpd\0"
  "movmskps\0"
  "movntdq\0"
  "movntdqa\0"
  "movnti\0"
  "movntpd\0"
  "movntps\0"
  "movntq\0"
  "movq\0"
  "movq2dq\0"
  "movsd\0"
  "movshdup\0"
  "movsldup\0"
  "movss\0"
  "movsx\0"
  "movsxd\0"
  "movupd\0"
  "movups\0"
  "movzx\0"
  "mov\0"
  "mpsadbw\0"
  "mul\0"
  "mulpd\0"
  "mulps\0"
  "mulsd\0"
  "mulss\0"
  "mwait\0"
  "neg\0"
  "nop\0"
  "not\0"
  "or\0"
  "orpd\0"
  "orps\0"
  "pabsb\0"
  "pabsd\0"
  "pabsw\0"
  "packssdw\0"
  "packsswb\0"
  "packusdw\0"
  "packuswb\0"
  "paddb\0"
  "paddd\0"
  "paddq\0"
  "paddsb\0"
  "paddsw\0"
  "paddusb\0"
  "paddusw\0"
  "paddw\0"
  "palignr\0"
  "pand\0"
  "pandn\0"
  "pause\0"
  "pavgb\0"
  "pavgw\0"
  "pblendvb\0"
  "pblendw\0"
  "pcmpeqb\0"
  "pcmpeqd\0"
  "pcmpeqq\0"
  "pcmpeqw\0"
  "pcmpestri\0"
  "pcmpestrm\0"
  "pcmpgtb\0"
  "pcmpgtd\0"
  "pcmpgtq\0"
  "pcmpgtw\0"
  "pcmpistri\0"
  "pcmpistrm\0"
  "pextrb\0"
  "pextrd\0"
  "pextrq\0"
  "pextrw\0"
  "pf2id\0"
  "pf2iw\0"
  "pfacc\0"
  "pfadd\0"
  "pfcmpeq\0"
  "pfcmpge\0"
  "pfcmpgt\0"
  "pfmax\0"
  "pfmin\0"
  "pfmul\0"
  "pfnacc\0"
  "pfpnacc\0"
  "pfrcp\0"
  "pfrcpit1\0"
  "pfrcpit2\0"
  "pfrsqit1\0"
  "pfrsqrt\0"
  "pfsub\0"
  "pfsubr\0"
  "phaddd\0"
  "phaddsw\0"
  "phaddw\0"
  "phminposuw\0"
  "phsubd\0"
  "phsubsw\0"
  "phsubw\0"
  "pi2fd\0"
  "pi2fw\0"
  "pinsrb\0"
  "pinsrd\0"
  "pinsrw\0"
  "pmaddubsw\0"
  "pmaddwd\0"
  "pmaxsb\0"
  "pmaxsd\0"
  "pmaxsw\0"
  "pmaxub\0"
  "pmaxud\0"
  "pmaxuw\0"
  "pminsb\0"
  "pminsd\0"
  "pminsw\0"
  "pminub\0"
  "pminud\0"
  "pminuw\0"
  "pmovmskb\0"
  "pmovsxbd\0"
  "pmovsxbq\0"
  "pmovsxbw\0"
  "pmovsxdq\0"
  "pmovsxwd\0"
  "pmovsxwq\0"
  "pmovzxbd\0"
  "pmovzxbq\0"
  "pmovzxbw\0"
  "pmovzxdq\0"
  "pmovzxwd\0"
  "pmovzxwq\0"
  "pmuldq\0"
  "pmulhrsw\0"
  "pmulhuw\0"
  "pmulhw\0"
  "pmulld\0"
  "pmullw\0"
  "pmuludq\0"
  "pop\0"
  "popad\0"
  "popcnt\0"
  "popfd\0"
  "popfq\0"
  "por\0"
  "prefetch\0"
  "psadbw\0"
  "pshufb\0"
  "pshufd\0"
  "pshufw\0"
  "pshufhw\0"
  "pshuflw\0"
  "psignb\0"
  "psignd\0"
  "psignw\0"
  "pslld\0"
  "pslldq\0"
  "psllq\0"
  "psllw\0"
  "psrad\0"
  "psraw\0"
  "psrld\0"
  "psrldq\0"
  "psrlq\0"
  "psrlw\0"
  "psubb\0"
  "psubd\0"
  "psubq\0"
  "psubsb\0"
  "psubsw\0"
  "psubusb\0"
  "psubusw\0"
  "psubw\0"
  "pswapd\0"
  "ptest\0"
  "punpckhbw\0"
  "punpckhdq\0"
  "punpckhqdq\0"
  "punpckhwd\0"
  "punpcklbw\0"
  "punpckldq\0"
  "punpcklqdq\0"
  "punpcklwd\0"
  "push\0"
  "pushad\0"
  "pushfd\0"
  "pushfq\0"
  "pxor\0"
  "rcl\0"
  "rcpps\0"
  "rcpss\0"
  "rcr\0"
  "rdtsc\0"
  "rdtscp\0"
  "ret\0"
  "rol\0"
  "ror\0"
  "roundpd\0"
  "roundps\0"
  "roundsd\0"
  "roundss\0"
  "rsqrtps\0"
  "rsqrtss\0"
  "sahf\0"
  "sal\0"
  "sar\0"
  "sbb\0"
  "sfence\0"
  "shl\0"
  "shld\0"
  "shr\0"
  "shrd\0"
  "shufps\0"
  "sqrtpd\0"
  "sqrtps\0"
  "sqrtsd\0"
  "sqrtss\0"
  "stc\0"
  "std\0"
  "stmxcsr\0"
  "sub\0"
  "subpd\0"
  "subps\0"
  "subsd\0"
  "subss\0"
  "test\0"
  "ucomisd\0"
  "ucomiss\0"
  "ud2\0"
  "unpckhpd\0"
  "unpckhps\0"
  "unpcklpd\0"
  "unpcklps\0"
  "xadd\0"
  "xchg\0"
  "xor\0"
  "xorpd\0"
  "xorps\0";

static const UInt16 instructionIndex[] =
{
  0, 
  4, 
  8, 
  14, 
  20, 
  26, 
  32, 
  41, 
  50, 
  63, 
  77, 
  81, 
  88, 
  95, 
  101, 
  107, 
  115, 
  123, 
  132, 
  141, 
  145, 
  149, 
  155, 
  158, 
  162, 
  166, 
  170, 
  175, 
  179, 
  184, 
  188, 
  192, 
  200, 
  204, 
  210, 
  217, 
  223, 
  230, 
  236, 
  242, 
  248, 
  255, 
  261, 
  268, 
  275, 
  283, 
  290, 
  298, 
  305, 
  312, 
  319, 
  327, 
  334, 
  342, 
  349, 
  356, 
  363, 
  370, 
  376, 
  382, 
  389, 
  396, 
  402, 
  408, 
  412, 
  418, 
  424, 
  430, 
  436, 
  444, 
  455, 
  465, 
  472, 
  479, 
  485, 
  491, 
  500, 
  509, 
  518, 
  527, 
  536, 
  545, 
  554, 
  563, 
  572, 
  581, 
  590, 
  599, 
  608, 
  617, 
  626, 
  635, 
  645, 
  655, 
  665, 
  675, 
  685, 
  695, 
  700, 
  704, 
  708, 
  712, 
  716, 
  722, 
  728, 
  734, 
  740, 
  745, 
  750, 
  755, 
  761, 
  771, 
  777, 
  782, 
  787, 
  793, 
  798, 
  804, 
  809, 
  815, 
  822, 
  830, 
  837, 
  845, 
  854, 
  862, 
  870, 
  877, 
  882, 
  888, 
  895, 
  901, 
  908, 
  913, 
  921, 
  926, 
  932, 
  938, 
  945, 
  951, 
  957, 
  963, 
  969, 
  976, 
  982, 
  989, 
  994, 
  1000, 
  1008, 
  1014, 
  1019, 
  1025, 
  1032, 
  1038, 
  1045, 
  1049, 
  1054, 
  1060, 
  1067, 
  1074, 
  1081, 
  1088, 
  1095, 
  1101, 
  1106, 
  1111, 
  1117, 
  1124, 
  1131, 
  1136, 
  1143, 
  1150, 
  1158, 
  1165, 
  1172, 
  1178, 
  1185, 
  1191, 
  1199, 
  1206, 
  1212, 
  1219, 
  1224, 
  1232, 
  1238, 
  1242, 
  1248, 
  1255, 
  1260, 
  1266, 
  1271, 
  1277, 
  1283, 
  1290, 
  1295, 
  1301, 
  1308, 
  1316, 
  1323, 
  1331, 
  1337, 
  1342, 
  1347, 
  1355, 
  1362, 
  1370, 
  1376, 
  1384, 
  1391, 
  1398, 
  1405, 
  1412, 
  1417, 
  1422, 
  1426, 
  1431, 
  1434, 
  1438, 
  1441, 
  1445, 
  1448, 
  1451, 
  1454, 
  1458, 
  1461, 
  1465, 
  1469, 
  1474, 
  1478, 
  1483, 
  1487, 
  1491, 
  1495, 
  1500, 
  1504, 
  1509, 
  1513, 
  1517, 
  1521, 
  1525, 
  1528, 
  1531, 
  1535, 
  1539, 
  1542, 
  1545, 
  1549, 
  1555, 
  1563, 
  1567, 
  1573, 
  1580, 
  1585, 
  1596, 
  1605, 
  1611, 
  1617, 
  1623, 
  1629, 
  1636, 
  1642, 
  1648, 
  1654, 
  1660, 
  1668, 
  1672, 
  1679, 
  1686, 
  1692, 
  1697, 
  1705, 
  1713, 
  1720, 
  1727, 
  1735, 
  1742, 
  1749, 
  1757, 
  1764, 
  1771, 
  1780, 
  1789, 
  1797, 
  1806, 
  1813, 
  1821, 
  1829, 
  1836, 
  1841, 
  1849, 
  1855, 
  1864, 
  1873, 
  1879, 
  1885, 
  1892, 
  1899, 
  1906, 
  1912, 
  1916, 
  1924, 
  1928, 
  1934, 
  1940, 
  1946, 
  1952, 
  1958, 
  1962, 
  1966, 
  1970, 
  1973, 
  1978, 
  1983, 
  1989, 
  1995, 
  2001, 
  2010, 
  2019, 
  2028, 
  2037, 
  2043, 
  2049, 
  2055, 
  2062, 
  2069, 
  2077, 
  2085, 
  2091, 
  2099, 
  2104, 
  2110, 
  2116, 
  2122, 
  2128, 
  2137, 
  2145, 
  2153, 
  2161, 
  2169, 
  2177, 
  2187, 
  2197, 
  2205, 
  2213, 
  2221, 
  2229, 
  2239, 
  2249, 
  2256, 
  2263, 
  2270, 
  2277, 
  2283, 
  2289, 
  2295, 
  2301, 
  2309, 
  2317, 
  2325, 
  2331, 
  2337, 
  2343, 
  2350, 
  2358, 
  2364, 
  2373, 
  2382, 
  2391, 
  2399, 
  2405, 
  2412, 
  2419, 
  2427, 
  2434, 
  2445, 
  2452, 
  2460, 
  2467, 
  2473, 
  2479, 
  2486, 
  2493, 
  2500, 
  2510, 
  2518, 
  2525, 
  2532, 
  2539, 
  2546, 
  2553, 
  2560, 
  2567, 
  2574, 
  2581, 
  2588, 
  2595, 
  2602, 
  2611, 
  2620, 
  2629, 
  2638, 
  2647, 
  2656, 
  2665, 
  2674, 
  2683, 
  2692, 
  2701, 
  2710, 
  2719, 
  2726, 
  2735, 
  2743, 
  2750, 
  2757, 
  2764, 
  2772, 
  2776, 
  2782, 
  2789, 
  2795, 
  2801, 
  2805, 
  2814, 
  2821, 
  2828, 
  2835, 
  2842, 
  2850, 
  2858, 
  2865, 
  2872, 
  2879, 
  2885, 
  2892, 
  2898, 
  2904, 
  2910, 
  2916, 
  2922, 
  2929, 
  2935, 
  2941, 
  2947, 
  2953, 
  2959, 
  2966, 
  2973, 
  2981, 
  2989, 
  2995, 
  3002, 
  3008, 
  3018, 
  3028, 
  3039, 
  3049, 
  3059, 
  3069, 
  3080, 
  3090, 
  3095, 
  3102, 
  3109, 
  3116, 
  3121, 
  3125, 
  3131, 
  3137, 
  3141, 
  3147, 
  3154, 
  3158, 
  3162, 
  3166, 
  3174, 
  3182, 
  3190, 
  3198, 
  3206, 
  3214, 
  3219, 
  3223, 
  3227, 
  3231, 
  3238, 
  3242, 
  3247, 
  3251, 
  3256, 
  3263, 
  3270, 
  3277, 
  3284, 
  3291, 
  3295, 
  3299, 
  3307, 
  3311, 
  3317, 
  3323, 
  3329, 
  3335, 
  3340, 
  3348, 
  3356, 
  3360, 
  3369, 
  3378, 
  3387, 
  3396, 
  3401, 
  3406, 
  3410, 
  3416
};

static const char* operandSize[] =
{
  NULL,
  "byte ptr ",
  "word ptr ",
  NULL,
  "dword ptr ",
  NULL,
  NULL,
  NULL,
  "qword ptr ",
  NULL,
  "tword ptr ",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  "dqword ptr "
};

static const char segmentName[] =
  "\0\0\0\0"
  "cs:\0"
  "ss:\0"
  "ds:\0"
  "es:\0"
  "fs:\0"
  "gs:\0";

// [String Functions]

static char* mycpy(char* dst, const char* src, SysUInt len = (SysUInt)-1)
{
  if (src == NULL) return dst;

  if (len == (SysUInt)-1)
  {
    while (*src) *dst++ = *src++;
  }
  else
  {
    memcpy(dst, src, len);
    dst += len;
  }
 
  return dst;
}

// not too effective, but this is debug logger:)
static char* myutoa(char* dst, SysUInt i, SysUInt base = 10)
{
  static const char letters[] = "0123456789ABCDEF";

  char buf[128];
  char* p = buf + 128;

  do {
    SysInt b = i % base;
    *--p = letters[(Int8)b];
    i /= base;
  } while (i);

  return mycpy(dst, p, (SysUInt)(buf + 128 - p));
}

static char* myitoa(char* dst, SysInt i, SysUInt base = 10)
{
  if (i < 0)
  {
    *dst++ = '-';
    i = -i;
  }

  return myutoa(dst, (SysUInt)i, base);
}

// [AsmJit::Logger]

char* Logger::dumpInstruction(char* buf, UInt32 code)
{
  ASMJIT_ASSERT(code < _INST_COUNT);
  return mycpy(buf, &instructionName[instructionIndex[code]]);
}

char* Logger::dumpOperand(char* buf, const Operand* op)
{
  if (op->isReg())
  {
    const BaseReg& reg = operand_cast<const BaseReg&>(*op);
    return dumpRegister(buf, reg.type(), reg.index());
  }
  else if (op->isMem())
  {
    const Mem& mem = operand_cast<const Mem&>(*op);

    if (op->size() <= 16) 
    {
      buf = mycpy(buf, operandSize[op->size()]);
    }

    buf = mycpy(buf, &segmentName[mem.segmentPrefix() * 4]);

    *buf++ = '[';

    // [base + index*scale + displacement]
    if (mem.hasBase())
    {
      buf = dumpRegister(buf, REG_GPN, mem.base());
    }
    // [label + index*scale + displacement]
    else if (mem.label())
    {
      buf = dumpLabel(buf, mem.label());
    }
    // [absolute]
    else
    {
      buf = myutoa(buf, (SysUInt)mem._mem.target, 16);
    }

    if (mem.hasIndex())
    {
      buf = mycpy(buf, " + ");
      buf = dumpRegister(buf, REG_GPN, mem.index());

      if (mem.shift())
      {
        buf = mycpy(buf, " * ");
        *buf++ = "1248"[mem.shift() & 3];
      }
    }

    if (mem.displacement())
    {
      SysInt d = mem.displacement();
      *buf++ = (d < 0) ? '-' : '+';
      buf = myitoa(buf, d);
    }

    *buf++ = ']';
    return buf;
  }
  else if (op->isImm())
  {
    const Immediate& i = operand_cast<const Immediate&>(*op);
    return myitoa(buf, (SysInt)i.value());
  }
  else if (op->isLabel())
  {
    return dumpLabel(buf, (const Label*)op);
  }
  else
    return buf;
}

char* Logger::dumpRegister(char* buf, UInt8 type, UInt8 index)
{
  const char regs1[] = "al\0" "cl\0" "dl\0" "bl\0" "ah\0" "ch\0" "dh\0" "bh\0";
  const char regs2[] = "ax\0" "cx\0" "dx\0" "bx\0" "sp\0" "bp\0" "si\0" "di\0";
  
  switch (type)
  {
    case REG_GPB:
      if (index < 8)
        return buf + sprintf(buf, "%s", &regs1[index*3]);
      else
        return buf + sprintf(buf, "r%ub", (UInt32)index);
    case REG_GPW:
      if (index < 8)
        return buf + sprintf(buf, "%s", &regs2[index*3]);
      else
        return buf + sprintf(buf, "r%uw", (UInt32)index);
    case REG_GPD:
      if (index < 8)
        return buf + sprintf(buf, "e%s", &regs2[index*3]);
      else
        return buf + sprintf(buf, "r%ud", (UInt32)index);
    case REG_GPQ:
      if (index < 8)
        return buf + sprintf(buf, "r%s", &regs2[index*3]);
      else
        return buf + sprintf(buf, "r%u", (UInt32)index);
    case REG_X87:
      return buf + sprintf(buf, "st%u", (UInt32)index);
    case REG_MM:
      return buf + sprintf(buf, "mm%u", (UInt32)index);
    case REG_XMM:
      return buf + sprintf(buf, "xmm%u", (UInt32)index);
    default:
      return buf;
  }
}

char* Logger::dumpLabel(char* buf, const Label* label)
{
  char* beg = buf;
  *buf++ = 'L';
  
  if (label->labelId())
    buf = myutoa(buf, label->labelId());
  else
    *buf++ = 'x';

  return buf;
}

} // AsmJit namespace
