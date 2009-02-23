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

// ============================================================================
// WARNING:
// This file was generated by AsmJitPrettyPrinter_regen.py script, do not
// modify it, instead modify the script.
// ============================================================================

// We are using sprintf() here.
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif // _MSC_VER

// [Dependencies]
#include "AsmJitPrettyPrinter.h"

#include <stdio.h>
#include <stdlib.h>

namespace AsmJit {

static const char prettyNames[] =
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
  "jmp\0"
  "lddqu\0"
  "ldmxcsr\0"
  "lea\0"
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
  "pshuhw\0"
  "pshulw\0"
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

static const UInt16 prettyIndex[] =
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
  765, 
  771, 
  776, 
  781, 
  787, 
  792, 
  798, 
  803, 
  809, 
  816, 
  824, 
  831, 
  839, 
  848, 
  856, 
  864, 
  871, 
  876, 
  882, 
  889, 
  895, 
  902, 
  907, 
  915, 
  920, 
  926, 
  932, 
  939, 
  945, 
  951, 
  957, 
  963, 
  970, 
  976, 
  983, 
  988, 
  994, 
  1002, 
  1008, 
  1013, 
  1019, 
  1026, 
  1032, 
  1039, 
  1043, 
  1048, 
  1054, 
  1061, 
  1068, 
  1075, 
  1082, 
  1089, 
  1095, 
  1100, 
  1105, 
  1111, 
  1118, 
  1125, 
  1130, 
  1137, 
  1144, 
  1152, 
  1159, 
  1166, 
  1172, 
  1179, 
  1185, 
  1193, 
  1200, 
  1206, 
  1213, 
  1218, 
  1226, 
  1232, 
  1236, 
  1242, 
  1249, 
  1254, 
  1260, 
  1265, 
  1271, 
  1277, 
  1284, 
  1289, 
  1295, 
  1302, 
  1310, 
  1317, 
  1325, 
  1331, 
  1336, 
  1341, 
  1349, 
  1356, 
  1364, 
  1370, 
  1378, 
  1385, 
  1392, 
  1399, 
  1406, 
  1411, 
  1416, 
  1420, 
  1425, 
  1428, 
  1432, 
  1435, 
  1439, 
  1442, 
  1445, 
  1448, 
  1452, 
  1455, 
  1459, 
  1463, 
  1468, 
  1472, 
  1477, 
  1481, 
  1485, 
  1489, 
  1494, 
  1498, 
  1503, 
  1507, 
  1511, 
  1515, 
  1519, 
  1522, 
  1525, 
  1529, 
  1533, 
  1536, 
  1539, 
  1543, 
  1547, 
  1553, 
  1561, 
  1565, 
  1572, 
  1577, 
  1588, 
  1597, 
  1603, 
  1609, 
  1615, 
  1621, 
  1628, 
  1634, 
  1640, 
  1646, 
  1652, 
  1660, 
  1664, 
  1671, 
  1678, 
  1684, 
  1689, 
  1697, 
  1705, 
  1712, 
  1719, 
  1727, 
  1734, 
  1741, 
  1749, 
  1756, 
  1763, 
  1772, 
  1781, 
  1789, 
  1798, 
  1805, 
  1813, 
  1821, 
  1828, 
  1833, 
  1841, 
  1847, 
  1856, 
  1865, 
  1871, 
  1877, 
  1884, 
  1891, 
  1898, 
  1904, 
  1908, 
  1916, 
  1920, 
  1926, 
  1932, 
  1938, 
  1944, 
  1950, 
  1954, 
  1958, 
  1962, 
  1965, 
  1970, 
  1975, 
  1981, 
  1987, 
  1993, 
  2002, 
  2011, 
  2020, 
  2029, 
  2035, 
  2041, 
  2047, 
  2054, 
  2061, 
  2069, 
  2077, 
  2083, 
  2091, 
  2096, 
  2102, 
  2108, 
  2114, 
  2120, 
  2129, 
  2137, 
  2145, 
  2153, 
  2161, 
  2169, 
  2179, 
  2189, 
  2197, 
  2205, 
  2213, 
  2221, 
  2231, 
  2241, 
  2248, 
  2255, 
  2262, 
  2269, 
  2275, 
  2281, 
  2287, 
  2293, 
  2301, 
  2309, 
  2317, 
  2323, 
  2329, 
  2335, 
  2342, 
  2350, 
  2356, 
  2365, 
  2374, 
  2383, 
  2391, 
  2397, 
  2404, 
  2411, 
  2419, 
  2426, 
  2437, 
  2444, 
  2452, 
  2459, 
  2465, 
  2471, 
  2478, 
  2485, 
  2492, 
  2502, 
  2510, 
  2517, 
  2524, 
  2531, 
  2538, 
  2545, 
  2552, 
  2559, 
  2566, 
  2573, 
  2580, 
  2587, 
  2594, 
  2603, 
  2612, 
  2621, 
  2630, 
  2639, 
  2648, 
  2657, 
  2666, 
  2675, 
  2684, 
  2693, 
  2702, 
  2711, 
  2718, 
  2727, 
  2735, 
  2742, 
  2749, 
  2756, 
  2764, 
  2768, 
  2774, 
  2781, 
  2787, 
  2793, 
  2797, 
  2806, 
  2813, 
  2820, 
  2827, 
  2834, 
  2841, 
  2848, 
  2855, 
  2862, 
  2869, 
  2875, 
  2882, 
  2888, 
  2894, 
  2900, 
  2906, 
  2912, 
  2919, 
  2925, 
  2931, 
  2937, 
  2943, 
  2949, 
  2956, 
  2963, 
  2971, 
  2979, 
  2985, 
  2992, 
  2998, 
  3008, 
  3018, 
  3029, 
  3039, 
  3049, 
  3059, 
  3070, 
  3080, 
  3085, 
  3092, 
  3099, 
  3106, 
  3111, 
  3115, 
  3121, 
  3127, 
  3131, 
  3137, 
  3144, 
  3148, 
  3152, 
  3156, 
  3164, 
  3172, 
  3180, 
  3188, 
  3196, 
  3204, 
  3209, 
  3213, 
  3217, 
  3221, 
  3228, 
  3232, 
  3237, 
  3241, 
  3246, 
  3253, 
  3260, 
  3267, 
  3274, 
  3281, 
  3285, 
  3289, 
  3297, 
  3301, 
  3307, 
  3313, 
  3319, 
  3325, 
  3330, 
  3338, 
  3346, 
  3350, 
  3359, 
  3368, 
  3377, 
  3386, 
  3391, 
  3396, 
  3400, 
  3406
};

PrettyPrinter::PrettyPrinter()
{
}

PrettyPrinter::~PrettyPrinter()
{
}

void PrettyPrinter::logInstruction(UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3)
{
  char buf[1024];
  char* cur = buf;

  // dump instruction
  cur += dumpInstruction(cur, code);
  
  // dump operands
  if (!o1->isNone()) { *cur++ = ' '; cur += dumpOperand(cur, o1); }
  if (!o2->isNone()) { *cur++ = ','; *cur++ = ' '; cur += dumpOperand(cur, o2); }
  if (!o3->isNone()) { *cur++ = ','; *cur++ = ' '; cur += dumpOperand(cur, o3); }
  
  *cur = '\0';
  log(buf);
}

void PrettyPrinter::logAlign(SysInt m)
{
  char buf[1024];
  sprintf(buf, ".align %d", (Int32)m);
  log(buf);
}

void PrettyPrinter::logLabel(const Label* label)
{
  char buf[1024];
  char* p = buf + dumpLabel(buf, label);
  *p++ = ':';
  *p = '\0';
  log(buf);
}

void PrettyPrinter::log(const char* buf)
{
  fprintf(stderr, "%s\n", buf);
}

SysInt PrettyPrinter::dumpInstruction(char* buf, UInt32 code)
{
  ASMJIT_ASSERT(code < _INST_COUNT);
  const char *name = &prettyNames[prettyIndex[code]];
  SysInt len = (SysInt)strlen(name);

  memcpy(buf, name, len);
  return len;
}

SysInt PrettyPrinter::dumpOperand(char* buf, const Operand* op)
{
  if (op->isReg())
  {
    const BaseReg& reg = operand_cast<const BaseReg&>(*op);

    return dumpRegister(buf, reg.type(), reg.index());
  }
  else if (op->isMem())
  {
    const char* beg = buf;
    const Mem& mem = operand_cast<const Mem&>(*op);

    *buf++ = '[';

    if (mem.hasBase())
    {
      buf += dumpRegister(buf, REG_GPN, mem.base());
    }

    if (mem.hasIndex())
    {
      *buf++ = ' ';
      *buf++ = '+';
      *buf++ = ' ';
      buf += dumpRegister(buf, REG_GPN, mem.index());

      if (mem.shift())
      {
        *buf++ = ' ';
        *buf++ = '*';
        *buf++ = ' ';
        *buf++ = "1248"[mem.shift()];
      }
    }

    if (mem.displacement())
    {
      Int32 d = (Int32)mem.displacement();
      buf += sprintf(buf, " %c %d", 
        d >= 0 ? '+' : '-', 
        d >= 0 ? d : -d);
    }

    *buf++ = ']';
    return (SysInt)(buf - beg);
  }
  else if (op->isImm())
  {
    const Immediate& i = operand_cast<const Immediate&>(*op);

    return sprintf(buf, "0x%llX", (Int64)i.value());
  }
  else if (op->isLabel())
  {
    return dumpLabel(buf, (const Label*)op);
  }
  else
    return 0;
}

SysInt PrettyPrinter::dumpRegister(char* buf, UInt8 type, UInt8 index)
{
  const char regs1[] = "al\0" "cl\0" "dl\0" "bl\0" "ah\0" "ch\0" "dh\0" "bh\0";
  const char regs2[] = "ax\0" "cx\0" "dx\0" "bx\0" "sp\0" "bp\0" "si\0" "di\0";
  
  switch (type)
  {
    case REG_GPB:
      if (index < 8)
        return sprintf(buf, "%s", &regs1[index*3]);
      else
        return sprintf(buf, "r%b", (Int32)index);
    case REG_GPW:
      if (index < 8)
        return sprintf(buf, "%s", &regs2[index*3]);
      else
        return sprintf(buf, "r%w", (Int32)index);
    case REG_GPD:
      if (index < 8)
        return sprintf(buf, "e%s", &regs2[index*3]);
      else
        return sprintf(buf, "r%d", (Int32)index);
    case REG_GPQ:
      if (index < 8)
        return sprintf(buf, "r%s", &regs2[index*3]);
      else
        return sprintf(buf, "r%", (Int32)index);
    case REG_X87:
      return sprintf(buf, "st(%d)", (Int32)index);
    case REG_MM:
      return sprintf(buf, "mm%d", (Int32)index);
    case REG_XMM:
      return sprintf(buf, "xmm%d", (Int32)index);
    default:
      return 0;
  }
}

SysInt PrettyPrinter::dumpLabel(char* buf, const Label* label)
{
  char* beg = buf;
  *buf++ = 'L';
  
  if (label->labelId())
    buf += sprintf(buf, "%d", (Int32)label->labelId());
  else
    *buf++ = 'x';

  return (SysInt)(buf - beg);
}

} // AsmJit namespace
