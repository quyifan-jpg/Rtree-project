//
// Created by Shengqiao Zhao on 2025-04-07.
//

#ifndef TESTGENERATOR_H
#define TESTGENERATOR_H
#include <random>

#include "src/RTree/impl/pojo/Point.h"


class TestGenerator {
    public:

    static void generate_uniform_distributed(int x_min, int x_max, int y_min, int y_max, int count,
                                             std::vector<RTree::Point>& points) {
        // Use a random device to seed the generator
        std::random_device rd;
        std::mt19937 gen(rd());

        // Uniform distributions for x and y
        std::uniform_int_distribution<> distX(x_min, x_max);
        std::uniform_int_distribution<> distY(y_min, y_max);

        for(int i = 0; i < count; i++) {

            int x = distX(gen);
            int y = distY(gen);

            double coordinates [] = {static_cast<double>(x), static_cast<double>(y)};
            RTree::Point p(coordinates, 2, i);
            points.push_back(p);
        }
    }

    static void generate_normal_distributed(int x_min, int x_max, int y_min, int y_max, int size,
        std::vector<RTree::Point>& points) {
        if (x_min > x_max) std::swap(x_min, x_max);

        // Set up random number generator
        std::random_device rd;
        std::mt19937 gen(rd());

        // Mean and standard deviation for normal distribution
        double mean_x = (x_min + x_max) / 2.0;
        double stddev_x = (x_max - x_min) / 6.0; // 99.7% values fall within [min, max]

        double mean_y = (y_min + y_max) / 2.0;
        double stddev_y = (y_max - y_min) / 6.0; // 99.7% values fall within [min, max]

        std::normal_distribution<> dist_x(mean_x, stddev_x);
        std::normal_distribution<> dist_y(mean_y, stddev_y);

        for(int i = 0; i < size; i++) {
            int x = static_cast<int>(std::round(dist_x(gen)));
            int y = static_cast<int>(std::round(dist_y(gen)));

            double coordinates [] = {static_cast<double>(x), static_cast<double>(y)};
            RTree::Point p(coordinates, 2, i);
            points.push_back(p);
        }
    }

    // 4*4 zone
    // 1  2  3  4
    // 5  6  7  8
    // 9  10 11 12
    // 13 14 15 16

    // total zones 16.
    // zone1 90+, other 10-
    // the cluster zone 1
    // insert zoned data first, then uniform
    static void generate_cluster_mode_1(int x_max, int y_max, int count,
                                        std::vector<RTree::Point> &points) {

        int cluster_count = count * 5 / 10;
        int rest_count = count - cluster_count;

        generate_normal_distributed(0, x_max / 4, y_max - y_max / 4, y_max, cluster_count, points);

        generate_normal_distributed(0, x_max, 0, y_max, rest_count, points);

    }

    // total zones 16.
    // zone1 45+, zone2 45, other 10- (close to each other)
    static void generate_cluster_mode_2(int x_max, int y_max, int count,
        std::vector<RTree::Point> &points) {

        int cluster_count = count * 45 / 100;
        int rest_count = count - cluster_count * 2;

        // zone 1
        generate_normal_distributed(0, x_max / 4, y_max - y_max / 4, y_max, cluster_count, points);
        // zone 2
        generate_normal_distributed(x_max / 4, x_max / 2, y_max - y_max / 4, y_max, cluster_count, points);

        generate_normal_distributed(0, x_max, 0, y_max, rest_count, points);

    }

    // total zones 16.
    // zone1 30, zone2 30, zone16 30, other 10
    static void generate_cluster_mode_3(int x_max, int y_max, int count, std::vector<RTree::Point> &points) {

        int cluster_count = count * 3 / 10;
        int rest_count = count - cluster_count * 3;

        // zone 1
        generate_normal_distributed(0, x_max / 4, y_max - y_max / 4, y_max, cluster_count, points);
        // zone 2
        generate_normal_distributed(x_max / 4, x_max / 2, y_max - y_max / 4, y_max, cluster_count, points);
        // zone 16
        generate_normal_distributed(x_max - x_max / 4, x_max, 0, y_max / 4, cluster_count, points);

        generate_normal_distributed(0, x_max, 0, y_max, rest_count, points);
    }

    // cluster zone 1
    // insert uniform first, then the zoned data
    static void generate_cluster_mode_4(int x_max, int y_max, int count, std::vector<RTree::Point> &points) {

        int cluster_count = count * 5 / 10;
        int rest_count = count - cluster_count;

        generate_normal_distributed(0, x_max, 0, y_max, rest_count, points);

        generate_normal_distributed(0, x_max / 4, y_max - y_max / 4, y_max, cluster_count, points);
    }


    static void generate_test_data(int mode, int x_max, int y_max, int count, std::vector<RTree::Point> &points) {
        switch (mode) {
            case 0:
                generate_normal_distributed(0, x_max, 0, y_max, count, points);
                break;
            case 1:
                generate_uniform_distributed(0, x_max, 0, y_max, count, points);
                break;
            case 2:
                generate_cluster_mode_1(x_max, y_max, count, points);
                break;
            case 3:
                generate_cluster_mode_2(x_max, y_max, count, points);
                break;
            case 4:
                generate_cluster_mode_3(x_max, y_max, count, points);
                break;
            case 5:
                generate_cluster_mode_4(x_max, y_max, count, points);

            default:
                break;
        }
    }

    // point query on all possible nodes
    // point query on missing nodes

    // slide a window across the whole set.
    // S 10 * 10
    // M 100 * 100
    // L 1000 * 1000
    // 5000 * 5000
    // 10000 * 10000
};



#endif //TESTGENERATOR_H
