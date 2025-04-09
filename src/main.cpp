// test_correctness.cpp
// Tests the correctness of R-tree implementation by comparing search results between vector and R-tree


#include <iostream>
#include <random>
#include <set>
#include <vector>
#include <__random/random_device.h>

#include "generator/TestGenerator.h"
#include "RTree/impl/strategy/LinearSplitStrategy.h"
#include "RTree/impl/strategy/QuadraticSplitStrategy.h"
#include "RTree/impl/strategy/RStarSplitStrategy.h"
#include "RTree/impl/tree/RTree.h"

// Define the structure of test data entry
struct TestPoint
{
    double x, y;
    int id;

    // Constructor
    TestPoint(double x, double y, int id) : x(x), y(y), id(id) {}

    // Determine if a point is within the query region
    bool isInRegion(double minX, double minY, double maxX, double maxY) const
    {
        return (x >= minX && x <= maxX && y >= minY && y <= maxY);
    }
};

// Generate random points
std::vector<TestPoint> generateRandomPoints(int count, double minX, double maxX, double minY, double maxY)
{
    std::vector<TestPoint> points;

    // Use random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distX(minX, maxX);
    std::uniform_real_distribution<> distY(minY, maxY);

    for (int i = 0; i < count; ++i)
    {
        points.emplace_back(distX(gen), distY(gen), i + 1);
    }

    return points;
}

// Print test results
void printTestResult(const std::string &testName, bool success)
{
    std::cout << testName << ": " << (success ? "PASSED" : "FAILED") << std::endl;
}

// Compare if two result sets are the same
bool compareResults(const std::set<int> &expected, const std::set<int> &actual)
{
    if (expected.size() != actual.size())
    {
        std::cout << "  Size mismatch: expected " << expected.size()
                  << ", got " << actual.size() << std::endl;
        return false;
    }

    // Check if each ID exists
    for (int id : expected)
    {
        if (actual.find(id) == actual.end())
        {
            std::cout << "  Missing ID in results: " << id << std::endl;
            return false;
        }
    }

    // Check for extra IDs
    for (int id : actual)
    {
        if (expected.find(id) == expected.end())
        {
            std::cout << "  Extra ID in results: " << id << std::endl;
            return false;
        }
    }
    return true;
}

void sanity() {
     std::cout << "===== R-tree Correctness Test =====" << std::endl
              << std::endl;

    // Define test parameters
    const int NUM_POINTS = 50000;
    const double MIN_X = 0.0, MAX_X = 100.0;
    const double MIN_Y = 0.0, MAX_Y = 100.0;

    // Test different split strategies
    std::cout << "Testing different split strategies:" << std::endl;

    // Linear split strategy
    RTree::LinearSplitStrategy linearSplitStrategy;
    RTree::RTree linearTree(2, 16, &linearSplitStrategy);
    std::cout << "  Created R-tree with LinearSplitStrategy" << std::endl;

    // Quadratic split strategy
    RTree::QuadraticSplitStrategy quadraticSplitStrategy;
    RTree::RTree quadraticTree(2, 16, &quadraticSplitStrategy);
    std::cout << "  Created R-tree with QuadraticSplitStrategy" << std::endl;

    // R*-tree split strategy
    RTree::RStarSplitStrategy rstarSplitStrategy;
    RTree::RTree rstarTree(2, 16, &rstarSplitStrategy);
    std::cout << "  Created R-tree with RStarSplitStrategy" << std::endl;

    // Generate random points
    std::vector<TestPoint> points = generateRandomPoints(NUM_POINTS, MIN_X, MAX_X, MIN_Y, MAX_Y);
    std::cout << "Generated " << points.size() << " random points" << std::endl;

    // Test 1: Linear Split Strategy
    std::cout << "1. Testing with Linear Split Strategy..." << std::endl;
    // Insert points
    auto insertStartTime = std::chrono::high_resolution_clock::now();
    for (const auto &point : points)
    {
        double low[2] = {point.x, point.y};
        double high[2] = {point.x, point.y};
        RTree::Region region(low, high, 2);

        linearTree.insert(region, point.id);
    }
    auto insertEndTime = std::chrono::high_resolution_clock::now();
    auto insertDuration = std::chrono::duration_cast<std::chrono::microseconds>(
                              insertEndTime - insertStartTime)
                              .count();
    std::cout << "  Linear Split: Insertion completed in " << insertDuration << " microseconds" << std::endl;
    std::cout << "  Linear Split: R-tree height: " << linearTree.getHeight() << std::endl
              << std::endl;

    // Test 2: Quadratic Split Strategy
    std::cout << "2. Testing with Quadratic Split Strategy..." << std::endl;
    // Insert points
    insertStartTime = std::chrono::high_resolution_clock::now();
    for (const auto &point : points)
    {
        double low[2] = {point.x, point.y};
        double high[2] = {point.x, point.y};
        RTree::Region region(low, high, 2);

        quadraticTree.insert(region, point.id);
    }
    insertEndTime = std::chrono::high_resolution_clock::now();
    insertDuration = std::chrono::duration_cast<std::chrono::microseconds>(
                         insertEndTime - insertStartTime)
                         .count();
    std::cout << "  Quadratic Split: Insertion completed in " << insertDuration << " microseconds" << std::endl;
    std::cout << "  Quadratic Split: R-tree height: " << quadraticTree.getHeight() << std::endl
              << std::endl;

    // Test 3: R*-tree Split Strategy
    std::cout << "3. Testing with R*-tree Split Strategy..." << std::endl;
    // Insert points
    insertStartTime = std::chrono::high_resolution_clock::now();
    for (const auto &point : points)
    {
        double low[2] = {point.x, point.y};
        double high[2] = {point.x, point.y};
        RTree::Region region(low, high, 2);

        rstarTree.insert(region, point.id);
    }
    insertEndTime = std::chrono::high_resolution_clock::now();
    insertDuration = std::chrono::duration_cast<std::chrono::microseconds>(
                         insertEndTime - insertStartTime)
                         .count();
    std::cout << "  R*-tree Split: Insertion completed in " << insertDuration << " microseconds" << std::endl;
    std::cout << "  R*-tree Split: R-tree height: " << rstarTree.getHeight() << std::endl
              << std::endl;

    // Define query regions for testing
    struct QueryRegion
    {
        std::string name;
        double minX, minY, maxX, maxY;
    };

    std::vector<QueryRegion> queryRegions = {
        {"Small region", 20.0, 20.0, 30.0, 30.0},
        {"Medium region", 30.0, 30.0, 70.0, 70.0},
        {"Large region", 10.0, 10.0, 90.0, 90.0}};

    // Test query performance with different strategies
    std::cout << "4. Comparing query performance:" << std::endl;

    for (const auto &qr : queryRegions)
    {
        std::cout << "  Testing " << qr.name << "..." << std::endl;

        // Prepare query region
        double low[2] = {qr.minX, qr.minY};
        double high[2] = {qr.maxX, qr.maxY};
        RTree::Region queryRegion(low, high, 2);

        // Linear split query
        auto queryStartTime = std::chrono::high_resolution_clock::now();
        std::vector<RTree::Data *> linearResults = linearTree.intersectionQuery(queryRegion);
        auto queryEndTime = std::chrono::high_resolution_clock::now();
        auto linearQueryDuration = std::chrono::duration_cast<std::chrono::microseconds>(
                                       queryEndTime - queryStartTime)
                                       .count();

        // Quadratic split query
        queryStartTime = std::chrono::high_resolution_clock::now();
        std::vector<RTree::Data *> quadraticResults = quadraticTree.intersectionQuery(queryRegion);
        queryEndTime = std::chrono::high_resolution_clock::now();
        auto quadraticQueryDuration = std::chrono::duration_cast<std::chrono::microseconds>(
                                          queryEndTime - queryStartTime)
                                          .count();

        // R*-tree split query
        queryStartTime = std::chrono::high_resolution_clock::now();
        std::vector<RTree::Data *> rstarResults = rstarTree.intersectionQuery(queryRegion);
        queryEndTime = std::chrono::high_resolution_clock::now();
        auto rstarQueryDuration = std::chrono::duration_cast<std::chrono::microseconds>(
                                      queryEndTime - queryStartTime)
                                      .count();

        std::cout << "    Linear Split: Found " << linearResults.size()
                  << " results in " << linearQueryDuration << " microseconds" << std::endl;
        std::cout << "    Quadratic Split: Found " << quadraticResults.size()
                  << " results in " << quadraticQueryDuration << " microseconds" << std::endl;
        std::cout << "    R*-tree Split: Found " << rstarResults.size()
                  << " results in " << rstarQueryDuration << " microseconds" << std::endl;
        std::cout << std::endl;
    }

    std::cout << "===== R-tree Split Strategy Comparison Completed =====" << std::endl;
}

void range_query(double max_x, double max_y, double window_unit,
    RTree::RTree & linearTree, RTree::RTree & quadraticTree, RTree::RTree & rstarTree) {
    for(double x_start = 0.0; x_start < max_x; x_start+=window_unit) {
        for(double y_start = 0.0; y_start < max_y; y_start+=window_unit) {
            double low[2] = {x_start, y_start};
            double high[2] = {x_start + window_unit, y_start + window_unit};
            RTree::Region queryRegion(low, high, 2);
            linearTree.intersectionQuery(queryRegion);
            quadraticTree.intersectionQuery(queryRegion);
            rstarTree.intersectionQuery(queryRegion);
        }
    }

    std::cout << window_unit << " range queries cost" << std::endl;
    std::cout << "Linear Split " << std::endl;
    linearTree.print_range_query_metrics("linear", window_unit);
    std::cout << std::endl;

    std::cout << "Quadratic Split " << std::endl;
    quadraticTree.print_range_query_metrics("quadratic", window_unit);
    std::cout << std::endl;

    std::cout << "R* Split " << std::endl;
    rstarTree.print_range_query_metrics("r-star", window_unit);
    std::cout << std::endl;
}

void benchmark(double max_x, double max_y,
               int dimension, int capacity,
               std::vector<RTree::Point> &points, bool construction_only) {

    std::cout << "capacity: " << capacity << std::endl;
    RTree::LinearSplitStrategy linearSplitStrategy;
    RTree::RTree linearTree(dimension, capacity, &linearSplitStrategy);

    // Quadratic split strategy
    RTree::QuadraticSplitStrategy quadraticSplitStrategy;
    RTree::RTree quadraticTree(dimension, capacity, &quadraticSplitStrategy);

    // R*-tree split strategy
    RTree::RStarSplitStrategy rstarSplitStrategy;
    RTree::RTree rstarTree(dimension, capacity, &rstarSplitStrategy);

    int count = 0;

    for (const auto & point : points) {
        count ++;
        double low[2] = {point.getCoordinate(0), point.getCoordinate(1)};
        double high[2] = {point.getCoordinate(0), point.getCoordinate(1)};
        RTree::Region region1(low, high, 2);
        RTree::Region region2(low, high, 2);
        RTree::Region region3(low, high, 2);
        linearTree.insert(region1, point.getId());
        quadraticTree.insert(region2, point.getId());
        rstarTree.insert(region3, point.getId());
    }

    linearTree.construction_finished();
    quadraticTree.construction_finished();
    rstarTree.construction_finished();

    std::cout << "Linear Split " << std::endl;
    linearTree.print_construction_metrics("linear");
    std::cout << std::endl;

    std::cout << "Quadratic Split " << std::endl;
    quadraticTree.print_construction_metrics("quadratic");
    std::cout << std::endl;

    std::cout << "R* Split " << std::endl;
    rstarTree.print_construction_metrics("r-star");
    std::cout << std::endl;

    if(!construction_only) {
        for (const auto & point : points) {
            linearTree.pointQuery(point);
            quadraticTree.pointQuery(point);
            rstarTree.pointQuery(point);
        }

        std::cout << "point queries cost" << std::endl;
        linearTree.print_point_query_metrics("linear");
        std::cout << std::endl;

        std::cout << "Quadratic Split " << std::endl;
        quadraticTree.print_point_query_metrics("quadratic");
        std::cout << std::endl;

        std::cout << "R* Split " << std::endl;
        rstarTree.print_point_query_metrics("r-star");
        std::cout << std::endl;

        std::cout << "range queries cost" << std::endl;
        range_query(max_x, max_y, 50, linearTree, quadraticTree, rstarTree);
        range_query(max_x, max_y, 100, linearTree, quadraticTree, rstarTree);
        range_query(max_x, max_y, 500, linearTree, quadraticTree, rstarTree);
        range_query(max_x, max_y, 1000, linearTree, quadraticTree, rstarTree);
        range_query(max_x, max_y, 5000, linearTree, quadraticTree, rstarTree);
        range_query(max_x, max_y, 10000, linearTree, quadraticTree, rstarTree);
    }
}

int main()
{
    constexpr int max_x = 1000;
    constexpr int max_y = 1000;

    int points_count_to_test[] = {500, 1000, 10000, 50000, 100000};
    // int points_count_to_test[] = {50000, 60000, 70000, 80000, 90000, 100000};
    std::vector<RTree::Point> points;
    int modes [] = {0, 1, 2, 3, 4, 5};
    // int modes [] = {0, 1};

    for(int mode : modes) {
        for(int points_count: points_count_to_test) {
            points.clear();
            TestGenerator::generate_test_data(mode, max_x, max_y, points_count, points);

            // int capacities[] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048};
            int capacities[] = {32};

            for (int capacity : capacities) {
                std::cout << "dataset mode: " << mode << std::endl;
                std::cout << "total points: " << points.size() << std::endl;
                benchmark(max_x, max_y, 2, capacity, points, false);
                std::cout << "Benchmark Split @@" << std::endl;
            }
        }
    }
}