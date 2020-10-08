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

#include "testimage_tools.h"


long get_filesize(const char *FileName) {
    struct stat file;
    if(!stat(FileName,&file)) {
        return file.st_size;
    }
    return 0;
}

std::string bytestostr(double bytes){
    double data=bytes;
    std::string form="%.0lf";
    std::string res=format(form,data);
    form="%.3lf";
    if (fabs(data)>=1024.0) res=format(form,data/1024.0)+" k";
    if (fabs(data)>=1024.0*1024.0) res=format(form,data/(1024.0*1024.0))+" M";
    if (fabs(data)>=1024.0*1024.0*1024.0) res=format(form,data/(1024.0*1024.0*1024.0))+" ";
    if (fabs(data)>=1024.0*1024.0*1024.0*1024.0) res=format(form,data/(1024.0*1024.0*1024.0*1024.0))+" G";
    if (fabs(data)>=1024.0*1024.0*1024.0*1024.0*1024.0) res=format(form,data/(1024.0*1024.0*1024.0*1024.0*1024.0))+" T";
    if (fabs(data)>=1024.0*1024.0*1024.0*1024.0*1024.0*1024.0) res=format(form,data/(1024.0*1024.0*1024.0*1024.0*1024.0*1024.0))+" E";
    if (fabs(data)==0) res="0 ";
    return res+"Bytes";
}
