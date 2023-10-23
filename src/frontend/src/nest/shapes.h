#pragma once
#include "../stdafx.h"
#include "clipper2/clipper.minkowski.h"
#include "glm/fwd.hpp"

namespace nest
{

    inline Clipper2Lib::Paths64 pattern_0()
    {
        Clipper2Lib::Paths64 polylines =
            {
                {Clipper2Lib::Point64(-954, -4337), Clipper2Lib::Point64(-954, -2847), Clipper2Lib::Point64(536, -2847), Clipper2Lib::Point64(536, -4337)}, //
                {Clipper2Lib::Point64(-557, -4055), Clipper2Lib::Point64(287, -4055), Clipper2Lib::Point64(287, -3332), Clipper2Lib::Point64(-557, -3332)}, //, Clipper2Lib::Point64(-557, -4055)

            };
        return polylines;
    }

    inline Clipper2Lib::Paths64 pattern_1()
    {
        Clipper2Lib::Paths64 polylines =
            {
                {Clipper2Lib::Point64(0, 0), Clipper2Lib::Point64(1000, 0), Clipper2Lib::Point64(1000, 1000), Clipper2Lib::Point64(0, 1000)}, //, Clipper2Lib::Point64(0, 0)

            };
        return polylines;
    }

    inline Clipper2Lib::Paths64 paths_0()
    {
        Clipper2Lib::Paths64 polylines =
            {
                {Clipper2Lib::Point64(-16623, 3190), Clipper2Lib::Point64(-16623, 13738), Clipper2Lib::Point64(1138, 13738), Clipper2Lib::Point64(1138, 3190), Clipper2Lib::Point64(-16623, 3190)},
                {Clipper2Lib::Point64(-7164, 5753), Clipper2Lib::Point64(-7164, 8793), Clipper2Lib::Point64(-3807, 8793), Clipper2Lib::Point64(-3807, 5753), Clipper2Lib::Point64(-7164, 5753)},
                {Clipper2Lib::Point64(-12585, 7001), Clipper2Lib::Point64(-12585, 10176), Clipper2Lib::Point64(-7595, 10176), Clipper2Lib::Point64(-7595, 7001), Clipper2Lib::Point64(-12585, 7001)},

            };
        return polylines;
    }

    inline Clipper2Lib::Path64
    MakeRandomPoly(int width, int height, unsigned vertCnt)
    {
        Clipper2Lib::Path64 result;
        result.reserve(vertCnt);
        for (unsigned i = 0; i < vertCnt; ++i)
            result.push_back(Clipper2Lib::Point64(rand() % width, rand() % height));
        return result;
    }

    inline Clipper2Lib::Path64 MakeStar(const Clipper2Lib::Point64 &center, int radius, int points)
    {
        if (!(points % 2))
            --points;
        if (points < 5)
            points = 5;
        Clipper2Lib::Path64 tmp = Clipper2Lib::Ellipse<int64_t>(center, radius, radius, points);
        Clipper2Lib::Path64 result;
        result.reserve(points);
        result.push_back(tmp[0]);
        for (int i = points - 1, j = i / 2; j;)
        {
            result.push_back(tmp[j--]);
            result.push_back(tmp[i--]);
        }
        return result;
    }
}