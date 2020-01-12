#pragma once
//ThreadPool.h 

#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <queue>
#include <future>

class ThreadPool {
public:
	explicit ThreadPool(size_t threadNum) : stop(false) {
		for (size_t i = 0; i < threadNum; ++i) {
			workers.emplace_back([this]() {
				while (true) {
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> ul(mtx);
						cv.wait(ul, [this]() { return stop || !tasks.empty(); });
						if (stop && tasks.empty()) { return; }
						task = std::move(tasks.front());
						tasks.pop();
					}
					task();
				}
				});
		}
	}
	~ThreadPool() {
		{
			std::unique_lock<std::mutex> ul(mtx);
			stop = true;
		}
		cv.notify_all();
		for (auto& worker : workers) {
			worker.join();
		}
	}
	template<typename F, typename... Args>
	auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
		auto taskPtr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		{
			std::unique_lock<std::mutex> ul(mtx);
			if (stop) { throw std::runtime_error("submit on stopped ThreadPool"); }
			tasks.emplace([taskPtr]() { (*taskPtr)(); });
		}
		cv.notify_one();
		return taskPtr->get_future();
	}
private:
	bool stop;
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;
	std::mutex mtx;
	std::condition_variable cv;
};

/* HowToUse:
ThreadPool pool(4);
std::vector<std::future<int>> results;
for(int i = 0; i < 8; ++i) {
	results.emplace_back(pool.submit([]() {
		// computing task and return result
	}));
}
for(auto && result: results)
	std::cout << result.get() << ' ';
*/