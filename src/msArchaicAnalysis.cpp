/*
 * msArchaicAnalysis.cpp
 *
 *  Created on: Oct 29, 2018
 *      Author: yuankai
 */

# include <iostream>
# include <cstdlib>

# include "omp.h"

# include "matching.hpp"

int main(int argc, char **argv)
{
    std::string path, outprefix, archaicLab, africanLab, testLab;
    for(int i = 0 ; i < argc; ++i)
    {
            std::string targv = argv[i];
            if(targv == "--file")
            	path = argv[++i];
            else if(targv == "--out")
            	outprefix = argv[++i];
            else if(targv == "--archaic")
            	archaicLab = argv[++i];
            else if(targv == "--african")
            	africanLab = argv[++i];
            else if(targv == "--test")
            	testLab = argv[++i];
            else if(targv == "--thread")
            	omp_set_num_threads(atoi(argv[++i]));
    }
    archaicSeekerSimAnalysis(path, outprefix, archaicLab, africanLab, testLab);
}



