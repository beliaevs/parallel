#include <thread>
#include <vector>
#include <charconv>
#include <chrono>
#include <iostream>
#include <random>
#include <algorithm>
#include <string>
#include <string_view>
#include <optional>

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

  const std::string &name() const
  {
    return d_name;
  }

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> t;
  std::string d_name;
};

template <typename It>
It parallel_max(It first, It last, int threads = 1)
{
  auto len = std::distance(first, last);
  const int r = len % threads;
  const int d = len / threads;
  std::vector<It> res(threads);
  std::vector<std::thread> pool;
  pool.reserve(threads);
  auto task = [](It a, It b, It &out)
  { out = std::max_element(a, b); };

  for (int i = 0; i != r; ++i)
  {
    It prev = first;
    std::advance(first, d + 1);
    pool.emplace_back(task,
                      prev, first, std::ref(res[i]));
  }

  for (int i = r; i != threads; ++i)
  {
    It prev = first;
    std::advance(first, d);
    pool.emplace_back(task, prev, first, std::ref(res[i]));
  }

  for (auto &t : pool)
  {
    t.join();
  }

  return *std::max_element(res.begin(), res.end(), [](auto a, auto b)
                           { return *a < *b; });
}

std::vector<double> random_vector(int i_size)
{
  std::vector<double> res(i_size);
  std::mt19937 gen;
  std::normal_distribution<> dist(0.);

  for (int i = 0; i != i_size; ++i)
  {
    res[i] = dist(gen);
  }
  return res;
}

static std::optional<int> get_int(std::string_view i_str)
{
  int num = 0;
  auto res = std::from_chars(i_str.begin(), i_str.end(), num);
  if (res.ec == std::errc{})
    return num;
  return {};
}

int main(int argc, const char *argv[])
{
  constexpr int N = 100000000;
  Stopwatch t("random vector creation");
  auto data = random_vector(N);
  std::cout << t.stamp() << "\n";
  // parallel launch
  int threads = 8;
  if (argc == 2)
  {
    auto num = get_int(argv[1]);
    if (num)
      threads = *num;
  }
  Stopwatch m("max calc");
  const auto res = std::max_element(data.begin(), data.end());
  std::cout << m.stamp() << ", max = " << *res << "\n";

  Stopwatch pm("parallel max calc");
  const auto res1 = parallel_max(data.begin(), data.end(), threads);
  std::cout << pm.stamp() << "[" << threads << "], max = " << *res1 << "\n";
  return 0;
}