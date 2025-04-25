#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <bitset>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <clocale>
#include "json.hpp"

constexpr int N = 256;
std::bitset<N> bs[N][N];

int main() {
    setlocale(LC_CTYPE, "ja_JP.UTF-8");

    std::ifstream ifs("Kivodle.json");
    auto o = nlohmann::json::parse(ifs);

    int n = (int)o["students"].size();
    std::vector<std::string> data[6];
    std::vector<std::array<int, 6>> idx(n);

    std::string categories[] = { "", "weapons", "classes", "schools", "attackTypes", "" };
    for (int k = 1; k <= 4; k++) {
        for (const auto &[key, value] : o[categories[k]].items()) {
            size_t idx = stoull(key);
            if (idx >= data[k].size()) data[k].resize(idx + 1);
            data[k][idx] = value;
        }
    }
    std::map<std::string, int> idx_mp;
    for (int i = 0; const auto &s : o["students"]) {
        data[0].push_back(s["studentName"]);
        idx[i][0] = i;
        idx[i][1] = s["data"]["weapon"];
        idx[i][2] = s["data"]["class"];
        idx[i][3] = s["data"]["school"];
        idx[i][4] = s["data"]["attackType"];
        std::string date = s["data"]["implementationDate"];
        if (!idx_mp.contains(date)) {
            idx_mp[date] = (int)data[5].size();
            data[5].push_back(date);
        }
        idx[i][5] = idx_mp[date];
        i++;
    }

    //iを解答してjが正解だったとき表示される情報
    auto pat = [&](int i, int j) {
        int r = 0;
        for (int k = 0; k < 6; k++) {
            r |= (k == 2 ? (idx[i][k] & idx[j][k]) != 0 : idx[i][k] == idx[j][k]) << k;
        }
        r |= (data[5][idx[i][5]] < data[5][idx[j][5]]) << 6;
        return r;
    };

    //iを解答してjが正解だったときどこまで絞れるか
    for (int i = 0; i < n; i++) {
        int c[N] = {};
        for (int j = 0; j < n; j++) {
            c[j] = pat(i, j);
        }
        c[i] = -1;
        for (int j = 0; j < n; j++) {
            if (c[j] < 0) continue;
            for (int h = j; h < n; h++) {
                if (c[j] == c[h]) bs[i][j][h] = true;
            }
            for (int h = j + 1; h < n; h++) {
                if (c[j] == c[h]) bs[i][h] = bs[i][j], c[h] = -1;
            }
        }
    }

    struct Result {
        bool operator<(const Result &r) const {
            return d != r.d ? d < r.d : s < r.s;
            //return s < r.s;
        }
        int d, s, i;
        //d:最悪ケースの手数, s:b.count()通りの正解全てに対する手数の和（各局面に対し最善手が定まり、正解を決めれば手数も決まる）, i:最善手（の一つ）
        //ただし、sが0のときはd手で正解できない場合があることを表す
    };

    //bitsetで表された生徒の集合に対して最善を尽くしてKivodleしたときの結果
    std::unordered_map<std::bitset<N>, Result> mp;
    int nodes = 0;

    auto f = [&](auto f, const std::bitset<N> b, int d) -> Result {
        nodes++;
        int m = (int)b.count();
        if (m == 1) return { 1, 1, -1 };
        if (d == 1) return { d, 0, -2 };
        if (auto it = mp.find(b); it != mp.end()) {
            auto &r = it->second;
            if (r.s == 0) {
                if (r.d >= d) return r;
            } else {
                if (r.d <= d) return r; else return { d, 0, -5 };
            }
        }
        Result r = { 10, 10 * n, -3 };
        for (int i = 0; i < n; i++) {
            auto b0 = b;
            Result r_i = { 0, 0, -4 };
            int bad = 0;
            for (int j = 0; j < n; j++) {
                if (!b0[j]) continue;
                Result r_j;
                if (i == j) {
                    r_j = { 0, 0, i };
                    b0[j] = false;
                } else {
                    auto b1 = b & bs[i][j];
                    if (b1 == b) {
                        bad = 1;
                        break;
                    }
                    r_j = f(f, b1, d - 1);
                    if (r_j.s == 0) {
                        bad = 1;
                        break;
                    }
                    b0 &= ~b1;
                }
                r_i = { std::max(r_i.d, r_j.d), r_i.s + r_j.s, i };
                if (!(r_i < r)) break;
            }
            if (bad) continue;
            r = std::min(r, r_i);
        }
        r.d += 1;
        r.s += m;
        if (r.i < 0) r = { d, 0, -5 };
        return mp[b] = r;
    };

    std::bitset<N> b = {};
    for (int i = 0; i < n; i++) {
        b[i] = data[5][idx[i][5]] <= "2025/04/22";
    }
    auto r = f(f, b, 4);
    int i = mp[b].i;
    if (i < 0) {
        std::cout << mp[b].d << "手では足りない" << std::endl;
    } else {
        std::cout << r.d << ' ' << (double)r.s / b.count() << ' ' << data[0][idx[i][0]] << std::endl;
    }
    std::cout << nodes << ' ' << mp.size() << std::endl;

    return 0;
}
