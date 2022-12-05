#pragma once
#include "../stdafx.h"
#include "clipper2/clipper.minkowski.h"
#include "glm/fwd.hpp"

namespace nest
{
    static float scale = 10000;

    inline void preprocess_polygon(std::vector<std::vector<glm::vec3>> &polygon_with_holes)
    {
        // find bottom left corner

        // sort from the largest boundary to the smallest using bbox
    }

    inline void convert_from_clipper(Clipper2Lib::Paths64 &in_p, std::vector<std::vector<glm::vec3>> &out_p, float z = 0)
    {

        out_p.reserve(in_p.size());
        for (int i = 0; i < in_p.size(); i++)
        {
            std::vector<glm::vec3> pline;
            pline.reserve((in_p[i].size() + 1));
            for (int j = 0; j < in_p[i].size(); j++)
                pline.emplace_back((float)in_p[i][j].x / scale, (float)in_p[i][j].y / scale, z);

            pline.emplace_back((float)in_p[i][0].x / scale, (float)in_p[i][0].y / scale, z);
            out_p.emplace_back(pline);
        }
    }

    inline void convert_to_clipper(std::vector<std::vector<glm::vec3>> &in_p, Clipper2Lib::Paths64 &out_p)
    {
        out_p.reserve(in_p.size());

        for (int i = 0; i < in_p.size(); i++)
        {
            Clipper2Lib::Path64 pline;
            pline.reserve((in_p[i].size() + 1));
            for (int j = 0; j < in_p[i].size(); j++)
                pline.emplace_back((int64_t)in_p[i][j].x * (int64_t)scale, (int64_t)in_p[i][j].y * (int64_t)scale);

            pline.emplace_back((int64_t)in_p[i][0].x * (int64_t)scale, (int64_t)in_p[i][0].y * (int64_t)scale);
            out_p.emplace_back(pline);
        }
    }
}