#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <chrono>

using namespace std;

const int NUM_THREADS = 24;
const int TOTAL_NUMBERS = 10000000;

mutex queueLock;
mutex resultLock;

long long computeCollatzSteps(long long num) {
    long long steps = 0;
    while (num != 1) {
        if (num % 2 == 0)
            num /= 2;
        else
            num = 3 * num + 1;
        steps++;
    }
    return steps;
}

void workerThread(queue<int>& taskQueue, vector<long long>& partialResults) {
    long long localSteps = 0;

    while (true) {
        int num;
        {
            lock_guard<mutex> lock(queueLock);
            if (taskQueue.empty())
                break;
            num = taskQueue.front();
            taskQueue.pop();
        }
        localSteps += computeCollatzSteps(num);
    }

    {
        lock_guard<mutex> lock(resultLock);
        partialResults.push_back(localSteps);
    }
}

int main() {
    queue<int> taskQueue;
    vector<long long> partialResults;
    partialResults.reserve(NUM_THREADS);

    for (int i = 1; i <= TOTAL_NUMBERS; i++) {
        taskQueue.push(i);
    }

    auto startTime = chrono::high_resolution_clock::now();

    vector<thread> threadPool(NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; i++) {
        threadPool[i] = thread(workerThread, ref(taskQueue), ref(partialResults));
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        threadPool[i].join();
    }

    auto endTime = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);

    long long totalSteps = 0;
    for (long long steps : partialResults) {
        totalSteps += steps;
    }
    double averageSteps = (double)totalSteps / TOTAL_NUMBERS;

    cout << "Execution time (ms): " << elapsed.count() << endl;
    cout << "Average steps: " << averageSteps << endl;

    return 0;
}
