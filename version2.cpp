#include <algorithm>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <math.h>
#include <numeric>
#include <omp.h>
#include <random>
#include <string>
#include <thread>
#include <vector>
using namespace std;

auto rng = default_random_engine{};
int totalPopulation = 0;
int totalBirth = 0;
int totalDeath = 0;
int genNum = 0;
int initial, k;
vector<int> population, births, death, total_birth, total_death, year, pop_year;
vector<double> gen_avg, g1_avg, g2_avg, g3_avg, g4_avg, gen_min, gen_max;
vector<double> g1, g2, g3, g4, all_luck;
std::random_device rd;
boost::mt19937 genRand(rd());
boost::normal_distribution cd{0.0, 0.780304146072379};

double uniformRand(double m, double f) {
  double diff = abs(m - f);
  std::uniform_real_distribution<> dis(-1 * diff, diff);
  return dis(genRand);
}

/*double normalvariate(double mean, double sigma) {
  double u1 = rand() / static_cast<double>(RAND_MAX);
  double u2 = rand() / static_cast<double>(RAND_MAX);
  double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
  return mean + sigma * z0;
}*/

void sortIt(vector<double> &arr) {
  sort(arr.begin(), arr.end(), greater<double>());
}

void shuffleIt(vector<double> &arr) { shuffle(arr.begin(), arr.end(), rng); }

double avg(const vector<double> &arr) {
  return accumulate(arr.begin(), arr.end(), 0.0) / arr.size();
}

double sum(const vector<double> &arr) {
  return accumulate(arr.begin(), arr.end(), 0.0);
}

double max(const vector<double> &arr) {
  return *max_element(arr.begin(), arr.end());
}

double min(const vector<double> &arr) {
  return *min_element(arr.begin(), arr.end());
}

void printArr(const vector<double> &arr, const string &name) {
  cout << "\n" << name << " [" << avg(arr) << ", " << arr.size() << "] = [";
  /*if (!arr.empty()) {
    for (size_t i = 0; i < arr.size() - 1; ++i) {
      cout << arr[i] << ", ";
    }
    cout << arr.back();
  }
  cout << "]" << endl;*/
}

void printArray(const vector<int> &arr, const string *name) {
  cout << name << " [" << arr.size() << "] = [";
  if (!arr.empty()) {
    for (size_t i = 0; i < arr.size() - 1; ++i) {
      cout << arr[i] << ", ";
    }
    cout << arr.back();
  }
  cout << "]" << endl;
}

void printArrAvg(const vector<double> &arr, const string *name) {
  cout << name << " avg: " << avg(arr) << endl;
}

void printArrMax(const vector<double> &arr, const string *name) {
  cout << name << " max: " << max(arr) << endl;
}

void printArrMin(const vector<double> &arr, const string *name) {
  cout << name << " min: " << min(arr) << endl;
}

void printGroups(vector<double> &arr) {
  int index = 0;
  for (int j = 1; j <= 4; j++) {
    int newIndex = round(((double)j / 4) * arr.size());
    if (newIndex % 2 != 0) {
      --newIndex;
    }
    for (int i = index; i < newIndex; i += 2) {
      if (j == 1) {
        g1.push_back(arr[i]);
        g1.push_back(arr[i + 1]);
      }
      if (j == 2) {
        g2.push_back(arr[i]);
        g2.push_back(arr[i + 1]);
      }
      if (j == 3) {
        g3.push_back(arr[i]);
        g3.push_back(arr[i + 1]);
      }
      if (j == 4) {
        g4.push_back(arr[i]);
        g4.push_back(arr[i + 1]);
      }
    }
    if (j == 1) {
      printArr(g1, "G1." + to_string(genNum + 1));
    } else if (j == 2) {
      printArr(g2, "G2." + to_string(genNum + 1));
    } else if (j == 3) {
      printArr(g3, "G3." + to_string(genNum + 1));
    } else {
      printArr(g4, "G4." + to_string(genNum + 1));
    }
    index = newIndex;
  }
}

void birth(vector<double> &male, vector<double> &female) {
  cout << "\n***** Generation " << genNum + 1 << " [Year: " << (genNum + 1) * 25
       << "] *****" << endl;
  vector<double> gen;
  int len_male = male.size();
  for (int i = 0; i < len_male; i++) {
    gen.push_back(male[i]);
    gen.push_back(female[i]);
  }

  male.clear();
  female.clear();

  printArr(gen, "Gen " + to_string(genNum + 1));
  printGroups(gen);

  gen_avg.push_back(avg(gen));
  g1_avg.push_back(avg(g1));
  g2_avg.push_back(avg(g2));
  g3_avg.push_back(avg(g3));
  g4_avg.push_back(avg(g4));
  gen_min.push_back(min(gen));
  gen_max.push_back(max(gen));
  year.push_back((genNum + 1) * 25);
  pop_year.push_back((genNum + 1) * 25);
  g1.clear();
  g2.clear();
  g3.clear();
  g4.clear();

  cout << "\n";
  int l = 0;
  for (int j = 0; j < 4; j++) {
    for (int i = 0;
         i < ((gen.size() - 1) - static_cast<int>(j / 4.0 * gen.size()));
         i = i + 2) {
      double pair_kid = (gen[i] + gen[i + 1]) / 2;
      double luck = uniformRand(gen[i], gen[i + 1]) + cd(genRand);
      // all_luck.push_back(luck);
      double kid = pair_kid + luck;
      // cout<<gen[i]<<" + "<<gen[i+1]<<" = "<<pair_kid<<" + "<<luck<<" =
      // "<<kid<<endl; cout << "l: " << l <<endl;
      if (l % 2 == 0) {
        male.push_back(kid);
      } else {
        female.push_back(kid);
      }
      l++;
      // child.push_back(kid);
    }
    // cout<<"*********************************************"<<endl;
  }
  gen.clear();

  /*if (child.size() % 2 != 0) {
    sortIt(child);
    cout << "Discarded Child: " << child.at(child.size() - 1) << endl;
    child.pop_back();
  }
  totalBirth += child.size();
  births.push_back(child.size());
  shuffleIt(child);
  for (int i = 0; i < child.size() / 2; i++) {
    male.push_back(child[2 * i]);
    female.push_back(child[2 * i + 1]);
  }
  child.clear();*/
  thread ms(sortIt, ref(male));
  thread fs(sortIt, ref(female));
  ms.join();
  fs.join();
  // sortIt(male);
  // sortIt(female);
  //
  if (male.size() > female.size()) {
    if (female.back() >= male.back()) {
      cout << "Discarded Child: " << male.back() << endl;
      male.pop_back();
    } else {
      female.pop_back();
      female.push_back(male.back());
      cout << "Discarded Child: " << male.back() << endl;
      male.pop_back();
    }
  }

  totalBirth += 2 * male.size();
  births.push_back(2 * male.size());

  printArr(male, "Male Child");
  printArr(female, "Female Child");
  cout << "\nNew Birth: " << births.back() << endl;
  if (genNum > 2) {
    cout << "Deaths: " << births[genNum - 3] << endl;
    totalDeath += births[genNum - 3];
    death.push_back(births[genNum - 3]);
    total_death.push_back(totalDeath);
  } else {
    death.push_back(0);
    total_death.push_back(totalDeath);
  }
  totalPopulation = totalBirth - totalDeath;
  cout << "Total Population: " << totalPopulation << endl;
  cout << "Total Birth: " << totalBirth << endl;
  cout << "Total Death: " << totalDeath << endl;
  population.push_back(totalPopulation);
  total_birth.push_back(totalBirth);
  ++genNum;
}

int main() {
  srand(time(nullptr));
  vector<double> male;
  vector<double> female;
  // cout << "Enter initial Male or Female Population: ";
  // cin >> initial;
  // cout << "Enter no. of Generation: ";
  // cin >> k;
  initial = 500;
  k = 50;
  normal_distribution d{100.0, 15.0};
  for (int i = 0; i < initial; i++) {
    male.push_back(d(genRand));
    female.push_back(d(genRand));
  }

  totalBirth += initial * 2;
  totalPopulation += male.size() + female.size();
  population.push_back(totalPopulation);
  births.push_back(initial * 2);
  death.push_back(0);
  total_birth.push_back(totalBirth);
  total_death.push_back(totalDeath);
  pop_year.push_back(0);

  thread th1(sortIt, ref(male));
  thread th2(sortIt, ref(female));
  th1.join();
  th2.join();
  printArr(male, "Male");
  printArr(female, "Female");

  for (int i = 0; i < k; i++) {
    birth(male, female);
  }

  printf("\nYear    Min       Avg       Max       G1        G2        G3       "
         " G4\n");
  for (int i = 0; i < k; i++) {
    cout << "(" << year[i] << " | " << gen_min[i] << " | " << gen_avg[i]
         << " | " << gen_max[i] << " | " << g1_avg[i] << " | " << g2_avg[i]
         << " | " << g3_avg[i] << " | " << g4_avg[i] << ")" << endl;
  }
  /*cout<<"\nSum: "<<sum(all_luck);
  cout<<"\nCount: "<<all_luck.size();
  cout<<"\nAverage: "<<avg(all_luck);*/
  /*printArray(births, "\nBirths");
  printArray(death, "Deaths");
  printArray(population, "Population");
  printArray(total_birth, "Total Birth");
  printArray(total_death, "Total Death");
  printArray(pop_year, "Years");*/
}