#include <iostream>
#include <numbers>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <map>
#include <algorithm>
#define EARTH_RADIUS 6372795 

using namespace std;

vector<pair<pair<float, float>, int>> sortMapByOrder(const map<pair<float, float>, int>& mapOfsums) {
    vector<pair<pair<float, float>, int>> sorted;
    sorted.reserve(mapOfsums.size());
    for (auto& planet : mapOfsums)
        sorted.push_back(planet);
    // sort by value
    sort(sorted.begin(), sorted.end(), [](auto l, auto r)->bool {return l.second < r.second; });
    return sorted;
}

class House {
public:
    House(const int& children, const map<pair<float, float>, float>& dis, const pair<float, float> cord, const map<pair<float, float>, float>& ratio_dis) {
        this->children = children;
        this->dis = dis;
        this->cord = cord;
        this->ratio_dis = ratio_dis;
    }
    pair<float, float> getCord() {
        return cord;
    }
    int getChildren() {
        return children;
    }
    map<pair<float, float>, float> getDis() {
        return dis;
    }
    map<pair<float, float>, float> getRatio_Dis() {
        return ratio_dis;
    }
    pair<pair<float, float>, float> getMin_ratio_dis() {
        pair<pair<float, float>, float> m;
        m.second = INT_MAX;
        for (auto i : ratio_dis)
            if (i.second < m.second) m = i;
        return m;
    }
    float getDifference() {
        return abs(ratio_dis.begin()->second - (++ratio_dis.begin())->second);
    }
private:
    map<pair<float, float>, float>dis;
    int children;
    pair<float, float>cord;
    map<pair<float, float>, float>ratio_dis;
};

float calculateTheDistance(const float& a1, const float& b1, const float& a2, const float& b2) {
    float cl1 = cos(a1 * numbers::pi / 180);
    float cl2 = cos(a2 * numbers::pi / 180);
    float sl1 = sin(a1 * numbers::pi / 180);
    float sl2 = sin(a2 * numbers::pi / 180);
    float cdelta = cos(b2 * numbers::pi / 180 - b1 * numbers::pi / 180);
    float sdelta = sin(b2 * numbers::pi / 180 - b1 * numbers::pi / 180);
    float y = sqrt(pow(cl2 * sdelta, 2) + pow(cl1 * sl2 - sl1 * cl2 * cdelta, 2));
    float x = sl1 * sl2 + cl1 * cl2 * cdelta;
    return atan2(y, x) * EARTH_RADIUS;
}

void foo(const map<pair<float, float>, int>& s_B, pair<pair<float, float>, int>& min_M, pair<float, float>& MAX) {
    pair<pair<float, float>, int> min, max;
    min.second = INT_MAX;
    max.second = 0;
    int ans = 0;
    int i = 0;
    for (const auto& it : s_B) {
        if (it.second < min.second)
            min = it;
        if (it.second > max.second)
            max = it;
    }
    min_M = min;//текущий max
    MAX = max.first;//текущий min
}



int main() {
    ifstream file("cord.txt");
    vector<pair<float, float>>cord_schools, cord_houses;
    vector<House>houses;
    while (!file.eof()) {
        string str;
        getline(file, str);
        size_t t = str.find(',');
        if (str[0] == '!')
            cord_schools.push_back({ stod(str.substr(1, t - 1)), stod(str.substr(t + 2)) });
        else
            cord_houses.push_back({ stod(str.substr(0, t)), stod(str.substr(t + 2)) });
    }

    for (auto cord_house : cord_houses) {
        map<pair<float, float>, float>dis, ratio_dis;
        int children = rand() % 300 + 1;
        for (auto cord_school : cord_schools) {
            dis[cord_school] = (calculateTheDistance(cord_house.first, cord_house.second, cord_school.first, cord_school.second));
            ratio_dis[cord_school] = children / dis[cord_school];
        }
        houses.push_back({ children, dis, cord_house,ratio_dis });
    }

    map<pair<float, float>, vector<pair<pair<float, float>, pair<float, int>>>> B, B_2;//массив школа -> дом -> характеристики, B_2 - отсортированный
    map<pair<float, float>, float>C;//Разность коэффициентов расстояний

    for (auto house : houses) {//работает только для двух школ
        auto tmp = house.getMin_ratio_dis();
        B[tmp.first].push_back({ house.getCord(),{tmp.second,house.getChildren()} });
        C[house.getCord()] = house.getDifference();
    }

    map<pair<float, float>, int> s_B;//сумма детей в каждой школе

    for (auto it : B)
        for (auto it2 : it.second)
            s_B[it.first] += it2.second.second;

    auto s = sortMapByOrder(s_B);//вектор сумм детей

    B_2 = B;

    for (auto& it : B_2)
        sort(it.second.begin(), it.second.end(),
            [&C](const pair<pair<float, float>, pair<float, int>>& l, const pair<pair<float, float>, pair<float, int>>& r) ->bool {return abs(C[l.first]) > abs(C[r.first]); });//сортировка по разности

    pair<pair<float, float>, int>t;
    pair<float, float>T;

    for (;;) {
        foo(s_B, t, T);
        auto it = --B_2[T].end();
        if (abs(s_B[T] - s_B[t.first]) < abs((s_B[T] - (*it).second.second) - (s_B[t.first] + (*it).second.second))) break;
        s_B[T] -= (*it).second.second;
        s_B[t.first] += (*it).second.second;
        B_2[t.first].push_back(*it);
        B_2[T].pop_back();
    }

    return 0;
}