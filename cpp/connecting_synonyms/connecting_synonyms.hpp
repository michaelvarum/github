#ifndef __FIND_SYNONYMS__
#define __FIND_SYNONYMS__

#include <unordered_map>
#include <string>
#include <set>
#include <mutex>

class FindSynonyms
{
public:
    FindSynonyms(std::string f1, std::string f2, std::string f3);
    ~FindSynonyms();

    void ThreadHandler(std::string f);
    void Print(std::string f);

private:
    std::unordered_map<std::string, int> m_words_map;
    std::unordered_map<int, std::set<std::string>> m_groups_map;
    std::mutex m_mutex;
};


#endif //__FIND_SYNONYMS__