#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <cmath>
#include <list>

struct Process
{
    char name;
    int arrivalTime;
    int serviceTime;
    int remainingTime;
    int startTime;
    int finishTime;
    int waitTime;
    float ratio;
    int i;
    int quantum;
    std::vector<char> timeline;
};

void printCenteredInt(int value, int width)
{
    std::string str = std::to_string(value);
    int padding = width - str.length();
    int leftPadding = padding / 2;
    int rightPadding = padding - leftPadding;

    std::cout << "|"
              << std::string(leftPadding, ' ')
              << str
              << std::string(rightPadding, ' ');
}

void printCenteredFloat(float value, int width)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    std::string str = oss.str();
    int padding = width - str.length();
    int rightPadding = padding / 2;
    int leftPadding = padding - rightPadding;

    std::cout << "|"
              << std::string(leftPadding, ' ')
              << str
              << std::string(rightPadding, ' ');
}

void tracePrint(std::vector<Process> &processes, int no_of_processes, std::string name, int simulationTime)
{
    std::cout << name;
    for (int i = 0; i <= simulationTime; ++i)
    {
        std::cout << i % 10 << " ";
    }
    std::cout << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    for (const auto &process : processes)
    {
        std::cout << process.name << "     ";
        for (int t = 0; t < simulationTime; t++)
        {
            std::cout << "|" << process.timeline[t];
        }
        std::cout << "| \n";
    }
    std::cout << "------------------------------------------------\n";
    std::cout << "\n";
}

void statPrint(std::vector<Process> &processes, int no_of_processes, std::string name)
{
    std::cout << name << "\n";
    std::cout << "Process" << "    ";
    for (const auto &process : processes)
    {
        std::cout << "|" << "  " << process.name << "  ";
    }
    std::cout << "|" << "\n"
              << "Arrival" << "    ";
    for (const auto &process : processes)
    {
        printCenteredInt(process.arrivalTime, 5);
    }
    std::cout << "|" << "\n"
              << "Service" << "    ";
    for (const auto &process : processes)
    {
        printCenteredInt(process.serviceTime, 5);
    }
    std::cout << "|" << " Mean|" << "\n"
              << "Finish" << "     ";
    for (const auto &process : processes)
    {
        printCenteredInt(process.finishTime, 5);
    }
    std::cout << "|" << "-----" << "|" << "\n"
              << "Turnaround" << " ";
    float sum = 0;
    for (const auto &process : processes)
    {
        sum = sum + process.finishTime - process.arrivalTime;
        printCenteredInt(process.finishTime - process.arrivalTime, 5);
    }
    float result = sum / no_of_processes;
    printCenteredFloat(result, 5);
    std::cout << "|" << "\n"
              << "NormTurn" << "   ";
    sum = 0;
    for (const auto &process : processes)
    {
        sum = sum + float(float((process.finishTime - process.arrivalTime)) / float(process.serviceTime));
        printCenteredFloat(float((float(process.finishTime) - float(process.arrivalTime)) / float(process.serviceTime)), 5);
    }
    result = sum / no_of_processes;
    printCenteredFloat(result, 5);
    std::cout << "|\n\n";
}

void FCFS(std::vector<Process> &processes, std::string mode, int simulationTime, int no_of_processes)
{
    int currentTime = 0;
    int maxTime = simulationTime;
    std::vector<std::string> timeline(maxTime, " ");
    if (mode == "trace")
    {
        std::cout << "FCFS  ";
        for (int i = 0; i <= maxTime; ++i)
        {
            std::cout << i % 10 << " ";
        }
        std::cout << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    for (auto &p : processes)
    {

        if (currentTime < p.arrivalTime)
        {
            currentTime = p.arrivalTime; // If the current time is less than the arrival time, jump to arrival time
        }

        p.startTime = currentTime;
        p.finishTime = currentTime + p.serviceTime;
        currentTime = p.finishTime;

        if (p.arrivalTime < p.startTime)
        { // put "." in the intervals where the cpu is not free and this process has already arrived
            for (int i = p.arrivalTime; i <= p.startTime; i++)
            {
                timeline[i] = ".";
            }
        }

        for (int i = p.startTime; i < p.startTime + p.serviceTime; i++)
        { // put "*" from start time along the service time
            timeline[i] = "*";
        }

        if (mode == "trace")
        {
            std::cout << p.name << "     " << "|";

            for (int t = 0; t <= maxTime - 1; ++t)
            { // print the string
                std::cout << timeline[t] << "|";
            }
            std::cout << " \n";

            for (int t = 0; t <= maxTime - 1; ++t)
            { // reset the timeline string
                timeline[t] = " ";
            }
        }
    }
    if (mode == "stats")
    {
        statPrint(processes, no_of_processes, "FCFS");
    }
    else
    {
        std::cout << "------------------------------------------------\n";
        std::cout << "\n";
    }
}

bool comp(Process *a, Process *b)
{
    return a->serviceTime < b->serviceTime;
}

void SPN(std::vector<Process> &processes, std::string mode, int simulationTime, int no_of_processes)
{
    int currentTime = 0;
    int completedProcesses = 0;
    std::vector<std::string> timeline(simulationTime, " ");
    std::vector<Process *> x;

    for (auto &process : processes)
    {
        process.timeline.resize(simulationTime, ' ');
        process.startTime = -1;
    }

    while (completedProcesses < no_of_processes)
    {

        for (int i = 0; i < no_of_processes; i++)
        {
            if (processes[i].arrivalTime <= currentTime && processes[i].startTime == -1)
            {
                x.push_back(&processes[i]);
            }
        }

        // sort vector x according to arrival time
        std::sort(x.begin(), x.end(), comp);

        for (int i = 0; i < x.size(); i++)
        {
            if (!x.empty())
            {

                Process *currentProcess = x.front();
                x.erase(x.begin());

                currentProcess->startTime = currentTime;
                for (int i = 0; i < currentProcess->serviceTime; ++i)
                {
                    currentProcess->timeline[currentTime] = '*';

                    for (auto &process : processes)
                    {
                        if (process.startTime == -1 && process.arrivalTime <= currentTime)
                        {
                            process.timeline[currentTime] = '.';
                        }
                    }

                    currentTime++;
                }
                currentProcess->finishTime = currentTime;
                completedProcesses++;
            }
            else
            {
                for (auto &process : processes)
                {
                    if (process.arrivalTime <= currentTime && process.startTime == -1)
                    {
                        process.timeline[currentTime] = '.';
                    }
                }
                currentTime++;
            }
        }
    }

    if (mode == "trace")
    {
        tracePrint(processes, no_of_processes, "SPN   ", simulationTime);
    }
    else if (mode == "stats")
    {
        statPrint(processes, no_of_processes, "SPN");
    }
}

bool highestResponseRatio(Process *a, Process *b)
{
    return a->ratio > b->ratio;
}

void HRRN(std::vector<Process> &processes, std::string mode, int simulationTime, int no_of_processes)
{

    int currentTime = 0;
    int completedProcesses = 0;
    std::vector<Process *> x;
    std::vector<std::string> timeline(simulationTime, " ");

    for (auto &process : processes)
    {
        process.timeline.resize(simulationTime, ' ');
        process.startTime = -1;
    }

    while (completedProcesses < no_of_processes)
    {

        for (int i = 0; i < no_of_processes; i++)
        {
            if (processes[i].arrivalTime <= currentTime && processes[i].startTime == -1)
            {
                x.push_back(&processes[i]);
                processes[i].waitTime = currentTime - processes[i].arrivalTime;
                processes[i].ratio = float((processes[i].waitTime + processes[i].serviceTime) / float(processes[i].serviceTime));
            }
        }

        // sort vector x according to response ratio
        std::sort(x.begin(), x.end(), highestResponseRatio);

        if (!x.empty())
        {
            Process *currentProcess = x.front();
            x.erase(x.begin());

            currentProcess->startTime = currentTime;
            for (int i = 0; i < currentProcess->serviceTime; ++i)
            {
                currentProcess->timeline[currentTime] = '*';

                for (auto &process : processes)
                {
                    if (process.startTime == -1 && process.arrivalTime <= currentTime)
                    {
                        process.timeline[currentTime] = '.';
                    }
                }

                currentTime++;
            }
            currentProcess->finishTime = currentTime;
            completedProcesses++;
            x.clear();
        }
        else
        {
            for (auto &process : processes)
            {
                if (process.arrivalTime <= currentTime && process.startTime == -1)
                {
                    process.timeline[currentTime] = '.';
                }
            }
            currentTime++;
        }
    }

    if (mode == "trace")
    {
        tracePrint(processes, no_of_processes, "HRRN  ", simulationTime);
    }
    else if (mode == "stats")
    {
        statPrint(processes, no_of_processes, "HRRN");
    }
}

bool shortestRemainingTime(Process *a, Process *b)
{
    return a->remainingTime < b->remainingTime;
}

void SRT(std::vector<Process> &processes, std::string mode, int simulationTime, int no_of_processes)
{
    int currentTime = 0;
    int completedProcesses = 0;
    std::vector<std::string> timeline(simulationTime, " ");
    std::vector<Process *> readyQueue;

    for (auto &process : processes)
    {
        process.timeline.resize(simulationTime, ' ');
        process.startTime = -1;
        process.remainingTime = process.serviceTime;
    }

    while (completedProcesses < no_of_processes)
    {
        for (auto &process : processes)
        {
            if (process.arrivalTime <= currentTime && process.startTime == -1)
            {
                readyQueue.push_back(&process);
                process.startTime = 0;
            }
        }

        readyQueue.erase(
            std::remove_if(readyQueue.begin(), readyQueue.end(),
                           [](Process *p)
                           { return p->remainingTime <= 0; }),
            readyQueue.end());

        std::sort(readyQueue.begin(), readyQueue.end(), shortestRemainingTime);

        if (!readyQueue.empty())
        {
            Process *currentProcess = readyQueue.front();

            currentProcess->timeline[currentTime] = '*';
            currentProcess->remainingTime--;

            for (auto &process : processes)
            {
                if (process.startTime == 0 && process.remainingTime > 0 && process.timeline[currentTime] == ' ')
                {
                    process.timeline[currentTime] = '.';
                }
            }

            if (currentProcess->remainingTime == 0)
            {
                currentProcess->finishTime = currentTime + 1;
                completedProcesses++;
            }
        }
        else
        {
            for (auto &process : processes)
            {
                if (process.startTime == 0 && process.remainingTime > 0)
                {
                    process.timeline[currentTime] = '.';
                }
            }
        }

        currentTime++;
    }

    if (mode == "trace")
    {
        tracePrint(processes, no_of_processes, "SRT   ", simulationTime);
    }
    else if (mode == "stats")
    {
        statPrint(processes, no_of_processes, "SRT");
    }
}

void RoundRobin(std::vector<Process> &processes, std::string mode, int simulationTime, int no_of_processes, int quantum)
{
    int mainQ = quantum;
    int currentTime = 0;
    int completedProcesses = 0;
    std::vector<int> temp;
    std::queue<Process *> queue;
    std::vector<std::string> timeline(simulationTime, " ");

    for (auto &process : processes)
    {
        temp.push_back(process.serviceTime);
    }

    for (auto &process : processes)
    {
        process.timeline.resize(simulationTime, ' ');
        process.startTime = -1;
    }

    while (completedProcesses < no_of_processes)
    {
        for (auto &process : processes)
        {
            if (process.arrivalTime <= currentTime && process.startTime == -1)
            {
                queue.push(&process);
                process.startTime = currentTime;
            }
        }

        if (!queue.empty())
        {
            Process *currentProcess = queue.front();
            queue.pop();
            quantum = mainQ;

            while (quantum > 0 && currentProcess->serviceTime > 0)
            {
                currentProcess->timeline[currentTime] = '*';

                for (auto &process : processes)
                {
                    if (process.serviceTime > 0 && &process != currentProcess && process.arrivalTime <= currentTime)
                    {
                        process.timeline[currentTime] = '.';
                    }
                }

                currentTime++;
                currentProcess->serviceTime--;
                quantum--;

                for (auto &process : processes)
                {
                    if (process.arrivalTime == currentTime && process.startTime == -1)
                    {
                        queue.push(&process);
                        process.startTime = currentTime;
                    }
                }
            }

            if (currentProcess->serviceTime > 0)
            {
                queue.push(currentProcess);
            }
            else
            {
                currentProcess->finishTime = currentTime;
                completedProcesses++;
            }
        }
        else
        {
            currentTime++;
        }
    }

    if (mode == "trace")
    {
        std::cout << "RR-" << mainQ << "  ";
        for (int i = 0; i <= simulationTime; ++i)
        {
            std::cout << i % 10 << " ";
        }
        std::cout << std::endl;
        std::cout << "------------------------------------------------" << std::endl;

        for (const auto &process : processes)
        {
            std::cout << process.name << "     ";
            for (int t = 0; t < simulationTime; t++)
            {
                std::cout << "|" << process.timeline[t];
            }
            std::cout << "| \n";
        }
        std::cout << "------------------------------------------------\n";
        std::cout << "\n";
    }
    else if (mode == "stats")
    {
        for (int i = 0; i < no_of_processes; i++)
        {
            processes[i].serviceTime = temp[i];
        }
        statPrint(processes, no_of_processes, "RR-" + std::to_string(mainQ));
    }
}

void FB1(std::vector<Process> &processes, std::string mode, int simulationTime, int no_of_processes)
{
    int currentTime = 0;
    int completedProcesses = 0;
    bool flag = 0;
    std::vector<int> temp;
    std::vector<std::vector<Process *>> listOfQueues;
    listOfQueues.emplace_back();
    std::vector<std::string> timeline(simulationTime, " ");

    for (auto &process : processes)
    {
        temp.push_back(process.serviceTime);
    }

    for (auto &process : processes)
    {
        process.timeline.resize(simulationTime, ' ');
        process.startTime = -1;
        process.i = -1;
        process.quantum = 0;
    }

    while (completedProcesses < no_of_processes)
    {
        for (auto &process : processes)
        {
            if (process.arrivalTime <= currentTime && process.startTime == -1 && process.i == -1)
            {
                listOfQueues[0].push_back(&process);
                process.i = 0;
            }
        }

        for (int k = 0; k < listOfQueues.size(); k++)
        {
            if (!listOfQueues[k].empty())
            {
                Process *currentProcess = listOfQueues[k].front();

                currentProcess->startTime = currentTime;

                currentProcess->timeline[currentTime] = '*';
                currentTime++;
                currentProcess->serviceTime--;

                for (auto &queue : listOfQueues)
                {
                    for (auto &processPtr : queue)
                    {
                        if (processPtr->serviceTime > 0 && processPtr->startTime != currentTime)
                        {
                            processPtr->timeline[currentTime] = '.';
                        }
                    }
                }

                for (auto &process : processes)
                {
                    if (process.arrivalTime <= currentTime && process.startTime == -1 && process.i == -1)
                    {
                        flag = 1;
                    }
                }

                if (currentProcess->serviceTime > 0 && flag == 1)
                {
                    listOfQueues[k].erase(listOfQueues[k].begin());

                    if ((k + 1) >= listOfQueues.size())
                    {
                        listOfQueues.emplace_back();
                    }
                    listOfQueues[k + 1].push_back(currentProcess);
                }
                else if (currentProcess->serviceTime == 0)
                {
                    currentProcess->finishTime = currentTime;
                    completedProcesses++;
                    listOfQueues[k].erase(listOfQueues[k].begin());
                }
                break;
            }
        }
    }

    //--------------------------------------------------------------------------

    if (mode == "trace")
    {
        tracePrint(processes, no_of_processes, "FB-1  ", simulationTime);
    }
    else if (mode == "stats")
    {
        for (int i = 0; i < no_of_processes; i++)
        {
            processes[i].serviceTime = temp[i];
        }
        statPrint(processes, no_of_processes, "FB-1");
    }
}

void FB2i(std::vector<Process> &processes, std::string mode, int simulationTime, int no_of_processes)
{
    int currentTime = 0;
    int completedProcesses = 0;
    bool flag = 0;
    std::vector<int> temp;
    std::vector<std::vector<Process *>> listOfQueues;
    listOfQueues.emplace_back();
    std::vector<std::string> timeline(simulationTime, " ");

    for (auto &process : processes)
    {
        temp.push_back(process.serviceTime);
    }

    for (auto &process : processes)
    {
        process.timeline.resize(simulationTime, ' ');
        process.startTime = -1;
        process.i = -1;
        process.quantum = 0;
    }

    while (completedProcesses < no_of_processes)
    {
        for (auto &process : processes)
        {
            if (process.arrivalTime <= currentTime && process.startTime == -1 && process.i == -1)
            {
                listOfQueues[0].push_back(&process);
                process.i = 0;
                process.quantum = 1;
            }
        }

        for (int k = 0; k < listOfQueues.size(); k++)
        {
            if (!listOfQueues[k].empty())
            {
                Process *currentProcess = listOfQueues[k].front();

                currentProcess->startTime = currentTime;

                while (currentProcess->quantum > 0 && currentProcess->serviceTime > 0)
                {
                    currentProcess->timeline[currentTime] = '*';

                    for (auto &process : processes)
                    {
                        if (process.serviceTime > 0 && &process != currentProcess && process.arrivalTime <= currentTime)
                        {
                            process.timeline[currentTime] = '.';
                        }
                    }
                    currentTime++;
                    currentProcess->serviceTime--;
                    currentProcess->quantum--;
                }

                for (auto &process : processes)
                {
                    if (process.arrivalTime <= currentTime && process.startTime == -1 && process.i == -1)
                    {
                        flag = 1;
                    }
                }

                if (currentProcess->serviceTime > 0 && flag == 1)
                {
                    currentProcess->i++;
                    currentProcess->quantum = pow(2, currentProcess->i);
                    listOfQueues[k].erase(listOfQueues[k].begin());

                    if ((k + 1) >= listOfQueues.size())
                    {
                        listOfQueues.emplace_back();
                    }
                    listOfQueues[k + 1].push_back(currentProcess);
                }
                else if (currentProcess->serviceTime <= 0)
                {
                    currentProcess->finishTime = currentTime;
                    completedProcesses++;
                    listOfQueues[k].erase(listOfQueues[k].begin());
                }
                else
                {
                    currentProcess->quantum = pow(2, currentProcess->i);
                }
                break;
            }
        }
    }

    //--------------------------------------------------------------------------

    if (mode == "trace")
    {
        tracePrint(processes, no_of_processes, "FB-2i ", simulationTime);
    }
    else if (mode == "stats")
    {
        for (int i = 0; i < no_of_processes; i++)
        {
            processes[i].serviceTime = temp[i];
        }
        statPrint(processes, no_of_processes, "FB-2i");
    }
}

int main()
{
    std::string mode;
    std::vector<std::string> policies;
    int simulationTime;
    int no_of_processes;
    std::vector<Process> processes;
    std::string num;
    std::vector<int> quantum;

    std::string line;

    //Mode
    std::getline(std::cin, mode);

    //Policy
    std::getline(std::cin, line);
    std::stringstream ss(line);
    std::string policy;

    while (std::getline(ss, policy, ','))
    {
        if (policy.find('-') != std::string::npos)
        {
            std::stringstream ss_policy(policy);
            std::string policy_number;
            std::string quantum_value;
            std::getline(ss_policy, policy_number, '-');
            std::getline(ss_policy, quantum_value);
            policies.push_back(policy_number);
            quantum.push_back(std::stoi(quantum_value));
        }
        else
        {
            policies.push_back(policy);
            quantum.push_back(-1);
        }
    }

    /*
        std::cout << "Policies extracted:" << std::endl;
        for (size_t i = 0; i < policies.size(); ++i) {
            std::cout << "Policy: " << policies[i] << ", Quantum: " << quantum[i] << std::endl;
        }
    */

    //Simulation time
    std::cin >> simulationTime;
    std::cin.ignore();

    //Number of processes
    std::cin >> no_of_processes;
    std::cin.ignore();

    //Process details
    for (int i = 0; i < no_of_processes; ++i)
    {
        Process p;
        std::getline(std::cin, line);
        ss.clear();
        ss.str(line);
        std::getline(ss, line, ',');
        p.name = line[0];
        std::getline(ss, line, ',');
        p.arrivalTime = std::stoi(line);
        std::getline(ss, line, ',');
        p.serviceTime = std::stoi(line);
        p.startTime = -1;
        p.finishTime = -1;
        processes.push_back(p);
    }


    for (int i = 0; i < policies.size(); ++i)
    {
        const std::string &policy = policies[i];
        // std::cout << "Policy: " << policy << ", Quantum: " << quantum[i] << "\n";

        if (policy == "1")
        {
            FCFS(processes, mode, simulationTime, no_of_processes);
        }
        else if (policy == "2")
        {
            RoundRobin(processes, mode, simulationTime, no_of_processes, quantum[i]);
        }
        else if (policy == "3")
        {
            SPN(processes, mode, simulationTime, no_of_processes);
        }
        else if (policy == "4")
        {
            SRT(processes, mode, simulationTime, no_of_processes);
        }
        else if (policy == "5")
        {
            HRRN(processes, mode, simulationTime, no_of_processes);
        }
        else if (policy == "6")
        {
            FB1(processes, mode, simulationTime, no_of_processes);
        }
        else if (policy == "7")
        {
            FB2i(processes, mode, simulationTime, no_of_processes);
        }
    }

    return 0;
}
