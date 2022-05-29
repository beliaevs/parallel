#include <thread>
#include <vector>
#include <chrono>
#include <iostream>
#include <random>
#include <algorithm>
#include <string>
#include <string_view>

class Stopwatch
{
public:
  explicit Stopwatch(std::string_view view) : t(std::chrono::high_resolution_clock::now()), d_name(view)
  {
  }

  double milliseconds() const
  {
    const std::chrono::duration<double, std::milli> d = std::chrono::high_resolution_clock::now() - t;
    return d.count();
  }

  std::string stamp() const
  {
    const auto t = milliseconds();
    std::string res = name() + ": " + std::to_string(t) + "[ms]";
    return res;
  }

  void reset()
  {
    t = std::chrono::high_resolution_clock::now();
  }

  const std::string& name() const
  {
    return d_name;
  }

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> t;
  std::string d_name;
};

template <typename It>
It parallel_max(It first, It last)
{
  It mid = first + (last - first)/2;
  It res1, res2;
  auto mymax1 = [first, mid, &res1](){ res1 = std::max_element(first, mid); };
  std::thread t1(mymax1);
   auto mymax2 = [mid, last, &res2](){ res2 = std::max_element(mid, last); };
  std::thread t2(mymax2);
  t1.join();
  t2.join();
  return (*res1 < *res2) ? res2 : res1;
}

std::vector<double> random_vector(int i_size)
{
  std::vector<double> res(i_size);
  std::mt19937 gen;
  constexpr double A = -1000000.;
  constexpr double B = 1000000.;
  std::uniform_real_distribution<> dist(A, B);
  for (int i = 0; i != i_size; ++i)
  {
    res[i] = dist(gen);
  }
  return res;
}

int main()
{
  constexpr int N = 100000000;
  Stopwatch t("random vector creation");
  auto data = random_vector(N);
  std::cout << t.stamp() << "\n";

  Stopwatch m("max calc");
  const auto res = std::max_element(data.begin(), data.end());
  std::cout << m.stamp() << ", max = " << *res << "\n";

  Stopwatch pm("parallel max calc");
  const auto res1 = parallel_max(data.begin(), data.end());
  std::cout << pm.stamp() << ", max = " << *res1 << "\n";
  return 0;
}