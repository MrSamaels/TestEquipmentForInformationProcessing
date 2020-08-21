#include "bso_gen.h"

using namespace std;

vector <int> my_R1_ready(

        int regime,
        unsigned long long int my_global_time,
        unsigned long long int& my_time,
        int last_packet,
        int& R1_ID) {

    vector <int> out_data;

    if (regime == 2 && my_global_time > my_time && last_packet) {
        if (R1_ID == 0) {
            my_time = my_global_time;
            out_data.push_back(33);
            out_data.push_back(37);

            out_data.push_back(1); //R1 ready

            int cnt = rand() % 2;

            out_data.push_back(cnt);

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
                out_data.insert(out_data.begin(), (my_time >> i*8) % 256); //time
            }

            out_data.insert(out_data.begin(), size%256); //size
            out_data.insert(out_data.begin(), size/256); //size

            out_data.insert(out_data.begin(), R1_ID); //ID

            out_data.insert(out_data.begin(), 0); //flag 0 == по времени

            out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
            out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
            out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
            out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей

            R1_ID++;
            my_time += size*16;
        }
    }
    return out_data;
}

vector <int> my_R1_gen (

        int regime,
        unsigned long long int& my_time,
        int& R1_ID) {

    vector <int> out_out_data;
    vector <int> out_data;

    if (regime == 2 && R1_ID != 0) {

        for (int i = 0; i < 5270; i++) {
            int cnt = rand() % 2;
            out_data.clear();

            out_data.push_back(33);
            out_data.push_back(37);

            out_data.push_back(3); //R1

            out_data.push_back(cnt*32 + i/256);
            out_data.push_back(i%256);

            for (int j = 0; j < bso_packet_amount; j++) {
                cnt = rand() % 256;
                out_data.push_back(cnt);
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
                out_data.insert(out_data.begin(), (my_time >> i*8) % 256); //time
            }

            out_data.insert(out_data.begin(), size%256); //size
            out_data.insert(out_data.begin(), size/256); //size

            out_data.insert(out_data.begin(), R1_ID); //ID

            out_data.insert(out_data.begin(), 1); //flag 1 == по запросу

            out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
            out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
            out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
            out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей

            my_time += size*16;

            Push_vector(out_data, out_out_data);
        }
        R1_ID++;
        if (R1_ID == 2) R1_ID = 0;
    }
    return out_out_data;
}

vector <int> my_MO_gen(

        int regime,
        unsigned long long int& my_time,
        int& cur_packet_MO,
        int& MO_flag) {

    vector <int> out_data;
//    out_data.clear();

    int prior = rand() % 3;

    if ((regime > 1)&&(prior == 0)&&(MO_flag < 1)) {
        out_data.push_back(33);
        out_data.push_back(37);

        out_data.push_back(0); //MO

        out_data.push_back(cur_packet_MO); //номер пакета - по порядку

        for (int i = 0; i < bso_packet_amount; i++) {
            int cnt = rand() % 256;
            out_data.push_back(cnt);
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
            out_data.insert(out_data.begin(), (my_time >> i*8) % 256); //time
        }

        out_data.insert(out_data.begin(), size%256); //size
        out_data.insert(out_data.begin(), size/256); //size

        out_data.insert(out_data.begin(), 0); //ID

        out_data.insert(out_data.begin(), 0); //flag 0 == по времени

        out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
        out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
        out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
        out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей

        cur_packet_MO++;
        my_time += size*16;
        if (cur_packet_MO == 9) {
            cur_packet_MO = 0;
            MO_flag += 1;
        }

    }
    return out_data;
}
