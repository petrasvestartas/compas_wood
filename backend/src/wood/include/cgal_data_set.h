#pragma once

//#include "../../stdafx.h"

// https://doc.cgal.org/latest/Box_intersection_d/index.html#Chapter_Intersecting_Sequences_of_dD_Iso-oriented_Boxes
// https://gist.github.com/afabri/827106024fd6b89fbacd6b2bbb7b3f94

inline std::vector<CGAL_Polyline> list_of_polyline_curves()
{
    std::vector<CGAL_Polyline> polylines;

    CGAL_Polyline p0{
        IK::Point_3(297.037012066934, 126.422271499885, -178.188030913779),
        IK::Point_3(321.489409568039, 169.111942318764, -186.869036108803),
        IK::Point_3(345.941807069144, 211.801613137642, -195.550041303827),
        IK::Point_3(370.394204570249, 254.491283956521, -204.231046498851),
    };

    CGAL_Polyline p1{
        IK::Point_3(182.634858082301, 98.7137182074149, -78.306080246581),
        IK::Point_3(211.235396578459, 105.640856530532, -103.27656791338),
        IK::Point_3(239.835935074617, 112.56799485365, -128.24705558018),
        IK::Point_3(268.436473570776, 119.495133176767, -153.217543246979),
        IK::Point_3(297.037012066934, 126.422271499885, -178.188030913779),
    };

    CGAL_Polyline p2{
        IK::Point_3(-125.19241232501, 107.875422065027, -39.2649958323028),
        IK::Point_3(-75.705277009549, 107.25372412331, -42.5192301219386),
        IK::Point_3(-26.8203151230943, 105.243836458838, -50.6895979092208),
        IK::Point_3(21.1131439030443, 102.043760643015, -63.0656900444498),
        IK::Point_3(67.9151143837992, 97.8400491497485, -78.9813305497922),
        IK::Point_3(113.521085871532, 92.7952640964856, -97.8579657684805),
        IK::Point_3(157.936920791687, 87.0452505103035, -119.213391744547),
        IK::Point_3(201.207350715535, 80.7012045875338, -142.653744283013),
    };

    CGAL_Polyline p3{
        IK::Point_3(-10.3599318150342, 38.7282123158674, -287.206518566188),
        IK::Point_3(-31.7764187502288, 49.1160173176098, -249.870793280387),
        IK::Point_3(-46.147480668278, 60.3975803199585, -209.510751764339),
        IK::Point_3(-54.662795188934, 72.1419219251921, -167.63145233557),
        IK::Point_3(-58.5361339307107, 84.0926783823342, -125.117457662038),
        IK::Point_3(-58.7606503977529, 96.1110401814458, -82.4406121991002),
        IK::Point_3(-56.0905042124202, 108.124674519414, -39.8417855305585),
        IK::Point_3(-51.0879024066377, 120.096820292103, 2.56039219002248),
        IK::Point_3(-44.1738209609492, 132.009588674272, 44.7113106559208),
    };

    CGAL_Polyline p4{
        IK::Point_3(-188.240159298455, 75.8808592794907, -151.530383786316),
        IK::Point_3(-145.963074583616, 73.7325840014969, -160.05232439351),
        IK::Point_3(-103.685989868776, 71.5843087235031, -168.574265000704),
        IK::Point_3(-61.4089051539373, 69.4360334455093, -177.096205607898),
        IK::Point_3(-19.1318204390981, 67.2877581675155, -185.618146215092),
        IK::Point_3(23.1452642757411, 65.1394828895218, -194.140086822286),
    };

    CGAL_Polyline p5{
        IK::Point_3(-96.6199846476256, 90.1021988025274, -110.227043500606),
        IK::Point_3(-48.9174102895825, 87.3983183807673, -111.648442107134),
        IK::Point_3(-1.21483593153943, 84.6944379590072, -113.069840713663),
        IK::Point_3(46.4877384265037, 81.9905575372471, -114.491239320191),
        IK::Point_3(94.1903127845467, 79.286677115487, -115.91263792672),
    };

    CGAL_Polyline p6{
        IK::Point_3(132.972124919423, 138.449055887252, 86.074977722597),
        IK::Point_3(167.267793637183, 134.152861891673, 62.679797641331),
        IK::Point_3(201.563462354943, 129.856667896093, 39.2846175600651),
        IK::Point_3(235.859131072703, 125.560473900514, 15.8894374787991),
    };

    CGAL_Polyline p7{
        IK::Point_3(129.410585091899, 71.8669625143157, -172.501597971961),
        IK::Point_3(151.870897769568, 83.1961266976715, -132.75167444875),
        IK::Point_3(174.331210447238, 94.5252908810273, -93.0017509255386),
        IK::Point_3(196.791523124907, 105.854455064383, -53.2518274023273),
        IK::Point_3(219.251835802577, 117.183619247739, -13.5019038791161),
        IK::Point_3(241.712148480247, 128.512783431095, 26.2480196440951),
        IK::Point_3(264.172461157916, 139.84194761445, 65.9979431673064),
    };

    CGAL_Polyline p8{
        IK::Point_3(164.938767042065, 160.473718227021, 163.598974047964),
        IK::Point_3(147.837376054901, 149.074854470926, 123.488214523064),
        IK::Point_3(131.218069322568, 137.625923389656, 83.1894892076615),
        IK::Point_3(115.06311204992, 126.130413515365, 42.715559558341),
        IK::Point_3(99.3556294473208, 114.591526704951, 2.07815097714067),
        IK::Point_3(84.0795669982659, 103.012205524298, -38.7119491181474),
        IK::Point_3(69.2196466793098, 91.3951546375981, -79.6448144234582),
        IK::Point_3(54.7613339074588, 79.7428674496858, -120.71128445365),
        IK::Point_3(40.6907906778078, 68.0576389813056, -161.902917836579),
        IK::Point_3(26.9948513423117, 56.3415940716198, -203.2118916656),
    };

    CGAL_Polyline p9{
        IK::Point_3(114.821035246594, 198.058111884719, 106.829580839264),
        IK::Point_3(139.879901144329, 179.26591505587, 135.214277443614),
        IK::Point_3(164.938767042065, 160.473718227021, 163.598974047964),
        IK::Point_3(189.9976329398, 141.681521398171, 191.983670652314),
        IK::Point_3(215.056498837535, 122.889324569322, 220.368367256664),
    };

    CGAL_Polyline p10{
        IK::Point_3(264.172461157916, 139.84194761445, 65.9979431673064),
        IK::Point_3(312.86620413861, 146.859577749078, 57.3169379722823),
        IK::Point_3(361.559947119303, 153.877207883706, 48.6359327772582),
        IK::Point_3(410.253690099997, 160.894838018334, 39.954927582234),
    };

    CGAL_Polyline p11{
        IK::Point_3(13.7562673016774, 164.804159616155, 178.132387775678),
        IK::Point_3(31.0555420852146, 160.035948344655, 140.217407062733),
        IK::Point_3(42.5288946455757, 155.666866376768, 100.123423196389),
        IK::Point_3(46.7750711952287, 151.895205966801, 58.5935066719183),
        IK::Point_3(42.9566247038049, 148.911475312983, 16.9636577178732),
        IK::Point_3(31.3140030201931, 146.81417226681, -23.2519859176756),
    };

    polylines.push_back(p0);
    polylines.push_back(p1);
    polylines.push_back(p2);
    polylines.push_back(p3);
    polylines.push_back(p4);
    polylines.push_back(p5);
    polylines.push_back(p6);
    polylines.push_back(p7);
    polylines.push_back(p8);
    polylines.push_back(p9);
    polylines.push_back(p10);
    polylines.push_back(p11);

    return polylines;
}
