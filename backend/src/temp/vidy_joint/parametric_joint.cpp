inline void ss_e_op_2(joint &joint)
    {
        joint.name = "ss_e_op_2";

        // Resize arrays
        joint.f[0].reserve(2);
        joint.f[1].reserve(2);
        joint.m[0].reserve(2);
        joint.m[1].reserve(2);

        ////////////////////////////////////////////////////////////////////
        // Number of divisions
        // Input joint line (its lengths)
        // Input distance for division
        ////////////////////////////////////////////////////////////////////

        int divisions = (int)std::max(4, std::min(20, joint.divisions));
        divisions += divisions % 2;
        ////////////////////////////////////////////////////////////////////
        // Interpolate points
        ////////////////////////////////////////////////////////////////////
        std::vector<IK::Point_3> arrays[4];

        interpolate_points(IK::Point_3(0.5, -0.5, -0.5), IK::Point_3(0.5, -0.5, 0.5), divisions, false, arrays[0]);
        interpolate_points(IK::Point_3(-0.5, -0.5, -0.5), IK::Point_3(-0.5, -0.5, 0.5), divisions, false, arrays[1]);
        interpolate_points(IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5), divisions, false, arrays[2]);
        interpolate_points(IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5), divisions, false, arrays[3]);

        ////////////////////////////////////////////////////////////////////
        // Move segments
        ////////////////////////////////////////////////////////////////////
        int start = 0;

        IK::Vector_3 v = joint.shift == 0 ? IK::Vector_3(0, 0, 0) : IK::Vector_3(0, 0, RemapNumbers(joint.shift, 0, 1.0, -0.5, 0.5) / (divisions + 1));

        for (int i = start; i < 4; i += 1)
        {
            int mid = (int)(arrays[i].size() * 0.5);
            for (int j = 0; j < arrays[i].size(); j++)
            {
                // int flip = (j % 2 < 0) ? 1 : -1;

                int flip = (j < mid) ? 1 : -1;

                if (i == 1)
                {
                    if (j < (mid - 1) || j > mid)
                        arrays[i][j] -= 4 * v * flip;
                }
                else if (i == 0 || i == 2)
                {
                    if (j < (mid - 1) || j > mid)
                        arrays[i][j] -= 2 * v * flip;
                }
            }
        }

        ////////////////////////////////////////////////////////////////////
        // Create Polylines
        ////////////////////////////////////////////////////////////////////

        for (int i = 0; i < 4; i += 2)
        {
            CGAL_Polyline pline;
            pline.reserve(arrays[0].size() * 2);

            for (int j = 0; j < arrays[0].size(); j++)
            {
                bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;

                if (flip)
                {
                    pline.push_back(arrays[i + 0][j]);
                    pline.push_back(arrays[i + 1][j]);
                }
                else
                {
                    pline.push_back(arrays[i + 1][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2)
            {
                joint.m[1] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]}};
            }
            else
            {
                joint.m[0] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]}};
            }
        }

        for (int i = 1; i < 4; i += 2)
        {
            CGAL_Polyline pline;
            pline.reserve(arrays[0].size() * 2);

            for (int j = 0; j < arrays[0].size(); j++)
            {
                bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;

                if (flip)
                {
                    pline.push_back(arrays[i + 0][j]);
                    pline.push_back(arrays[(i + 1) % 4][j]);
                }
                else
                {
                    pline.push_back(arrays[(i + 1) % 4][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2)
            {
                joint.f[0] = {
                    pline,
                    //{ pline[0],pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]}};
            }
            else
            {
                joint.f[1] = {
                    pline,
                    //{ pline[0],pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]}};
            }
        }

        joint.f_boolean_type = {edge_insertion, edge_insertion};
        joint.m_boolean_type = {edge_insertion, edge_insertion};
    }