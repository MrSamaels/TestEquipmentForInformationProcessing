#include <main.h>

using namespace std;

int analise(
        string in_path,
        string out_path)
{
    ifstream out_data(out_path);
    ifstream in_data_vip(in_path + "\\vip");
    ifstream in_data_MO(in_path + "\\MO");
    ifstream in_data_R1(in_path + "\\R1");
    ifstream in_data_video(in_path + "\\compressed_data");

//    out_data.open(out_path, ofstream::app | ofstream::binary);
//    in_data.open(in_path, ofstream::app | ofstream::binary);

    vector <int> in_data_vector_vip;
    vector <int> in_data_vector_MO;
    vector <int> in_data_vector_R1;
    vector <int> in_data_vector_video;
    vector <int> out_data_vector;

    streampos out_fileSize = out_data.tellg();
    streampos in_fileSize_vip = in_data_vip.tellg();
    streampos in_fileSize_MO = in_data_MO.tellg();
    streampos in_fileSize_R1 = in_data_R1.tellg();
    streampos in_fileSize_video = in_data_video.tellg();

    vector <int> frame_regime;
    
    if (!out_data.eof() && !out_data.fail())
    {
        out_data.seekg(0,end);
        out_data_vector.resize(out_fileSize);

        out_data.seekg(0, begin);
        out_data.read(&out_data_vector[0], fileSize);
    }

    if (!in_data_vip.eof() && !in_data_vip.fail())
    {
        in_data_vip.seekg(0,end);
        in_data_vector_vip.resize(in_fileSize_vip);

        in_data_vip.seekg(0, begin);
        in_data_vip.read(&in_data_vector_vip[0], fileSize);
    }

    if (!in_data_MO.eof() && !in_data_MO.fail())
    {
        in_data_MO.seekg(0,end);
        in_data_vector_MO.resize(in_fileSize_MO);

        in_data_MO.seekg(0, begin);
        in_data_MO.read(&in_data_vector_MP[0], fileSize);
    }

    if (!in_data_R1.eof() && !in_data_R1.fail())
    {
        in_data_R1.seekg(0,end);
        in_data_vector_R1.resize(in_fileSize_R1);

        in_data_R1.seekg(0, begin);
        in_data_R1.read(&in_data_vector_R1[0], fileSize);
    }

    if (!in_data_video.eof() && !in_data_video.fail())
    {
        in_data_video.seekg(0,end);
        in_data_vector_video.resize(in_fileSize_video);

        in_data_video.seekg(0, begin);
        in_data_video.read(&in_data_vector_video[0], fileSize);
    }
    
    //последовательность режимов
    for (int i = 0; i < in_fileSize_vip;)
    {
        if (i == 0) frame_regime.push_back(in_data_vector_vip[6 + i*packet_size]);
        int packet_size = in_data_vector_vip[i + 6] << 8 + in_data_vector_vip[i + 7];
        if (in_data_vector_vip[i + reserved_bytes + 1] == 32 && in_data_vector_vip[i + reserved_bytes + 2] == 6)
        {
            frame_regime.push_back(in_data_vector_vip[i + reserved_bytes + 3] - (in_data_vector_vip[i + reserved_bytes + 3] < 2)*4);
        }
        i += packet_size + reserved_bytes + (packet_size % 8);
    }
    
    for (i = 0; i < frame_regime.size(); i++)
    {
        //режим 0
        if (frame_regime[i] == 0)
        {
            if (out_data_vector.size() > 0) {
                printf("Error: wrong data frame_size = %i, was expected nothing in regime 0", out_data_vector.size());
                return 1;
            }
        }
        
        //режим 1
        if (frame_regime[i] == 1)
             if (out_data_vector.size() < MO_frame) {
                 int frame_cnt = 0;                 
                 for (int j = 0; j < in_fileSize_MO;) {
                     int packet_size = in_data_vector_MO[j + 6] << 8 + in_data_vector_MO[j + 7];
                     j += reserved_bytes + 4;
                     for (j; j < packet_size; j++) {
                         if (in_data_vector_MO[j] == out_data_vector[frame_cnt]) frame_cnt++;
                         else {
                             printf("Error: wrong data, MO_data was %i on pos %i, was expected %i on pos %i in regime 1", out_data_vector[frame_cnt], frame_cnt, in_data_vector_MO[j], j);
                             return 1;
                         }
                     }
                     j += packet_size % 8;
                 }
             }
             else {
                 ("Error: too much data %i, was expected %i in regime 1", out_data_vector.size(), MO_frame);
                 return 1;
             }
        
        //режим 2
        if (frame_regime[i] == 2) {
            if (out_data_vector.size() == frame_out_data_length_amount * frame_out_data_width_amount) {
                int frame_cnt = 0;
                // MO_data
                for (int j = 0; j < in_fileSize_MO;) {
                    int packet_size = in_data_vector_MO[j + 6] << 8 + in_data_vector_MO[j + 7];
                    j += reserved_bytes + 4;
                    for (j; j < packet_size; j++) {
                        if (in_data_vector_MO[j] == out_data_vector[frame_cnt]) frame_cnt++;
                        else {
                            printf("Error: wrong data, MO_data was %i on pos %i, was expected %i on pos %i in regime 2", out_data_vector[frame_cnt], frame_cnt, in_data_vector_MO[j], j);
                            return 1;
                        }
                    }
                    j += (packet_size % 8);
                }
                //telemetry_data
                for (int j = 0; i < in_fileSize_vip;) {
                    int packet_size = in_data_vector_vip[j + 6] << 8 + in_data_vector_vip[j + 7];
                    if (in_data_vector_vip[j + reserved_bytes + 1] == 32 && in_data_vector_vip[j + reserved_bytes + 2] == 7) {
                        for (int k = j + reserved_bytes + 4; k < j + reserved_bytes + packet_size; k++) {
                            if (in_data_vector_vip[k] == out_data_vector[frame_cnt]) frame_cnt++;
                            else {
                                printf("Error: wrong data, telemetry_data was %i on pos %i, was expected %i on pos %i in regime 2", out_data_vector[frame_cnt]. frame_cnt, in_data_vector_vip[k], k);
                                return 1;                                
                            }
                        }
                    }
                    j += packet_size + reserved_bytes + (packet_size % 8);
                }
                
                frame_cnt += BS_UFK_M_telemetry_amount;
                
                //video_data
                for (int j = 0; i < in_fileSize_video;) {
                    int packet_counter = 0;
                    video_test_start:
                    vector <int> data_pointer;
                    data_pointer.push_back(out_data_vector[frame_cnt + frame_out_data_width_amount -1]);
                    if (out_data_vector[frame_cnt + frame_out_data_width_amount] != 0) 
                        data_pointer.push_back(out_data_vector[frame_cnt + frame_out_data_width_amount]);
                    frame_cnt += data_pointer[0];
                    if (j == (packet_counter + 1) * compress_amount) {
                        packet_counter++;
                        if (data_pointer.size() > 1) frame_cnt = data_pointer[1];
                        else {
                            frame_cnt += frame_out_data_width_amount - (frame_cnt % frame_out_data_width_amount);
                            goto video_test_start;
                        }
                    }
                    if (out_data_vector[frame_cnt] == in_data_vector_video[j]) {
                        j++;
                        frame_cnt++;
                    }
                    else {
                        printf("Error: wrong data, video_data was %i, was expected %i in regime 2", out_data_vector[frame_cnt], in_data_vector_video[j]);
                        return 1;                                
                    }
                }
            }
            else {
                printf("Error: wrong data number, was expected %i in regime 2", frame_out_data_length_amount * frame_out_data_width_amount);
            }
        }
        
        //режим 3
        if (frame_regime[i] == 3) {
            if (out_data_vector.size() == frame_out_data_length_amount * frame_out_data_width_amount) {
                int frame_cnt = 0;
                // MO_data
                for (int j = 0; j < in_fileSize_MO;) {
                    int packet_size = in_data_vector_MO[j + 6] << 8 + in_data_vector_MO[j + 7];
                    j += reserved_bytes + 4;
                    for (j; j < packet_size; j++) {
                        if (in_data_vector_MO[j] == out_data_vector[frame_cnt]) frame_cnt++;
                        else {
                            printf("Error: wrong data, MO_data was %i, was expected %i in regime 3", out_data_vector[frame_cnt], in_data_vector_MO[j]);
                            return 1;
                        }
                    }
                    j += (packet_size % 8);
                }
                //telemetry_data
                for (int j = 0; i < in_fileSize_vip;) {
                    int packet_size = in_data_vector_vip[j + 6] << 8 + in_data_vector_vip[j + 7];
                    if (in_data_vector_vip[j + reserved_bytes + 1] == 32 && in_data_vector_vip[j + reserved_bytes + 2] == 7) {
                        for (int k = j + reserved_bytes + 4; k < j + reserved_bytes + packet_size; k++) {
                            if (in_data_vector_vip[k] == out_data_vector[frame_cnt]) frame_cnt++;
                            else {
                                printf("Error: wrong data, telemetry_data was %i, was expected %i in regime 3", out_data_vector[frame_cnt], in_data_vector_vip[k]);
                                return 1;                                
                            }
                        }
                    }
                    j += packet_size + reserved_bytes + (packet_size % 8);
                }
                
                frame_cnt += BS_UFK_M_telemetry_amount;
                
                //video_data
                for (int j = 0; i < in_fileSize_video;) {
                    int packet_counter = 0, frame_type = 0;
                    int R1_cnt = 0;
                    video_test_start:
                    vector <int> data_pointer;
                    data_pointer.push_back(out_data_vector[frame_cnt + frame_out_data_width_amount -1]);
                    if (out_data_vector[frame_cnt + frame_out_data_width_amount] != 0) 
                        data_pointer.push_back(out_data_vector[frame_cnt + frame_out_data_width_amount]);
                    frame_cnt += data_pointer[0];
                    if (j == (packet_counter + 1) * compress_amount) {
                        packet_counter++;
                        if (data_pointer.size() > 1) frame_cnt = data_pointer[1];
                        else {
                            frame_cnt += frame_out_data_width_amount - (frame_cnt % frame_out_data_width_amount);
                            goto video_test_start;
                        }
                    }
                    if (out_data_vector[frame_cnt] == in_data_vector_video[j]) {
                        j++;
                        frame_cnt++;
                    }
                    else {
                        printf("Error: wrong data, video_data was %i, was expected %i in regime 3", out_data_vector[frame_cnt], in_data_vector_video[j]);
                        return 1;                                
                    }
                    if (packet_counter == compress_packets_amount * frame_type) {
                        frame_type++;
                        for (R1_cnt; j < in_fileSize_R1;) {
                            R1_test_start:
                            int packet_size = in_data_vector_R1[R1_cnt + 6] << 8 + in_data_vector_R1[R1_cnt + 7];
                            if (in_data_vector_R1[R1_cnt + 4] != 1) {
                                R1_cnt += reserved_bytes + packet_size + (packet_size % 8);
                                goto R1_test_start;
                            }
                            else {
                                R1_cnt += reserved_bytes + 4;
                                for (R1_cnt; R1_cnt < packet_size; R1_cnt++) {
                                    if (in_data_vector_R1[R1_cnt] == out_data_vector[frame_cnt]) frame_cnt++;
                                    else {
                                        printf("Error: wrong data, R1_data was %i, was expected %i in regime 3", out_data_vector[frame_cnt], in_data_vector_R1[R1_cnt]);
                                        return 1;
                                    }
                                }
                                R1_cnt += (packet_size % 8);
                            }
                        }                       
                    }
                }
            }
            else {
                printf("Error: wrong data number %i, was expected %i in regime 3", out_data.size(), frame_out_data_length_amount * frame_out_data_width_amount);
            }
        }        
    }
}
