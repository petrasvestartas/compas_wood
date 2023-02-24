#pragma once
#include "../stdafx.h"
#include "clipper2/clipper.minkowski.h"
#include "glm/fwd.hpp"
#include "conversion.h"
#include "shapes.h"
#include <execution>
// minkowski + holes
// what to do with multiple minkowski sum outlines or I just need to take one item?
// multiple minkovskies

namespace nest
{
    static std::vector<int> placed_id;
    static std::vector<Clipper2Lib::Paths64> placed_paths;
    static std::vector<std::array<float, 3>> placed_rotationtranslation;
    static std::vector<bool> placed_pattern_or_path; // there can be multiple sheets
    static std::chrono::milliseconds total_duration;

    inline void
    nfp(Clipper2Lib::Paths64 &pattern, Clipper2Lib::Paths64 &result, int temp) // Clipper2Lib::Paths64 &path,
    {

        Clipper2Lib::Point64 start_point = pattern[0][0];
        for (int i = 0; i < pattern.size(); i++)
            for (int j = 0; j < pattern[i].size(); j++)
                pattern[i][j] = Clipper2Lib::Point64(pattern[i][j].x - start_point.x, pattern[i][j].y - start_point.y);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Compute minkowski sum for two polygons with holes
        // Do not union them, do it once iterated through all patterns
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //   auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < placed_paths.size(); i++)
        {
            // std::cout << placed_paths[i].size() << " \n";
            for (int j = 0; j < placed_paths[i].size(); j++) //
            {

                Clipper2Lib::Paths64 mink_diff = Clipper2Lib::MinkowskiDiff(pattern[0], placed_paths[i][j], true); // take only boundary of the element
                size_t start = -1;
                size_t end = -1;

                if (placed_pattern_or_path[i])
                {
                    start = j != 0 ? 1 : 0;
                    end = j != 0 ? mink_diff.size() : 1;
                }
                else
                {
                    start = j == 0 ? 1 : 0;
                    end = j == 0 ? mink_diff.size() : 1;
                }

                for (int w = start; w < end; w++)
                    result.emplace_back(mink_diff[w]);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Boolean union
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        result = Clipper2Lib::Union(result, Clipper2Lib::FillRule::Negative);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Choose the bottom left point
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        start_point = result[0][0];
        for (int i = 0; i < result.size(); i++)
            for (int j = 0; j < result[i].size(); j++)
                if (result[i][j].x < start_point.x || result[i][j].y < start_point.y)
                    start_point = result[i][j];

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Place one pattern in a loop
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < pattern.size(); i++)
            for (int j = 0; j < pattern[i].size(); j++)
                pattern[i][j] = Clipper2Lib::Point64(pattern[i][j].x + start_point.x, pattern[i][j].y + start_point.y);

        for (int i = 0; i < pattern.size(); i++)
            pattern[i].emplace_back(pattern[i][0]);

        placed_paths.emplace_back(pattern);
        std::array<float, 3> rot_tran = {0, start_point.x / scale, start_point.y / scale};
        placed_rotationtranslation.emplace_back(rot_tran);
        placed_pattern_or_path.emplace_back(true); // there can be multiple sheets
                                                   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                                   // Do another iteration
                                                   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    inline void
    nfp_parallel(Clipper2Lib::Paths64 &pattern, Clipper2Lib::Paths64 &result, int temp) // Clipper2Lib::Paths64 &path,
    {

        Clipper2Lib::Point64 start_point = pattern[0][0];
        for (int i = 0; i < pattern.size(); i++)
            for (int j = 0; j < pattern[i].size(); j++)
                pattern[i][j] = Clipper2Lib::Point64(pattern[i][j].x - start_point.x, pattern[i][j].y - start_point.y);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Compute minkowski sum for two polygons with holes
        // Do not union them, do it once iterated through all patterns
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //   auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < placed_paths.size(); i++)
        {
            // std::cout << placed_paths[i].size() << " \n";
            for (int j = 0; j < placed_paths[i].size(); j++) //
            {

                Clipper2Lib::Paths64 mink_diff = Clipper2Lib::MinkowskiDiff(pattern[0], placed_paths[i][j], true); // take only boundary of the element
                size_t start = -1;
                size_t end = -1;

                if (placed_pattern_or_path[i])
                {
                    start = j != 0 ? 1 : 0;
                    end = j != 0 ? mink_diff.size() : 1;
                }
                else
                {
                    start = j == 0 ? 1 : 0;
                    end = j == 0 ? mink_diff.size() : 1;
                }

                for (int w = start; w < end; w++)
                    result.emplace_back(mink_diff[w]);
            }
        }
        // auto stop = std::chrono::high_resolution_clock::now();
        // std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        // std::cout << "Time taken by function: " << duration.count() << " ";

        // // start = std::chrono::high_resolution_clock::now();
        // std::vector<Clipper2Lib::Paths64> mink_diffs;
        // int count = 0;
        // transform(placed_paths.begin(), placed_paths.end(), placed_pattern_or_path.begin(), std::back_inserter(mink_diffs),
        //           [&](Clipper2Lib::Paths64 &placed_path, bool placed_pattern_or_path_)
        //           {
        //               Clipper2Lib::Paths64 result_temp;

        //               result_temp.reserve(placed_path.size() * 3);
        //               for (int j = 0; j < placed_path.size(); j++) // iterate though element and its holes
        //               {

        //                   Clipper2Lib::Paths64 mink_diff = Clipper2Lib::MinkowskiDiff(pattern[0], placed_path[j], true); // take only boundary of the element
        //                   size_t start = -1;
        //                   size_t end = -1;

        //                   if (placed_pattern_or_path_) // is it sheet or element
        //                   {
        //                       start = j != 0 ? 1 : 0;
        //                       end = j != 0 ? mink_diff.size() : 1;
        //                   }
        //                   else
        //                   {
        //                       start = j == 0 ? 1 : 0;
        //                       end = j == 0 ? mink_diff.size() : 1;
        //                   }

        //                   for (int w = start; w < end; w++)
        //                   {
        //                       result_temp.emplace_back(mink_diff[w]);
        //                       count++;
        //                   }
        //               }

        //               return result_temp;
        //           });

        std::vector<std::size_t> indexes(placed_paths.size());
        std::iota(indexes.begin(), indexes.end(), 0);

        // Clipper2Lib::Paths64 result_temp;
        std::vector<Clipper2Lib::Paths64> mink_diffs(placed_paths.size());
        // result_temp.reserve(placed_paths.size() * 3);
        std::for_each(std::execution::par, indexes.begin(), indexes.end(), [&](size_t i)
                      {
                          // std::cout << i << ',' << d[i] << '\n';

                          for (int j = 0; j < placed_paths[i].size(); j++) // iterate though element and its holes
                          {

                              Clipper2Lib::Paths64 mink_diff = Clipper2Lib::MinkowskiDiff(pattern[0], placed_paths[i][j], true); // take only boundary of the element
                                                size_t start = -1;
                                                size_t end = -1;

                                                if (placed_pattern_or_path[i]) // is it sheet or element
                                                {
                                                    start = j != 0 ? 1 : 0;
                                                    end = j != 0 ? mink_diff.size() : 1;
                                                }
                                                else
                                                {
                                                    start = j == 0 ? 1 : 0;
                                                    end = j == 0 ? mink_diff.size() : 1;
                                                }

                                               // mink_diffs[i] = mink_diff;
                                                Clipper2Lib::Paths64 mink_diff_;
                                                mink_diff_.reserve(mink_diff.size());
                                                for (int w = start; w < end; w++)
                                                {
                                                     mink_diff_.emplace_back(mink_diff[w]);
                                                 }
                                                 mink_diffs[i] = mink_diff_;
                          } });

        // stop = std::chrono::high_resolution_clock::now();
        // duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        // std::cout << "| " << duration.count() << " ms \n";
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Boolean union
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Clipper2Lib::Paths64 all_paths;
        // if(temp == 2){
        // // // all_paths.reserve(count);
        //  for (auto &p : mink_diffs){
        //      std::cout << "\n";
        //      for (auto &pp : p){
        //          all_paths.emplace_back(pp);
        //          for(auto &ppp : pp){
        //              std::cout << ppp.x << " " << ppp.y << "\n";
        //          }

        //      }

        //  }
        // }

        // for loop parallel
        //  std::vector<std::size_t> indexes(mink_diffs.size());
        //  std::iota(indexes.begin(), indexes.end(), 0);

        // result = Clipper2Lib::Union(all_paths, Clipper2Lib::FillRule::Negative);
        result = Clipper2Lib::Union(result, Clipper2Lib::FillRule::Negative);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Choose the bottom left point
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        start_point = result[0][0];
        for (int i = 0; i < result.size(); i++)
            for (int j = 0; j < result[i].size(); j++)
                if (result[i][j].x < start_point.x || result[i][j].y < start_point.y)
                    start_point = result[i][j];

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Place one pattern in a loop
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < pattern.size(); i++)
            for (int j = 0; j < pattern[i].size(); j++)
                pattern[i][j] = Clipper2Lib::Point64(pattern[i][j].x + start_point.x, pattern[i][j].y + start_point.y);

        for (int i = 0; i < pattern.size(); i++)
            pattern[i].emplace_back(pattern[i][0]);

        placed_paths.emplace_back(pattern);
        std::array<float, 3> rot_tran = {0, start_point.x / scale, start_point.y / scale};
        placed_rotationtranslation.emplace_back(rot_tran);
        placed_pattern_or_path.emplace_back(true); // there can be multiple sheets
                                                   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                                   // Do another iteration
                                                   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    inline void nfps(
        std::vector<std::vector<std::vector<glm::vec3>>> &pattern_glm,
        std::vector<std::vector<std::vector<glm::vec3>>> &path_glm,
        std::vector<std::vector<std::vector<glm::vec3>>> &nfp_glm,
        std::vector<std::vector<std::vector<glm::vec3>>> &results)
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Iteration 0
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // inputs
        Clipper2Lib::Paths64 pattern = pattern_0();
        // Clipper2Lib::Paths64 pattern_closed = pattern_0();
        // pattern_closed.emplace_back(pattern_closed[0]);
        Clipper2Lib::Paths64 path = paths_0();
        Clipper2Lib::Paths64 result_clipper;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Iteration 0 - Initialize the first sheet
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        placed_id.emplace_back(0);
        placed_paths.emplace_back(path);

        for (int i = 0; i < placed_paths.size(); i++)
            for (int j = 0; j < placed_paths[i].size(); j++) //
                placed_paths[i][j].emplace_back(placed_paths[i][j][0]);

        std::array<float, 3> rot_tran = {0, 0, 0};
        placed_rotationtranslation.emplace_back(rot_tran);
        placed_pattern_or_path.emplace_back(false); // there can be multiple sheets

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Iterations
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // function
        nfp(pattern, result_clipper, 0);

        // pattern.emplace_back(pattern[0]);
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 120; i++)
        {
            // auto start = std::chrono::high_resolution_clock::now();
            result_clipper.clear(); // clear preview
            pattern = pattern_1();
            // pattern.emplace_back(pattern[0]); // assign nesting element
            //  assign nesting element
            nfp(pattern, result_clipper, i); // run nesting method

            // auto stop = std::chrono::high_resolution_clock::now();
            // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            // std::cout << "Time taken by function: " << duration.count() << " ms" << std::endl;
        }

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        std::cout << "Time taken by function: " << duration.count() << " ms" << std::endl;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // output
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        nfp_glm.clear();
        for (int i = 1; i < placed_paths.size(); i++)
        {
            std::vector<std::vector<glm::vec3>> nfp_glm_;
            convert_from_clipper(placed_paths[i], nfp_glm_);
            nfp_glm.emplace_back(nfp_glm_);
        }

        std::vector<std::vector<glm::vec3>> pattern_glm_;
        convert_from_clipper(pattern, pattern_glm_);
        pattern_glm.emplace_back(pattern_glm_);

        path_glm.clear();
        std::vector<std::vector<glm::vec3>> path_glm_;
        convert_from_clipper(path, path_glm_);
        path_glm.emplace_back(path_glm_);

        results.clear();
        std::vector<std::vector<glm::vec3>> results_;
        convert_from_clipper(result_clipper, results_, 0.01);
        results.emplace_back(results_);
    }
}