#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <pthread.h>
#include <sys/param.h>
#include <unistd.h>
using namespace std;
int unit;
int NumofTech;
int NumofLib;
int LowerLeftX;
int LowerLeftY;
int UpperRightX;
int UpperRighty;
int TopDieMaxUtil;
int BottomDieMaxUtil;
int StartX_t;
int StartY_t;
int RowLength_t;
int RowHeight_t;
int RepeatCount_t;
int StartX_b;
int StartY_b;
int RowLength_b;
int RowHeight_b;
int RepeatCount_b;
int Terminal_X;
int Terminal_Y;
int Terminal_Spacing;
int Instance_cnt;
int NetCnt;
int bigger; //-1 is part 1, 1 is part 2
int divide_cnt = 0;
int hmetis_cnt = 0;
int top = 0;
int bot = 0;
int best_cut = 10000000;
long long best_HPWL = 2147483645;
unsigned long long TopDieMaxArea;
unsigned long long BottomDieMaxArea;
unsigned long long TopDieRealArea;
unsigned long long BottomDieRealArea;
unsigned long long best_top_area = 0;
unsigned long long best_bot_area = 0;
vector<int> best_partition_map;
vector<bool> NetCross;
vector<int> partition_map;          //[Instnum]
vector<int> Lib_for_Inst;           //[Instnum]
vector<int> NumofPin;               //[Net]
vector<vector<pair<int, int>>> net; //[Net][Pin] net.first = instance net.second = pin
vector<vector<int>> LibCellSize_x;  //[tech][Libcnt]
vector<vector<int>> LibCellSize_y;
vector<vector<int>> LibCellSize;
vector<vector<vector<int>>> Pin_x; //[tech][Libcnt][Pincnt]
vector<vector<vector<int>>> Pin_y;
vector<vector<int>> cell_array; //[instnum][idx]
vector<int> inst_size;          // size of instance
vector<int> Cell_in_top;        // Which cell in top die and bottom die (cell_number)
vector<int> Cell_in_bottom;
vector<int> cell_gain; //[Instnum]
ifstream in_file;
ofstream out_file;
ofstream out_1, out_2, out_3, out_4, out_5, out_6, out_7, out_0;
vector<string> Techname;
string TopDieTech;
string BottomDieTech;
vector<int> cutname; //[netnum];
string replaced_top = "./top_die_final.def";
string replaced_bot = "./bot_die_final.def";
vector<pair<int, int>> terminal_position; //[netnum] center of terminal x,y
int cut_num;
vector<int> degree;
vector<int> degree_net;
vector<pair<int, int>> inst_position;
int terminal_job;
vector<int> mbpm_map;
pthread_mutex_t mutex;

// test
vector<vector<int>> partition_map_test;
vector<vector<int>> cell_in_top_test;
vector<vector<int>> cell_in_bottom_test;
vector<vector<int>> cutname_test;
vector<vector<bool>> NetCross_test;
vector<vector<pair<int, int>>> term_pos_test;
vector<vector<pair<int, int>>> inst_pos_test;
vector<vector<string>> topdiename_test;
vector<vector<string>> botdiename_test;
vector<int> cut_num_test;
vector<int> best_hpwl_test;
vector<vector<pair<int, int>>> best_inst_test;
vector<vector<pair<int, int>>> best_term_test;

#pragma pack(1)
typedef struct mypara
{
    int index;
    char *data1;
    char *data2;
} Para;

void legalize();

void parse_input()
{
    int tmp_int;
    string tmp;
    while (in_file >> tmp)
    {
        if (tmp == "NumTechnologies")
        {
            in_file >> NumofTech;
            for (int i = 0; i < NumofTech; i++)
            {
                in_file >> tmp;
                string tn;
                in_file >> tn;
                Techname.push_back(tn);
                in_file >> NumofLib;
                vector<int> tmp_vec_x;
                vector<int> tmp_vec_y;
                vector<vector<int>> tmp_pin_x_1;
                vector<vector<int>> tmp_pin_y_1;
                vector<int> libsizevec;
                for (int j = 0; j < NumofLib; j++)
                {
                    in_file >> tmp;
                    in_file >> tmp;
                    int libx;
                    in_file >> libx;
                    tmp_vec_x.push_back(libx);
                    int liby;
                    in_file >> liby;
                    tmp_vec_y.push_back(liby);
                    int libsize = libx * liby;
                    libsizevec.push_back(libsize);
                    int pincnt;
                    in_file >> pincnt;
                    vector<int> tmp_pin_x;
                    vector<int> tmp_pin_y;
                    for (int k = 0; k < pincnt; k++)
                    {
                        in_file >> tmp;
                        in_file >> tmp;
                        in_file >> tmp_int;
                        tmp_pin_x.push_back(tmp_int);
                        in_file >> tmp_int;
                        tmp_pin_y.push_back(tmp_int);
                    }
                    tmp_pin_x_1.push_back(tmp_pin_x);
                    tmp_pin_y_1.push_back(tmp_pin_y);
                }
                LibCellSize_x.push_back(tmp_vec_x);
                LibCellSize_y.push_back(tmp_vec_y);
                LibCellSize.push_back(libsizevec);
                Pin_x.push_back(tmp_pin_x_1);
                Pin_y.push_back(tmp_pin_y_1);
            }
            // for input checking
            //  for(int i = 0; i<NumofTech; i++){
            //      for(int j = 0; j<NumofLib; j++){
            //          cout<<j<<" x: "<<LibCellSize_x[i][j]<<" y: "<<LibCellSize_y[i][j]<<endl;
            //      }
            //  }
            //  for(int i = 0; i<NumofTech; i++){
            //      for(int j = 0; j<NumofLib ;j++){
            //          if(Pin_x[i][j].size()!=Pin_y[i][j].size())cout<<"WRONG!!"<<endl;
            //          for(int k = 0; k<Pin_x[i][j].size(); k++){
            //              cout<<"MC"<<j<<" Pin"<<k<<" x:"<<Pin_x[i][j][k]<<" y:"<<Pin_y[i][j][k]<<endl;
            //          }
            //      }
            //  }
        }
        else if (tmp == "DieSize")
        {
            in_file >> LowerLeftX;
            in_file >> LowerLeftY;
            in_file >> UpperRightX;
            in_file >> UpperRighty;
        }
        else if (tmp == "TopDieMaxUtil")
        {
            in_file >> TopDieMaxUtil;
        }
        else if (tmp == "BottomDieMaxUtil")
        {
            in_file >> BottomDieMaxUtil;
        }
        else if (tmp == "TopDieRows")
        {
            in_file >> StartX_t;
            in_file >> StartY_t;
            in_file >> RowLength_t;
            in_file >> RowHeight_t;
            in_file >> RepeatCount_t;
        }
        else if (tmp == "BottomDieRows")
        {
            in_file >> StartX_b;
            in_file >> StartY_b;
            in_file >> RowLength_b;
            in_file >> RowHeight_b;
            in_file >> RepeatCount_b;
        }
        else if (tmp == "TopDieTech")
        {
            in_file >> TopDieTech;
        }
        else if (tmp == "BottomDieTech")
        {
            in_file >> BottomDieTech;
        }
        else if (tmp == "TerminalSize")
        {
            in_file >> Terminal_X;
            in_file >> Terminal_Y;
        }
        else if (tmp == "TerminalSpacing")
        {
            in_file >> Terminal_Spacing;
        }
        else if (tmp == "NumInstances")
        {
            in_file >> Instance_cnt;
            for (int i = 0; i < Instance_cnt; i++)
            {
                in_file >> tmp;
                in_file >> tmp;
                string MCcnt;
                in_file >> MCcnt;
                // cout << MCcnt << endl;
                int mcsize = MCcnt.size();
                int t = 0;
                for (int i = 2; i < mcsize; i++)
                {
                    t *= 10;
                    t += MCcnt[i] - '0';
                }
                Lib_for_Inst.push_back(t - 1);
                // cout << "LibforInst: " << t-1 << endl;
            }
        }
        else if (tmp == "NumNets")
        {
            in_file >> NetCnt;
            vector<bool> temp;
            for (int i = 0; i < NetCnt; i++)
            {
                vector<pair<int, int>> buf;
                temp.push_back(false);
                NetCross.push_back(false);
                in_file >> tmp;
                in_file >> tmp;
                int pinc;
                in_file >> pinc;
                NumofPin.push_back(pinc);
                for (int t = 0; t < pinc; t++)
                {
                    in_file >> tmp;
                    string cp;
                    in_file >> cp;
                    int c_cnt = 0;
                    int p_cnt = 0;
                    int cursor = 0;
                    int cp_len = cp.size();
                    for (int j = 1; j < cp_len; j++)
                    {
                        if (cp[j] == '/')
                        {
                            cursor = j + 1;
                            break;
                        }
                        c_cnt *= 10;
                        c_cnt += cp[j] - '0';
                    }
                    for (int j = cursor; j < cp_len; j++)
                    {
                        if (cp[j] != 'P')
                        {
                            p_cnt *= 10;
                            p_cnt += cp[j] - '0';
                        }
                    }
                    buf.push_back(make_pair(c_cnt - 1, p_cnt - 1));
                }
                net.push_back(buf);
            }
            for (int i = 0; i < 8; i++)
            {
                NetCross_test.push_back(temp);
            }
        }
    }
}

void output_hmetis()
{
    int div = pow(10, divide_cnt);
    out_file.open("hmetis_input.txt");
    out_file << NetCnt << " " << Instance_cnt << " 10" << endl;
    for (int i = 0; i < NetCnt; i++)
    {
        int iter = net[i].size();
        for (int j = 0; j < iter; j++)
        {
            out_file << (net[i][j].first + 1);
            if (j != iter - 1)
                out_file << " ";
        }
        out_file << endl;
    }
    for (int i = 0; i < Instance_cnt; i++)
    {
        int tmp = inst_size[i] / div;
        if (i != (Instance_cnt - 1))
            out_file << tmp << endl;
        else
            out_file << tmp;
    }
    out_file.close();
}

void calc_size()
{

    if (NumofTech == 2)
    {
        int max = -1;
        for (int i = 0; i < Instance_cnt; i++)
        {
            int libnum = Lib_for_Inst[i];
            int sizee;
            if (LibCellSize[0][libnum] > LibCellSize[1][libnum])
            {
                sizee = LibCellSize[0][libnum];
            }
            else
            {
                sizee = LibCellSize[1][libnum];
            }
            inst_size.push_back(sizee);
        }
    }
    else
    {
        for (int i = 0; i < Instance_cnt; i++)
        {
            int libnum = Lib_for_Inst[i];
            inst_size.push_back(LibCellSize[0][libnum]);
        }
    }
}

void perform_hmetis()
{ ////////  need to correct to linux cmd  ////////
    int utilize = 0;

    if (NumofTech == 2)
    {
        utilize = 10;
    }
    else
        utilize = 1;
    stringstream ss;
    ss << utilize;
    string UBfactor = ss.str();
    ss.str("");
    string cmd = "./hmetis hmetis_input.txt 2 ";
    cmd += UBfactor;
    cmd += " 5 1 2 3 0 0";

    system(cmd.c_str());

    // check no overflow
    in_file.open("hmetis_input.txt.part.2");
    int cnt = -1;
    unsigned long long par0_area = 0;
    unsigned long long par1_area = 0;
    unsigned long long par0_area_2 = 0;
    unsigned long long par1_area_2 = 0;
    int i = 0;
    if (NumofTech == 2)
    {
        for (int k = 0; k < Instance_cnt; k++)
        { // Give the partition with larger area to the die with larger max utilization
            in_file >> cnt;
            if (cnt == 0)
            {
                int lib = Lib_for_Inst[i];
                partition_map[i] = 0;
                par0_area += LibCellSize[top][lib];
                par0_area_2 += LibCellSize[bot][lib];
            }
            else
            {
                int lib = Lib_for_Inst[i];
                partition_map[i] = 1;
                par1_area += LibCellSize[bot][lib];
                par1_area_2 += LibCellSize[top][lib];
            }
            i++;
        }
        /*cout << par0_area << endl;
        cout << par1_area << endl;
        cout << par0_area_2 << endl;
        cout << par1_area_2 << endl;*/

        if (par0_area + par1_area > par0_area_2 + par1_area_2)
        {
            for (int j = 0; j < Instance_cnt; j++)
            {
                if (partition_map[j] == 0)
                {
                    partition_map[j] = 1;
                }
                else
                {
                    partition_map[j] = 0;
                }
            }
            par1_area = par0_area_2;
            par0_area = par1_area_2;
        }
    }
    else
    {
        for (int k = 0; k < Instance_cnt; k++)
        {
            in_file >> cnt;
            if (cnt == 0)
            {
                int lib = Lib_for_Inst[i];
                partition_map[i] = 0;
                par0_area += LibCellSize[0][lib];
            }
            else
            {
                int lib = Lib_for_Inst[i];
                partition_map[i] = 1;
                par1_area += LibCellSize[0][lib];
            }
            i++;
        }
    }
    TopDieRealArea = par0_area;
    BottomDieRealArea = par1_area;
    cout << "TopDieMaxArea: " << TopDieMaxArea << endl;
    cout << "TopDieRealArea: " << TopDieRealArea << endl;
    cout << "BottomDieMaxArea: " << BottomDieMaxArea << endl;
    cout << "BottomDieRealArea: " << BottomDieRealArea << endl;
    in_file.close();
}

void calc_divide_cnt()
{
    if (NumofTech == 1)
    {
        unsigned long long size = 0;
        for (int i = 0; i < Instance_cnt; i++)
        {
            int libnum = Lib_for_Inst[i];
            size += LibCellSize[0][libnum];
        }
        for (int i = 0; i < 10; i++)
        {
            i--;
            if (size < 10)
                break;
            divide_cnt += 1;
            size /= 10;
        }
        if (divide_cnt > 7)
        {
            divide_cnt -= 7;
        }
        else
            divide_cnt = 0;
    }
    else
    {
        unsigned long long size = 0;
        for (int i = 0; i < Instance_cnt; i++)
        {
            int libnum = Lib_for_Inst[i];
            if (LibCellSize[0][libnum] > LibCellSize[1][libnum])
            {
                size += LibCellSize[0][libnum];
            }
            else
            {
                size += LibCellSize[1][libnum];
            }
        }
        for (int i = 0; i < 10; i++)
        {
            i--;
            if (size < 10)
                break;
            divide_cnt += 1;
            size /= 10;
        }
        if (divide_cnt > 7)
        {
            divide_cnt -= 7;
        }
        else
            divide_cnt = 0;
        // cout<<"div: "<<divide_cnt<<endl;
    }
}

void partition()
{
    calc_size();
    calc_divide_cnt();
    output_hmetis();
    in_file.close();
    TopDieMaxArea = 0;
    BottomDieMaxArea = 0;
    TopDieMaxArea = (unsigned)(UpperRightX - LowerLeftX) * (UpperRighty - LowerLeftY);
    BottomDieMaxArea = TopDieMaxArea;
    TopDieMaxArea = TopDieMaxArea / 100 * TopDieMaxUtil;
    BottomDieMaxArea = BottomDieMaxArea / 100 * BottomDieMaxUtil;
    for (int i = 0; i < Instance_cnt; i++)
    {
        partition_map.push_back(0);
        best_partition_map.push_back(0);
    }
    if (NumofTech == 2)
    {
        if (TopDieTech == Techname[0])
        {
            top = 0;
            if (BottomDieTech == Techname[0])
            {
                bot = 0;
            }
            else
            {
                bot = 1;
            }
        }
        else
        {
            top = 1;
            if (BottomDieTech == Techname[0])
            {
                bot = 0;
            }
            else
            {
                bot = 1;
            }
        }
    }
    for (int i = 0; i < Instance_cnt; i++)
    {
        cell_gain.push_back(0);
    }
    for (int i = 0; i < 10; i++)
    {
        vector<int> tmp;
        partition_map_test.push_back(tmp);
        vector<int> tmp1, tmp2;
        cell_in_top_test.push_back(tmp1);
        cell_in_bottom_test.push_back(tmp2);
    }
    for (int k = 0; k < 10; k++)
    {
        k--;
        if (hmetis_cnt == 10)
            break;
        perform_hmetis();
        legalize();
        partition_map_test[hmetis_cnt].assign(partition_map.begin(), partition_map.end());
        for (int i = 0; i < partition_map.size(); i++)
        {
            if (partition_map[i] == 0)
                cell_in_top_test[hmetis_cnt].push_back(i);
            else if (partition_map[i] == 1)
                cell_in_bottom_test[hmetis_cnt].push_back(i);
        }
        hmetis_cnt++;
    }
    partition_map.assign(best_partition_map.begin(), best_partition_map.end());
    Cell_in_top.clear();
    Cell_in_bottom.clear();
    for (int i = 0; i < partition_map.size(); i++)
    {
        if (partition_map[i] == 0)
            Cell_in_top.push_back(i);
        else if (partition_map[i] == 1)
            Cell_in_bottom.push_back(i);
    }
    TopDieRealArea = best_top_area;
    BottomDieRealArea = best_bot_area;
    cout << "best_cut: " << best_cut << endl;
    cout << "TopDieRealArea: " << TopDieRealArea << "   " << best_top_area << endl;
    cout << "BottomDieRealArea: " << BottomDieRealArea << "   " << best_bot_area << endl;
    Cell_in_bottom.clear();
    Cell_in_top.clear();
    for (int i = 0; i < Instance_cnt; i++)
    {
        if (partition_map[i] == 0)
            Cell_in_top.push_back(i);
        else if (partition_map[i] == 1)
            Cell_in_bottom.push_back(i);
    }
    int cut = 0;
    for (int i = 0; i < NetCnt; i++)
    {
        int size = net[i].size();
        int from = 0; // from = top die
        int to = 0;   // to = bottom die
        for (int j = 0; j < size; j++)
        {
            if (partition_map[net[i][j].first] == 0)
            {
                from++;
            }
            else if (partition_map[net[i][j].first] == 1)
            {
                to++;
            }
        }
        if (from > 0 && to > 0)
        {
            cut++;
        }
    }
    cout << "real cut: " << cut << endl;
}

void legalize()
{
    // compute gain
    for (int i = 0; i < Instance_cnt; i++)
    {
        cell_gain[i] = 0;
    }
    for (int i = 0; i < NetCnt; i++)
    {
        int size = net[i].size();
        int from = 0; // from = top die
        int to = 0;   // to = bottom die
        for (int j = 0; j < size; j++)
        {
            if (partition_map[net[i][j].first] == 0)
            {
                from++;
            }
            else if (partition_map[net[i][j].first] == 1)
            {
                to++;
            }
        }
        for (int j = 0; j < size; j++)
        {
            if (partition_map[net[i][j].first] == 0)
            {
                if (from == 1)
                {
                    cell_gain[net[i][j].first]++;
                }
                else if (to == 0)
                {
                    cell_gain[net[i][j].first]--;
                }
            }
            else if (partition_map[net[i][j].first] == 1)
            {
                if (to == 1)
                {
                    cell_gain[net[i][j].first]++;
                }
                else if (from == 0)
                {
                    cell_gain[net[i][j].first]--;
                }
            }
        }
    }
    // create cell_array
    vector<vector<int>> cell_array; //[instnum][idx]
    for (int i = 0; i < Instance_cnt; i++)
    {
        vector<int> tmp;
        cell_array.push_back(tmp);
    }
    for (int i = 0; i < NetCnt; i++)
    {
        int size = net[i].size();
        for (int j = 0; j < size; j++)
        {
            cell_array[net[i][j].first].push_back(i);
        }
    }
    int stop = 0;
    int flag = -1;
    for (int hhh = 0; hhh < 10; hhh++)
    {
        hhh--;
        if ((TopDieRealArea < TopDieMaxArea) && (BottomDieRealArea < BottomDieMaxArea))
        {
            stop++;
        }
        if (stop == 1)
        {
            if (TopDieMaxArea < 50000 || BottomDieMaxArea < 50000)
                break;
        }
        if (stop == 5)
        {
            if (TopDieMaxArea < 100000000 || BottomDieMaxArea < 1000000)
                break;
        }
        if (stop == 10)
        {
            break;
        }
        if (TopDieRealArea >= TopDieMaxArea)
            flag = 0;
        else if (BottomDieRealArea >= BottomDieMaxArea)
            flag = 1;
        int swap_inst = -1;
        int max_gain = -400000000;
        if (NumofTech == 2)
        {
            if (flag == 0)
            {
                for (int i = 0; i < Instance_cnt; i++)
                {
                    if (partition_map[i] == 0)
                    {
                        if ((BottomDieRealArea + LibCellSize[bot][Lib_for_Inst[i]]) < BottomDieMaxArea)
                        {
                            if (cell_gain[i] > max_gain)
                            {
                                swap_inst = i;
                                max_gain = cell_gain[i];
                            }
                        }
                    }
                }
                if (swap_inst != -1)
                {
                    // update gain(before swap)
                    int size = cell_array[swap_inst].size();
                    for (int i = 0; i < size; i++)
                    {
                        int net_num = cell_array[swap_inst][i];
                        int to = 0;
                        for (int j = 0; j < net[net_num].size(); j++)
                        {
                            if (partition_map[net[net_num][j].first] == 1)
                            {
                                to++;
                            }
                        }
                        if (to == 0)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    cell_gain[net[net_num][j].first]++;
                                }
                            }
                        }
                        else if (to == 1)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    cell_gain[net[net_num][j].first]--;
                                    break;
                                }
                            }
                        }
                    }
                    // swap instance
                    partition_map[swap_inst] = 1;
                    BottomDieRealArea += LibCellSize[bot][Lib_for_Inst[swap_inst]];
                    TopDieRealArea -= LibCellSize[top][Lib_for_Inst[swap_inst]];
                    // update gain(after swap)
                    for (int i = 0; i < size; i++)
                    {
                        int net_num = cell_array[swap_inst][i];
                        int from = 0;
                        for (int j = 0; j < net[net_num].size(); j++)
                        {
                            if (partition_map[net[net_num][j].first] == 0)
                            {
                                from++;
                            }
                        }
                        if (from == 0)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    cell_gain[net[net_num][j].first]--;
                                }
                            }
                        }
                        else if (from == 1)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    cell_gain[net[net_num][j].first]++;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            else if (flag == 1)
            {
                for (int i = 0; i < Instance_cnt; i++)
                {
                    if (partition_map[i] == 1)
                    {
                        if ((TopDieRealArea + LibCellSize[top][Lib_for_Inst[i]]) < TopDieMaxArea)
                        {
                            if (cell_gain[i] > max_gain)
                            {
                                swap_inst = i;
                                max_gain = cell_gain[i];
                            }
                        }
                    }
                }
                if (swap_inst != -1)
                {
                    // update gain(before swap)
                    int size = cell_array[swap_inst].size();
                    for (int i = 0; i < size; i++)
                    {
                        int net_num = cell_array[swap_inst][i];
                        int from = 0;
                        int to = 0;
                        for (int j = 0; j < net[net_num].size(); j++)
                        {
                            if (partition_map[net[net_num][j].first] == 0)
                            {
                                to++;
                            }
                        }
                        if (to == 0)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    cell_gain[net[net_num][j].first]++;
                                }
                            }
                        }
                        else if (to == 1)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    cell_gain[net[net_num][j].first]--;
                                    break;
                                }
                            }
                        }
                    }
                    // swap instance
                    partition_map[swap_inst] = 0;
                    BottomDieRealArea -= LibCellSize[bot][Lib_for_Inst[swap_inst]];
                    TopDieRealArea += LibCellSize[top][Lib_for_Inst[swap_inst]];
                    for (int i = 0; i < size; i++)
                    {
                        int net_num = cell_array[swap_inst][i];
                        int from = 0;
                        for (int j = 0; j < net[net_num].size(); j++)
                        {
                            if (partition_map[net[net_num][j].first] == 1)
                            {
                                from++;
                            }
                        }
                        if (from == 0)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    cell_gain[net[net_num][j].first]--;
                                }
                            }
                        }
                        else if (from == 1)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    cell_gain[net[net_num][j].first]++;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (NumofTech == 1)
        {
            if (flag == 0)
            {
                for (int i = 0; i < Instance_cnt; i++)
                {
                    if (partition_map[i] == 0)
                    {
                        if ((BottomDieRealArea + LibCellSize[bot][Lib_for_Inst[i]]) < BottomDieMaxArea)
                        {
                            if (cell_gain[i] > max_gain)
                            {
                                swap_inst = i;
                                max_gain = cell_gain[i];
                            }
                        }
                    }
                }
                if (swap_inst != -1)
                {
                    // update gain(before swap)
                    int size = cell_array[swap_inst].size();
                    for (int i = 0; i < size; i++)
                    {
                        int net_num = cell_array[swap_inst][i];
                        int from = 0;
                        int to = 0;
                        for (int j = 0; j < net[net_num].size(); j++)
                        {
                            if (partition_map[net[net_num][j].first] == 1)
                            {
                                to++;
                            }
                        }
                        if (to == 0)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    cell_gain[net[net_num][j].first]++;
                                }
                            }
                        }
                        else if (to == 1)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    cell_gain[net[net_num][j].first]--;
                                    break;
                                }
                            }
                        }
                    }
                    // swap instance
                    partition_map[swap_inst] = 1;
                    BottomDieRealArea += LibCellSize[bot][Lib_for_Inst[swap_inst]];
                    TopDieRealArea -= LibCellSize[top][Lib_for_Inst[swap_inst]];
                    for (int i = 0; i < size; i++)
                    {
                        int net_num = cell_array[swap_inst][i];
                        int from = 0;
                        for (int j = 0; j < net[net_num].size(); j++)
                        {
                            if (partition_map[net[net_num][j].first] == 0)
                            {
                                from++;
                            }
                        }
                        if (from == 0)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    cell_gain[net[net_num][j].first]--;
                                }
                            }
                        }
                        else if (from == 1)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    cell_gain[net[net_num][j].first]++;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            else if (flag == 1)
            {
                for (int i = 0; i < Instance_cnt; i++)
                {
                    if (partition_map[i] == 1)
                    {
                        if ((TopDieRealArea + LibCellSize[top][Lib_for_Inst[i]]) < TopDieMaxArea)
                        {
                            if (cell_gain[i] > max_gain)
                            {
                                swap_inst = i;
                                max_gain = cell_gain[i];
                            }
                        }
                    }
                }
                if (swap_inst != -1)
                {
                    // update gain(before swap)
                    int size = cell_array[swap_inst].size();
                    for (int i = 0; i < size; i++)
                    {
                        int net_num = cell_array[swap_inst][i];
                        int from = 0;
                        int to = 0;
                        for (int j = 0; j < net[net_num].size(); j++)
                        {
                            if (partition_map[net[net_num][j].first] == 0)
                            {
                                to++;
                            }
                        }
                        if (to == 0)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    cell_gain[net[net_num][j].first]++;
                                }
                            }
                        }
                        else if (to == 1)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    cell_gain[net[net_num][j].first]--;
                                    break;
                                }
                            }
                        }
                    }
                    // swap instance
                    partition_map[swap_inst] = 0;
                    BottomDieRealArea -= LibCellSize[bot][Lib_for_Inst[swap_inst]];
                    TopDieRealArea += LibCellSize[top][Lib_for_Inst[swap_inst]];
                    for (int i = 0; i < size; i++)
                    {
                        int net_num = cell_array[swap_inst][i];
                        int from = 0;
                        for (int j = 0; j < net[net_num].size(); j++)
                        {
                            if (partition_map[net[net_num][j].first] == 1)
                            {
                                from++;
                            }
                        }
                        if (from == 0)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 0)
                                {
                                    cell_gain[net[net_num][j].first]--;
                                }
                            }
                        }
                        else if (from == 1)
                        {
                            for (int j = 0; j < net[net_num].size(); j++)
                            {
                                if (partition_map[net[net_num][j].first] == 1)
                                {
                                    cell_gain[net[net_num][j].first]++;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    int cut = 0;
    for (int i = 0; i < NetCnt; i++)
    {
        int size = net[i].size();
        int from = 0; // from = top die
        int to = 0;   // to = bottom die
        for (int j = 0; j < size; j++)
        {
            if (partition_map[net[i][j].first] == 0)
            {
                from++;
            }
            else if (partition_map[net[i][j].first] == 1)
            {
                to++;
            }
        }
        if (from > 0 && to > 0)
        {
            cut++;
        }
    }
    if (cut < best_cut)
    {
        best_cut = cut;
        best_partition_map.assign(partition_map.begin(), partition_map.end());
        best_bot_area = BottomDieRealArea;
        best_top_area = TopDieRealArea;
    }
    /*cout <<"TopDieMaxArea: "<< TopDieMaxArea << endl;
    cout <<"TopDieRealArea: "<< TopDieRealArea << endl;
    cout <<"BottomDieMaxArea: "<< BottomDieMaxArea << endl;
    cout <<"BottomDieRealArea: "<< BottomDieRealArea << endl;*/
    cout << "cut: " << cut << endl;
}

/*---------------Generate Lef---------------*/

void *generate_lef_version(ofstream &out)
{
    out << "VERSION 5.8 ;" << endl;
    out << "BUSBITCHARS \"[]\" ; " << endl;
    out << "UNITS" << endl;
    out << "  DATABASE MICRONS " << unit << " ;" << endl;
    out << "END UNITS" << endl;
    out << "DIVIDERCHAR \"/\" ;" << endl;
    out << "MANUFACTURINGGRID 0.0005 ;" << endl;
    out << endl;
    out << "LAYER Metal" << endl;
    out << "  TYPE ROUTING ;" << endl;
    out << "  DIRECTION HORIZONTAL ;" << endl;
    out << "  PITCH 0.0001 ;" << endl;
    out << "  WIDTH 0.0001 ;" << endl;
    out << "END Metal" << endl;
    out << endl;
}

void *generate_def_version(ofstream &out)
{
    out << "VERSION 5.8 ;" << endl;
    out << "BUSBITCHARS \"[]\" ; " << endl;
    out << "DIVIDERCHAR \"/\" ;" << endl;
    out << "DESIGN HW ;" << endl;
    out << "UNITS DISTANCE MICRONS " << unit << " ;" << endl;
    out << endl;
}

void *generate_top_macro_lef(int index, ofstream &out)
{
    int macro_cnt = 0;
    int die_tech = -1;
    if (TopDieTech == Techname[0])
    {
        macro_cnt = LibCellSize[0].size();
        die_tech = 0;
    }
    else if (TopDieTech == Techname[1])
    {
        macro_cnt = LibCellSize[1].size();
        die_tech = 1;
    }

    out << "SITE Site" << endl;
    out << "  CLASS CORE ;" << endl;
    out << "  SIZE 0.01 BY " << (float)RowHeight_t / unit << " ;" << endl;
    out << "END Site" << endl;
    out << endl;

    for (int i = 0; i < macro_cnt; i++)
    {
        out << "MACRO MC" << i + 1 << "_TOP" << endl;
        out << "  CLASS CORE ;" << endl;
        out << "  ORIGIN 0 0 ;" << endl;
        out << "  FOREIGN MC" << i + 1 << "_TOP 0 0 ;" << endl;
        out << "  SIZE " << (float)LibCellSize_x[die_tech][i] / unit << " BY " << (float)LibCellSize_y[die_tech][i] / unit << " ;" << endl;
        out << "  SITE Site ;" << endl;
        for (int j = 0; j < Pin_x[die_tech][i].size(); j++)
        {
            out << "  PIN P" << j + 1 << endl;
            out << "    DIRECTION INPUT ;" << endl;
            out << "    USE SIGNAL ;" << endl;
            out << "    PORT" << endl;
            out << "      LAYER Metal ;" << endl;
            out << "        RECT " << (float)Pin_x[die_tech][i][j] / unit << " " << (float)Pin_y[die_tech][i][j] / unit << " " << (float)Pin_x[die_tech][i][j] / unit << " " << (float)Pin_y[die_tech][i][j] / unit << " ;" << endl;
            out << "    END" << endl;
            out << "  END P" << j + 1 << endl;
        }
        out << "END MC" << i + 1 << "_TOP" << endl;
        out << endl;
    }
}

void *generate_bottom_macro_lef(int index, ofstream &out)
{
    int macro_cnt = 0;
    int die_tech = -1;
    if (BottomDieTech == Techname[0])
    {
        macro_cnt = LibCellSize[0].size();
        die_tech = 0;
    }
    else if (BottomDieTech == Techname[1])
    {
        macro_cnt = LibCellSize[1].size();
        die_tech = 1;
    }

    out << "SITE Site" << endl;
    out << "  CLASS CORE ;" << endl;
    out << "  SIZE 0.01 BY " << (float)RowHeight_b / unit << " ;" << endl;
    out << "END Site" << endl;
    out << endl;

    for (int i = 0; i < macro_cnt; i++)
    {
        out << "MACRO MC" << i + 1 << "_BOT" << endl;
        out << "  CLASS CORE ;" << endl;
        out << "  ORIGIN 0 0 ;" << endl;
        out << "  FOREIGN MC" << i + 1 << "_BOT 0 0 ;" << endl;
        out << "  SIZE " << (float)LibCellSize_x[die_tech][i] / unit << " BY " << (float)LibCellSize_y[die_tech][i] / unit << " ;" << endl;
        out << "  SITE Site ;" << endl;
        for (int j = 0; j < Pin_x[die_tech][i].size(); j++)
        {
            out << "  PIN P" << j + 1 << endl;
            out << "    DIRECTION INPUT ;" << endl;
            out << "    USE SIGNAL ;" << endl;
            out << "    PORT" << endl;
            out << "      LAYER Metal ;" << endl;
            out << "        RECT " << (float)Pin_x[die_tech][i][j] / unit << " " << (float)Pin_y[die_tech][i][j] / unit << " " << (float)Pin_x[die_tech][i][j] / unit << " " << (float)Pin_y[die_tech][i][j] / unit << " ;" << endl;
            out << "    END" << endl;
            out << "  END P" << j + 1 << endl;
        }
        out << "END MC" << i + 1 << "_BOT" << endl;
        out << endl;
    }
}

void *generate_top_def(int index, ofstream &out)
{
    out << "DIEAREA ( " << LowerLeftX << " " << LowerLeftY << " ) ( " << UpperRightX << " " << UpperRighty << " ) ;" << endl;
    out << endl;

    for (int i = 0; i < RepeatCount_t; i++)
    {
        out << "ROW ROW_TOP_" << i << " Site 0 " << i * RowHeight_t << " N DO " << UpperRightX << " BY 1 STEP 1 0" << endl;
        out << " ;" << endl;
    }
    out << endl;

    int par_size = partition_map.size();
    int top_size = cell_in_top_test[index].size();

    out << "COMPONENTS " << top_size + cutname_test[index].size() << " ;" << endl;
    for (int i = 0; i < top_size; i++)
    {
        out << "- C" << cell_in_top_test[index][i] + 1 << " MC" << Lib_for_Inst[cell_in_top_test[index][i]] + 1 << "_TOP + PLACED ( " << inst_pos_test[index][cell_in_top_test[index][i]].first << " " << inst_pos_test[index][cell_in_top_test[index][i]].second << " ) N ;" << endl;
    }
    for (int i = 0; i < cutname_test[index].size(); i++)
    {
        out << "- T" << cutname_test[index][i] + 1 << " TER_" << cutname_test[index][i] + 1 << " + FIXED ( " << term_pos_test[index][i].first << " " << term_pos_test[index][i].second << " ) N ;" << endl;
    }
    out << "END COMPONENTS" << endl;
    out << endl;
    out << endl;

    out << "NETS " << NetCnt << " ;" << endl;
    for (int i = 0; i < NetCnt; i++)
    {
        bool has_net = false;
        int pin_cnt = NumofPin[i];
        int actual_cnt = 0;
        out << "- N" << i + 1 << endl;
        has_net = true;
        if (NetCross_test[index][i])
        {
            out << " ( T" << i + 1 << " P1 )";
            actual_cnt++;
        }
        for (int j = 0; j < pin_cnt; j++)
        {
            vector<int>::iterator it = find(cell_in_top_test[index].begin(), cell_in_top_test[index].end(), net[i][j].first);
            if (it != cell_in_top_test[index].end())
            {
                out << " ( C" << net[i][j].first + 1 << " P" << net[i][j].second + 1 << " )";
                actual_cnt++;
            }
        }
        out << " ;";
        out << endl;
        actual_cnt = 0;
    }
    out << "END NETS" << endl;
    out << endl;

    out << "END DESIGN";
}

void *generate_bottom_def(int index, ofstream &out)
{
    out << "DIEAREA ( " << LowerLeftX << " " << LowerLeftY << " ) ( " << UpperRightX << " " << UpperRighty << " ) ;" << endl;
    out << endl;

    for (int i = 0; i < RepeatCount_b; i++)
    {
        out << "ROW ROW_BOT_" << i << " Site 0 " << i * RowHeight_b << " N DO " << UpperRightX << " BY 1 STEP 1 0" << endl;
        out << " ;" << endl;
    }
    out << endl;

    int par_size = partition_map_test[index].size();
    int bot_size = cell_in_bottom_test[index].size();

    out << "COMPONENTS " << bot_size + cutname_test[index].size() << " ;" << endl;
    for (int i = 0; i < bot_size; i++)
    {
        out << "- C" << cell_in_bottom_test[index][i] + 1 << " MC" << Lib_for_Inst[cell_in_bottom_test[index][i]] + 1 << "_BOT + PLACED ( " << inst_pos_test[index][cell_in_bottom_test[index][i]].first << " " << inst_pos_test[index][cell_in_bottom_test[index][i]].second << " ) N ;" << endl;
    }
    for (int i = 0; i < cutname_test[index].size(); i++)
    {
        out << "- T" << cutname_test[index][i] + 1 << " TER_" << cutname_test[index][i] + 1 << " + FIXED ( " << term_pos_test[index][i].first << " " << term_pos_test[index][i].second << " ) N ;" << endl;
    }
    out << "END COMPONENTS" << endl;
    out << endl;
    out << endl;

    out << "NETS " << NetCnt << " ;" << endl;
    for (int i = 0; i < NetCnt; i++)
    {
        bool has_net = false;
        int pin_cnt = NumofPin[i];
        int actual_cnt = 0;
        out << "- N" << i + 1 << endl;
        if (NetCross_test[index][i])
        {
            out << " ( T" << i + 1 << " P1 )";
            actual_cnt++;
        }
        for (int j = 0; j < pin_cnt; j++)
        {
            vector<int>::iterator it = find(cell_in_bottom_test[index].begin(), cell_in_bottom_test[index].end(), net[i][j].first);
            if (it != cell_in_bottom_test[index].end())
            {
                out << " ( C" << net[i][j].first + 1 << " P" << net[i][j].second + 1 << " )";
                actual_cnt++;
            }
        }
        out << " ;";
        out << endl;
        actual_cnt = 0;
    }
    out << "END NETS" << endl;
    out << endl;

    out << "END DESIGN";
}

void *generate_terminal_lef(int index, ofstream &out)
{
    for (int i = 0; i < cutname_test[index].size(); i++)
    {
        out << "MACRO TER_" << cutname_test[index][i] + 1 << endl;
        out << "  CLASS CORE ;" << endl;
        out << "  ORIGIN " << (float)term_pos_test[index][i].first / unit << " " << (float)term_pos_test[index][i].second / unit << " ;" << endl;
        out << "  SIZE 0 BY 0 ;" << endl;
        out << "  SITE Site ;" << endl;
        out << "  PIN P1" << endl;
        out << "    DIRECTION INPUT ;" << endl;
        out << "    USE SIGNAL ;" << endl;
        out << "    PORT" << endl;
        out << "      LAYER Metal ;" << endl;
        out << "        RECT 0 0 0 0 ;" << endl;
        out << "    END" << endl;
        out << "  END P1" << endl;
        out << "END TER_" << cutname_test[index][i] + 1 << endl;
        out << endl;
    }
}

void *generate_top(void *index)
{
    int ind = *((int *)(&index));
    switch (ind)
    {
    case 0:
        out_0.open("top_die_0.lef");
        generate_lef_version(out_0);
        generate_top_macro_lef(ind, out_0);
        generate_terminal_lef(ind, out_0);
        out_0.close();
        out_0.open("top_die_0.def");
        if (out_0.is_open())
            cout << "out_0 success" << endl;
        generate_def_version(out_0);
        generate_top_def(ind, out_0);
        out_0.close();
        break;
    case 1:
        out_1.open("top_die_1.lef");
        generate_lef_version(out_1);
        generate_top_macro_lef(ind, out_1);
        generate_terminal_lef(ind, out_1);
        out_1.close();
        out_1.open("top_die_1.def");
        if (out_1.is_open())
            cout << "out_1 success" << endl;
        generate_def_version(out_1);
        generate_top_def(ind, out_1);
        out_1.close();
        break;
    case 2:
        out_2.open("top_die_2.lef");
        generate_lef_version(out_2);
        generate_top_macro_lef(ind, out_2);
        generate_terminal_lef(ind, out_2);
        out_2.close();
        out_2.open("top_die_2.def");
        if (out_2.is_open())
            cout << "out_2 success" << endl;
        generate_def_version(out_2);
        generate_top_def(ind, out_2);
        out_2.close();
        break;
    case 3:
        out_3.open("top_die_3.lef");
        generate_lef_version(out_3);
        generate_top_macro_lef(ind, out_3);
        generate_terminal_lef(ind, out_3);
        out_3.close();
        out_3.open("top_die_3.def");
        if (out_3.is_open())
            cout << "out_3 success" << endl;
        generate_def_version(out_3);
        generate_top_def(ind, out_3);
        out_3.close();
        break;
    case 4:
        out_4.open("top_die_4.lef");
        generate_lef_version(out_4);
        generate_top_macro_lef(ind, out_4);
        generate_terminal_lef(ind, out_4);
        out_4.close();
        out_4.open("top_die_4.def");

        if (out_4.is_open())
            cout << "out_4 success" << endl;
        generate_def_version(out_4);
        generate_top_def(ind, out_4);
        out_4.close();
        break;
    case 5:
        out_5.open("top_die_5.lef");
        generate_lef_version(out_5);
        generate_top_macro_lef(ind, out_5);
        generate_terminal_lef(ind, out_5);
        out_5.close();
        out_5.open("top_die_5.def");

        if (out_5.is_open())
            cout << "out_5 success" << endl;
        generate_def_version(out_5);
        generate_top_def(ind, out_5);
        out_5.close();
        break;
    case 6:
        out_6.open("top_die_6.lef");
        generate_lef_version(out_6);
        generate_top_macro_lef(ind, out_6);
        generate_terminal_lef(ind, out_6);
        out_6.close();
        out_6.open("top_die_6.def");

        if (out_6.is_open())
            cout << "out_6 success" << endl;
        generate_def_version(out_6);
        generate_top_def(ind, out_6);
        out_6.close();
        break;
    case 7:
        out_7.open("top_die_7.lef");
        generate_lef_version(out_7);
        generate_top_macro_lef(ind, out_7);
        generate_terminal_lef(ind, out_7);
        out_7.close();
        out_7.open("top_die_7.def");

        if (out_7.is_open())
            cout << "out_7 success" << endl;
        generate_def_version(out_7);
        generate_top_def(ind, out_7);
        out_7.close();
        break;
    default:
        break;
    }
}

void *generate_bottom(void *index)
{
    int ind = *((int *)(&index));
    switch (ind)
    {
    case 0:
        out_0.open("bot_die_0.lef");
        generate_lef_version(out_0);
        generate_bottom_macro_lef(ind, out_0);
        generate_terminal_lef(ind, out_0);
        out_0.close();
        out_0.open("bot_die_0.def");
        generate_def_version(out_0);
        generate_bottom_def(ind, out_0);
        out_0.close();
        break;
    case 1:
        out_1.open("bot_die_1.lef");
        generate_lef_version(out_1);
        generate_bottom_macro_lef(ind, out_1);
        generate_terminal_lef(ind, out_1);
        out_1.close();
        out_1.open("bot_die_1.def");
        generate_def_version(out_1);
        generate_bottom_def(ind, out_1);
        out_1.close();
        break;
    case 2:
        out_2.open("bot_die_2.lef");
        generate_lef_version(out_2);
        generate_bottom_macro_lef(ind, out_2);
        generate_terminal_lef(ind, out_2);
        out_2.close();
        out_2.open("bot_die_2.def");
        generate_def_version(out_2);
        generate_bottom_def(ind, out_2);
        out_2.close();
        break;
    case 3:
        out_3.open("bot_die_3.lef");
        generate_lef_version(out_3);
        generate_bottom_macro_lef(ind, out_3);
        generate_terminal_lef(ind, out_3);
        out_3.close();
        out_3.open("bot_die_3.def");
        generate_def_version(out_3);
        generate_bottom_def(ind, out_3);
        out_3.close();
        break;
    case 4:
        out_4.open("bot_die_4.lef");
        generate_lef_version(out_4);
        generate_bottom_macro_lef(ind, out_4);
        generate_terminal_lef(ind, out_4);
        out_4.close();
        out_4.open("bot_die_4.def");
        generate_def_version(out_4);
        generate_bottom_def(ind, out_4);
        out_4.close();
        break;
    case 5:
        out_5.open("bot_die_5.lef");
        generate_lef_version(out_5);
        generate_bottom_macro_lef(ind, out_5);
        generate_terminal_lef(ind, out_5);
        out_5.close();
        out_5.open("bot_die_5.def");
        generate_def_version(out_5);
        generate_bottom_def(ind, out_5);
        out_5.close();
        break;
    case 6:
        out_6.open("bot_die_6.lef");
        generate_lef_version(out_6);
        generate_bottom_macro_lef(ind, out_6);
        generate_terminal_lef(ind, out_6);
        out_6.close();
        out_6.open("bot_die_6.def");
        generate_def_version(out_6);
        generate_bottom_def(ind, out_6);
        out_6.close();
        break;
    case 7:
        out_7.open("bot_die_7.lef");
        generate_lef_version(out_7);
        generate_bottom_macro_lef(ind, out_7);
        generate_terminal_lef(ind, out_7);
        out_7.close();
        out_7.open("bot_die_7.def");
        generate_def_version(out_7);
        generate_bottom_def(ind, out_7);
        out_7.close();
        break;
    default:
        break;
    }
}

void generate_lefdef()
{
    pthread_t *g = (pthread_t *)malloc(8 * sizeof(pthread_t));
    // pthread_t t0, t1, t2, t3, t4, t5, t6, t7;
    unit = 100;
    int ind = 0;
    pthread_create(&g[0], NULL, &generate_top, (void *)ind);
    ind = 1;
    pthread_create(&g[1], NULL, &generate_top, (void *)ind);
    ind = 2;
    pthread_create(&g[2], NULL, &generate_top, (void *)ind);
    ind = 3;
    pthread_create(&g[3], NULL, &generate_top, (void *)ind);
    ind = 4;
    pthread_create(&g[4], NULL, &generate_top, (void *)ind);
    ind = 5;
    pthread_create(&g[5], NULL, &generate_top, (void *)ind);
    ind = 6;
    pthread_create(&g[6], NULL, &generate_top, (void *)ind);
    ind = 7;
    pthread_create(&g[7], NULL, &generate_top, (void *)ind);
    pthread_join(g[0], NULL);
    pthread_join(g[1], NULL);
    pthread_join(g[2], NULL);
    pthread_join(g[3], NULL);
    pthread_join(g[4], NULL);
    pthread_join(g[5], NULL);
    pthread_join(g[6], NULL);
    pthread_join(g[7], NULL);
    ind = 0;
    pthread_create(&g[0], NULL, &generate_bottom, (void *)ind);
    ind = 1;
    pthread_create(&g[1], NULL, &generate_bottom, (void *)ind);
    ind = 2;
    pthread_create(&g[2], NULL, &generate_bottom, (void *)ind);
    ind = 3;
    pthread_create(&g[3], NULL, &generate_bottom, (void *)ind);
    ind = 4;
    pthread_create(&g[4], NULL, &generate_bottom, (void *)ind);
    ind = 5;
    pthread_create(&g[5], NULL, &generate_bottom, (void *)ind);
    ind = 6;
    pthread_create(&g[6], NULL, &generate_bottom, (void *)ind);
    ind = 7;
    pthread_create(&g[7], NULL, &generate_bottom, (void *)ind);
    pthread_join(g[0], NULL);
    pthread_join(g[1], NULL);
    pthread_join(g[2], NULL);
    pthread_join(g[3], NULL);
    pthread_join(g[4], NULL);
    pthread_join(g[5], NULL);
    pthread_join(g[6], NULL);
    pthread_join(g[7], NULL);
    free(g);
}

/*---------------Generate bookshelf---------------*/

void generate_nodes(int index, ofstream &out)
{
    /*---------top die---------*/
    out.open("top_die_" + to_string(index) + ".nodes");
    out << "UCLA nodes 1.0" << endl;
    out << endl;
    int c_num = cutname_test[index].size();
    out << "NumNodes	:	" << cell_in_top_test[index].size() + c_num << endl;
    out << "NumTerminals	:	" << c_num << endl;
    int die_tech = top;
    for (int i = 0; i < cell_in_top_test[index].size(); i++)
    {
        int lib = Lib_for_Inst[cell_in_top_test[index][i]];
        out << "	C" << cell_in_top_test[index][i] + 1 << "	" << LibCellSize_x[die_tech][lib] << "	" << LibCellSize_y[die_tech][lib] << endl;
    }
    for (int i = 0; i < c_num; i++)
    {
        out << "TER_" << cutname_test[index][i] + 1 << "	0	0	terminal" << endl;
    }
    out.close();
    /*---------bot die---------*/
    out.open("bot_die_" + to_string(index) + ".nodes");
    out << "UCLA nodes 1.0" << endl;
    out << endl;
    out << "NumNodes	:	" << cell_in_bottom_test[index].size() + c_num << endl;
    out << "NumTerminals	:	" << c_num << endl;
    die_tech = bot;
    int bot_size = cell_in_bottom_test[index].size();
    for (int i = 0; i < bot_size; i++)
    {
        int lib = Lib_for_Inst[cell_in_bottom_test[index][i]];
        out << "	C" << cell_in_bottom_test[index][i] + 1 << "	" << LibCellSize_x[die_tech][lib] << "	" << LibCellSize_y[die_tech][lib] << endl;
    }
    for (int i = 0; i < c_num; i++)
    {
        out << "TER_" << cutname_test[index][i] + 1 << "	0	0	terminal" << endl;
    }
    out.close();
}

void generate_nets(int index, ofstream &out)
{
    /*---------top die---------*/
    out.open("top_die_" + to_string(index) + ".nets");
    out << "UCLA nets 1.0" << endl;
    out << endl;
    vector<vector<pair<int, int>>> net_top_inst;
    int top_inst = 0;
    int NumNets = 0;
    for (int i = 0; i < NetCnt; i++)
    {
        int net_size = net[i].size();
        vector<pair<int, int>> tmp;
        tmp.clear();
        for (int j = 0; j < net_size; j++)
        {
            if (partition_map_test[index][net[i][j].first] == 0)
            {
                tmp.push_back(make_pair(net[i][j].first, net[i][j].second));
            }
        }
        top_inst += tmp.size();
        if (NetCross_test[index][i])
            top_inst++;
        if (tmp.size() != 0)
            NumNets++;
        net_top_inst.push_back(tmp);
    }
    out << "NumNets	:	" << NumNets << endl;
    out << "NumPins	:	" << top_inst << endl;
    out << endl;

    for (int i = 0; i < NetCnt; i++)
    {
        if (net_top_inst[i].size() != 0)
        {
            int dgr = net_top_inst[i].size();
            if (NetCross_test[index][i])
                dgr++;
            out << "NetDegree	:	" << dgr << "	N" << i + 1 << endl;
            for (int j = 0; j < net_top_inst[i].size(); j++)
            {
                out << "	C" << net_top_inst[i][j].first + 1 << "	I	:	";
                int lib = Lib_for_Inst[net_top_inst[i][j].first];
                float mid_x = (float)LibCellSize_x[top][lib] / 2;
                float tmp_x = (float)Pin_x[top][lib][net_top_inst[i][j].second] - mid_x;
                out << tmp_x << "	";
                float mid_y = (float)LibCellSize_y[top][lib] / 2;
                float tmp_y = (float)Pin_y[top][lib][net_top_inst[i][j].second] - mid_y;
                out << tmp_y << endl;
            }
            if (NetCross_test[index][i])
                out << "	TER_" << i + 1 << "	I	:	0	0" << endl;
        }
    }
    out.close();
    /*---------bot die---------*/
    out.open("bot_die_" + to_string(index) + ".nets");
    out << "UCLA nets 1.0" << endl;
    out << endl;
    vector<vector<pair<int, int>>> net_bot_inst;
    int bot_inst = 0;
    NumNets = 0;
    for (int i = 0; i < NetCnt; i++)
    {
        int net_size = net[i].size();
        vector<pair<int, int>> tmp;
        for (int j = 0; j < net_size; j++)
        {
            if (partition_map_test[index][net[i][j].first] == 1)
            {
                tmp.push_back(make_pair(net[i][j].first, net[i][j].second));
            }
        }
        bot_inst += tmp.size();
        if (NetCross_test[index][i])
            bot_inst++;
        if (tmp.size() != 0)
            NumNets++;
        net_bot_inst.push_back(tmp);
    }
    out << "NumNets	:	" << NumNets << endl;
    out << "NumPins	:	" << bot_inst << endl;
    out << endl;

    for (int i = 0; i < NetCnt; i++)
    {
        if (net_bot_inst[i].size() != 0)
        {
            int dgr = net_bot_inst[i].size();
            if (NetCross_test[index][i])
                dgr++;
            out << "NetDegree	:	" << dgr << "	N" << i + 1 << endl;
            for (int j = 0; j < net_bot_inst[i].size(); j++)
            {
                out << "	C" << net_bot_inst[i][j].first + 1 << "	I	:	";
                int lib = Lib_for_Inst[net_bot_inst[i][j].first];
                float mid_x = (float)LibCellSize_x[bot][lib] / 2;
                float tmp_x = (float)Pin_x[bot][lib][net_bot_inst[i][j].second] - mid_x;
                out << tmp_x << "	";
                float mid_y = (float)LibCellSize_y[bot][lib] / 2;
                float tmp_y = (float)Pin_y[bot][lib][net_bot_inst[i][j].second] - mid_y;
                out << tmp_y << endl;
            }
            if (NetCross_test[index][i])
                out << "	TER_" << i + 1 << "	I	:	0	0" << endl;
        }
    }
    out.close();
}

void generate_scl(int index, ofstream &out)
{
    /*---------top die---------*/
    out.open("top_die_" + to_string(index) + ".scl");
    out << "UCLA scl 1.0" << endl;
    out << endl;
    out << "NumRows	:	" << RepeatCount_t << endl;
    out << endl;
    for (int i = 0; i < RepeatCount_t; i++)
    {
        out << "CoreRow Horizontal" << endl;
        out << "	Coordinate	:	" << RowHeight_t * i << endl;
        out << "	Height	:	" << RowHeight_t << endl;
        out << "	Sitewidth	:	1" << endl;
        out << "	Sitespacing	:	1" << endl;
        out << "	Siteorient	:	1" << endl;
        out << "	SubrowOrigin	:	0	NumSites	:	" << RowLength_t << endl;
        out << "End" << endl;
    }
    out.close();
    /*---------bot die---------*/
    out.open("bot_die_" + to_string(index) + ".scl");
    out << "UCLA scl 1.0" << endl;
    out << endl;
    out << "NumRows	:	" << RepeatCount_b << endl;
    out << endl;
    for (int i = 0; i < RepeatCount_b; i++)
    {
        out << "CoreRow Horizontal" << endl;
        out << "	Coordinate	:	" << RowHeight_b * i << endl;
        out << "	Height	:	" << RowHeight_b << endl;
        out << "	Sitewidth	:	1" << endl;
        out << "	Sitespacing	:	1" << endl;
        out << "	Siteorient	:	1" << endl;
        out << "	SubrowOrigin	:	0	NumSites	:	" << RowLength_b << endl;
        out << "End" << endl;
    }
    out.close();
}

void generate_pl(int index, ofstream &out)
{
    /*---------top die---------*/
    out.open("top_die_" + to_string(index) + ".pl");
    out << "UCLA pl 1.0" << endl;
    out << endl;
    for (int i = 0; i < cell_in_top_test[index].size(); i++)
    {
        // out << "C" << cell_in_top_test[index][i] + 1 << "	" << inst_position[cell_in_top_test[index][i]].first << "	" << inst_position[cell_in_top_test[index][i]].second << "	:	N" << endl;
        out << "C" << cell_in_top_test[index][i] + 1 << "	" << inst_pos_test[index][cell_in_top_test[index][i]].first << "	" << inst_pos_test[index][cell_in_top_test[index][i]].second << "	:	N" << endl;
    }
    for (int i = 0; i < cutname_test[index].size(); i++)
    {
        out << "TER_" << cutname_test[index][i] + 1 << "	" << term_pos_test[index][i].first << " " << term_pos_test[index][i].second << "	:	N	 /FIXED" << endl;
    }
    out.close();
    /*---------bot die---------*/
    out.open("bot_die_" + to_string(index) + ".pl");
    out << "UCLA pl 1.0" << endl;
    out << endl;
    for (int i = 0; i < cell_in_bottom_test[index].size(); i++)
    {
        // out << "C" << cell_in_bottom_test[index][i] + 1 << "	" << inst_position[cell_in_bottom_test[index][i]].first << "	" << inst_position[cell_in_bottom_test[index][i]].second << "	:	N" << endl;
        out << "C" << cell_in_bottom_test[index][i] + 1 << "	" << inst_pos_test[index][cell_in_bottom_test[index][i]].first << "	" << inst_pos_test[index][cell_in_bottom_test[index][i]].second << "	:	N" << endl;
    }
    for (int i = 0; i < cutname_test[index].size(); i++)
    {
        out << "TER_" << cutname_test[index][i] + 1 << "	" << term_pos_test[index][i].first << " " << term_pos_test[index][i].second << "	:	N	 /FIXED" << endl;
    }
    out.close();
}

void generate_aux(int index, ofstream &out)
{
    out.open("top_die_" + to_string(index) + ".aux");
    out << "RowBasedPlacement : top_die_" + to_string(index) + ".nodes top_die_" + to_string(index) + ".nets top_die_" + to_string(index) + ".wts top_die_" + to_string(index) + ".pl top_die_" + to_string(index) + ".scl " << endl;
    out.close();
    out.open("bot_die_" + to_string(index) + ".aux");
    out << "RowBasedPlacement : bot_die_" + to_string(index) + ".nodes bot_die_" + to_string(index) + ".nets bot_die_" + to_string(index) + ".wts bot_die_" + to_string(index) + ".pl bot_die_" + to_string(index) + ".scl " << endl;
    out.close();
}

void generate_wts(int index, ofstream &out)
{
    out.open("top_die_" + to_string(index) + ".wts");
    out << "UCLA wts 1.0" << endl;
    out.close();
    out.open("bot_die_" + to_string(index) + ".wts");
    out << "UCLA wts 1.0" << endl;
    out.close();
}

void except_pl(int index, ofstream &out)
{
    generate_nodes(index, out);
    generate_nets(index, out);
    generate_scl(index, out);
    generate_aux(index, out);
    generate_wts(index, out);
}

void parse_top(int index)
{
    string tmp_str;
    int tmp;
    ifstream in_top;
    string top_file_name = "top_die_" + to_string(index) + "_final.def";
    in_top.open(top_file_name);
    int ccnt = -1;
    int flag = 0;
    for (int j = 0; j < 10; j++)
    {
        j--;
        in_top >> tmp_str;
        if (tmp_str == "COMPONENTS")
        {
            in_top >> ccnt;
            in_top >> tmp_str;
            for (int i = 0; i < ccnt; i++)
            {
                string cname;
                flag = 0;
                int x;
                int y;
                for (int k = 0; k < 10; k++)
                {
                    k--;
                    in_top >> tmp_str;
                    if (tmp_str == "-")
                    {
                        in_top >> cname;
                        /// cheat
                        if (cname[0] == 'C')
                        {
                            // int mcsize = cname.size();
                            // int t = 0;
                            // for (int i = 1; i < mcsize; i++)
                            // {
                            //     t *= 10;
                            //     t += cname[i] - '0';
                            // }
                            flag = 1;
                        }
                        else if (cname[0] == 'T')
                        {
                            // int mcsize = cname.size();
                            // int t = 0;
                            // for (int i = 1; i < mcsize; i++)
                            // {
                            //     t *= 10;
                            //     t += cname[i] - '0';
                            // }
                            // cutname.push_back(t);
                            flag = -1;
                        }
                        /// cheat
                    }
                    if (tmp_str == "(")
                    {
                        in_top >> x;
                        in_top >> y;
                        break;
                    }
                }
                if (flag == 1)
                {
                    int mcsize = cname.size();
                    int t = 0;
                    for (int ku = 1; ku < mcsize; ku++)
                    {
                        t *= 10;
                        t += cname[ku] - '0';
                    }
                    topdiename_test[index].push_back(cname);
                    inst_position[t - 1].first = x;
                    inst_position[t - 1].second = y;
                    inst_pos_test[index][t - 1].first = x;
                    inst_pos_test[index][t - 1].second = y;
                }
            }
            break;
        }
    }
    in_top.close();
}
void parse_bottom(int index)
{
    string tmp_str;
    int tmp;
    ifstream in_bot;
    string bot_file_name = "bot_die_" + to_string(index) + "_final.def";
    in_bot.open(bot_file_name);
    int ccnt = -1;
    int flag = 0;
    for (int j = 0; j < 10; j++)
    {
        j--;
        in_bot >> tmp_str;
        if (tmp_str == "COMPONENTS")
        {
            in_bot >> ccnt;
            in_bot >> tmp_str;
            for (int i = 0; i < ccnt; i++)
            {
                string cname;
                flag = 0;
                int x;
                int y;
                for (int k = 0; k < 10; k++)
                {
                    k--;
                    in_bot >> tmp_str;
                    if (tmp_str == "-")
                    {
                        in_bot >> cname;
                        /// cheat
                        if (cname[0] == 'C')
                        {
                            // int mcsize = cname.size();
                            // int t = 0;
                            // for (int i = 1; i < mcsize; i++)
                            // {
                            //     t *= 10;
                            //     t += cname[i] - '0';
                            // }
                            flag = 1;
                        }
                        else
                        {
                            flag = -1;
                        }

                        /// cheat
                    }
                    if (tmp_str == "(")
                    {
                        in_bot >> x;
                        in_bot >> y;
                        break;
                    }
                }
                if (flag == 1)
                {
                    int mcsize = cname.size();
                    int t = 0;
                    for (int ku = 1; ku < mcsize; ku++)
                    {
                        t *= 10;
                        t += cname[ku] - '0';
                    }
                    botdiename_test[index].push_back(cname);
                    inst_position[t - 1].first = x;
                    inst_position[t - 1].second = y;
                    inst_pos_test[index][t - 1].first = x;
                    inst_pos_test[index][t - 1].second = y;
                }
            }
            break;
        }
    }
    in_bot.close();
}
void parse_def(int index)
{
    // for(int i = 0; i<Instance_cnt; i++){
    //     partition_map.push_back(-1);
    // }
    cout << "parse index : " << index << " def start" << endl;
    topdiename_test[index].clear();
    topdiename_test[index].shrink_to_fit();
    botdiename_test[index].clear();
    botdiename_test[index].shrink_to_fit();
    cout << "parse top : " << index << " def start" << endl;
    parse_top(index);
    cout << "parse top : " << index << " def done" << endl;
    cout << "parse bottom : " << index << "  def start" << endl;
    parse_bottom(index);
    cout << "parse bottom : " << index << "  def done" << endl;
    cout << "parse def : " << index << "  done" << endl;
}
void parse_pl(int pos, int index)
{
    // pos==0 -> top, pos==1 -> bottom
    ifstream pl_in;
    string filename = (pos == 0) ? "top_die_" + to_string(index) + ".ntup.pl" : "bot_die_" + to_string(index) + ".ntup.pl";
    // in_file.open(filename);
    pl_in.open(filename);
    if (pl_in.is_open())
        cout << "Open: " << filename << " successfully" << endl;
    else
        cout << "Failed open file: " << filename << endl;
    string tmps;
    int tmpx;
    int tmpy;
    int total_inst = 0;
    if (pos == 0)
    {
        total_inst = cell_in_top_test[index].size();
    }
    else
    {
        total_inst = cell_in_bottom_test[index].size();
    }
    for (int l = 0; l < total_inst;)
    {
        pl_in >> tmps;
        string ttt = tmps;
        if (ttt[0] == 'C')
        {
            l++;
            int inst_idx;
            int mcsize = ttt.size();
            int t = 0;
            for (int i = 1; i < mcsize; i++)
            {
                t *= 10;
                t += ttt[i] - '0';
            }
            inst_idx = t - 1;
            pl_in >> tmpx >> tmpy;
            if (inst_idx >= Instance_cnt || inst_idx < 0)
            {
                cout << "WRONG!! " << inst_idx << endl;
            }
            // inst_position[inst_idx].first = tmpx;
            // inst_position[inst_idx].second = tmpy;
            inst_pos_test[index][inst_idx].first = tmpx;
            inst_pos_test[index][inst_idx].second = tmpy;
        }
    }
    pl_in.close();
}
int stoi_new(string k)
{
    int stringsize = k.size();
    int t = 0;
    for (int i = 1; i < stringsize; i++)
    {
        t *= 10;
        t += k[i] - '0';
    }
    return t;
}
void output(int index, ofstream &out)
{
    int inscnt = topdiename_test[index].size();
    // int inscnt = topdiename.size();
    out << "TopDiePlacement ";
    out << inscnt << endl;
    for (int i = 0; i < inscnt; i++)
    {
        int idx = stoi_new(topdiename_test[index][i]) - 1;
        out << "Inst " << topdiename_test[index][i] << " " << inst_pos_test[index][idx].first << " " << inst_pos_test[index][idx].second << endl;
        // int idx = stoi_new(topdiename[i]) - 1;
        // out_file << "Inst " << topdiename[i] << " " << inst_position[idx].first << " " << inst_position[idx].second << endl;
    }
    out << "BottomDiePlacement ";
    inscnt = botdiename_test[index].size();
    // inscnt = botdiename.size();
    out << inscnt << endl;
    for (int i = 0; i < inscnt; i++)
    {
        int idx = stoi_new(botdiename_test[index][i]) - 1;
        out << "Inst " << botdiename_test[index][i] << " " << inst_pos_test[index][idx].first << " " << inst_pos_test[index][idx].second << endl;
        // int idx = stoi_new(botdiename[i]) - 1;
        // out_file << "Inst " << botdiename[i] << " " << inst_position[idx].first << " " << inst_position[idx].second << endl;
    }

    out << "NumTerminals " << cutname_test[index].size() << endl;
    for (int i = 0; i < cutname_test[index].size(); i++)
    {
        string tname = "N" + to_string(cutname_test[index][i] + 1);
        out << "Terminal " << tname << " " << term_pos_test[index][i].first << " " << term_pos_test[index][i].second << endl;
    }
    out << flush;
}
bool sortbydegree(int a, int b)
{
    if (degree_net[a] == degree_net[b])
        return a < b;
    return degree_net[a] > degree_net[b];
}
void terminal_placement(int index)
{
    for (int i = 0; i < Instance_cnt; i++)
    {
        int libnum = Lib_for_Inst[i];
        int sizee;
        if (NumofTech == 2)
        {
            if (partition_map_test[index][i] == 0)
            {
                sizee = LibCellSize[0][libnum];
            }
            else
            {
                sizee = LibCellSize[1][libnum];
            }
        }
        else
        {
            sizee = LibCellSize[0][libnum];
        }
        inst_size[i] = sizee;
    }
    for (int i = 0; i < Instance_cnt; i++)
        degree.push_back(0);
    vector<int> mapforterminal;

    for (int i = 0; i < NetCnt; i++)
    {
        mapforterminal.push_back(0);
        for (int j = 0; j < net[i].size(); j++)
        {
            int instindex = net[i][j].first;
            degree[instindex]++;
        }
        degree_net.push_back(0);
    }
    for (int i = 0; i < NetCnt; i++)
    {
        for (int j = 0; j < net[i].size(); j++)
        {
            int instindex = net[i][j].first;
            degree_net[i] += inst_size[instindex];
        }
    }
    vector<int> terminal_sorted_by_pin;
    vector<int> tmp;
    vector<pair<int, int>> tmp_p;
    for (int i = 0; i < NetCnt; i++)
    {
        int first = partition_map_test[index][net[i][0].first];
        for (int j = 0; j < net[i].size(); j++)
        {
            if (first != partition_map_test[index][net[i][j].first])
            {
                tmp.push_back(i);
                terminal_sorted_by_pin.push_back(i);
                mapforterminal[i] = cut_num_test[index];
                tmp_p.push_back(make_pair(0, 0));
                NetCross_test[index][i] = true;
                cut_num_test[index]++;
                break;
            }
        }
    }
    cutname_test[index].assign(tmp.begin(), tmp.end());
    term_pos_test[index].assign(tmp_p.begin(), tmp_p.end());

    sort(terminal_sorted_by_pin.begin(), terminal_sorted_by_pin.end(), sortbydegree);
    int gridx = (UpperRightX - Terminal_Spacing) / (Terminal_X + Terminal_Spacing);
    int gridy = (UpperRighty - Terminal_Spacing) / (Terminal_Y + Terminal_Spacing);
    terminal_job = gridx * gridy;
    int centerx = (gridx - 1) / 2;
    int centery = (gridy - 1) / 2;
    int pos = centery * gridx + centerx;
    int cnt = 0;
    int max = (gridx * gridy) / (cut_num_test[index] + 1);
    int portion = max * 1 / 5;
    for (int i = 0; i < terminal_sorted_by_pin.size(); i++)
    {
        int t_index = mapforterminal[terminal_sorted_by_pin[i]];
        int coordinate_x = (cnt * portion) % gridx;
        int coordinate_y = (cnt * portion) / gridx;
        int realx = Terminal_Spacing + (Terminal_X + Terminal_Spacing) * coordinate_x + Terminal_X / 2;
        int realy = Terminal_Spacing + (Terminal_Y + Terminal_Spacing) * coordinate_y + Terminal_Y / 2;
        term_pos_test[index][t_index].first = realx;
        term_pos_test[index][t_index].second = realy;
        if (realx <= 0 || realy <= 0)
            cout << cnt << " " << coordinate_x << endl;
        cnt++;
    }
}
void eval(const char *arg1, const char *arg2, int index)
{

    cout << arg1 << " " << arg2 << endl;

    string sin(arg1), sout(arg2);

    string cmd;

    string outfname = sin.substr(0, sin.size() - 4);

    outfname = outfname + "_" + to_string(index) + "_eval.txt";

    cmd = "./Eval/evaluator " + sin + " " + sout + " >" + outfname;
    cout << "KEKWPUSSY" << endl;
    system(cmd.c_str());
    cout << "KEKWPUSSY" << endl;
}

void clean()
{
    // system("rm top_die_final.def bot_die_final.def");
}
void check_boundary(int index)
{
    int cnt = 0;
    vector<int> hi;
    for (int i = 0; i < cutname_test[index].size(); i++)
    {
        int netindex = cutname_test[index][i];
        int up = 0;
        int down = 9999999;
        int left = 99999999;

        int right = 0;
        for (int j = 0; j < net[netindex].size(); j++)
        {
            int inst_index = net[netindex][i].first;
            int x = inst_pos_test[index][inst_index].first;
            int y = inst_pos_test[index][inst_index].second;
            if (x < left)
            {
                left = x;
            }
            if (x > right)
            {
                right = x;
            }
            if (y < down)
            {
                down = y;
            }
            if (y > up)
            {
                up = y;
            }
        }
        int tx = term_pos_test[index][i].first;
        int ty = term_pos_test[index][i].second;
        if (tx < left || tx > right || ty < down || ty > up)
        {
            cnt++;
            hi.push_back(i);
        }
    }
    cout << "There are " << cutname_test[index].size() << " Terminals in total" << endl;
    cout << "There are " << cnt << " Terminals decide HPWL" << endl;
    cout << endl;
}
int calc_HPWL(int netindex, int tx, int ty, bool c, int index)
{
    if (!c)
    {
        int up = 0;
        int down = 9999999;
        int left = 99999999;
        int right = 0;
        for (int j = 0; j < net[netindex].size(); j++)
        {
            int pin_index = net[netindex][j].second;
            int inst_index = net[netindex][j].first;
            // int x = inst_position[inst_index].first;
            // int y = inst_position[inst_index].second;
            int x = inst_pos_test[index][inst_index].first;
            int y = inst_pos_test[index][inst_index].second;
            int tech = 0;
            int lib = Lib_for_Inst[inst_index];
            if (NumofTech == 2)
            {
                tech = partition_map_test[index][inst_index];
            }
            else
            {
                tech = 0;
            }
            x += Pin_x[tech][lib][pin_index];
            y += Pin_x[tech][lib][pin_index];
            if (x < left)
            {
                left = x;
            }
            if (x > right)
            {
                right = x;
            }
            if (y < down)
            {
                down = y;
            }
            if (y > up)
            {
                up = y;
            }
        }

        return (up - down + right - left);
    }
    else
    {
        int res = 0;
        for (int i = 0; i < 2; i++)
        {
            int up = 0;
            int down = 9999999;
            int left = 99999999;
            int right = 0;
            for (int j = 0; j < net[netindex].size(); j++)
            {
                if (partition_map[j] == i)
                {
                    int pin_index = net[netindex][j].second;
                    int inst_index = net[netindex][j].first;
                    int x = inst_pos_test[index][inst_index].first;
                    int y = inst_pos_test[index][inst_index].second;
                    // int x = inst_position[inst_index].first;
                    // int y = inst_position[inst_index].second;
                    int tech = 0;
                    int lib = Lib_for_Inst[inst_index];
                    if (NumofTech == 2)
                    {
                        tech = partition_map_test[index][inst_index];
                    }
                    else
                    {
                        tech = 0;
                    }
                    x += Pin_x[tech][lib][pin_index];
                    y += Pin_x[tech][lib][pin_index];
                    if (x < left)
                    {
                        left = x;
                    }
                    if (x > right)
                    {
                        right = x;
                    }
                    if (y < down)
                    {
                        down = y;
                    }
                    if (y > up)
                    {
                        up = y;
                    }
                }
                if (tx < left)
                {
                    left = tx;
                }
                if (tx > right)
                {
                    right = tx;
                }
                if (ty < down)
                {
                    down = ty;
                }
                if (ty > up)
                {
                    up = ty;
                }
            }
            res += (up - down + right - left);
        }
        return res;
    }
}

void count(int index)
{
    long long first = 0;
    long long second = 0;
    int inst_first = 0;
    int inst_second = 0;
    for (int i = 0; i < NetCnt; i++)
    {
        if (NetCross_test[index][i])
        {
        }
        else
        {
            second += calc_HPWL(i, 0, 0, false, index);
            inst_second += net[i].size();
        }
    }
    for (int i = 0; i < cutname_test[index].size(); i++)
    {
        int netidx = cutname_test[index][i];
        first += calc_HPWL(netidx, term_pos_test[index][i].first, term_pos_test[index][i].second, true, index);
        inst_first += net[netidx].size();
    }
    cout << "Net with cut has " << inst_first << " instances, HPWL: " << first << endl;
    cout << "Net without cut has " << inst_second << " instances, HPWL: " << second << endl;
}
void replace_terminal(int index)
{
    vector<bool> bool_map;
    for (int i = 0; i < terminal_job; i++)
        bool_map.push_back(false);
    // create map for maximum bipartite matching
    int first = 0;
    int second = 0;
    for (int i = 0; i < cut_num_test[index]; i++)
    {

        int netidx = cutname_test[index][i];
        int tmp = calc_HPWL(netidx, term_pos_test[index][i].first, term_pos_test[index][i].second, true, index);
        first += tmp;
        int gridx = (UpperRightX - Terminal_Spacing) / (Terminal_X + Terminal_Spacing);
        int mini = 9999999;
        int target = -1;
        int best_x;
        int best_y;
        for (int j = 0; j < terminal_job; j++)
        {

            int coordinate_x = j % gridx;
            int coordinate_y = j / gridx;
            int realx = Terminal_Spacing + (Terminal_X + Terminal_Spacing) * coordinate_x + Terminal_X / 2;
            int realy = Terminal_Spacing + (Terminal_Y + Terminal_Spacing) * coordinate_y + Terminal_Y / 2;
            int weight = calc_HPWL(netidx, realx, realy, true, index);
            if (weight < mini && !bool_map[j])
            {
                mini = weight;
                target = j;
                best_x = realx;
                best_y = realy;
            }
        }
        second += mini;
        bool_map[target] = true;
        term_pos_test[index][i].first = best_x;
        term_pos_test[index][i].second = best_y;
    }
    cout << "First : " << first << " Second : " << second << endl;
    count(index);
}

long long parse_eval(string filename)
{
    ifstream in_eval;
    in_eval.open(filename);
    if (in_eval.is_open())
        cout << "Open: " << filename << " successfully" << endl;
    else
        cout << "Failed open file: " << filename << endl;
    string tmps;
    long long tmp;
    for (int i = 0; i < 10; i++)
    {
        i--;
        in_eval >> tmps;
        if (tmps == "design")
        {
            in_eval >> tmps;
            in_eval >> tmp;
            break;
        }
    }
    in_eval.close();
    return tmp;
}

void *perform_replace(void *arg)
{
    char cmd[150] = {
        0,
    };
    system("pwd");
    cout << "perform_replace 1" << endl;
    // system("cp top_die.def top_die.lef bot_die.def bot_die.lef ./RunReplace");
    // char cmd[50] = "cd ./RunReplace";
    // system(cmd);
    // system("chmod +x *");
    // system("pwd");
    // system("ls -l");
    Para *data = (Para *)arg;
    string command = "";
    int index = data->index;
    string thread_dir = "./test_" + to_string(data->index);
    command = "mkdir " + thread_dir;
    system(command.c_str());
    string thread_file = "./test_" + to_string(data->index) + "/RunReplace";
    command = "mkdir " + thread_file;
    system(command.c_str());
    system("pwd");
    string sin(data->data1), sout(data->data2);
    string filename = sin.substr(0, sin.size() - 4);
    string dirname = "/area_tune_density_";
    string dir = thread_dir + dirname + filename + "_" + to_string(data->index);
    command = "mkdir " + dir;
    system(command.c_str());
    dir = thread_dir + dirname + filename + "_" + to_string(data->index) + "/run1";
    command = "mkdir " + dir;
    system(command.c_str());
    for (int i = 0; i < Instance_cnt; i++)
        best_inst_test[index].push_back(make_pair(0, 0));
    for (int i = 0; i < cutname_test[index].size(); i++)
        best_term_test[index].push_back(make_pair(0, 0));
    for (int i = 1; i < 6; i++)
    {
        ////    generate def with new terminal position
        int ind = data->index;
        cout << "current index is " << ind << endl;
        switch (ind)
        {
        case 0:
            cout << "out 0 start" << endl;
            out_0.open("bot_die_0.def");
            generate_def_version(out_0);
            generate_bottom_def(ind, out_0);
            out_0.close();
            out_0.open("top_die_0.def");
            generate_def_version(out_0);
            generate_top_def(ind, out_0);
            out_0.close();
            cout << "out 0 done" << endl;
            break;
        case 1:
            cout << "out 1 start" << endl;
            out_1.open("bot_die_1.def");
            generate_def_version(out_1);
            generate_bottom_def(ind, out_1);
            out_1.close();
            out_1.open("top_die_1.def");
            generate_def_version(out_1);
            generate_top_def(ind, out_1);
            out_1.close();
            cout << "out 1 done" << endl;
            break;
        case 2:
            cout << "out 2 start" << endl;
            out_2.open("bot_die_2.def");
            generate_def_version(out_2);
            generate_bottom_def(ind, out_2);
            out_2.close();
            out_2.open("top_die_2.def");
            generate_def_version(out_2);
            generate_top_def(ind, out_2);
            out_2.close();
            cout << "out 2 done" << endl;
            break;
        case 3:
            cout << "out 3 start" << endl;
            out_3.open("bot_die_3.def");
            generate_def_version(out_3);
            generate_bottom_def(ind, out_3);
            out_3.close();
            out_3.open("top_die_3.def");
            generate_def_version(out_3);
            generate_top_def(ind, out_3);
            out_3.close();
            cout << "out 3 done" << endl;
            break;
        case 4:
            cout << "out 4 start" << endl;
            out_4.open("bot_die_4.def");
            generate_def_version(out_4);
            generate_bottom_def(ind, out_4);
            out_4.close();
            out_4.open("top_die_4.def");
            generate_def_version(out_4);
            generate_top_def(ind, out_4);
            out_4.close();
            cout << "out 4 done" << endl;
            break;
        case 5:
            cout << "out 5 start" << endl;
            out_5.open("bot_die_5.def");
            generate_def_version(out_5);
            generate_bottom_def(ind, out_5);
            out_5.close();
            out_5.open("top_die_5.def");
            generate_def_version(out_5);
            generate_top_def(ind, out_5);
            out_5.close();
            cout << "out 5 done" << endl;
            break;
        case 6:
            cout << "out 6 start" << endl;
            out_6.open("bot_die_6.def");
            generate_def_version(out_6);
            generate_bottom_def(ind, out_6);
            out_6.close();
            out_6.open("top_die_6.def");
            generate_def_version(out_6);
            generate_top_def(ind, out_6);
            out_6.close();
            cout << "out 6 done";
            break;
        case 7:
            cout << "out 7 start" << endl;
            out_7.open("bot_die_7.def");
            generate_def_version(out_7);
            generate_bottom_def(ind, out_7);
            out_7.close();
            out_7.open("top_die_7.def");
            generate_def_version(out_7);
            generate_top_def(ind, out_7);
            out_7.close();
            cout << "out 7 done" << endl;
            break;
        default:
            break;
        }

        ////
        ofstream out_filename;
        command = "mkdir " + dir + "/" + to_string(i);
        system(command.c_str());
        command = "./RunReplace/replace -lef top_die_" + to_string(data->index) + ".lef -def top_die_" + to_string(data->index) + ".def -overflow 0.12 -onlyGP -den 1.0 -output ./test_" + to_string(data->index);
        system(command.c_str());
        command = "cp ./test_" + to_string(data->index) + "/etc/top_die_" + to_string(data->index) + "/experiment000/top_die_" + to_string(data->index) + "_final.def .";
        system(command.c_str());
        command = "rm -rf ./test_" + to_string(data->index) + "/etc";
        system(command.c_str());
        command = "./RunReplace/replace -lef bot_die_" + to_string(data->index) + ".lef -def bot_die_" + to_string(data->index) + ".def -overflow 0.12 -onlyGP -den 1.0 -output ./test_" + to_string(data->index);
        system(command.c_str());
        command = "cp ./test_" + to_string(data->index) + "/etc/bot_die_" + to_string(data->index) + "/experiment000/bot_die_" + to_string(data->index) + "_final.def .";
        system(command.c_str());
        command = "rm -rf ./test_" + to_string(data->index) + "/etc";
        system(command.c_str());
        in_file.close();
        parse_def(data->index);
        cout << " parse def done " << endl;

        switch (ind)
        {
        case 0:
            cout << "index 0 generating bookshelf." << endl;
            except_pl(ind, out_0);
            generate_pl(ind, out_0);
            break;
        case 1:
            cout << "index 1 generating bookshelf." << endl;
            except_pl(ind, out_1);
            generate_pl(ind, out_1);
            break;
        case 2:
            cout << "index 2 generating bookshelf." << endl;
            except_pl(ind, out_2);
            generate_pl(ind, out_2);
            break;
        case 3:
            cout << "index 3 generating bookshelf." << endl;
            except_pl(ind, out_3);
            generate_pl(ind, out_3);
            break;
        case 4:
            cout << "index 4 generating bookshelf." << endl;
            except_pl(ind, out_4);
            generate_pl(ind, out_4);
            break;
        case 5:
            cout << "index 5 generating bookshelf." << endl;
            except_pl(ind, out_5);
            generate_pl(ind, out_5);
            break;
        case 6:
            cout << "index 6 generating bookshelf." << endl;
            except_pl(ind, out_6);
            generate_pl(ind, out_6);
            break;
        case 7:
            cout << "index 7 generating bookshelf." << endl;
            except_pl(ind, out_7);
            generate_pl(ind, out_7);
            break;
        default:
            break;
        }

        cout << "index: " << index << " start to first ntu." << endl;
        command = "./ntuplace3 -aux bot_die_" + to_string(data->index) + ".aux -loadpl bot_die_" + to_string(data->index) + ".pl -util 1 -noglobal > bot_die_" + to_string(data->index) + "_place.txt";
        system(command.c_str());
        command = "./ntuplace3 -aux top_die_" + to_string(data->index) + ".aux -loadpl top_die_" + to_string(data->index) + ".pl -util 1 -noglobal > top_die_" + to_string(data->index) + "_place.txt";
        system(command.c_str());
        parse_pl(0, data->index);
        parse_pl(1, data->index);

        replace_terminal(index);
        string out_file_name = sout.substr(0, sout.size() - 4) + "_" + to_string(data->index) + ".txt";
        out_filename.open(out_file_name);
        if (out_filename.is_open())
            cout << " outfile: " << out_file_name << " done " << endl;
        else
            cout << " outfile: " << out_file_name << " failed " << endl;

        out_filename << unitbuf;
        int inscnt = topdiename_test[index].size();
        out_filename << "TopDiePlacement ";
        out_filename << inscnt << endl;
        for (int i = 0; i < inscnt; i++)
        {
            int idx = stoi_new(topdiename_test[index][i]) - 1;
            out_filename << "Inst " << topdiename_test[index][i] << " " << inst_pos_test[index][idx].first << " " << inst_pos_test[index][idx].second << endl
                         << flush;
        }
        out_filename << "BottomDiePlacement " << flush;
        inscnt = botdiename_test[index].size();
        out_filename << inscnt << endl
                     << flush;
        for (int i = 0; i < inscnt; i++)
        {
            int idx = stoi_new(botdiename_test[index][i]) - 1;
            out_filename << "Inst " << botdiename_test[index][i] << " " << inst_pos_test[index][idx].first << " " << inst_pos_test[index][idx].second << endl
                         << flush;
        }

        out_filename << "NumTerminals " << cutname_test[index].size() << endl
                     << flush;
        for (int i = 0; i < cutname_test[index].size(); i++)
        {
            string tname = "N" + to_string(cutname_test[index][i] + 1);
            out_filename << "Terminal " << tname << " " << term_pos_test[index][i].first << " " << term_pos_test[index][i].second << endl
                         << flush;
        }
        cout << " output done " << endl;
        out_filename.flush();
        out_filename.close();
        eval(data->data1, out_file_name.c_str(), data->index);
        long long hpwl = parse_eval(filename + "_" + to_string(data->index) + "_eval.txt");
        if (hpwl < best_hpwl_test[index])
        {
            best_inst_test[index].assign(inst_pos_test[index].begin(), inst_pos_test[index].end());
            best_term_test[index].assign(term_pos_test[index].begin(), term_pos_test[index].end());
            best_hpwl_test[index] = hpwl;
        }
        cout << "HPWL for replace iteration " << i + 1 << " : " << hpwl << endl;
    }
    inst_pos_test[index].assign(best_inst_test[index].begin(), best_inst_test[index].end());
    term_pos_test[index].assign(best_term_test[index].begin(), best_term_test[index].end());
}

void iterative_ntuplace(int pass, const char *arg1, const char *arg2, int index)
{
    string sin(arg1), sout(arg2);
    string outfname = sin.substr(0, sin.size() - 4);
    outfname = outfname + "_" + to_string(index) + "_eval.txt";
    char cmd[150] = {
        0,
    };
    char date[50] = {
        0,
    };
    sprintf(date, "date +%T");
    for (int i = 0; i < pass; i++)
    {

        cout << "ntuplace pass " << i + 1 << " start. ";
        system(date);
        cout << endl;
        switch (index)
        {
        case 0:
            generate_pl(index, out_0);
            break;
        case 1:
            generate_pl(index, out_1);
            break;
        case 2:
            generate_pl(index, out_2);
            break;
        case 3:
            generate_pl(index, out_3);
            break;
        case 4:
            generate_pl(index, out_4);
            break;
        case 5:
            generate_pl(index, out_5);
            break;
        case 6:
            generate_pl(index, out_6);
            break;
        case 7:
            generate_pl(index, out_7);
            break;
        default:
            break;
        }
        //  not checked
        string command = "./ntuplace3 -aux bot_die_" + to_string(index) + ".aux -loadpl bot_die_" + to_string(index) + ".pl -util 1 -noglobal > bot_die_" + to_string(index) + "_place.txt";
        system(command.c_str());
        command = "./ntuplace3 -aux top_die_" + to_string(index) + ".aux -loadpl top_die_" + to_string(index) + ".pl -util 1 -noglobal > top_die_" + to_string(index) + "_place.txt";
        system(command.c_str());
        parse_pl(0, index);
        parse_pl(1, index);
        replace_terminal(index);
        cout << "ntuplace pass " << i + 1 << " done. ";
        switch (index)
        {
        case 0:
            out_0.open(arg2);
            output(index, out_0);
            out_0.close();
            break;
        case 1:
            out_1.open(arg2);
            output(index, out_1);
            out_1.close();
            break;
        case 2:
            out_2.open(arg2);
            output(index, out_2);
            out_2.close();
            break;
        case 3:
            out_3.open(arg2);
            output(index, out_3);
            out_3.close();
            break;
        case 4:
            out_4.open(arg2);
            output(index, out_4);
            out_4.close();
            break;
        case 5:
            out_5.open(arg2);
            output(index, out_5);
            out_5.close();
            break;
        case 6:
            out_6.open(arg2);
            output(index, out_6);
            out_6.close();
            break;
        case 7:
            out_7.open(arg2);
            output(index, out_7);
            out_7.close();
            break;
        default:
            break;
        }

        eval(arg1, arg2, index);
        long long hpwl = parse_eval(outfname);
        if (hpwl < best_hpwl_test[index])
        {
            best_inst_test[index].assign(inst_pos_test[index].begin(), inst_pos_test[index].end());
            best_term_test[index].assign(term_pos_test[index].begin(), term_pos_test[index].end());
            best_hpwl_test[index] = hpwl;
        }
        cout << "HPWL for ntuplace iteration " << i + 1 << " : " << hpwl << endl;
        system(date);
        cout << endl;
    }
    inst_pos_test[index].assign(best_inst_test[index].begin(), best_inst_test[index].end());
    term_pos_test[index].assign(best_term_test[index].begin(), best_term_test[index].end());
}

void *perform_replace_2(void *arg)
{
    char cmd[150] = {
        0,
    };
    char date[50] = {
        0,
    };
    sprintf(date, "date +%T");
    system("pwd");
    // system("cp top_die.def top_die.lef bot_die.def bot_die.lef ./RunReplace");
    // char cmd[50] = "cd ./RunReplace";
    // system(cmd);
    // system("chmod +x *");
    // system("pwd");
    // system("ls -l");
    Para *data = (Para *)arg;
    string command = "";
    int index = data->index;
    string thread_dir = "./test_" + to_string(data->index);
    string thread_file = "./test_" + to_string(data->index) + "/RunReplace";
    string sin(data->data1), sout(data->data2);
    string filename = sin.substr(0, sin.size() - 4);
    string dirname = "/area_tune_density_";
    string dir = thread_dir + dirname + filename + "_" + to_string(data->index) + "/run2";
    command = "mkdir " + dir;
    system(command.c_str());
    for (double i = 1.0; i <= 1.0; i += 0.1)
    {
        ofstream out_filename;
        command = "mkdir " + dir + "/" + to_string(i);
        system(command.c_str());
        cout << " Start iterative NTUplace ";

        switch (index)
        {
        case 0:
            except_pl(index, out_0);
            generate_pl(index, out_0);
            // pthread_mutex_unlock(&mutex);
            break;
        case 1:
            except_pl(index, out_1);
            generate_pl(index, out_1);
            // pthread_mutex_unlock(&mutex);
            break;
        case 2:
            except_pl(index, out_2);
            generate_pl(index, out_2);
            // pthread_mutex_unlock(&mutex);
            break;
        case 3:
            except_pl(index, out_3);
            generate_pl(index, out_3);
            // pthread_mutex_unlock(&mutex);
            break;
        case 4:
            except_pl(index, out_4);
            generate_pl(index, out_4);
            // pthread_mutex_unlock(&mutex);
            break;
        case 5:
            except_pl(index, out_5);
            generate_pl(index, out_5);
            // pthread_mutex_unlock(&mutex);
            break;
        case 6:
            except_pl(index, out_6);
            generate_pl(index, out_6);
            // pthread_mutex_unlock(&mutex);
            break;
        case 7:
            except_pl(index, out_7);
            generate_pl(index, out_7);
            // pthread_mutex_unlock(&mutex);
            break;
        default:
            // pthread_mutex_unlock(&mutex);
            break;
        }

        system(date);
        cout << endl;
        string out_file_name = sout.substr(0, sout.size() - 4) + "_" + to_string(data->index) + ".txt";
        iterative_ntuplace(3, data->data1, out_file_name.c_str(), data->index);
        cout << "End iterative NTUplace ";
        string bookshelfdir = thread_dir + dirname + filename + "_" + to_string(data->index) + "/bookshelf_files";
        command = "mkdir " + bookshelfdir;
        system(command.c_str());
        command = "mkdir " + bookshelfdir + "/top_die";
        system(command.c_str());
        command = "mv top_die_" + to_string(data->index) + ".aux top_die_" + to_string(data->index) + ".lg.pl top_die_" + to_string(data->index) + ".lg.plt top_die_" + to_string(data->index) + ".nets top_die_" + to_string(data->index) + ".nodes top_die_" + to_string(data->index) + ".ntup.pl top_die_" + to_string(data->index) + ".ntup.plt " + bookshelfdir + "/top_die";
        system(command.c_str());
        command = "mv top_die_" + to_string(data->index) + ".pl top_die_" + to_string(data->index) + ".scl top_die_" + to_string(data->index) + ".wts top_die_" + to_string(data->index) + "_place.txt " + bookshelfdir + "/top_die";
        system(command.c_str());
        command = "mkdir " + bookshelfdir + "/bot_die";
        system(command.c_str());
        command = "mv bot_die_" + to_string(data->index) + ".aux bot_die_" + to_string(data->index) + ".lg.pl bot_die_" + to_string(data->index) + ".lg.plt bot_die_" + to_string(data->index) + ".nets bot_die_" + to_string(data->index) + ".nodes bot_die_" + to_string(data->index) + ".ntup.pl bot_die_" + to_string(data->index) + ".ntup.plt " + bookshelfdir + "/bot_die";
        system(command.c_str());
        command = "mv bot_die_" + to_string(data->index) + ".pl bot_die_" + to_string(data->index) + ".scl bot_die_" + to_string(data->index) + ".wts bot_die_" + to_string(data->index) + "_place.txt " + bookshelfdir + "/bot_die";
        system(command.c_str());
        system(date);
        cout << endl;

        out_filename.open(out_file_name);

        if (out_filename.is_open())
            cout << "Perform_Replace_2 outfile: " << out_file_name << " done " << endl;
        else
            cout << "Perform_Replace_2 outfile: " << out_file_name << " failed " << endl;

        out_filename << unitbuf;
        int inscnt = topdiename_test[index].size();
        out_filename << "TopDiePlacement ";
        out_filename << inscnt << endl;
        for (int i = 0; i < inscnt; i++)
        {
            int idx = stoi_new(topdiename_test[index][i]) - 1;
            out_filename << "Inst " << topdiename_test[index][i] << " " << inst_pos_test[index][idx].first << " " << inst_pos_test[index][idx].second << endl
                         << flush;
        }
        out_filename << "BottomDiePlacement " << flush;
        inscnt = botdiename_test[index].size();
        out_filename << inscnt << endl
                     << flush;
        for (int i = 0; i < inscnt; i++)
        {
            int idx = stoi_new(botdiename_test[index][i]) - 1;
            out_filename << "Inst " << botdiename_test[index][i] << " " << inst_pos_test[index][idx].first << " " << inst_pos_test[index][idx].second << endl
                         << flush;
        }

        out_filename << "NumTerminals " << cutname_test[index].size() << endl
                     << flush;
        for (int i = 0; i < cutname_test[index].size(); i++)
        {
            string tname = "N" + to_string(cutname_test[index][i] + 1);
            out_filename << "Terminal " << tname << " " << term_pos_test[index][i].first << " " << term_pos_test[index][i].second << endl
                         << flush;
        }
        cout << " output done " << endl;
        out_filename.flush();
        out_filename.close();
        eval(data->data1, out_file_name.c_str(), data->index);
        command = "mv " + filename + "_" + to_string(data->index) + "_eval.txt " + sout.substr(0, sout.size() - 4) + "_" + to_string(data->index) + ".txt " + dir + "/" + to_string(i);
        system(command.c_str());
    }
    {
        vector<int>().swap(cell_in_bottom_test[index]);
        vector<int>().swap(cell_in_top_test[index]);
        vector<int>().swap(partition_map_test[index]);
        vector<bool>().swap(NetCross_test[index]);
    }

    // system("rm top_die.def top_die.lef bot_die.def ./bot_die.lef");
    // system("cd ..");
    command = "mv top_die_" + to_string(index) + ".def top_die_" + to_string(index) + ".lef bot_die_" + to_string(index) + ".def bot_die_" + to_string(index) + ".lef " + dir;
    system(command.c_str());
}

int main(int argc, char *argv[])
{
    char cmd[150] = {
        0,
    };
    sprintf(cmd, "date +%T");
    cout << "start execution : ";
    system(cmd);
    cout << endl;
    in_file.open(argv[1]);

    // out_file.open(argv[2]);

    parse_input();

    cout << " parse input done";
    system(cmd);
    cout << endl;
    for (int i = 0; i < Instance_cnt; i++)
    {
        inst_position.push_back(make_pair(0, 0));
    }

    for (int i = 0; i < 8; i++)
    {
        vector<pair<int, int>> inst_tmp;
        for (int j = 0; j < Instance_cnt; j++)
        {
            inst_tmp.push_back(make_pair(0, 0));
        }
        inst_pos_test.push_back(inst_tmp);
    }

    for (int i = 0; i < 8; i++)
    {
        vector<string> tmp;
        vector<int> tmp_i;
        vector<pair<int, int>> tmp_p;
        topdiename_test.push_back(tmp);
        botdiename_test.push_back(tmp);
        cut_num_test.push_back(0);
        cutname_test.push_back(tmp_i);
        term_pos_test.push_back(tmp_p);
        best_hpwl_test.push_back(2147483645);
        best_inst_test.push_back(tmp_p);
        best_term_test.push_back(tmp_p);
    }
    partition();

    cout << " partition done ";
    system(cmd);
    cout << endl;

    legalize();

    cout << " legalize done ";
    system(cmd);
    cout << endl;

    for (int i = 0; i < 8; i++)
        terminal_placement(i);

    cout << " terminal placement done ";
    system(cmd);
    cout << endl;

    in_file.close();

    cout << " infile close done ";
    system(cmd);
    cout << endl;

    in_file.open(argv[1]);

    cout << " infile open done ";
    system(cmd);
    cout << endl;

    generate_lefdef();

    cout << " generate lefdef done ";
    system(cmd);
    cout << endl;

    in_file.close();

    cout << " infile close done ";

    system(cmd);
    cout << endl;

    pthread_t *t = (pthread_t *)malloc(8 * sizeof(pthread_t));
    // pthread_t t0, t1, t2, t3, t4, t5, t6, t7;
    Para *pass0 = new Para;
    pass0->index = 0;
    pass0->data1 = argv[1];
    pass0->data2 = argv[2];
    pthread_create(&t[0], NULL, &perform_replace, (void *)pass0);
    Para *pass1 = new Para;
    pass1->index = 1;
    pass1->data1 = argv[1];
    pass1->data2 = argv[2];
    pthread_create(&t[1], NULL, &perform_replace, (void *)pass1);
    Para *pass2 = new Para;
    pass2->index = 2;
    pass2->data1 = argv[1];
    pass2->data2 = argv[2];
    pthread_create(&t[2], NULL, &perform_replace, (void *)pass2);
    Para *pass3 = new Para;
    pass3->index = 3;
    pass3->data1 = argv[1];
    pass3->data2 = argv[2];
    pthread_create(&t[3], NULL, &perform_replace, (void *)pass3);
    Para *pass4 = new Para;
    pass4->index = 4;
    pass4->data1 = argv[1];
    pass4->data2 = argv[2];
    pthread_create(&t[4], NULL, &perform_replace, (void *)pass4);
    Para *pass5 = new Para;
    pass5->index = 5;
    pass5->data1 = argv[1];
    pass5->data2 = argv[2];
    pthread_create(&t[5], NULL, &perform_replace, (void *)pass5);
    Para *pass6 = new Para;
    pass6->index = 6;
    pass6->data1 = argv[1];
    pass6->data2 = argv[2];
    pthread_create(&t[6], NULL, &perform_replace, (void *)pass6);
    Para *pass7 = new Para;
    pass7->index = 7;
    pass7->data1 = argv[1];
    pass7->data2 = argv[2];
    pthread_create(&t[7], NULL, &perform_replace, (void *)pass7);
    pthread_join(t[0], NULL);
    pthread_join(t[1], NULL);
    pthread_join(t[2], NULL);
    pthread_join(t[3], NULL);
    pthread_join(t[4], NULL);
    pthread_join(t[5], NULL);
    pthread_join(t[6], NULL);
    pthread_join(t[7], NULL);

    cout << " perform replace done ";
    system(cmd);
    cout << endl;

    // pthread_create(&t[0], NULL, &perform_replace_2, (void *)pass0);
    // pthread_create(&t[1], NULL, &perform_replace_2, (void *)pass1);
    // pthread_create(&t[2], NULL, &perform_replace_2, (void *)pass2);
    // pthread_create(&t[3], NULL, &perform_replace_2, (void *)pass3);
    // pthread_create(&t[4], NULL, &perform_replace_2, (void *)pass4);
    // pthread_create(&t[5], NULL, &perform_replace_2, (void *)pass5);
    // pthread_create(&t[6], NULL, &perform_replace_2, (void *)pass6);
    // pthread_create(&t[7], NULL, &perform_replace_2, (void *)pass7);
    // pthread_join(t[0], NULL);
    // pthread_join(t[1], NULL);
    // pthread_join(t[2], NULL);
    // pthread_join(t[3], NULL);
    // pthread_join(t[4], NULL);
    // pthread_join(t[5], NULL);
    // pthread_join(t[6], NULL);
    // pthread_join(t[7], NULL);

    delete pass0;
    delete pass1;
    delete pass2;
    delete pass3;
    delete pass4;
    delete pass5;
    delete pass6;
    delete pass7;
    free(t);

    cout << " perform replace_2 done ";
    system(cmd);
    cout << endl;

    int best_index = -1;
    int best_answer = 2147483645;
    for (int i = 0; i < 8; i++)
    {
        cout << "current index hpwl: " << best_hpwl_test[i] << endl;
        cout << "best answer: " << best_answer << endl;
        if (best_hpwl_test[i] < best_answer)
        {
            best_answer = best_hpwl_test[i];
            best_index = i;
        }
    }

    out_file.open(argv[2]);

    int inscnt = topdiename_test[best_index].size();
    out_file << "TopDiePlacement ";
    out_file << inscnt << endl;
    for (int i = 0; i < inscnt; i++)
    {
        int idx = stoi_new(topdiename_test[best_index][i]) - 1;
        out_file << "Inst " << topdiename_test[best_index][i] << " " << inst_pos_test[best_index][idx].first << " " << inst_pos_test[best_index][idx].second << endl;
    }
    out_file << "BottomDiePlacement ";
    inscnt = botdiename_test[best_index].size();
    // inscnt = botdiename.size();
    out_file << inscnt << endl;
    for (int i = 0; i < inscnt; i++)
    {
        int idx = stoi_new(botdiename_test[best_index][i]) - 1;
        out_file << "Inst " << botdiename_test[best_index][i] << " " << inst_pos_test[best_index][idx].first << " " << inst_pos_test[best_index][idx].second << endl;
    }

    out_file << "NumTerminals " << cutname_test[best_index].size() << endl;
    for (int i = 0; i < cutname_test[best_index].size(); i++)
    {
        string tname = "N" + to_string(cutname_test[best_index][i] + 1);
        out_file << "Terminal " << tname << " " << term_pos_test[best_index][i].first << " " << term_pos_test[best_index][i].second << endl;
    }
    out_file << flush;
    out_file.close();

    {
        vector<vector<pair<int, int>>>().swap(term_pos_test);
        vector<vector<pair<int, int>>>().swap(inst_pos_test);
        vector<vector<string>>().swap(topdiename_test);
        vector<vector<string>>().swap(botdiename_test);
        vector<int>().swap(cut_num_test);
        vector<int>().swap(best_hpwl_test);
        vector<vector<pair<int, int>>>().swap(best_inst_test);
        vector<vector<pair<int, int>>>().swap(best_term_test);
    }

    string command = "rm *.nodes *.wts *.nets *.scl *.pl *.plt *place.txt *_eval.txt *.aux *.def *.lef";
    system(command.c_str());

    for (int i = 0; i < 8; i++)
    {
        command = "rm *out_" + to_string(i) + ".txt";
        system(command.c_str());
        command = "rm -rf test_" + to_string(i);
        system(command.c_str());
    }

    int count1 = 0, count2 = 0;
    for (int i = 0; i < Instance_cnt; i++)
    {
        if (partition_map[i] == 0)
            count1++;
        else
            count2++;
    }
    cout << "Top instance = " << count1 << endl;
    cout << "Bottom instance = " << count2 << endl;
    // check_boundary();
    //  clean();
    return 0;
}
