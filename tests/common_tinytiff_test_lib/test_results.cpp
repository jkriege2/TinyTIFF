/*
    Copyright (c) 2020-2020 Jan W. Krieger (<jan@jkrieger.de>)

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

#include "test_results.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cmath>


std::string writeTestSummary(const std::vector<TestResult> &test_results) {
    std::ostringstream str;
    str<<"TEST SUMMARY:\n";
    size_t textcolwidth=60;
    size_t durcol=15;
    size_t durpfcol=15;
    size_t durcolsingle=15;
    size_t frmcol=10;
    for (auto& r: test_results) {
        textcolwidth=std::max(textcolwidth, r.name.size()+2);
        if (r.duration_ms>=0 && r.durationerror_ms>0) durcol=30;
        if (r.perframe_duration_ms>=0 && r.perframe_durationerror_ms>0) durpfcol=30;
    }
    str<<std::setw(textcolwidth)<<"NAME"<<" | "<<std::setw(6)<<"OK?"<<" | "<<std::setw(frmcol)<<"#FRAMES"<<" | "<<std::setw(durcol)<<"DURATION"<<" | "<<std::setw(durcol/2)<<"DURATION/frames"<<" | "<<std::setw(durcol)<<"DURATION_PER_FRAME"<<std::endl;
    for (auto& r: test_results) {
        str<<std::setw(textcolwidth)<<r.name;
        if (r.success) str<<" | "<<std::setw(6)<<"    OK";
        else           str<<" | "<<std::setw(6)<<"FAILED";

        str<<" | "<<std::setw(frmcol)<<r.numImages;

        {
            std::ostringstream strloc;
            if (r.duration_ms>=0) {
                if (r.durationerror_ms>0) {
                    strloc<<"("<<r.duration_ms<< " +/- " <<r.durationerror_ms<<")ms";

                } else {
                    strloc<<r.duration_ms<<"ms";
                }
            } else {
                strloc<<"---";
            }
            str<<" | "<<std::setw(durcol)<<strloc.str();
        }
        {
            std::ostringstream strloc;
            if (r.duration_ms>=0 && r.numImages>0) {
                strloc<<r.duration_ms/static_cast<double>(r.numImages)<<"ms";
            } else {
                strloc<<"---";
            }
            str<<" | "<<std::setw(durcolsingle)<<strloc.str();
        }
        {
            std::ostringstream strloc;
            if (r.perframe_duration_ms>=0) {
                if (r.perframe_durationerror_ms>0) {
                    strloc<<"("<<r.perframe_duration_ms<< " +/- " <<r.perframe_durationerror_ms<<")ms";

                } else {
                    strloc<<r.perframe_duration_ms<<"ms";
                }
            } else {
                strloc<<"---";
            }
            str<<" | "<<std::setw(durpfcol)<<strloc.str();
        }
        str<<std::endl;
    }
    str<<std::endl;
    str<<std::endl;
    return str.str();
}

Statistics evaluateRuntimes(std::vector<double> runtimes, double remove_slowest_percent) {
    std::sort(runtimes.begin(), runtimes.end());
    size_t removed_records=0;
    if (remove_slowest_percent>0 && remove_slowest_percent/100.0*runtimes.size()>0) {
        removed_records=static_cast<size_t>(remove_slowest_percent/100.0*runtimes.size());
        runtimes.erase(runtimes.end()-removed_records, runtimes.end());
    }
    double sum=0, sum2=0, mmin=0, mmax=0;

    for (size_t i=0; i<runtimes.size(); i++) {

        sum+=runtimes[i];
        sum2+=(runtimes[i]*runtimes[i]);
        if (i==0) mmin=mmax=runtimes[i];
        else {
            if (runtimes[i]>mmax) mmax=runtimes[i];
            if (runtimes[i]<mmin) mmin=runtimes[i];
        }
    }
    Statistics stat;
    const double NN=static_cast<double>(runtimes.size());
    stat.mean=sum/NN;
    stat.std=sqrt((sum2-sum*sum/NN)/(NN-1.0));
    stat.min=mmin;
    stat.max=mmax;
    stat.removed_records=removed_records;
    return stat;
}

void reportRuntimes(const std::string& name, const std::vector<double> &runtimes, double remove_slowest_percent, std::vector<TestResult>* test_results) {
    Statistics stat=evaluateRuntimes(runtimes, 0);
    std::cout<<"RUNTIME-REPORT for \""<<name<<"\":"<<std::endl;
    std::cout<<"  ALL:     average time to write one image: "<<stat.mean<<" usecs    std: "<<stat.std<<" usecs     range: ["<<stat.min<<".."<<stat.max<<"] usecs\n";
    std::cout<<"  ALL:     average image rate: "<<1.0/(stat.mean)*1000.0<<" kHz\n";
    stat=evaluateRuntimes(runtimes, remove_slowest_percent);
    std::cout<<"  CLEANED: removed slowest "<<stat.removed_records<<" records\n";
    std::cout<<"  CLEANED: average time to write one image: "<<stat.mean<<" usecs    std: "<<stat.std<<" usecs     range: ["<<stat.min<<".."<<stat.max<<"] usecs\n";
    std::cout<<"  CLEANED: average image rate: "<<1.0/(stat.mean)*1000.0<<" kHz\n";

    if (test_results) {
        test_results->back().perframe_duration_ms=stat.mean/1e3;
        test_results->back().perframe_durationerror_ms=stat.std/1e3;
    }
}
