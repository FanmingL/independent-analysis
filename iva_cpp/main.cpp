/*************************************************************************
	> File Name: main.cpp
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/03/18
 ************************************************************************/
#include "iva.h"
#include <iostream>
#include <fstream>
int main(int argc, char** argv)
{
	iva::iva iva_algorithm;
    float **raw_data = iva_algorithm.get_raw_data();
    float **estimate_data ;
    int source_num = iva_algorithm.get_source_num();
    int time_points =iva_algorithm.get_time_points();
    int shift_size = iva_algorithm.get_shift_size();
    std::ofstream outfile;
    std::string data_path_out = iva_algorithm.get_data_path();
    data_path_out  += ".csv";
    outfile.open(data_path_out);
    if(!outfile) std::cout<<"error"<<std::endl;
    
    
    for (int i = 0;i<time_points;i++)
    {
        std::vector<float> current_observed(source_num,0);
        for (int s = 0;s<source_num;s++)
        {
            current_observed[s] = raw_data[i][s];
        }
        iva_algorithm.data_prepare(current_observed);
        iva_algorithm.process();
        if (iva_algorithm.is_processed())
        {
            estimate_data = iva_algorithm.get_current_batch();
            for (int i_i = 0;i_i<shift_size;i_i++)
            {
                for (int s_s = 0;s_s<source_num;s_s++)
                {
                    outfile<<estimate_data[i_i][s_s];
                    if(s_s != source_num-1){
                        outfile<<", ";
                    }
                    
                }
                outfile<<"\n";
            }
        }
    }

    
    outfile.close();
    return 0;
    
    
    
	
	return 0;
}


