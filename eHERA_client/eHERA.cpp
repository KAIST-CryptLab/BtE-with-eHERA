#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <iomanip>
#include "eHERA.h"
#include "parms.h"
#include "../BLAKE3/c/blake3.h"

__attribute__((aligned(32))) uint64_t INPUT_CONSTANT[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

// Pre-computed 16-power table of low-half and high-half
uint64_t lo_table_exp16[] = {0, 1, 65536, 65537, 10240, 10241, 75776, 75777, 1088, 1089, 66624, 66625, 11328, 11329, 76864, 76865, 170, 171, 65706, 65707, 10410, 10411, 75946, 75947, 1258, 1259, 66794, 66795, 11498, 11499, 77034, 77035, 655377, 655376, 720913, 720912, 665617, 665616, 731153, 731152, 656465, 656464, 722001, 722000, 666705, 666704, 732241, 732240, 655547, 655546, 721083, 721082, 665787, 665786, 731323, 731322, 656635, 656634, 722171, 722170, 666875, 666874, 732411, 732410, 1052672, 1052673, 1118208, 1118209, 1062912, 1062913, 1128448, 1128449, 1053760, 1053761, 1119296, 1119297, 1064000, 1064001, 1129536, 1129537, 1052842, 1052843, 1118378, 1118379, 1063082, 1063083, 1128618, 1128619, 1053930, 1053931, 1119466, 1119467, 1064170, 1064171, 1129706, 1129707, 1708049, 1708048, 1773585, 1773584, 1718289, 1718288, 1783825, 1783824, 1709137, 1709136, 1774673, 1774672, 1719377, 1719376, 1784913, 1784912, 1708219, 1708218, 1773755, 1773754, 1718459, 1718458, 1783995, 1783994, 1709307, 1709306, 1774843, 1774842, 1719547, 1719546, 1785083, 1785082, 164480, 164481, 230016, 230017, 174720, 174721, 240256, 240257, 165568, 165569, 231104, 231105, 175808, 175809, 241344, 241345, 164394, 164395, 229930, 229931, 174634, 174635, 240170, 240171, 165482, 165483, 231018, 231019, 175722, 175723, 241258, 241259, 557713, 557712, 623249, 623248, 567953, 567952, 633489, 633488, 558801, 558800, 624337, 624336, 569041, 569040, 634577, 634576, 557627, 557626, 623163, 623162, 567867, 567866, 633403, 633402, 558715, 558714, 624251, 624250, 568955, 568954, 634491, 634490, 1217152, 1217153, 1282688, 1282689, 1227392, 1227393, 1292928, 1292929, 1218240, 1218241, 1283776, 1283777, 1228480, 1228481, 1294016, 1294017, 1217066, 1217067, 1282602, 1282603, 1227306, 1227307, 1292842, 1292843, 1218154, 1218155, 1283690, 1283691, 1228394, 1228395, 1293930, 1293931, 1610385, 1610384, 1675921, 1675920, 1620625, 1620624, 1686161, 1686160, 1611473, 1611472, 1677009, 1677008, 1621713, 1621712, 1687249, 1687248, 1610299, 1610298, 1675835, 1675834, 1620539, 1620538, 1686075, 1686074, 1611387, 1611386, 1676923, 1676922, 1621627, 1621626, 1687163, 1687162, 17476, 17477, 83012, 83013, 27716, 27717, 93252, 93253, 16388, 16389, 81924, 81925, 26628, 26629, 92164, 92165, 17646, 17647, 83182, 83183, 27886, 27887, 93422, 93423, 16558, 16559, 82094, 82095, 26798, 26799, 92334, 92335, 672853, 672852, 738389, 738388, 683093, 683092, 748629, 748628, 671765, 671764, 737301, 737300, 682005, 682004, 747541, 747540, 673023, 673022, 738559, 738558, 683263, 683262, 748799, 748798, 671935, 671934, 737471, 737470, 682175, 682174, 747711, 747710, 1070148, 1070149, 1135684, 1135685, 1080388, 1080389, 1145924, 1145925, 1069060, 1069061, 1134596, 1134597, 1079300, 1079301, 1144836, 1144837, 1070318, 1070319, 1135854, 1135855, 1080558, 1080559, 1146094, 1146095, 1069230, 1069231, 1134766, 1134767, 1079470, 1079471, 1145006, 1145007, 1725525, 1725524, 1791061, 1791060, 1735765, 1735764, 1801301, 1801300, 1724437, 1724436, 1789973, 1789972, 1734677, 1734676, 1800213, 1800212, 1725695, 1725694, 1791231, 1791230, 1735935, 1735934, 1801471, 1801470, 1724607, 1724606, 1790143, 1790142, 1734847, 1734846, 1800383, 1800382, 181956, 181957, 247492, 247493, 192196, 192197, 257732, 257733, 180868, 180869, 246404, 246405, 191108, 191109, 256644, 256645, 181870, 181871, 247406, 247407, 192110, 192111, 257646, 257647, 180782, 180783, 246318, 246319, 191022, 191023, 256558, 256559, 575189, 575188, 640725, 640724, 585429, 585428, 650965, 650964, 574101, 574100, 639637, 639636, 584341, 584340, 649877, 649876, 575103, 575102, 640639, 640638, 585343, 585342, 650879, 650878, 574015, 574014, 639551, 639550, 584255, 584254, 649791, 649790, 1234628, 1234629, 1300164, 1300165, 1244868, 1244869, 1310404, 1310405, 1233540, 1233541, 1299076, 1299077, 1243780, 1243781, 1309316, 1309317, 1234542, 1234543, 1300078, 1300079, 1244782, 1244783, 1310318, 1310319, 1233454, 1233455, 1298990, 1298991, 1243694, 1243695, 1309230, 1309231, 1627861, 1627860, 1693397, 1693396, 1638101, 1638100, 1703637, 1703636, 1626773, 1626772, 1692309, 1692308, 1637013, 1637012, 1702549, 1702548, 1627775, 1627774, 1693311, 1693310, 1638015, 1638014, 1703551, 1703550, 1626687, 1626686, 1692223, 1692222, 1636927, 1636926, 1702463, 1702462, 264874, 264875, 330410, 330411, 271018, 271019, 336554, 336555, 265962, 265963, 331498, 331499, 272106, 272107, 337642, 337643, 264704, 264705, 330240, 330241, 270848, 270849, 336384, 336385, 265792, 265793, 331328, 331329, 271936, 271937, 337472, 337473, 920251, 920250, 985787, 985786, 926395, 926394, 991931, 991930, 921339, 921338, 986875, 986874, 927483, 927482, 993019, 993018, 920081, 920080, 985617, 985616, 926225, 926224, 991761, 991760, 921169, 921168, 986705, 986704, 927313, 927312, 992849, 992848, 1317546, 1317547, 1383082, 1383083, 1323690, 1323691, 1389226, 1389227, 1318634, 1318635, 1384170, 1384171, 1324778, 1324779, 1390314, 1390315, 1317376, 1317377, 1382912, 1382913, 1323520, 1323521, 1389056, 1389057, 1318464, 1318465, 1384000, 1384001, 1324608, 1324609, 1390144, 1390145, 1972923, 1972922, 2038459, 2038458, 1979067, 1979066, 2044603, 2044602, 1974011, 1974010, 2039547, 2039546, 1980155, 1980154, 2045691, 2045690, 1972753, 1972752, 2038289, 2038288, 1978897, 1978896, 2044433, 2044432, 1973841, 1973840, 2039377, 2039376, 1979985, 1979984, 2045521, 2045520, 428074, 428075, 493610, 493611, 434218, 434219, 499754, 499755, 429162, 429163, 494698, 494699, 435306, 435307, 500842, 500843, 428160, 428161, 493696, 493697, 434304, 434305, 499840, 499841, 429248, 429249, 494784, 494785, 435392, 435393, 500928, 500929, 821307, 821306, 886843, 886842, 827451, 827450, 892987, 892986, 822395, 822394, 887931, 887930, 828539, 828538, 894075, 894074, 821393, 821392, 886929, 886928, 827537, 827536, 893073, 893072, 822481, 822480, 888017, 888016, 828625, 828624, 894161, 894160, 1480746, 1480747, 1546282, 1546283, 1486890, 1486891, 1552426, 1552427, 1481834, 1481835, 1547370, 1547371, 1487978, 1487979, 1553514, 1553515, 1480832, 1480833, 1546368, 1546369, 1486976, 1486977, 1552512, 1552513, 1481920, 1481921, 1547456, 1547457, 1488064, 1488065, 1553600, 1553601, 1873979, 1873978, 1939515, 1939514, 1880123, 1880122, 1945659, 1945658, 1875067, 1875066, 1940603, 1940602, 1881211, 1881210, 1946747, 1946746, 1874065, 1874064, 1939601, 1939600, 1880209, 1880208, 1945745, 1945744, 1875153, 1875152, 1940689, 1940688, 1881297, 1881296, 1946833, 1946832, 282350, 282351, 347886, 347887, 288494, 288495, 354030, 354031, 281262, 281263, 346798, 346799, 287406, 287407, 352942, 352943, 282180, 282181, 347716, 347717, 288324, 288325, 353860, 353861, 281092, 281093, 346628, 346629, 287236, 287237, 352772, 352773, 937727, 937726, 1003263, 1003262, 943871, 943870, 1009407, 1009406, 936639, 936638, 1002175, 1002174, 942783, 942782, 1008319, 1008318, 937557, 937556, 1003093, 1003092, 943701, 943700, 1009237, 1009236, 936469, 936468, 1002005, 1002004, 942613, 942612, 1008149, 1008148, 1335022, 1335023, 1400558, 1400559, 1341166, 1341167, 1406702, 1406703, 1333934, 1333935, 1399470, 1399471, 1340078, 1340079, 1405614, 1405615, 1334852, 1334853, 1400388, 1400389, 1340996, 1340997, 1406532, 1406533, 1333764, 1333765, 1399300, 1399301, 1339908, 1339909, 1405444, 1405445, 1990399, 1990398, 2055935, 2055934, 1996543, 1996542, 2062079, 2062078, 1989311, 1989310, 2054847, 2054846, 1995455, 1995454, 2060991, 2060990, 1990229, 1990228, 2055765, 2055764, 1996373, 1996372, 2061909, 2061908, 1989141, 1989140, 2054677, 2054676, 1995285, 1995284, 2060821, 2060820, 445550, 445551, 511086, 511087, 451694, 451695, 517230, 517231, 444462, 444463, 509998, 509999, 450606, 450607, 516142, 516143, 445636, 445637, 511172, 511173, 451780, 451781, 517316, 517317, 444548, 444549, 510084, 510085, 450692, 450693, 516228, 516229, 838783, 838782, 904319, 904318, 844927, 844926, 910463, 910462, 837695, 837694, 903231, 903230, 843839, 843838, 909375, 909374, 838869, 838868, 904405, 904404, 845013, 845012, 910549, 910548, 837781, 837780, 903317, 903316, 843925, 843924, 909461, 909460, 1498222, 1498223, 1563758, 1563759, 1504366, 1504367, 1569902, 1569903, 1497134, 1497135, 1562670, 1562671, 1503278, 1503279, 1568814, 1568815, 1498308, 1498309, 1563844, 1563845, 1504452, 1504453, 1569988, 1569989, 1497220, 1497221, 1562756, 1562757, 1503364, 1503365, 1568900, 1568901, 1891455, 1891454, 1956991, 1956990, 1897599, 1897598, 1963135, 1963134, 1890367, 1890366, 1955903, 1955902, 1896511, 1896510, 1962047, 1962046, 1891541, 1891540, 1957077, 1957076, 1897685, 1897684, 1963221, 1963220, 1890453, 1890452, 1955989, 1955988, 1896597, 1896596, 1962133, 1962132};
uint64_t hi_table_exp16[] = {696577, 296, 696361, 524333, 172332, 524549, 172036, 802821, 450820, 803117, 450604, 278568, 975145, 278784, 974849, 322048, 936705, 322344, 936489, 846381, 412460, 846597, 412164, 567813, 133892, 568109, 133676, 43560, 658217, 43776, 657921, 47632, 662289, 47928, 662073, 571965, 138044, 572181, 137748, 850453, 416532, 850749, 416316, 326200, 940857, 326416, 940561, 282640, 979217, 282936, 979001, 806973, 454972, 807189, 454676, 528405, 176404, 528701, 176188, 4152, 700729, 4368, 700433, 1053328, 1749905, 1053624, 1749689, 1577661, 1225660, 1577877, 1225364, 1856149, 1504148, 1856445, 1503932, 1331896, 2028473, 1332112, 2028177, 1374352, 1989009, 1374648, 1988793, 1898685, 1464764, 1898901, 1464468, 1620117, 1186196, 1620413, 1185980, 1095864, 1710521, 1096080, 1710225, 1091712, 1706369, 1092008, 1706153, 1616045, 1182124, 1616261, 1181828, 1894533, 1460612, 1894829, 1460396, 1370280, 1984937, 1370496, 1984641, 1327744, 2024321, 1328040, 2024105, 1852077, 1500076, 1852293, 1499780, 1573509, 1221508, 1573805, 1221292, 1049256, 1745833, 1049472, 1745537, 1213124, 1582021, 1213420, 1581805, 1737449, 1057768, 1737665, 1057472, 2015937, 1336256, 2016233, 1336040, 1491692, 1860589, 1491908, 1860293, 1468612, 1886661, 1468908, 1886445, 1992937, 1362408, 1993153, 1362112, 1714369, 1083840, 1714665, 1083624, 1190124, 1608173, 1190340, 1607877, 1194196, 1612245, 1194492, 1612029, 1718521, 1087992, 1718737, 1087696, 1997009, 1366480, 1997305, 1366264, 1472764, 1890813, 1472980, 1890517, 1495764, 1864661, 1496060, 1864445, 2020089, 1340408, 2020305, 1340112, 1741521, 1061840, 1741817, 1061624, 1217276, 1586173, 1217492, 1585877, 168020, 536917, 168316, 536701, 692345, 12664, 692561, 12368, 970833, 291152, 971129, 290936, 446588, 815485, 446804, 815189, 424532, 842581, 424828, 842365, 948857, 318328, 949073, 318032, 670289, 39760, 670585, 39544, 146044, 564093, 146260, 563797, 141892, 559941, 142188, 559725, 666217, 35688, 666433, 35392, 944705, 314176, 945001, 313960, 420460, 838509, 420676, 838213, 442436, 811333, 442732, 811117, 966761, 287080, 966977, 286784, 688193, 8512, 688489, 8296, 163948, 532845, 164164, 532549, 443470, 812367, 443750, 812135, 967779, 288098, 968011, 287818, 689227, 9546, 689507, 9314, 164966, 533863, 165198, 533583, 142926, 560975, 143206, 560743, 667235, 36706, 667467, 36426, 945739, 315210, 946019, 314978, 421478, 839527, 421710, 839247, 425566, 843615, 425846, 843383, 949875, 319346, 950107, 319066, 671323, 40794, 671603, 40562, 147062, 565111, 147294, 564831, 169054, 537951, 169334, 537719, 693363, 13682, 693595, 13402, 971867, 292186, 972147, 291954, 447606, 816503, 447838, 816223, 1496798, 1865695, 1497078, 1865463, 2021107, 1341426, 2021339, 1341146, 1742555, 1062874, 1742835, 1062642, 1218294, 1587191, 1218526, 1586911, 1195230, 1613279, 1195510, 1613047, 1719539, 1089010, 1719771, 1088730, 1998043, 1367514, 1998323, 1367282, 1473782, 1891831, 1474014, 1891551, 1469646, 1887695, 1469926, 1887463, 1993955, 1363426, 1994187, 1363146, 1715403, 1084874, 1715683, 1084642, 1191142, 1609191, 1191374, 1608911, 1214158, 1583055, 1214438, 1582823, 1738467, 1058786, 1738699, 1058506, 2016971, 1337290, 2017251, 1337058, 1492710, 1861607, 1492942, 1861327, 1328778, 2025355, 1329058, 2025123, 1853095, 1501094, 1853327, 1500814, 1574543, 1222542, 1574823, 1222310, 1050274, 1746851, 1050506, 1746571, 1092746, 1707403, 1093026, 1707171, 1617063, 1183142, 1617295, 1182862, 1895567, 1461646, 1895847, 1461414, 1371298, 1985955, 1371530, 1985675, 1375386, 1990043, 1375666, 1989811, 1899703, 1465782, 1899935, 1465502, 1621151, 1187230, 1621431, 1186998, 1096882, 1711539, 1097114, 1711259, 1054362, 1750939, 1054642, 1750707, 1578679, 1226678, 1578911, 1226398, 1857183, 1505182, 1857463, 1504950, 1332914, 2029491, 1333146, 2029211, 283674, 980251, 283954, 980019, 807991, 455990, 808223, 455710, 529439, 177438, 529719, 177206, 5170, 701747, 5402, 701467, 48666, 663323, 48946, 663091, 572983, 139062, 573215, 138782, 851487, 417566, 851767, 417334, 327218, 941875, 327450, 941595, 323082, 937739, 323362, 937507, 847399, 413478, 847631, 413198, 568847, 134926, 569127, 134694, 44578, 659235, 44810, 658955, 1034, 697611, 1314, 697379, 525351, 173350, 525583, 173070, 803855, 451854, 804135, 451622, 279586, 976163, 279818, 975883, 978602, 282539, 978818, 282243, 454279, 806790, 454575, 806574, 175791, 528302, 176007, 528006, 700034, 3971, 700330, 3755, 656554, 42411, 656770, 42115, 132231, 566662, 132527, 566446, 410799, 845230, 411015, 844934, 935042, 320899, 935338, 320683, 939194, 325051, 939410, 324755, 414871, 849302, 415167, 849086, 136383, 570814, 136599, 570518, 660626, 46483, 660922, 46267, 704186, 8123, 704402, 7827, 179863, 532374, 180159, 532158, 458431, 810942, 458647, 810646, 982674, 286611, 982970, 286395, 2030650, 1334587, 2030866, 1334291, 1506327, 1858838, 1506623, 1858622, 1227839, 1580350, 1228055, 1580054, 1752082, 1056019, 1752378, 1055803, 1709626, 1095483, 1709842, 1095187, 1185303, 1619734, 1185599, 1619518, 1463871, 1898302, 1464087, 1898006, 1988114, 1373971, 1988410, 1373755, 1984042, 1369899, 1984258, 1369603, 1459719, 1894150, 1460015, 1893934, 1181231, 1615662, 1181447, 1615366, 1705474, 1091331, 1705770, 1091115, 1748010, 1051947, 1748226, 1051651, 1223687, 1576198, 1223983, 1575982, 1502255, 1854766, 1502471, 1854470, 2026498, 1330435, 2026794, 1330219, 1862766, 1494383, 1862982, 1494087, 1338435, 2018626, 1338731, 2018410, 1059947, 1740138, 1060163, 1739842, 1584198, 1215815, 1584494, 1215599, 1607278, 1189743, 1607494, 1189447, 1082947, 1713986, 1083243, 1713770, 1361515, 1992554, 1361731, 1992258, 1885766, 1468231, 1886062, 1468015, 1889918, 1472383, 1890134, 1472087, 1365587, 1996626, 1365883, 1996410, 1087099, 1718138, 1087315, 1717842, 1611350, 1193815, 1611646, 1193599, 1588350, 1219967, 1588566, 1219671, 1064019, 1744210, 1064315, 1743994, 1342587, 2022778, 1342803, 2022482, 1866838, 1498455, 1867134, 1498239, 818942, 450559, 819158, 450263, 294611, 974802, 294907, 974586, 16123, 696314, 16339, 696018, 540374, 171991, 540670, 171775, 562430, 144895, 562646, 144599, 38099, 669138, 38395, 668922, 316667, 947706, 316883, 947410, 840918, 423383, 841214, 423167, 836846, 419311, 837062, 419015, 312515, 943554, 312811, 943338, 34027, 665066, 34243, 664770, 558278, 140743, 558574, 140527, 536302, 167919, 536518, 167623, 11971, 692162, 12267, 691946, 290539, 970730, 290755, 970434, 814790, 446407, 815086, 446191, 535268, 166885, 535500, 166605, 10953, 691144, 11233, 690912, 289505, 969696, 289737, 969416, 813772, 445389, 814052, 445157, 835812, 418277, 836044, 417997, 311497, 942536, 311777, 942304, 32993, 664032, 33225, 663752, 557260, 139725, 557540, 139493, 561396, 143861, 561628, 143581, 37081, 668120, 37361, 667888, 315633, 946672, 315865, 946392, 839900, 422365, 840180, 422133, 817908, 449525, 818140, 449245, 293593, 973784, 293873, 973552, 15089, 695280, 15321, 695000, 539356, 170973, 539636, 170741, 1587316, 1218933, 1587548, 1218653, 1063001, 1743192, 1063281, 1742960, 1341553, 2021744, 1341785, 2021464, 1865820, 1497437, 1866100, 1497205, 1888884, 1471349, 1889116, 1471069, 1364569, 1995608, 1364849, 1995376, 1086065, 1717104, 1086297, 1716824, 1610332, 1192797, 1610612, 1192565, 1606244, 1188709, 1606476, 1188429, 1081929, 1712968, 1082209, 1712736, 1360481, 1991520, 1360713, 1991240, 1884748, 1467213, 1885028, 1466981, 1861732, 1493349, 1861964, 1493069, 1337417, 2017608, 1337697, 2017376, 1058913, 1739104, 1059145, 1738824, 1583180, 1214797, 1583460, 1214565, 1746976, 1050913, 1747208, 1050633, 1222669, 1575180, 1222949, 1574948, 1501221, 1853732, 1501453, 1853452, 2025480, 1329417, 2025760, 1329185, 1983008, 1368865, 1983240, 1368585, 1458701, 1893132, 1458981, 1892900, 1180197, 1614628, 1180429, 1614348, 1704456, 1090313, 1704736, 1090081, 1708592, 1094449, 1708824, 1094169, 1184285, 1618716, 1184565, 1618484, 1462837, 1897268, 1463069, 1896988, 1987096, 1372953, 1987376, 1372721, 2029616, 1333553, 2029848, 1333273, 1505309, 1857820, 1505589, 1857588, 1226805, 1579316, 1227037, 1579036, 1751064, 1055001, 1751344, 1054769, 703152, 7089, 703384, 6809, 178845, 531356, 179125, 531124, 457397, 809908, 457629, 809628, 981656, 285593, 981936, 285361, 938160, 324017, 938392, 323737, 413853, 848284, 414133, 848052, 135349, 569780, 135581, 569500, 659608, 45465, 659888, 45233, 655520, 41377, 655752, 41097, 131213, 565644, 131493, 565412, 409765, 844196, 409997, 843916, 934024, 319881, 934304, 319649, 977568, 281505, 977800, 281225, 453261, 805772, 453541, 805540, 174757, 527268, 174989, 526988, 699016, 2953, 699296, 2721, 764577, 68512, 764809, 68232, 240268, 592781, 240548, 592549, 518820, 871333, 519052, 871053, 1043081, 347016, 1043361, 346784, 999585, 385440, 999817, 385160, 475276, 909709, 475556, 909477, 196772, 631205, 197004, 630925, 721033, 106888, 721313, 106656, 725169, 111024, 725401, 110744, 200860, 635293, 201140, 635061, 479412, 913845, 479644, 913565, 1003673, 389528, 1003953, 389296, 1047217, 351152, 1047449, 350872, 522908, 875421, 523188, 875189, 244404, 596917, 244636, 596637, 768665, 72600, 768945, 72368, 1816625, 1120560, 1816857, 1120280, 1292316, 1644829, 1292596, 1644597, 1570868, 1923381, 1571100, 1923101, 2095129, 1399064, 2095409, 1398832, 2052657, 1438512, 2052889, 1438232, 1528348, 1962781, 1528628, 1962549, 1249844, 1684277, 1250076, 1683997, 1774105, 1159960, 1774385, 1159728, 1770017, 1155872, 1770249, 1155592, 1245708, 1680141, 1245988, 1679909, 1524260, 1958693, 1524492, 1958413, 2048521, 1434376, 2048801, 1434144, 2091041, 1394976, 2091273, 1394696, 1566732, 1919245, 1567012, 1919013, 1288228, 1640741, 1288460, 1640461, 1812489, 1116424, 1812769, 1116192, 1648741, 1280356, 1648973, 1280076, 1124424, 1804617, 1124704, 1804385, 1402976, 2083169, 1403208, 2082889, 1927245, 1558860, 1927525, 1558628, 1950309, 1532772, 1950541, 1532492, 1425992, 2057033, 1426272, 2056801, 1147488, 1778529, 1147720, 1778249, 1671757, 1254220, 1672037, 1253988, 1675893, 1258356, 1676125, 1258076, 1151576, 1782617, 1151856, 1782385, 1430128, 2061169, 1430360, 2060889, 1954397, 1536860, 1954677, 1536628, 1931381, 1562996, 1931613, 1562716, 1407064, 2087257, 1407344, 2087025, 1128560, 1808753, 1128792, 1808473, 1652829, 1284444, 1653109, 1284212, 604917, 236532, 605149, 236252, 80600, 760793, 80880, 760561, 359152, 1039345, 359384, 1039065, 883421, 515036, 883701, 514804, 905461, 487924, 905693, 487644, 381144, 1012185, 381424, 1011953, 102640, 733681, 102872, 733401, 626909, 209372, 627189, 209140, 622821, 205284, 623053, 205004, 98504, 729545, 98784, 729313, 377056, 1008097, 377288, 1007817, 901325, 483788, 901605, 483556, 879333, 510948, 879565, 510668, 355016, 1035209, 355296, 1034977, 76512, 756705, 76744, 756425, 600781, 232396, 601061, 232164, 880367, 511982, 880583, 511686, 356034, 1036227, 356330, 1036011, 77546, 757739, 77762, 757443, 601799, 233414, 602095, 233198, 623855, 206318, 624071, 206022, 99522, 730563, 99818, 730347, 378090, 1009131, 378306, 1008835, 902343, 484806, 902639, 484590, 906495, 488958, 906711, 488662, 382162, 1013203, 382458, 1012987, 103674, 734715, 103890, 734419, 627927, 210390, 628223, 210174, 605951, 237566, 606167, 237270, 81618, 761811, 81914, 761595, 360186, 1040379, 360402, 1040083, 884439, 516054, 884735, 515838, 1932415, 1564030, 1932631, 1563734, 1408082, 2088275, 1408378, 2088059, 1129594, 1809787, 1129810, 1809491, 1653847, 1285462, 1654143, 1285246, 1676927, 1259390, 1677143, 1259094, 1152594, 1783635, 1152890, 1783419, 1431162, 2062203, 1431378, 2061907, 1955415, 1537878, 1955711, 1537662, 1951343, 1533806, 1951559, 1533510, 1427010, 2058051, 1427306, 2057835, 1148522, 1779563, 1148738, 1779267, 1672775, 1255238, 1673071, 1255022, 1649775, 1281390, 1649991, 1281094, 1125442, 1805635, 1125738, 1805419, 1404010, 2084203, 1404226, 2083907, 1928263, 1559878, 1928559, 1559662, 2092075, 1396010, 2092291, 1395714, 1567750, 1920263, 1568046, 1920047, 1289262, 1641775, 1289478, 1641479, 1813507, 1117442, 1813803, 1117226, 1771051, 1156906, 1771267, 1156610, 1246726, 1681159, 1247022, 1680943, 1525294, 1959727, 1525510, 1959431, 2049539, 1435394, 2049835, 1435178, 2053691, 1439546, 2053907, 1439250, 1529366, 1963799, 1529662, 1963583, 1250878, 1685311, 1251094, 1685015, 1775123, 1160978, 1775419, 1160762, 1817659, 1121594, 1817875, 1121298, 1293334, 1645847, 1293630, 1645631, 1571902, 1924415, 1572118, 1924119, 2096147, 1400082, 2096443, 1399866, 1048251, 352186, 1048467, 351890, 523926, 876439, 524222, 876223, 245438, 597951, 245654, 597655, 769683, 73618, 769979, 73402, 726203, 112058, 726419, 111762, 201878, 636311, 202174, 636095, 480446, 914879, 480662, 914583, 1004691, 390546, 1004987, 390330, 1000619, 386474, 1000835, 386178, 476294, 910727, 476590, 910511, 197806, 632239, 198022, 631943, 722051, 107906, 722347, 107690, 765611, 69546, 765827, 69250, 241286, 593799, 241582, 593583, 519854, 872367, 520070, 872071, 1044099, 348034, 1044395, 347818, 345099, 1041674, 345379, 1041442, 869414, 517415, 869646, 517135, 590862, 238863, 591142, 238631, 66595, 763170, 66827, 762890, 110091, 724746, 110371, 724514, 634406, 200487, 634638, 200207, 912910, 478991, 913190, 478759, 388643, 1003298, 388875, 1003018, 392731, 1007386, 393011, 1007154, 917046, 483127, 917278, 482847, 638494, 204575, 638774, 204343, 114227, 728882, 114459, 728602, 70683, 767258, 70963, 767026, 594998, 242999, 595230, 242719, 873502, 521503, 873782, 521271, 349235, 1045810, 349467, 1045530, 1398427, 2095002, 1398707, 2094770, 1922742, 1570743, 1922974, 1570463, 1644190, 1292191, 1644470, 1291959, 1119923, 1816498, 1120155, 1816218, 1162395, 1777050, 1162675, 1776818, 1686710, 1252791, 1686942, 1252511, 1965214, 1531295, 1965494, 1531063, 1440947, 2055602, 1441179, 2055322, 1436811, 2051466, 1437091, 2051234, 1961126, 1527207, 1961358, 1526927, 1682574, 1248655, 1682854, 1248423, 1158307, 1772962, 1158539, 1772682, 1115787, 1812362, 1116067, 1812130, 1640102, 1288103, 1640334, 1287823, 1918606, 1566607, 1918886, 1566375, 1394339, 2090914, 1394571, 2090634, 1558223, 1927118, 1558503, 1926886, 2082530, 1402851, 2082762, 1402571, 1803978, 1124299, 1804258, 1124067, 1279719, 1648614, 1279951, 1648334, 1256655, 1674702, 1256935, 1674470, 1780962, 1150435, 1781194, 1150155, 2059466, 1428939, 2059746, 1428707, 1535207, 1953254, 1535439, 1952974, 1539295, 1957342, 1539575, 1957110, 2063602, 1433075, 2063834, 1432795, 1785050, 1154523, 1785330, 1154291, 1260791, 1678838, 1261023, 1678558, 1283807, 1652702, 1284087, 1652470, 1808114, 1128435, 1808346, 1128155, 2086618, 1406939, 2086898, 1406707, 1562359, 1931254, 1562591, 1930974, 513119, 882014, 513399, 881782, 1037426, 357747, 1037658, 357467, 758874, 79195, 759154, 78963, 234615, 603510, 234847, 603230, 212575, 630622, 212855, 630390, 736882, 106355, 737114, 106075, 1015386, 384859, 1015666, 384627, 491127, 909174, 491359, 908894, 486991, 905038, 487271, 904806, 1011298, 380771, 1011530, 380491, 732746, 102219, 733026, 101987, 208487, 626534, 208719, 626254, 230479, 599374, 230759, 599142, 754786, 75107, 755018, 74827, 1033290, 353611, 1033570, 353379, 509031, 877926, 509263, 877646, 229445, 598340, 229741, 598124, 753768, 74089, 753984, 73793, 1032256, 352577, 1032552, 352361, 508013, 876908, 508229, 876612, 485957, 904004, 486253, 903788, 1010280, 379753, 1010496, 379457, 731712, 101185, 732008, 100969, 207469, 625516, 207685, 625220, 211541, 629588, 211837, 629372, 735864, 105337, 736080, 105041, 1014352, 383825, 1014648, 383609, 490109, 908156, 490325, 907860, 512085, 880980, 512381, 880764, 1036408, 356729, 1036624, 356433, 757840, 78161, 758136, 77945, 233597, 602492, 233813, 602196, 1282773, 1651668, 1283069, 1651452, 1807096, 1127417, 1807312, 1127121, 2085584, 1405905, 2085880, 1405689, 1561341, 1930236, 1561557, 1929940, 1538261, 1956308, 1538557, 1956092, 2062584, 1432057, 2062800, 1431761, 1784016, 1153489, 1784312, 1153273, 1259773, 1677820, 1259989, 1677524, 1255621, 1673668, 1255917, 1673452, 1779944, 1149417, 1780160, 1149121, 2058432, 1427905, 2058728, 1427689, 1534189, 1952236, 1534405, 1951940, 1557189, 1926084, 1557485, 1925868, 2081512, 1401833, 2081728, 1401537, 1802944, 1123265, 1803240, 1123049, 1278701, 1647596, 1278917, 1647300, 1114753, 1811328, 1115049, 1811112, 1639084, 1287085, 1639300, 1286789, 1917572, 1565573, 1917868, 1565357, 1393321, 2089896, 1393537, 2089600, 1435777, 2050432, 1436073, 2050216, 1960108, 1526189, 1960324, 1525893, 1681540, 1247621, 1681836, 1247405, 1157289, 1771944, 1157505, 1771648, 1161361, 1776016, 1161657, 1775800, 1685692, 1251773, 1685908, 1251477, 1964180, 1530261, 1964476, 1530045, 1439929, 2054584, 1440145, 2054288, 1397393, 2093968, 1397689, 2093752, 1921724, 1569725, 1921940, 1569429, 1643156, 1291157, 1643452, 1290941, 1118905, 1815480, 1119121, 1815184, 69649, 766224, 69945, 766008, 593980, 241981, 594196, 241685, 872468, 520469, 872764, 520253, 348217, 1044792, 348433, 1044496, 391697, 1006352, 391993, 1006136, 916028, 482109, 916244, 481813, 637460, 203541, 637756, 203325, 113209, 727864, 113425, 727568, 109057, 723712, 109353, 723496, 633388, 199469, 633604, 199173, 911876, 477957, 912172, 477741, 387625, 1002280, 387841, 1001984, 344065, 1040640, 344361, 1040424, 868396, 516397, 868612, 516101, 589828, 237829, 590124, 237613, 65577, 762152, 65793, 761856};

// Initialization of 32-byte aligned uint64_t array
block_t block_init(size_t sz)
{
    block_t block = static_cast<uint64_t*>
        (aligned_alloc(32, sizeof(uint64_t) * sz));
    memset(block, 0, sizeof(uint64_t) * sz);
    return block;
}

// Carry-less multiplication
uint64_t clmul(uint64_t a, uint64_t b)
{
    uint64_t res = 0;
    int64_t a_copy = (int64_t) a;

    a_copy <<= 43;
    b <<= DEG - 1;

    for (int i = 0; i < DEG; i++)
    {
        res ^= (a_copy >> 63) & b;
        a_copy <<= 1;
        b >>= 1;
    }

    return res;
}

// Reduction by modulus polynomial.
// The output is of degree less than e.
uint64_t reduction(uint64_t a)
{
    uint64_t lower_mask = ((1 << DEG) - 1);
    uint64_t upper_mask = 0xffffffffffffffff - lower_mask;
    uint64_t res = a;

    uint64_t tmp = a & upper_mask;
    res &= lower_mask;
    res ^= (tmp >> 21);
    res ^= (tmp >> 19);
    tmp = res & upper_mask;
    res &= lower_mask;
    res ^= (tmp >> 21);
    res ^= (tmp >> 19);

    return res;
}

// Reduction when the argument is of degree not much larger than e.
uint64_t semi_reduction(uint64_t a)
{
    uint64_t lower_mask = ((1 << DEG) - 1);
    uint64_t upper_mask = 0xffffffffffffffff - lower_mask;
    uint64_t res = a;

    uint64_t tmp = a & upper_mask;
    res &= lower_mask;
    res ^= (tmp >> 21);
    res ^= (tmp >> 19);

    return res;

}

// Linear layer including MixColumns and MixRows
void linear(block_t state)
{
    uint64_t buf[BLOCKSIZE];

    // MixColumns
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            buf[row * 4 + col] = 2 * state[row * 4 + col];
            buf[row * 4 + col] ^= 2 * state[((row + 1) % 4) * 4 + col];
            buf[row * 4 + col] ^= state[((row + 1) % 4) * 4 + col];
            buf[row * 4 + col] ^= state[((row + 2) % 4) * 4 + col];
            buf[row * 4 + col] ^= state[((row + 3) % 4) * 4 + col];
        }
    }

    // MixRows
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            state[row * 4 + col] = 2 * buf[row * 4 + col];
            state[row * 4 + col] ^= 2 * buf[row * 4 + (col + 1) % 4];
            state[row * 4 + col] ^= buf[row * 4 + (col + 1) % 4];
            state[row * 4 + col] ^= buf[row * 4 + (col + 2) % 4];
            state[row * 4 + col] ^= buf[row * 4 + (col + 3) % 4];
        }
    }
}

// S-box layer which evaluates x -> x^17 in finite field
void exp(block_t state)
{
    uint64_t mask = (1 << 10) - 1;
    uint64_t buf[BLOCKSIZE];
    for (int i = 0; i < BLOCKSIZE; i++)
    {
        buf[i] = lo_table_exp16[mask & *(state + i)];
        buf[i] ^= hi_table_exp16[(*(state + i) >> 10) - 1];
    }
    for (int i = 0; i < BLOCKSIZE; i++)
    {
       *(state + i) = clmul(*(state + i), buf[i]);
       *(state + i) = reduction(*(state + i));
    }

    return;
}

// Initialization of secret key
void eHera::set_key(uint64_t *key)
{
    for (int i = 0; i < BLOCKSIZE; i++)
    {
        key_[i] = key[i];
    }
}

// Initialization of nonce and counter
void eHera::init(uint64_t nonce, uint64_t counter)
{
    nonce_ = nonce;
    counter_ = counter;
    keyschedule();
}

// Copying round constants to output
void eHera::get_rand_vectors(uint64_t *output)
{
    memcpy(output, rand_vectors_, sizeof(uint64_t) * XOF_ELEMENT_COUNT);
}

// Copying round keys to output
void eHera::get_round_keys(uint64_t *output)
{
    memcpy(output, round_keys_, sizeof(uint64_t) * XOF_ELEMENT_COUNT);
}

// Key schedule of eHERA including Hadamard multiplication
void eHera::keyschedule()
{
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    uint64_t mask = (1 << DEG) - 1;

    unsigned char buf[16];
    memcpy(buf, &nonce_, 8);
    memcpy(buf + 8, &counter_, 8);
    blake3_hasher_update(&hasher, buf, 16);
    uint8_t output[XOF_BYTE_COUNT + 4];
    blake3_hasher_finalize(&hasher, output, XOF_BYTE_COUNT + 4);
    int bad_cnt = 0;
    for (int i = 0; i < XOF_ELEMENT_COUNT; i++)
    {
        uint32_t tmp = *(uint32_t *) (output + 4 * i);
        if (tmp != 0)
        {
            rand_vectors_[i - bad_cnt] = (uint64_t) tmp;
            rand_vectors_[i - bad_cnt] = rand_vectors_[i - bad_cnt] & mask;
        }
        else
            bad_cnt++;
    }

    for (int i = 0; i < XOF_ELEMENT_COUNT; i++)
    {
        round_keys_[i] = clmul(rand_vectors_[i], key_[i % BLOCKSIZE]);
        round_keys_[i] = reduction(round_keys_[i]);
    }
    return;
}

// ARK: add round keys to out
void eHera::add_round_key(block_t out, int round)
{
    for (int i = 0; i < BLOCKSIZE; i++)
    {
        out[i] ^= round_keys_[round * BLOCKSIZE + i];
    }
}

// Encrypt by eHERA with no AVX instructions
void eHera::crypt_naive(block_t out)
{
    block_t res = block_init(BLOCKSIZE);
    for (int i = 0; i < BLOCKSIZE; i++)
        res[i] ^= INPUT_CONSTANT[i];

    for (int r = 0; r < ROUNDS; r++)
    {
        add_round_key(res, r);
        linear(res);
        for (int i = 0; i < BLOCKSIZE; i++)
        {
            res[i] = semi_reduction(res[i]);
        }
        exp(res);
    }
    linear(res);
    add_round_key(res, ROUNDS);
    for (int i = 0; i < BLOCKSIZE; i++)
    {
        res[i] = semi_reduction(res[i]);
    }

    for (int i = 0; i < BLOCKSIZE; i++)
        out[i] ^= res[i];

    return;
}


