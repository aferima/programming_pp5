
#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <syncstream>
#include <thread>

using namespace std;

void calculate(const string& name, int seconds) {
    this_thread::sleep_for(chrono::seconds(seconds));
    osyncstream(cout) << name << '\n';
}

void work() {
    const auto start = chrono::steady_clock::now();

    promise<void> a1Promise;
    promise<void> b2Promise;

    future<void> a1Done = a1Promise.get_future();
    future<void> b2Done = b2Promise.get_future();

    auto critical = async(launch::async, [&] {
        calculate("A1", 7);
        a1Promise.set_value();

        calculate("B1", 7);

        b2Done.wait();
        calculate("D", 1);
    });

    calculate("A2", 1);
    calculate("B2", 1);
    b2Promise.set_value();

    calculate("C2", 1);

    a1Done.wait();
    calculate("C1", 1);

    critical.get();

    const auto elapsed =
        chrono::duration<double>(
            chrono::steady_clock::now() - start
        ).count();

    osyncstream(cout)
        << "Execution time: " << elapsed << " seconds\n"
        << "Work is done!\n";
}

int main() {
    work();
    return 0;
}
