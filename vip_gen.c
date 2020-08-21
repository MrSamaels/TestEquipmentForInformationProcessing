#include "vip_gen.h"

using namespace std;

vector <int> Vip_Generation(

        unsigned long long int& my_time,
        int regime,
        int& frame_changed_flag,
        int& new_fragment_flag,
        int& telemetry_flag) {

        vector <int> out_data;

        int prior;
        prior = rand() % 2;

        out_data.push_back(prior*128+33);
        out_data.push_back(32);

        recomand:
        int comand;
        comand = 6 + 2 * (rand() % 7);
        if (comand == 14) comand = 7;
        if (comand == 12) comand = 10;
        if (comand == 16) comand = 6;
        if (comand == 18) comand = 10;
        if (comand == 6) {
            if (frame_changed_flag != 0 || rand()%10 != 0) goto recomand;
            else frame_changed_flag = 1;
        }

        if (comand == 10) {
            if (new_fragment_flag || regime < 2)
                goto recomand;
            else {
                new_fragment_flag = 1;
            }
        }

        if (comand == 7) {
            if (telemetry_flag > 0)
                goto recomand;
            else {
                telemetry_flag++;
            }
        }

        out_data.push_back(comand);

        switch(comand)
        {
                case 6:
                        int extract_type, work_type;
                        extract_type = rand() % 2;
                        work_type = rand() % 4;

                        out_data.push_back(4*extract_type + work_type);
                        break;

                case 7:
                        int reserve_type, packet_num, cnt, i;
                        reserve_type = rand() % 2;
                        packet_num = rand() % 8;

                        out_data.push_back(reserve_type*128 + packet_num);
                        for (i = 0; i < telemetry_packet_amount; ++i) {
                                cnt = rand() % 256;
                                out_data.push_back(cnt);
                        }
                        break;

                case 8: break;

                case 10:
                        int BPO_adrr, priority;
                        int	block_num0, line_num0_12_8, line_num0_7_0;
                        int	block_num1, line_num1_12_8, line_num1_7_0;

                        for (i = 3; i < 27; ++i) {
                                priority = rand() % 2;
                                BPO_adrr = 40 + (rand() % 24);

                                out_data.push_back(priority*128 + BPO_adrr);
                        }

                        block_num0 = rand() & 1;
                        invalid_num0:
                        line_num0_12_8 = rand() % 32;
                        line_num0_7_0 = rand() %256;

                        if ((line_num0_12_8 << 8) + line_num0_7_0 > 6667) goto invalid_num0;
                        out_data.push_back(block_num0*32+line_num0_12_8);
                        out_data.push_back(line_num0_7_0);

                        invalid_num1:
                        block_num1 = rand() % 2;
                        line_num1_12_8 = rand() % 32;
                        line_num1_7_0 = rand() % 256;

                        if ((line_num1_12_8 << 8) + line_num1_7_0 > 6667) goto invalid_num1;

                        out_data.push_back(block_num1*32+line_num1_12_8);
                        out_data.push_back(line_num1_7_0);
                        break;

                default:
                        int q_size, rand_cnt;

                        q_size = rand() % 256;
                        for (i = 0; i < q_size; ++i) {
                                rand_cnt = rand() % 256;
                                out_data.push_back(rand_cnt);
                        }
                        break;
        }

    cm_t cm;
    p_cm_t p_cm = &cm;
    p_cm->cm_width = 16;
    p_cm->cm_poly = 0x8005L;
    p_cm->cm_init = 0L;
    p_cm->cm_refin = FALSE;
    p_cm->cm_refot = FALSE;
    p_cm->cm_xorot = 0L;

    cm_ini(p_cm);
    for (int i = 0; i < out_data.size(); i++) cm_nxt(p_cm, out_data[i]);

    int crc = cm_crc(p_cm);
    int buf_val;
    buf_val = crc % 256;
    crc = crc / 256;
    out_data.push_back(crc);
    out_data.push_back(buf_val);

    int size = out_data.size(); 

    if (out_data.size() % 8 > 0) {
        int auto_space = 8 - (out_data.size() % 8);
        for (int i = 0; i < auto_space; i++)
             out_data.push_back(0);
    }

    for (int i = 0; i < 8; i++) {
        out_data.insert(out_data.begin(), (my_time >> i*8) % 256);
    }

    out_data.insert(out_data.begin(), size%256);
    out_data.insert(out_data.begin(), size/256);

    out_data.insert(out_data.begin(), regime); //ID

    out_data.insert(out_data.begin(), 0); //flag 0 == по времени

    out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
    out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
    out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
    out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей

    my_time += size*16;
    return out_data;
}
