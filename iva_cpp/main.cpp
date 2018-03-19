/*************************************************************************
	> File Name: main.cpp
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/03/18
 ************************************************************************/
#include "iva.h"
int main(int argc, char** argv)
{
	iva::iva iva_algorithm;
    float **raw_data = iva_algorithm.get_raw_data();
    int source_num = iva_algorithm.get_source_num();
    int time_points =iva_algorithm.get_time_points();
    for (int i = 0;i<time_points;i++)
    {
        std::vector<float> current_observed(source_num,0);
        for (int s = 0;s<source_num;s++)
        {
            current_observed[s] = raw_data[i][s];
            if (i<100)std::cout<<raw_data[i][s]<<" ";
        }
        if (i<100)std::cout<<std::endl;
        iva_algorithm.data_prepare(current_observed);
        iva_algorithm.process();
    }
    
    
    
    
	
	return 0;
}


