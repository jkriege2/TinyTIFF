/*
    Copyright (c) 2008-2020 Jan W. Krieger (<jan@jkrieger.de>)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef TEST_RESULTS_H
#define TEST_RESULTS_H

#include <string>
#include <vector>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

struct TestResult {
    inline TestResult(): name(""), success(false), message(""), duration_ms(-1), durationerror_ms(0), perframe_duration_ms(-1), perframe_durationerror_ms(0), numImages(0) {};
    std::string name;
    bool success;
    std::string message;
    double duration_ms;
    double durationerror_ms;
    double perframe_duration_ms;
    double perframe_durationerror_ms;
    int numImages;
};

std::string writeTestSummary(const std::vector<TestResult>& test_results);

void writeJUnit(const std::string& filename, const std::string& testsuitename, const std::vector<TestResult> &test_results);



struct Statistics {
    double mean;
    double std;
    double min;
    double max;
    size_t removed_records;
};

Statistics evaluateRuntimes(std::vector<double> runtimes, double remove_slowest_percent=0.1);

void reportRuntimes(const std::string &name, const std::vector<double>& runtimes, double remove_slowest_percent, std::vector<TestResult> *test_results=nullptr);

#endif // TEST_RESULTS_H
