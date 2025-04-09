//
// Created by Shengqiao Zhao on 2025-04-06.
//

#ifndef METRICMANAGER_H
#define METRICMANAGER_H
#include <iostream>
#include <numeric>
#include <vector>

// insertion order test. vs z-order
    // cluster test
    // uniform test

// 10000 * 10000

// different capacity 8, 16, 32, 64, 128, 256, 512, 1024, 2048
// construction cost
// data size: S, M, L, XL
// 100, 1000, 10000, 50000
// data distribution: uniform, skew
// cluster to 1 - 10 zones.
// 1 zone 90, other 10
// 1 zone 45, 1 zone 45, other 10 (skew close to each other, skew far from each other)
// 1 zone 30, 1 zone 30, 1 zone 30, other 10 (2 close to each other, 1 far from each other)
// 1 tiny location contains 80, other 20


// space cost
// Total number of nodes
// Average node occupancy
// max node occupancy
// min node occupancy
// Tree height

// query cost

class MetricManager {
    // construction
    long split_op_count = 0;
    long long total_split_time = 0;
    long long max_split_time = 0;

    long long total_insert_time = 0;
    long long max_insert_time = 0;

    // post construction
    long tree_height = 0;
    long total_leaf_nodes = 0;
    double max_leaf_node_capacity_percent = 0;
    double min_leaf_node_capacity_percent = 0;
    double mean_leaf_node_capacity = 0;
    double median_leaf_node_capacity = 0;

    // query cost
    // total, min, max,mean, median
    std::vector<long long> positive_point_query_time = {};
    std::vector<long long> negative_point_query_time = {};

    std::vector<long long> positive_range_query_time = {};
    std::vector<long long> negative_range_query_time = {};

    static double mean(const std::vector<double>& v) {
        if (v.empty()) return 0.0;

        double sum = std::accumulate(v.begin(), v.end(), 0.0); // promote to double
        return sum / v.size();
    }

    static double median(std::vector<double> v) {
        if (v.empty()) return 0.0;

        sort(v.begin(), v.end());

        int n = v.size();
        if (n % 2 == 1) {
            return v[n / 2]; // middle element
        }
        return (v[n / 2 - 1] + v[n / 2]) / 2.0; // average of two middle elements
    }

    static long long median(std::vector<long long> v) {
        if (v.empty()) return 0.0;

        sort(v.begin(), v.end());

        unsigned long n = v.size();
        if (n % 2 == 1) {
            return v[n / 2]; // middle element
        }
        return (v[n / 2 - 1] + v[n / 2]) / 2; // average of two middle elements
    }

    static long long total(std::vector<long long> times) {
        long long total = 0;
        for (auto t : times) {
            total += t;
        }
        return total;
    }

    static double min(std::vector<double> times) {
        double min = std::numeric_limits<double>::max();
        for (auto t : times) {
            if (t < min) {
                min = t;
            }
        }
        return min;
    }

    static double max(std::vector<double> times) {
        double max = 0;
        for (auto t : times) {
            if (t > max) {
                max = t;
            }
        }
        return max;
    }

    static long long min(std::vector<long long> times) {
        long long min = std::numeric_limits<long long>::max();
        for (auto t : times) {
            if (t < min) {
                min = t;
            }
        }
        return min;
    }

    static long long max(std::vector<long long> times) {
        long long max = 0;
        for (auto t : times) {
            if (t > max) {
                max = t;
            }
        }
        return max;
    }

public:
    MetricManager() = default;
    ~MetricManager() = default;

    void increment_split_count();
    void record_insertion_time(const long long time) {
        total_insert_time += time;
        if(max_insert_time < time) {
            max_insert_time = time;
        }
    }
    void record_split_time(const long long time) {
        split_op_count++;
        total_split_time += time;
        if(max_split_time < time) {
            max_split_time = time;
        }
    }

    void record_post_construction_metrics(long height, std::vector<double>& capacity_percent) {
        this->tree_height = height;
        this->total_leaf_nodes = capacity_percent.size();
        this->max_leaf_node_capacity_percent = max(capacity_percent);
        this->min_leaf_node_capacity_percent = min(capacity_percent);
        this->mean_leaf_node_capacity = mean(capacity_percent);
        this->median_leaf_node_capacity = median(capacity_percent);
    }

    void print_construction_metrics(std::string name) const {
        std::cout<< " Total split count - "<< name << ": " << split_op_count << std::endl;
        std::cout<< " Total split time - "<< name << ": " << total_split_time << std::endl;
        std::cout<< " Average split time - "<< name << ": " << total_split_time << std::endl;
        std::cout<< " Max split time - "<< name << ": " << max_split_time << std::endl;
        std::cout<< " Total insert time - "<< name << ": " << total_insert_time << std::endl;
        std::cout<< " Max insert time - "<< name << ": " << max_insert_time << std::endl;
        std::cout<< " Tree height - "<< name << ": " << tree_height << std::endl;
        std::cout<< " Total leaf nodes - "<< name << ": " << total_leaf_nodes << std::endl;
        // std::cout<< " Max leaf node capacity percent - "<< name << ": " << max_leaf_node_capacity_percent << std::endl;
        // std::cout<< " Min leaf node capacity percent - "<< name << ": " << min_leaf_node_capacity_percent << std::endl;
        std::cout<< " Mean leaf node capacity percent - "<< name << ": " << mean_leaf_node_capacity << std::endl;
        std::cout<< " Median leaf node capacity percent - "<< name << ": " << median_leaf_node_capacity << std::endl;
    }

    void record_point_query_time(bool positive, const long long time) {
        if(positive) {
            positive_point_query_time.push_back(time);
        } else {
            negative_point_query_time.push_back(time);
        }
    }

    void record_range_query_time(bool positive, const long long time) {
        if(positive) {
            positive_range_query_time.push_back(time);
        } else {
            negative_range_query_time.push_back(time);
        }
    }

    void reset_query_metrics() {
        positive_point_query_time.clear();
        negative_point_query_time.clear();
        positive_range_query_time.clear();
        negative_range_query_time.clear();
    }

    void print_point_query_metrics(std::string name) const {
        // total, min, max, mean, median
        std::cout << " Total point query time - " << name << ": " <<  total(positive_point_query_time) << std::endl;
        std::cout << " mean point query time - " << name << ": " << total(positive_point_query_time) / positive_point_query_time.size() << std::endl;
        std::cout << " median point query time - " << name << ": "<< median(positive_point_query_time) << std::endl;
        std::cout << " min point query time - " << name << ": "<< min(positive_point_query_time) << std::endl;
        std::cout << " max point query time - " << name << ": "<< max(positive_point_query_time) << std::endl;
    }

    void print_range_query_metrics(std::string name, double window) {
        // total, min, max, mean, median
        std::cout << " Total range query time -" << name << window << ": "<< total(positive_range_query_time) << std::endl;
        std::cout << " mean range query time -" << name << window << ": "<< total(positive_range_query_time) / positive_range_query_time.size() << std::endl;
        std::cout << " median range query time -" << name << window << ": "<< median(positive_range_query_time) << std::endl;
        std::cout << " min range query time -" << name << window << ": "<< min(positive_range_query_time) << std::endl;
        std::cout << " max range query time -" << name << window << ": "<< max(positive_range_query_time) << std::endl;
        reset_query_metrics();
    }
};



#endif //METRICMANAGER_H
