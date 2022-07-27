#include<iostream>
#include<thread>
#include <chrono>
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

  const std::string &name() const
  {
    return d_name;
  }

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> t;
  std::string d_name;
};

int main()
{
    constexpr int N = 1000;
    double sum_start = 0.;
    double sum_join = 0.;

    for(int i = 0; i != N; ++i)
    {
        Stopwatch sw("start thread");
        auto work = [&](){ 
            sum_start += sw.milliseconds();
            sw.reset(); 
            };
        std::thread t(work);
        t.join();
        sum_join += sw.milliseconds();
    }

    std::cout << "average start time: " << sum_start/N << "[ms]\n";
    std::cout << "average join time: " << sum_join/N << "[ms]\n";
    return 0;
}