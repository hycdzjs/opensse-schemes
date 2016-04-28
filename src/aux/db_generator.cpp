//
//  db_generator.cpp
//  sophos
//
//  Created by Raphael Bost on 28/04/2016.
//  Copyright © 2016 Raphael Bost. All rights reserved.
//

#include "db_generator.hpp"
#include "logger.hpp"

#include <sse/crypto/fpe.hpp>

#include <string>
#include <thread>
#include <vector>
#include <iostream>

namespace sse {
    namespace sophos {

        const std::string kKeyword01PercentBase    = "0.1";
        const std::string kKeyword1PercentBase     = "1";
        const std::string kKeyword10PercentBase    = "10";

        const std::string kKeyword10GroupBase    = "Group-10^";

        constexpr uint32_t max_10_counter = 1e5;
        
        static void generation_job(SophosClientRunner* client, unsigned int thread_id, size_t N_entries, size_t step, crypto::Fpe *rnd_perm)
        {
            size_t counter = thread_id;
            std::string id_string = std::to_string(thread_id);
            
            uint32_t counter_10_1 = 0, counter_10_2 = 0, counter_10_3 = 0, counter_10_4 = 0, counter_10_5 = 0;
            
            for (size_t i = 0; counter < N_entries; counter += step, i++) {
                size_t ind = rnd_perm->encrypt_64(counter);
                
                uint32_t ind_01 = ind % 1000;
                uint32_t ind_1  = ind_01 % 100;
                uint32_t ind_10 = ind_1 % 10;
                
                std::string keyword_01  = kKeyword01PercentBase    + "_" + std::to_string(ind_01);
                std::string keyword_1   = kKeyword1PercentBase     + "_" + std::to_string(ind_1);
                std::string keyword_10  = kKeyword10PercentBase    + "_" + std::to_string(ind_10);
                
                client->update(keyword_01, ind);
                client->update(keyword_1, ind);
                client->update(keyword_10, ind);
                
                if (counter_10_1 < max_10_counter) {
                    std::string kw = kKeyword10GroupBase  + "1_" + id_string + "_" + std::to_string(counter_10_1);
                    client->update(kw, ind);
                    
                    if((i+1)%10 == 0)
                    {
                        logger::log(logger::TRACE) << "Random DB generation: completed keyword: " << kw << std::endl;
                        counter_10_1++;
                    }
                }
                if (counter_10_2 < max_10_counter) {
                    std::string kw = kKeyword10GroupBase  + "2_" + id_string + "_" + std::to_string(counter_10_2);
                    client->update(kw, ind);

                    if((i+1)%100 == 0)
                    {
                        logger::log(logger::TRACE) << "Random DB generation: completed keyword: " << kw << std::endl;
                        counter_10_2++;
                    }
                }
                if (counter_10_3 < max_10_counter) {
                    std::string kw = kKeyword10GroupBase  + "3_" + id_string + "_" + std::to_string(counter_10_3);
                    client->update(kw, ind);

                    if((i+1)%((size_t)(1e3)) == 0)
                    {
                        logger::log(logger::TRACE) << "Random DB generation: completed keyword: " << kw << std::endl;
                        counter_10_3++;
                    }
                }
                if (counter_10_4 < max_10_counter) {
                    std::string kw = kKeyword10GroupBase  + "4_" + id_string + "_" + std::to_string(counter_10_4);
                    client->update(kw, ind);
                    
                    if((i+1)%((size_t)(1e4)) == 0)
                    {
                        logger::log(logger::TRACE) << "Random DB generation: completed keyword: " << kw << std::endl;
                        counter_10_4++;
                    }
                }
                if (counter_10_5 < max_10_counter) {
                    std::string kw = kKeyword10GroupBase  + "5_" + id_string + "_" + std::to_string(counter_10_5);
                    client->update(kw, ind);
                    
                    if((i+1)%((size_t)(1e5)) == 0)
                    {
                        logger::log(logger::TRACE) << "Random DB generation: completed keyword: " << kw << std::endl;
                        counter_10_5++;
                    }
                }
            }
            
            std::string log = "Random DB generation: thread " + std::to_string(thread_id) + " completed: (" + std::to_string(counter_10_1) + ", " + std::to_string(counter_10_2) + ", "+ std::to_string(counter_10_3) + ", "+ std::to_string(counter_10_4) + ", "+ std::to_string(counter_10_5) + ")";
            logger::log(logger::INFO) << log << std::endl;
        }
        
        
        void gen_db(SophosClientRunner& client, size_t N_entries)
        {
            crypto::Fpe rnd_perm;
            
            unsigned int n_threads = std::thread::hardware_concurrency();
            std::vector<std::thread> threads;
            
            for (unsigned int i = 0; i < n_threads; i++) {
                threads.push_back(std::thread(generation_job, &client, i, N_entries, n_threads, &rnd_perm));
            }

            for (unsigned int i = 0; i < n_threads; i++) {
                threads[i].join();
            }

        }

    }
}