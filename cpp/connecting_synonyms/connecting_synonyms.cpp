#include <thread>
#include <fstream>
#include <iostream>
#include <iterator>

#include "connecting_synonyms.hpp"

int id_words_group = 1;

FindSynonyms::FindSynonyms(std::string f1, std::string f2, std::string f3) :
                            m_words_map(), m_groups_map()
{
    size_t num_threads = 3;
    std::thread readers[num_threads];

    readers[0] = std::thread(&FindSynonyms::ThreadHandler, std::ref(*this), f1);
    readers[1] = std::thread(&FindSynonyms::ThreadHandler, std::ref(*this), f2);
    readers[2] = std::thread(&FindSynonyms::ThreadHandler, std::ref(*this), f3);


    for(size_t i = 0; i < num_threads; ++i)
    {
        readers[i].join();
    }
}

/* ----------------------------------------- */

void FindSynonyms::ThreadHandler(std::string f)
{
    std::ifstream t_file(f);
    
    if(!t_file.is_open())
    {
        std::cout << "No such file" << std::endl;
        return;
    }

    std::string line;
    while(getline(t_file, line))
    {       
        std::string first_word;
        std::string word;
        
        for(std::string::iterator runner = line.begin(); 
            runner != line.end(); ++runner)
        {
            //find words pair
            *runner = tolower(*runner);
            if(*runner == '=')
            {
                first_word = word;
                word.clear();
            }
            else if(*runner == ' ')
            {
                // update maps
                m_mutex.lock();

                int t_words_id = id_words_group;
                ++id_words_group;

                if(m_words_map.find(first_word) == m_words_map.end() &&
                    m_words_map.find(word) == m_words_map.end())
                {
                    m_words_map[first_word] = t_words_id;
                    m_groups_map[t_words_id].insert(first_word);
                    m_words_map[word] = t_words_id;
                    m_groups_map[t_words_id].insert(word);
                }
                else if(m_words_map.find(first_word) == m_words_map.end())
                {
                    t_words_id = m_words_map[word];
                    m_words_map[first_word] = t_words_id;
                    m_groups_map[t_words_id].insert(first_word);
                }
                else if(m_words_map.find(word) == m_words_map.end())
                {
                    t_words_id = m_words_map[first_word];
                    m_words_map[word] = t_words_id;
                    m_groups_map[t_words_id].insert(word);
                }
                else
                {
                    if(m_words_map[first_word] != m_words_map[word])
                    {
                        t_words_id = m_words_map[first_word];
                        int change_id = m_words_map[word];
                        for(auto it = m_groups_map[change_id].begin(); 
                            it != m_groups_map[change_id].end(); ++it)
                        {
                            m_words_map[*it] = t_words_id; 
                        }
                        m_groups_map[t_words_id].merge(m_groups_map[change_id]);
                        m_groups_map.erase(change_id);
                    }  
                }
                
                m_mutex.unlock(); 
                word.clear();
            }
            else
            {
                word.push_back(*runner);
            }
        }
    }

    t_file.close();
}

/* ----------------------------------------- */

void FindSynonyms::Print(std::string f)
{
    std::ofstream res_file(f);
    if(!res_file.is_open())
    {
        std::cout << "No such file" << std::endl;
        return;
    }

    for(auto it = m_groups_map.begin(); it != m_groups_map.end(); ++it)
    {
        for(auto p_set = it->second.begin(); p_set != it->second.end(); ++p_set)
        {
            res_file << *p_set;
            if(std::next(p_set) != it->second.end())
            {
                res_file << ",";
            }
        }
        res_file << '\n';
    }

    res_file.close();
}


/* ----------------------------------------- */

FindSynonyms::~FindSynonyms() {}