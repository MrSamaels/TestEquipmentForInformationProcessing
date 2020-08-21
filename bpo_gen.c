#include <bpo_gen.h>

using namespace std;

unsigned long long int My_Video_Gen(

        string path,
        int change_regime_flag,
        vector <int> in_data,
        int regime,
        int buf_regime,
        unsigned long long int frame_start,
        unsigned long long int my_global_time,
        unsigned long long int& my_time,
        int& bpo_index,
        vector <int>& start_pos,
        vector <int>& BPO_mas,
        int& cur_packet,
        int& last_packet,
        int& packet_counter,
        int& compress_packet,
        int& compress_packet_number) {

    vector <int> out_data;
    unsigned long long int out_value;
    int compress_packet_number = 0;
    ofstream os_vid;
    os_vid.open(path+"\\vid", ofstream::app | ofstream::binary);

    if (change_regime_flag == 1) {
        BPO_mas.clear();
        start_pos.clear();
        cur_packet = 0;
        bpo_index = 0;
        compress_packet_number = 0;
    }

    if ((in_data.empty() != 1)&&(in_data[2+reserved_bytes] == 10)) {
        if (regime == 3) {
            for (int bpo_add = 3; bpo_add < 27; bpo_add++) {
                int prior = rand() % 2;

                int k = in_data[bpo_add+reserved_bytes] - _out(in_data, bpo_add+reserved_bytes, 7)*128;
                BPO_mas.push_back(prior*128 + k);
            }
        }
        else if (regime == 2) {
            int prior = rand() % 2;
            int k = in_data[3+reserved_bytes] - _out(in_data, 3+reserved_bytes, 7)*128;
            BPO_mas.push_back(prior*128 + k);
            start_pos.push_back(((in_data[27+reserved_bytes] - _out(in_data, 27+reserved_bytes, 5)*32)*256)+in_data[28+reserved_bytes]);
            start_pos.push_back(((in_data[29+reserved_bytes] - _out(in_data, 29+reserved_bytes, 5)*32)*256)+in_data[30+reserved_bytes]);
            if (cur_packet == 0 && bpo_index == 0) {
                cur_packet = start_pos[0]*4;
                my_time += start_pos[0]*328*14*4;
            }
        }
    }
    int buf_val_0, buf_val_1;
    int crc;
    int buf_val;
    cm_t cm;
    p_cm_t p_cm = &cm;

    if ((regime == 3)&&(BPO_mas.empty()!=1)&&(my_global_time > my_time)) {
        start_packets:
        out_data.clear();
        int prior = rand() % 2;
        out_data.push_back(prior*128 + 33);

        prior = rand() % 2;

        out_data.push_back(BPO_mas[bpo_index]);

        out_data.push_back(5); //packet type == video

        out_data.push_back(packet_counter*64+(BPO_mas[bpo_index]-(BPO_mas[bpo_index]>127)*128)-40);

        int fragment_num, reverse_bit;
        fragment_num = rand() % 24; 
        reverse_bit = rand() % 2;   
        last_packet = (rand() % 10) / 9;
        out_data.push_back(last_packet*64+reverse_bit*32+fragment_num);

        buf_val_0 = cur_packet % 256;
        buf_val_1 = cur_packet / 256;

        out_data.push_back(buf_val_1);
        out_data.push_back(buf_val_0);

        for (int i = 0; i < bpo_packet_amount; i++) {
            prior = rand() % 256;
            out_data.push_back(prior);
        }

        p_cm->cm_width = 16;
        p_cm->cm_poly = 0x8005L;
        p_cm->cm_init = 0L;
        p_cm->cm_refin = FALSE;
        p_cm->cm_refot = FALSE;
        p_cm->cm_xorot = 0L;

        cm_ini(p_cm);
        for (int i = 0; i < out_data.size(); i++) cm_nxt(p_cm, out_data[i]);

        crc = cm_crc(p_cm);
        buf_val = crc % 256;
        crc = crc / 256;
        out_data.push_back(crc);
        out_data.push_back(buf_val);

        int size = out_data.size(); 

        int buf_time = compress(out_data, compress_packet, compress_packet_number); //сжатие

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

        out_data.insert(out_data.begin(), bpo_index); //ID

        out_data.insert(out_data.begin(), 1); //flag 1 == по запросу

        out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
        out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
        out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
        out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей

        my_time += buf_time*16;

        copy(out_data.begin(), out_data.end(), ostream_iterator<char>(os_vid));

        if ((my_time > (frame_start + time_regime_2)) && (buf_regime != regime)) {
            if (os_vid.tellp() > 0) out_value = os_vid.tellp();
            else out_value = 0;
            os_vid.close();
            return out_value;
        }
//
        cur_packet++;

        if (cur_packet > frame_amount) {
            cur_packet = 0;
            bpo_index++;
            packet_counter++;
            if (packet_counter == 4) packet_counter = 0;
        }

        if (bpo_index > 23) bpo_index = 0;

        if (BPO_mas.size() > 24 && bpo_index == 0 && cur_packet == 0) {
            for (bpo_index = 0; bpo_index < 24; bpo_index++) BPO_mas.erase(BPO_mas.begin());
            bpo_index = 0;
        }

        if (my_global_time > my_time) goto start_packets;
    }

    if ((regime == 2)&&(BPO_mas.empty()!=1)&&(my_global_time > my_time)) {
            unsigned long long int my_new_time = 0;
            int prior = rand() % 2;
            out_data.push_back(prior*128 + 33);

            prior = rand() % 2;

            out_data.push_back(BPO_mas[0]);

            out_data.push_back(5); //packet type == video

            out_data.push_back(packet_counter*64+(BPO_mas[0]-(BPO_mas[0]>127)*128)-40);

            int fragment_num, reverse_bit;
            fragment_num = rand() % 24;  //необходимость выноса из цикла?
            reverse_bit = rand() % 2;    //необходимость выноса из цикла?
            last_packet = (rand() % 10) / 9;
            out_data.push_back(last_packet*64+reverse_bit*32+fragment_num);

            buf_val_0 = cur_packet % 256;
            buf_val_1 = cur_packet >> 8;

            out_data.push_back(buf_val_1);
            out_data.push_back(buf_val_0);

            for (int i = 0; i < bpo_packet_amount; i++) {
                prior = rand() % 256;
                out_data.push_back(prior);
            }

            p_cm->cm_width = 16;
            p_cm->cm_poly = 0x8005L;
            p_cm->cm_init = 0L;
            p_cm->cm_refin = FALSE;
            p_cm->cm_refot = FALSE;
            p_cm->cm_xorot = 0L;

            cm_ini(p_cm);
            for (int i = 0; i < out_data.size(); i++) cm_nxt(p_cm, out_data[i]);

            crc = cm_crc(p_cm);
            buf_val = crc % 256;
            crc = crc >> 8;
            out_data.push_back(crc);
            out_data.push_back(buf_val);

            cur_packet++;

            int size = out_data.size(); 

            int buf_time = compress(out_data, compress_packet, compress_packet_number); //сжатие

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

            out_data.insert(out_data.begin(), bpo_index); //ID

            out_data.insert(out_data.begin(), 0); //flag 0 == по времени
            my_time += size*16;

            out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
            out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
            out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей
            out_data.insert(out_data.begin(), 0);   //заполнитель до кратности 8 байтам для доп полей

            if ((last_packet == 1)||(cur_packet > frame_amount)) {
                packet_counter++;
                if (packet_counter == 4) packet_counter = 0;
                if (bpo_index == 0) {
                    bpo_index = 1;
                    my_new_time += (frame_amount - cur_packet)*buf_time*14;
                    my_new_time += start_pos[1]*328*14*4;
                    cur_packet = start_pos[1]*4;
                }
                else {
                    bpo_index = 0;
                    my_new_time += (frame_amount - cur_packet)*buf_time*14;
                    if (BPO_mas.size() > 1) {
                        BPO_mas.erase(BPO_mas.begin());
                        start_pos.erase(start_pos.begin());
                        start_pos.erase(start_pos.begin());
                    }
                    cur_packet = start_pos[0]*4;
                    my_new_time += start_pos[0]*328*14*4;
                }
            }
            else my_new_time = 0;

            my_time += my_new_time;

            copy(out_data.begin(), out_data.end(), ostream_iterator<char>(os_vid));
    }



    if (os_vid.tellp() > 0) out_value = os_vid.tellp();
    else out_value = 0;
    os_vid.close();
    return out_value;
}
