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

class House {
public:
    House(const int& children, const map<pair<float, float>, float>& dis, const pair<float, float> cord, const map<pair<float, float>, float>& ratio_dis, const int&id) {
        this->children = children;
        this->dis = dis;
        this->cord = cord;
        this->ratio_dis = ratio_dis;
        this->id = id;
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
    pair<pair<float, float>, float> getMax_ratio_dis() {
        pair<pair<float, float>, float> m;
        m.second = -1;
        for (auto i : ratio_dis)
            if (i.second > m.second) m = i;
        return m;
    }
    float getDifference(const pair<float,float>&school_m, const pair<float, float>&school_M) {
        return abs(ratio_dis[school_m] - ratio_dis[school_M]);
    }
    int getID() {
        return id;
    }
private:
    map<pair<float, float>, float>dis;
    int children;
    pair<float, float>cord;
    map<pair<float, float>, float>ratio_dis;
    int id;
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

void getMin_Max(const map<pair<float, float>, int>& s_B, pair<float, float>& min_S, pair<float, float>& max_S) {
    pair<pair<float, float>, int> min, max;
    min.second = INT_MAX;
    max.second = -1;
    for (const auto& it : s_B) {
        if (it.second < min.second)
            min = it;
        if (it.second > max.second)
            max = it;
    }
    min_S = min.first;//текущий min
    max_S = max.first;//текущий max
}


void sorting(const pair<float,float>&school_m, const pair<float, float>& school_M, map<const pair<float, float>, vector<pair<pair<pair<float, float>, pair<float, int>>, int>>>& map, vector<pair<House, int>>&houses) {
    sort(map[school_M].begin(), map[school_M].end(),
        [&school_M, &school_m, &houses]
    (pair<pair<pair<float, float>, pair<float, int>>, int>&l, pair<pair<pair<float, float>, pair<float, int>>, int>&r)->bool
        {
            int id_1 = l.second, id_2 = r.second;
            float dif_1 = houses[id_1].first.getDifference(school_m, school_M), dif_2= houses[id_2].first.getDifference(school_m, school_M);
            return dif_1 > dif_2;
        });
}

int main() {
    ifstream file("cord.txt");
    vector<pair<float, float>>cord_schools, cord_houses;
    vector<pair<House, int>>houses;
    while (!file.eof()) {//считывание из файла координат домов и школ
        string str;
        getline(file, str);
        size_t t = str.find(',');
        if (str[0] == '!')
            cord_schools.push_back({ stod(str.substr(1, t - 1)), stod(str.substr(t + 2)) });
        else
            cord_houses.push_back({ stod(str.substr(0, t)), stod(str.substr(t + 2)) });
    }
    int id = 0;
    for (auto cord_house : cord_houses) {
        map<pair<float, float>, float>dis, ratio_dis;//массивы расстояний и коэф расстояний
        int children = rand() % 20 + 1;//случайная генерация кол-ва детей от 1 до 20
        for (auto cord_school : cord_schools) {//расчет расстояния от дома до каждой школы
            dis[cord_school] = (calculateTheDistance(cord_house.first, cord_house.second, cord_school.first, cord_school.second));
            ratio_dis[cord_school] = children / dis[cord_school];//расчет коэф расстояния от дома до каждой школы
        }
        houses.push_back({ { children, dis, cord_house,ratio_dis, id}, id });//создание объекта одного дома
        ++id;
    }

    map<const pair<float, float>, vector<pair<pair<pair<float, float>, pair<float, int>>, int>>> B, B_2;
    //массив: школа -> дом -> характеристики, B_2 - отсортированный по коэф
    map<pair<float, float>, int> s_B;//сумма детей в каждой школе

    for (auto house : houses) {//работает только для двух школ
        auto tmp = house.first.getMax_ratio_dis();//максимальный коэф
        B[tmp.first].push_back({ { house.first.getCord(),{tmp.second,house.first.getChildren()} }, house.first.getID() });
        //распределение домов к школе с максимальным коэф
        s_B[tmp.first] += house.first.getChildren();//подсчет суммы детей в школе
    }

    B_2 = B;

    pair<float, float>min_S;
    pair<float, float>max_S;

    for (;;) {
        getMin_Max(s_B, min_S, max_S);//поиск минимальной и максимальной суммы детей в школе
        sorting(min_S, max_S, B_2, houses);
        auto it = --B_2[max_S].end();//последний дом в max сумме
        int* ptr = &((*it).first.second.second);
        if (abs(s_B[max_S] - s_B[min_S]) < abs((s_B[max_S] - *ptr) - (s_B[min_S] + *ptr)))
            break;
        s_B[max_S] -= *ptr;
        s_B[min_S] += *ptr;
        B_2[min_S].push_back(*it);
        B_2[max_S].pop_back();
    }

    return 0;
}