#ifndef __PUPIL_TRACKER_UTILS_H__
#define __PUPIL_TRACKER_UTILS_H__

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <map>
#include <algorithm>
#include <iomanip>

namespace pupiltracker {
class ConfigFile {
  public:
    bool read(std::string file) {
      std::ifstream fin(file);
      if (!fin.is_open()) {
        std::cout << "Can not find file " << file << std::endl;
        return false;
      }
      while(!fin.eof()) {
        std::string line;
        std::getline(fin, line);
        size_t pos = line.find(":");
        line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
        if (pos == std::string::npos) {
            continue;
        }
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        key_value[key] = value;
      }
      fin.close();
      return true;
    }
    void write(std::string file) {
      std::ofstream fout(file);
      if (!fout.is_open()) {
        std::cout << "Can not open file " << file << std::endl;
      }
      for (const auto & it : key_value) {
        fout << it.first << ": " << it.second << "\n";
      }
      fout.close();
    }

    template<typename T>
    T get(const std::string key) {
      auto it = key_value.find(key);
      if (it == key_value.end()) return T();
      else {
        std::string value = key_value[key];
        std::stringstream ss(value);
        T ret;
        ss >> ret;
        return ret;
      }
    }
    template<typename T>
    void set(const std::string& key, T value) {
      std::stringstream ss;
      ss << std::setprecision(8) << value;
      key_value[key] = ss.str();
    }
    void clear() {
      key_value.clear();
    }
  private:
    std::map<std::string, std::string> key_value;
};

class MakeString
{
public:
    std::stringstream stream;
    operator std::string() const { return stream.str(); }

    template<class T>
    MakeString& operator<<(T const& VAR) { stream << VAR; return *this; }
};

inline int pow2(int n)
{
    return 1 << n;
}

template<typename T>
inline T sq(T n)
{
    return n * n;
}

template<typename T>
inline T lerp(const T& val1, const T& val2, double alpha)
{
    return val1*(1-alpha) + val2*alpha;
}

int random(int min, int max);
int random(int min, int max, unsigned int seed);

template<typename T>
std::vector<T> randomSubset(const std::vector<T>& src, typename std::vector<T>::size_type size)
{
    if (size > src.size())
        throw std::range_error("Subset size out of range");

    std::vector<T> ret;
    std::set<size_t> vals;

    for (size_t j = src.size() - size; j < src.size(); ++j)
    {
        size_t idx = random(0, j); // generate a random integer in range [0, j]

        if (vals.find(idx) != vals.end())
            idx = j;

        ret.push_back(src[idx]);
        vals.insert(idx);
    }

    return ret;
}

template<typename T>
std::vector<T> randomSubset(const std::vector<T>& src, typename std::vector<T>::size_type size, unsigned int seed)
{
    if (size > src.size())
        throw std::range_error("Subset size out of range");

    std::vector<T> ret;
    std::set<size_t> vals;

    for (size_t j = src.size() - size; j < src.size(); ++j)
    {
        size_t idx = random(0, j, seed+j); // generate a random integer in range [0, j]

        if (vals.find(idx) != vals.end())
            idx = j;

        ret.push_back(src[idx]);
        vals.insert(idx);
    }

    return ret;
}

} //namespace pupiltracker

#endif // __PUPIL_TRACKER_UTILS_H__
