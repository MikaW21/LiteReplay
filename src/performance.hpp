#pragma once
#include "common.hpp"
#include "monitor_math.hpp"
#include <psapi.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <condition_variable>
namespace lr {
struct PerformanceSnapshot {
    double systemCpu=-1, processCpu=-1, systemGpu=-1, processGpu=-1, encodeGpu=-1;
    uint64_t usedRam=0,totalRam=0,privateRam=0,workingRam=0;
    bool ramValid=false,processRamValid=false;
    unsigned long long sampledAt=0;
};
class Performance {
    std::atomic<bool> stop_{false};
    std::mutex mutex_;
    PerformanceSnapshot snapshot_;
    std::thread worker_;
    std::mutex waitMutex_;
    std::condition_variable wake_;
    void nap(int ms){std::unique_lock<std::mutex> lock(waitMutex_);wake_.wait_for(lock,std::chrono::milliseconds(ms),[this]{return stop_.load();});}
    static uint64_t time(FILETIME t){return uint64_t(t.dwLowDateTime)|(uint64_t(t.dwHighDateTime)<<32);}
    void run(){
        PDH_HQUERY query=nullptr;PDH_HCOUNTER gpu=nullptr;
        if(PdhOpenQueryW(nullptr,0,&query)==ERROR_SUCCESS){
            if(PdhAddEnglishCounterW(query,L"\\GPU Engine(*)\\Utilization Percentage",0,&gpu)!=ERROR_SUCCESS)gpu=nullptr;
        }
        struct Close {PDH_HQUERY q;~Close(){if(q)PdhCloseQuery(q);}} close{query};
        bool gpuWarm=false;
        uint64_t oldTotal=0,oldIdle=0,oldProcess=0,oldClock=0;
        bool hadSystem=false,hadProcess=false;
        SYSTEM_INFO sys{};GetSystemInfo(&sys);
        while(!stop_){
            if(!enabled){hadSystem=hadProcess=false;gpuWarm=false;nap(200);continue;}
            PerformanceSnapshot out;
            FILETIME idle{},kernel{},user{},created{},exit{};
            if(GetSystemTimes(&idle,&kernel,&user)){
                auto total=time(kernel)+time(user),free=time(idle);
                if(hadSystem&&total>=oldTotal&&free>=oldIdle)out.systemCpu=cpuBusy(total-oldTotal,free-oldIdle);
                oldTotal=total;oldIdle=free;hadSystem=true;
            }else hadSystem=false;
            const uint64_t now=uint64_t(clock100ns());
            if(GetProcessTimes(GetCurrentProcess(),&created,&exit,&kernel,&user)){
                const auto cpu=time(kernel)+time(user);
                if(hadProcess&&now>oldClock&&cpu>=oldProcess)
                    out.processCpu=std::clamp(100.0*double(cpu-oldProcess)/double(now-oldClock)/sys.dwNumberOfProcessors,0.0,100.0);
                oldProcess=cpu;oldClock=now;hadProcess=true;
            }else hadProcess=false;
            MEMORYSTATUSEX memory{};memory.dwLength=sizeof memory;
            if(GlobalMemoryStatusEx(&memory)&&memory.ullTotalPhys>0){out.ramValid=true;out.totalRam=memory.ullTotalPhys;out.usedRam=memory.ullTotalPhys-memory.ullAvailPhys;}
            PROCESS_MEMORY_COUNTERS_EX pm{};pm.cb=sizeof pm;
            if(GetProcessMemoryInfo(GetCurrentProcess(),reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pm),sizeof pm)){
                out.processRamValid=true;out.privateRam=pm.PrivateUsage;out.workingRam=pm.WorkingSetSize;
            }
            if(gpu&&PdhCollectQueryData(query)==ERROR_SUCCESS){
                DWORD bytes=0,count=0;
                if(PdhGetFormattedCounterArrayW(gpu,PDH_FMT_DOUBLE,&bytes,&count,nullptr)==PDH_MORE_DATA && bytes<16*1024*1024){
                    // Aligned allocation for PDH structs; cap allocation under unusual driver output.
                    std::vector<uint64_t> storage((bytes+7)/8);
                    auto items=reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(storage.data());
                    if(PdhGetFormattedCounterArrayW(gpu,PDH_FMT_DOUBLE,&bytes,&count,items)==ERROR_SUCCESS){
                        GpuUsage usage;
                        for(DWORD i=0;i<count;++i)if(items[i].szName &&
                            (items[i].FmtValue.CStatus==PDH_CSTATUS_VALID_DATA||items[i].FmtValue.CStatus==PDH_CSTATUS_NEW_DATA))
                            usage.add(items[i].szName,items[i].FmtValue.doubleValue,GetCurrentProcessId());
                        if(gpuWarm){out.systemGpu=GpuUsage::peak(usage.system);out.processGpu=GpuUsage::peak(usage.process);out.encodeGpu=GpuUsage::peak(usage.encode);}
                    }
                }
            }
            gpuWarm=true;
            out.sampledAt=GetTickCount64();
            {std::lock_guard<std::mutex> lock(mutex_);snapshot_=out;}
            nap(intervalMs.load());
        }

    }
public:
    std::atomic<bool> enabled{true};
    std::atomic<int> intervalMs{1000};
    Performance(){worker_=std::thread([this]{try{run();}catch(...){std::lock_guard<std::mutex> lock(mutex_);snapshot_={};}});}
    ~Performance(){stop_=true;wake_.notify_one();if(worker_.joinable())worker_.join();}
    PerformanceSnapshot snapshot(){std::lock_guard<std::mutex> lock(mutex_);return snapshot_;}
    static std::wstring percent(double n){if(n<0)return L"暂不可用";std::wostringstream s;s<<std::fixed<<std::setprecision(1)<<n<<L"%";return s.str();}
    std::wstring read(bool detailed=false){
        const auto s=snapshot();
        if(!s.sampledAt)return L"正在读取性能数据…";
        std::wostringstream out;out<<std::fixed<<std::setprecision(1);
        if(detailed){
            out<<L"全机 CPU："<<percent(s.systemCpu)<<L"     LiteReplay CPU："<<percent(s.processCpu)
               <<L"\r\n\r\n全机 GPU 最忙引擎："<<percent(s.systemGpu)
               <<L"\r\nLiteReplay GPU 最忙引擎："<<percent(s.processGpu)
               <<L"\r\nLiteReplay 视频编码引擎："<<percent(s.encodeGpu)<<L"\r\n\r\n";
            if(s.ramValid)out<<L"全机物理内存："<<s.usedRam/1073741824.0<<L" / "<<s.totalRam/1073741824.0<<L" GiB（"<<percent(100.0*s.usedRam/s.totalRam)<<L"）";
            else out<<L"全机物理内存：暂不可用";
            out<<L"\r\n";
            if(s.processRamValid)out<<L"LiteReplay 私有提交内存："<<s.privateRam/1048576.0<<L" MiB\r\nLiteReplay 驻留工作集："<<s.workingRam/1048576.0<<L" MiB";
            else out<<L"LiteReplay 内存：暂不可用";
            out<<L"\r\n\r\n每秒采样；窗口都隐藏/最小化时暂停采样。\r\nGPU 按最忙引擎统计，不将不同引擎直接相加。\r\n暂不可用表示未取得有效计数，不等于 0%。";
        }else{
            out<<L"全机 CPU "<<percent(s.systemCpu)<<L"  |  GPU "<<percent(s.systemGpu)<<L"  |  内存 ";
            if(s.ramValid)out<<percent(100.0*s.usedRam/s.totalRam);else out<<L"暂不可用";
            out<<L"\n本程序 CPU "<<percent(s.processCpu)<<L"  |  私有内存 ";
            if(s.processRamValid)out<<s.privateRam/1048576.0<<L" MiB";else out<<L"暂不可用";
        }
        return out.str();
    }
};
}
