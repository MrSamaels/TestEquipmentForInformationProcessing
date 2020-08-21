#include "compress.h"

using namespace std;

int compress(string path,
             vector <int>& in_bpo_data,
             int& compress_packet,
             int& compress_packet_number) {

    start_compressing:
    vector <int> in_data = in_bpo_data;
    vector <int> buf_compress_data;
    vector <int> compress_data;
    
    ofstream os_compressed_data;
    os_compressed_data.open(path+"\\compressed_data", ofstream::app | ofstream::binary);    

    int packet_number, cnt;

        int packet_num_1, packet_num_0;
        int scan_type;
        int fragment_num;
        unsigned long long int countdown;
        int ref_value;

        scan_type = _out(in_data, 3+reserved_bytes, 5); //тип развертки
        fragment_num = (in_data[3+reserved_bytes] % 32);
        packet_num_1 = in_data[4+reserved_bytes];
        packet_num_0 = in_data[5+reserved_bytes];

        int packet_num = (packet_num_1 << 8) + packet_num_0; //номер пакета сжатых данных

        for (int i = 0; i < 3; i++)          //оставить только видео данные
            in_data.erase(in_data.begin());   //оставить только видео данные
        for (int i = 0; i < 2; i++)           //оставить только видео данные
            in_data.erase(in_data.end()-1);   //оставить только видео данные

        int buf_value_0, buf_value_1;


        //перевод в 10битовые отсчеты
        for (int i = 0; i < in_data.size()-1; i++) {
            switch (i % 5)
            {
            case 0:
                buf_value_1 = in_data[i] << 2;
                break;
            case 1:
                buf_value_1 = (in_data[i] % 64) << 4;
                break;
            case 2:
                buf_value_1 = (in_data[i] % 16) << 6;
                break;
            case 3:
                buf_value_1 = (in_data[i] % 4) << 8;
                break;
            case 4:
                buf_value_1 = -1;
                break;
            }
            switch ((i + 1) % 5)
            {
            case 0:
                buf_value_0 = -1;
                break;
            case 1:
                buf_value_0 = in_data[i+1] / 64;
                break;
            case 2:
                buf_value_0 = in_data[i+1] / 16;
                break;
            case 3:
                buf_value_0 = in_data[i+1] / 4;
                break;
            case 4:
                buf_value_0 = in_data[i+1];
                break;
            }

            if ((buf_value_0 + buf_value_1) >= 0)
                buf_compress_data.push_back(buf_value_0 + buf_value_1);
         }
         compress_packet++; //счетчик обработанных пакетов
         //создание запакованной информации

         countdown = packet_num * 256; 

         compress_data.push_back(scan_type*8 + (packet_number>>10));
         compress_data.push_back((compress_packet_number % 2048) >> 2);
         compress_data.push_back((compress_packet_number % 4)+(fragment_num << 1) + (countdown >> 22));
         compress_data.push_back((countdown % 4194304) >> 14);
         compress_data.push_back((countdown % 16384) >> 6);
         compress_data.push_back((countdown % 64) + (ref_value >> 8));
         compress_data.push_back(ref_value % 256);

         for (int compress_packet_number; ; compress_packet_number++) { 
             vector <int> cnt_diff; //массив модулей разностей
             vector <int> cnt_dif_znak; //массив знаков
             vector <int> buf_segment; //для хранения сжатого сегмента
             if (compress_packet_number == 0) {
                 cnt_diff.push_back(abs(buf_compress_data[compress_packet_number*8 + cnt] - 0));
                 if ((buf_compress_data[compress_packet_number*8 + cnt] - 0) >= 0) cnt_dif_znak.push_back(1);
                 else cnt_dif_znak.push_back(0);

                 for (int cnt = 1; cnt < 8; cnt++) {
                     cnt_diff.push_back(abs(buf_compress_data[compress_packet_number*8 + cnt] - buf_compress_data[compress_packet_number*8 + cnt - 1]));
                     if ((buf_compress_data[compress_packet_number*8 + cnt] - buf_compress_data[compress_packet_number*8 + cnt - 1]) >= 0) cnt_dif_znak.push_back(1);
                     else cnt_dif_znak.push_back(0);
                 }

                 //поиск максимальной разности
                 int max_diff = cnt_diff[0];
                 for (int k = 0; k<8; k++) {
                     if (cnt[k] > max_diff) max_diff = cnt[k];
                 }
                 max_diff++;

                 if (max_diff == 1)
                 {
                     buf_segment.push_back(buf_compress_data[compress_packet_number*8]);
                 }
                 else if (max_diff > 511)
                     for (int j=0; j<8;j++)
                         buf_segment.push_back(buf_compress_data[compress_packet_number*8 + j]);
                 else {
                      buf_segment.push_back(cnt_dif_znak[0]<<7 +
                                            cnt_dif_znak[1]<<6 +
                                            cnt_dif_znak[2]<<5 +
                                            cnt_dif_znak[3]<<4 +
                                            cnt_dif_znak[4]<<3 +
                                            cnt_dif_znak[5]<<2 +
                                            cnt_dif_znak[6]<<1 +
                                            cnt_dif_znak[7]<<0);
                      buf_segment.push_back(cnt_diff[0]<<7*cnt_diff.size() +
                                            cnt_diff[1]<<6*cnt_diff.size() +
                                            cnt_diff[2]<<5*cnt_diff.size() +
                                            cnt_diff[3]<<4*cnt_diff.size() +
                                            cnt_diff[4]<<3*cnt_diff.size() +
                                            cnt_diff[5]<<2*cnt_diff.size() +
                                            cnt_diff[6]<<1*cnt_diff.size() +
                                            cnt_diff[7]<<0*cnt_diff.size());
                 }

             }
             else {
                  for (int cnt = 0; cnt < 8; cnt++) {
                     cnt_diff.push_back(abs(buf_compress_data[compress_packet_number*8 + cnt] - buf_compress_data[compress_packet_number*8 + cnt - 1]));
                     if ((buf_compress_data[compress_packet_number*8 + cnt] - buf_compress_data[compress_packet_number*8 + cnt - 1]) >= 0) cnt_dif_znak.push_back(1);
                     else cnt_dif_znak.push_back(0);
                 }

                 //поиск максимальной разности
                 int max_diff = cnt_diff[0];
                 for (int k = 0; k<8; k++) {
                     if (cnt[k] > max_diff) max_diff = cnt[k];
                 }
                 max_diff++;

                 if (max_diff == 1)
                 {
                     buf_segment.push_back(buf_compress_data[compress_packet_number*8]);
                 }
                 else if (max_diff > 511)
                     for (int j=0; j<8;j++)
                         buf_segment.push_back(buf_compress_data[compress_packet_number*8 + j]);
                 else {
                      buf_segment.push_back(cnt_dif_znak[0]<<7 +
                                            cnt_dif_znak[1]<<6 +
                                            cnt_dif_znak[2]<<5 +
                                            cnt_dif_znak[3]<<4 +
                                            cnt_dif_znak[4]<<3 +
                                            cnt_dif_znak[5]<<2 +
                                            cnt_dif_znak[6]<<1 +
                                            cnt_dif_znak[7]<<0);
                      buf_segment.push_back(cnt_diff[0]<<7*cnt_diff.size() +
                                            cnt_diff[1]<<6*cnt_diff.size() +
                                            cnt_diff[2]<<5*cnt_diff.size() +
                                            cnt_diff[3]<<4*cnt_diff.size() +
                                            cnt_diff[4]<<3*cnt_diff.size() +
                                            cnt_diff[5]<<2*cnt_diff.size() +
                                            cnt_diff[6]<<1*cnt_diff.size() +
                                            cnt_diff[7]<<0*cnt_diff.size());
                 }
                 compress_data.push_back(buf_segment);
                 if (compress_data.size() == compress_amount) goto start_compressing;
             }
         }

         packet_number++;
         copy(compress_data.begin(), compress_data.end(), ostream_iterator<char>(os_compressed_data));

    return compress_data.size();
} 