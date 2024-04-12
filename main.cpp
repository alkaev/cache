#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

class Cache_Settings {
public:
    int MEM_SIZE = 65536;
    int CACHE_SIZE = 4096;
    int CACHE_LINE_SIZE = 32;
    int CACHE_LINE_COUNT = 128;
    int CACHE_WAY = 4;
    int CACHE_SETS_COUNT = 32;    
    int ADDR_LEN = 16;  
    int CACHE_TAG_LEN = 6;
    int CACHE_IDX_LEN = 5;
    int CACHE_OFFSET_LEN = 5;  
    int Cache_to_Mem_Ticks = 101;
    int Cache_Find_Ticks = 6;
    int Cache_Eror_Ticks = 113;
    int Ask = 0;
    int Miss = 0;
    int Ticks = 0;          
    std::string Politics;
    std::vector<std::vector<int>> Tags;
    std::vector<std::vector<int>> Times;
    std::vector<std::vector<int>> Modified;
    

    Cache_Settings(std::string Politics) : Politics(Politics) {
        Tags = std::vector<std::vector<int>>(CACHE_SETS_COUNT, std::vector<int>(CACHE_WAY, -1));
        Times = std::vector<std::vector<int>>(CACHE_SETS_COUNT, std::vector<int>(CACHE_WAY, 0));
        Modified = std::vector<std::vector<int>>(CACHE_SETS_COUNT, std::vector<int>(CACHE_WAY, 0));

        if (Politics == "RR") {
            for (int i = 0; i < CACHE_SETS_COUNT; i++) {
                Times[i][0] = 1;
            }
        }
    }

    void cache_request(int Address, std::string Command) {
        int tag = Address >> (CACHE_OFFSET_LEN + CACHE_IDX_LEN);
        int Index = (Address >> CACHE_OFFSET_LEN) % CACHE_SETS_COUNT;
        Ask += 1;

        auto it = std::find(Tags[Index].begin(), Tags[Index].end(), tag);
        if (it != Tags[Index].end()) {
            Ticks += Cache_Find_Ticks;
            int Second_index = std::distance(Tags[Index].begin(), it);
            if (Politics == "LRU") {
            for (int i = 0; i < CACHE_WAY; i++) {
                Times[Index][i] += 1;
            }
            Times[Index][Second_index] = 0;
        } else if (Politics == "pLRU") {
            Times[Index][Second_index] = 1;
            int Full = 1;
            for (int i = 0; i < CACHE_WAY; i++)
                if (Times[Index][i] == 0)     
                    Full = 0;
            if (Full == 1) {
                Times[Index] = std::vector<int>(CACHE_WAY, 0);
                Times[Index][Second_index] = 1;
            }
        } 
            if (Command == "Write") {
                Modified[Index][Second_index] = 1;
            }
        } else {
            Miss += 1;
            Ticks += Cache_Eror_Ticks;
            int Second_index = 0;
            if (Politics == "LRU") {
                int max_time = *std::max_element(Times[Index].begin(), Times[Index].end());
                Second_index = std::distance(Times[Index].begin(), std::find(Times[Index].begin(), Times[Index].end(), max_time));
            } else if (Politics == "pLRU") {
                Second_index = std::distance(Times[Index].begin(), std::find(Times[Index].begin(), Times[Index].end(), 0));
            } else if (Politics == "RR") {
                Second_index = std::distance(Times[Index].begin(), std::find(Times[Index].begin(), Times[Index].end(), 1));
            }

            if (Modified[Index][Second_index] == 1) {
                Ticks += Cache_to_Mem_Ticks;
            }
            Tags[Index][Second_index] = tag;
            if (Command == "Read"){
                Modified[Index][Second_index] = 0;
            }
            else {
                Modified[Index][Second_index] = 1;
            }
            if (Politics == "LRU") {
            for (int i = 0; i < CACHE_WAY; i++) {
                Times[Index][i] += 1;
            }
            Times[Index][Second_index] = 0;
        } else if (Politics == "pLRU") {
            Times[Index][Second_index] = 1;
            int Full = 1;
            for (int i = 0; i < CACHE_WAY; i++)
                if (Times[Index][i] == 0)     
                    Full = 0;
            if (Full == 1) {
                Times[Index] = std::vector<int>(CACHE_WAY, 0);
                Times[Index][Second_index] = 1;
            }
        } else if (Politics == "RR" ) {
            for (int i = 0; i < CACHE_WAY; i++) {
                if (Times[Index][i] == 1) {
                    Times[Index][i] = 0;
                    Times[Index][(i + 1) % CACHE_WAY] = 1;
                    break;
                }
            }
        }
        }
    }
};

void cache_implementation(std::string Politics) {
    int M = 64;
    int N = 60;
    int K = 32;
    Cache_Settings Cache(Politics);

    std::vector<std::vector<int>> a(M, std::vector<int>(K, 0));
    std::vector<std::vector<int>> b(K, std::vector<int>(N, 0));
    std::vector<std::vector<int>> c(M, std::vector<int>(N, 0));

    int Address = 1 << 10;

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < K; ++j) {
            a[i][j] = Address;
            Address += 1;
        }
    }

    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N; j++) {
            b[i][j] = Address;
            Address += 2;
        }
    }

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            c[i][j] = Address;
            Address += 4;
        }
    }

    Cache.Ticks += 3;

    for (int i = 0; i < M; i++) {
        Cache.Ticks += 2;
        for (int j = 0; j < N; j++) {
            Cache.Ticks += 5;
            for (int k = 0; k < K; k++) {
                Cache.Ticks += 11;
                Cache.cache_request(a[i][k],  "Read");
                Cache.cache_request(b[k][j], "Read");
            }
            Cache.cache_request(c[i][j], "Write");
            Cache.Ticks += 2;
        }
    }

    double perc = (Cache.Ask - Cache.Miss) * 100.0 / Cache.Ask;
    std::cout << Politics << ":\thit perc. " << perc << "%\ttime: " << Cache.Ticks << std::endl;
}

int main() {
    cache_implementation("LRU");
    cache_implementation("pLRU");
    cache_implementation("RR");
    return 0;
}