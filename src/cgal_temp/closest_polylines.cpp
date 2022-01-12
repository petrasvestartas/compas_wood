//https://github.com/CGAL/cgal/issues/6089
//https://gist.github.com/sloriot/0943fec878401e04930b7bcbd73baf91
    inline void closest_curves(std::vector<CGAL_Polyline>& polylines, std::vector<std::vector<double>>& polylines_segment_radii, double& min_distance,
        std::vector<std::array<int, 4>>& polyline0_id_segment0_id_polyline1_id_segment1_id,
        std::vector<std::array<IK::Point_3, 2>>& point_pairs
        ) {

#ifdef DEBUG
                CGAL_Debug(polylines.size(), polylines_segment_radii.size());
#endif





        //CGAL_Debug(polylines.size());
        /////////////////////////////////////////////////////////////////////
        //Segment callback
        /////////////////////////////////////////////////////////////////////

        auto segment = [&polylines](std::size_t pid, std::size_t sid)
        {
            return IK::Segment_3(polylines[pid][sid], polylines[pid][sid + 1]);
            //return EK::Segment_3(
            //    EK::Point_3(polylines[pid][sid][0], polylines[pid][sid][1], polylines[pid][sid][2]),
            //    EK::Point_3(polylines[pid + 1][sid][0], polylines[pid][sid + 1][1], polylines[pid][sid + 1][2])
            //);
        };

        auto segment_inflated = [&polylines, &polylines_segment_radii](std::size_t pid, std::size_t sid)
        {

            IK::Segment_3 segment(polylines[pid][sid], polylines[pid][sid + 1]);

            IK::Vector_3 v0 = segment.to_vector();
            segment = IK::Segment_3(segment[0] - v0*0.5, segment[1] + v0*0.5);


            double radius = polylines_segment_radii[pid][sid];

            IK::Vector_3 zaxis = segment.to_vector();
            IK::Plane_3 plane(segment[0], zaxis);
            IK::Vector_3 x_axis = plane.base1();



            IK::Vector_3 y_axis = plane.base2();//CGAL::cross_product(x_axis, zaxis);

            x_axis = cgal_vector_util::Unit(x_axis);
            y_axis = cgal_vector_util::Unit(y_axis);
            x_axis *= radius;
            y_axis *= radius;

            std::vector<IK::Point_3> pts = {
                segment[0] + x_axis + y_axis,
                segment[0] - x_axis + y_axis,
                segment[0] - x_axis - y_axis,
                segment[0] + x_axis - y_axis,

                segment[1] + x_axis + y_axis,
                segment[1] - x_axis + y_axis,
                segment[1] - x_axis - y_axis,
                segment[1] + x_axis - y_axis,
            };


            CGAL::Bbox_3 box = CGAL::bbox_3(pts.begin(), pts.end(), IK());
            IK::Segment_3 segment_inflated(IK::Point_3(box.xmin(), box.ymin(), box.zmin()), IK::Point_3(box.xmax(), box.ymax(), box.zmax()));
            return segment_inflated;
        };

        /////////////////////////////////////////////////////////////////////
        // Create the corresponding vector of bounding boxes
        /////////////////////////////////////////////////////////////////////
        std::vector<Box> boxes;
        for (std::size_t pid = 0; pid < polylines.size(); ++pid)
            for (std::size_t sid = 0; sid < polylines[pid].size() - 1; ++sid) 
                boxes.push_back(Box(segment_inflated(pid, sid).bbox(), std::make_pair(pid, sid)));




        /////////////////////////////////////////////////////////////////////
        //do_interesect call_back
        /////////////////////////////////////////////////////////////////////
      
        std::map<uint64_t, std::tuple<double, int, int, int, int>> pair_collisions;

        auto callback = [&segment, &min_distance, &pair_collisions](const Box& b1, const Box& b2)
        {


            if (b1.info().first != b2.info().first) {


                IK::Segment_3 s0 = segment(b1.info().first, b1.info().second);
                IK::Segment_3 s1 = segment(b2.info().first, b2.info().second);
                double distance = CGAL::squared_distance(s0, s1);
#ifdef DEBUG
                CGAL_Debug(b1.info().first, b2.info().first, distance);
#endif


                if (distance < min_distance * min_distance) {

                    //Snapping_result<EK> snapping = exact_snapping(s0, s1, min_distance);//check if type is equal to 1

                    //if (snapping.type == 1) {
                        //cgal_intersection_util::Snapping_result<EK> snapping = cgal_intersection_util::exact_snapping(s0, s1, max_distance);//check if type is equal to 1
                        //if (snapping.type == 1) {


                        size_t first_0, first_1;
                        bool flipped = false;
                        uint64_t id;
                        if (b2.info().first > b1.info().first) {
                            flipped = true;
                            id = (uint64_t)b2.info().first << 32 | b1.info().first;
                        } else {
                            flipped = false;
                            id = (uint64_t)b1.info().first << 32 | b2.info().first;
                        }


                        auto dist_and_segment_ids = flipped ? std::make_tuple(distance, b1.info().first, b1.info().second, b2.info().first, b2.info().second) : std::make_tuple(distance, b2.info().first, b2.info().second, b1.info().first, b1.info().second);

                        //Add elements to std::map
                        if (pair_collisions.find(id) == pair_collisions.end()) {
                            //not found
                            pair_collisions.insert(std::make_pair(id, dist_and_segment_ids));

                        } else if (distance < std::get<0>(pair_collisions[id])) {
                            // found and distance is smaller that before found
                            pair_collisions[id] = dist_and_segment_ids;
                        }
                    //}

                }//check if lines closer than the given distance

            }//check if boxes do not belong to the same group b.info().first

        };

        /////////////////////////////////////////////////////////////////////
        //self intersection
        /////////////////////////////////////////////////////////////////////
        CGAL::box_self_intersection_d(boxes.begin(), boxes.end(), callback);

        /////////////////////////////////////////////////////////////////////
        //Iterate the result, get insertion points and parameter on the lines
        /////////////////////////////////////////////////////////////////////


        //std::vector<std::array<int,4>> polyline0_id_segment0_id_polyline1_id_segment1_id;
        polyline0_id_segment0_id_polyline1_id_segment1_id.reserve(pair_collisions.size() * 4);
        //std::vector<std::array<IK::Point_3, 2>> point_pairs;
        point_pairs.reserve(point_pairs.size() * 2);

        for (auto const& x : pair_collisions) {


            IK::Segment_3 s0 = segment(std::get<1>(x.second), std::get<2>(x.second));
            IK::Segment_3 s1 = segment(std::get<3>(x.second), std::get<4>(x.second));
            //IK::Vector_3 v0 = s0.to_vector();
            //IK::Vector_3 v1 = s1.to_vector();
            //s0 = IK::Segment_3(s0[0] - v0, s0[1] + v0);
            //s1 = IK::Segment_3(s1[0] - v1, s1[1] + v1);

            Snapping_result<EK> snapping = exact_snapping(s0, s1, min_distance);//check if type is equal to 1

            if (snapping.type == 1) {

                polyline0_id_segment0_id_polyline1_id_segment1_id.emplace_back(std::array<int, 4>{std::get<1>(x.second), std::get<2>(x.second), std::get<3>(x.second), std::get<4>(x.second)});

                IK_to_EK ie; EK_to_IK ei;
               // Snapping_result<EK> ek = .;
               // IK::Point_3 p0 = ei(*boost::get<EK::Point_3>(&((std::get<5>(x.second)).first)));
                //IK::Point_3 p1 = ei(*boost::get<EK::Point_3>(&((std::get<5>(x.second)).second)));
                IK::Point_3 p0 = ei(*boost::get<EK::Point_3>(&snapping.first));
                IK::Point_3 p1 = ei(*boost::get<EK::Point_3>(&snapping.second));
                point_pairs.emplace_back(std::array<IK::Point_3, 2>{p0, p1});
#ifdef DEBUG
                CGAL_Debug(p0);
                CGAL_Debug(p1);
#endif
           }


        }
