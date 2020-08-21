#include "main.h"
#include "bpo_gen.h"
#include "bso_gen.h"
#include "vip_gen.h"

using namespace std;

int main(int argc, char **argv) {
    string path = argv[1];
    int cur_file = 0;

    int regime = argv[2], change_regime_flag = 0;
    int buf_regime = regime;

    unsigned long long int my_global_time = 0;
    unsigned long long int my_time_vid = 0;
    unsigned long long int my_time_MO = 0;
    unsigned long long int my_time_R1 = 0;

    unsigned long long int video_data_size;

    int last_packet = 0;
    int cur_packet_MO = 0;
    int telemetry_flag = 0;
    int bpo_index = 0, cur_packet = 0;
    int R1_ID = 0;
    unsigned long long int frame_start = 0;
    int frame_changed_flag = 0;
    int new_fragment_flag = 0;
    int MO_flag = 0;
    int packet_counter = 0;
    int compress_packet = 0;

    vector <int> start_pos, BPO_mas;
    start_pos.clear();
    BPO_mas.clear();

    vector <int> vip_data;
    vector <int> video_data;
    vector <int> bso_data_MO;
    vector <int> bso_data_R1;
    vector <int> control_data;

        if (regime == 0 && my_global_time > frame_start + time_regime_0) {
            if (regime != buf_regime) {
                regime = buf_regime;
                change_regime_flag = 1;
                my_time_vid = frame_start + time_regime_0;
                my_time_R1 = frame_start + time_regime_0;
            }
            else change_regime_flag = 0;
            MO_flag = 0;
            new_fragment_flag = 0;
            telemetry_flag = 0;
            frame_start += time_regime_0;
            my_time_MO = my_global_time;
            frame_changed_flag = 0;
        }
        if (regime == 1 && my_global_time > frame_start + time_regime_1) {
            if (regime != buf_regime) {
                regime = buf_regime;
                change_regime_flag = 1;
                my_time_vid = frame_start + time_regime_1;
                my_time_R1 = frame_start + time_regime_1;
            }
            else change_regime_flag = 0;
            MO_flag = 0;
            new_fragment_flag = 0;
            telemetry_flag = 0;
            frame_start += time_regime_1;
            my_time_MO = my_global_time;
            frame_changed_flag = 0;
        }
        if (regime >  1 && my_global_time > frame_start + time_regime_2) {
            if (regime != buf_regime) {
                regime = buf_regime;
                change_regime_flag = 1;
                my_time_vid = frame_start + time_regime_2;
                my_time_R1 = frame_start + time_regime_2;
            }
            else change_regime_flag = 0;
            MO_flag = 0;
            new_fragment_flag = 0;
            telemetry_flag = 0;
            frame_start += time_regime_2;
            my_time_MO = my_global_time;
            frame_changed_flag = 0;
        }
        ofstream os_bso_MO;
        os_bso_MO.open(path+"\\bso_MO", ofstream::app | ofstream::binary);
        ofstream os_bso_R1;
        os_bso_R1.open(path+"\\bso_R1", ofstream::app | ofstream::binary);
        ofstream os_vip;
        os_vip.open(path+"\\vip", ofstream::app | ofstream::binary);
        ofstream os_control;
        os_control.open(path+"\\control", ofstream::app | ofstream::binary);

        vip_data.clear();
        video_data.clear();
        bso_data_MO.clear();
        bso_data_R1.clear();
        control_data.clear();

        int cnt = rand() % 100;
        if (cnt == 0)
            vip_data = Vip_Generation(my_global_time,
                                      regime,
                                      frame_changed_flag,
                                      new_fragment_flag,
                                      telemetry_flag);
        if ((vip_data.empty() != 1)&&(vip_data[2+reserved_bytes] == 6)) {
            control_data.clear();
            buf_regime = vip_data[3+reserved_bytes] - (vip_data[3+reserved_bytes]>3)*4;
            control_data.push_back(buf_regime);
            int size = control_data.size();
             control_data.clear();
            for (int i = 0; i < reserved_bytes; i++)
                control_data.push_back(vip_data[i]);
            control_data[7] = size%256;
            control_data[6] = size/256;
            control_data.push_back(buf_regime);
            if (control_data.size() % 8 > 0) {
                int auto_space = 8 - (control_data.size() % 8);
                for (int i = 0; i < auto_space; i++)
                     control_data.push_back(0);
            }
        }

        video_data_size = My_Video_Gen(path,
                                       change_regime_flag,
                                       vip_data,
                                       regime,
                                       buf_regime,
                                       frame_start,
                                       my_global_time,
                                       my_time_vid,
                                       bpo_index,
                                       start_pos,
                                       BPO_mas,
                                       cur_packet,
                                       last_packet,
                                       packet_counter);

        bso_data_R1 = my_R1_ready(regime,
                                  my_time_vid,
                                  my_time_R1,
                                  last_packet,
                                  R1_ID);
        if (bso_data_R1.empty() != 1) copy(bso_data_R1.begin(), bso_data_R1.end(), ostream_iterator<char>(os_bso_R1));

        bso_data_R1 = my_R1_gen(regime,
                                my_time_R1,
                                R1_ID);
        if (bso_data_R1.empty() != 1) copy(bso_data_R1.begin(), bso_data_R1.end(), ostream_iterator<char>(os_bso_R1));

        bso_data_MO = my_MO_gen(regime,
                                my_time_MO,
                                cur_packet_MO,
                                MO_flag);

        if (vip_data.empty() != 1) copy(vip_data.begin(), vip_data.end(), ostream_iterator<char>(os_vip));
        if (bso_data_MO.empty() != 1) copy(bso_data_MO.begin(), bso_data_MO.end(), ostream_iterator<char>(os_bso_MO));
        if (control_data.empty() != 1) copy(control_data.begin(), control_data.end(), ostream_iterator<char>(os_control));

        my_global_time += /*1200000*/100;

        if(regime == 0) my_global_time += 1200000;

        int a = 257;
        int b = (a >> 2) << 2;

	if((os_bso_MO.tellp()+os_bso_R1.tellp()+os_vip.tellp()+video_data_size) > (4294967296)) {
            if (cur_file == 0) path = "C:\\QT\\Projects\\second";
            if (cur_file == 1) path = "C:\\QT\\Projects\\third";
            if (cur_file == 2) path = "C:\\QT\\Projects\\forth";
            if (cur_file == 3) path = "C:\\QT\\Projects\\fifth";
            if (cur_file == 4) path = "C:\\QT\\Projects\\sexth";
            if (cur_file == 5) break;

            cur_file++;
        }
        os_bso_MO.close();
        os_bso_R1.close();
        os_vip.close();
        }
    return 0;
}
