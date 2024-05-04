#ifndef QMEM_H
#define QMEM_H

#include "../qhash/qhash.hpp"
#include "../qbase/qbase.hpp"

#include <thread>
#include <vector>
#include <mutex>

namespace qmem {

    /*
    /**************************************************************************************************************
    *																											 *
    *  QMem is a wrapper class around qhash which can verify memory region integrity at runtime                  *
    *																											 *
    ***************************************************************************************************************
    */

#pragma region Constants

#define WATCH_INACTIVE 0x00

#define WATCH_ACTIVE 0x01

#pragma endregion

#pragma region Globals

    extern std::mutex callback_mutex;

#pragma endregion

#pragma region Type Definitions

    typedef struct qregion_t {

        /* r/w access mutex */
        std::mutex region_mutex;

        /* r/w flag */
        bool region_lock;

        /* memory address of allocation */
        void_t address;
        
#ifdef _WIN64 

        /* current hash of the data as it should be */
        qhash::qhash64_t hash_o{};

        /* hash of the data as per the last integrity violation */
        qhash::qhash64_t hash_v{};

#elif defined(_WIN32)

        /* current hash of the data as it should be */
        qhash::qhash32_t hash_o{};

        /* hash of the data as per the last integrity violation */
        qhash::qhash32_t hash_v{};

#endif

        /* sizeof(allocation) */
        size_t size;

        /* addressof(qwatcher_instance) */
        void* watcher;

        /* index in parent(watcher) vector */
        size_t index;

        qregion_t(){}

        qregion_t(const qmem::qregion_t& other) {
            if (this == &other)
                return;

            std::lock_guard<std::mutex> lock(region_mutex);
            std::lock_guard<std::mutex> lockinst(*const_cast<std::mutex*>(&other.region_mutex));

            // Copy other member variables
            region_lock = other.region_lock;
            address = other.address;
            hash_o = other.hash_o;
            hash_v = other.hash_v;
            size = other.size;
            watcher = other.watcher;
            index = other.index;

        }

        qregion_t& operator=(const qregion_t& other) {
            if (this == &other)
                return *this;

            std::lock_guard<std::mutex> lock(region_mutex);
            std::lock_guard<std::mutex> lockinst(*const_cast<std::mutex*>(&other.region_mutex));

            // Copy other member variables
            region_lock = other.region_lock;
            address = other.address;
            hash_o = other.hash_o;
            hash_v = other.hash_v;
            size = other.size;
            watcher = other.watcher;
            index = other.index;

            return *this;
        }

        ~qregion_t() {
            std::lock_guard<std::mutex> lock(region_mutex);

        }
    };

#pragma endregion

    class qwatcher {

    private:

#pragma region Globals

        /* thread in which integrity scans occur */
        std::thread watch_thr;

        /* flag to stop / run watch thread  */
        char watch_state = WATCH_INACTIVE;

        /* watch_lock will be set to false when performing memory operations on our target memory area and switched back to true following */
        bool watch_lock = false;

        /* mutex for accessing regions vector */
        public: std::mutex mtx{};

        /* callback function for when rogue memory access occurs  */
        qcallback::qmem_exception_rogue_c callback;

        /* memory regions guarded by this thread / instance */
        std::vector<qregion_t> regions{};

#pragma endregion

#pragma region Thread Callbacks

         void do_watch() {

            std::chrono::microseconds thread_sleep_time(1);

            while (watch_state) {
                /* lower cpu usage, might need to raise this eventually */
                std::this_thread::sleep_for(thread_sleep_time);
                /* vector may be in process of modification right now, wait  */
                std::lock_guard<std::mutex> lock(mtx); 

                for (auto& entry : regions) {
                    /* memory region may be in process of modification */
                    std::lock_guard<std::mutex> lock_i(entry.region_mutex);

                    /* in - thread sanity check */
                    if (!entry.address)
                        continue;
                    if (!entry.size)
                        continue;

                    /* check if we are supposed to scan memory at this time */
                    if (!entry.region_lock)
                        continue;

#ifdef _WIN64

                    auto hash_i = qhash::qhash64(reinterpret_cast<void*>(entry.address), entry.size);

#elif defined(_WIN32)

                    auto hash_i = qhash::qhash32(reinterpret_cast<void*>(entry.address), entry.size);

#endif

                    /* data hashes match, move to next iteration */
                    if (hash_i.hash == entry.hash_o.hash) {
                        continue;
                    }

                    /* raise callback indicating altered memory if this is not the same violation as the previous occurence */
                    if (entry.hash_v.hash == hash_i.hash)
                        continue;

                    entry.hash_v = std::move(hash_i);

                    std::lock_guard<std::mutex> lock(callback_mutex);

                    callback(qexcept::q_rogueaccess(entry.hash_o.hash, entry.hash_v.hash), reinterpret_cast<void*>(entry.address)); // TODO add condition for previous rogue alteration to ensure this doesn't infinitely repeat when triggered
                }
            }

        }

#pragma endregion

#pragma region Helper Methods

        __forceinline void update_hash(qregion_t& instance) {

            if (!instance.address)
                return;
            if (!instance.size)
                return;

#ifdef _WIN64

            instance.hash_o = qhash::qhash64(reinterpret_cast<void*>(instance.address), instance.size);

#elif defined(_WIN32)

            instance.hash_o = qhash::qhash32(reinterpret_cast<void*>(instance.address), instance.size);

#endif

        }


#pragma endregion

    public:

#pragma region Property Accessors
        /* This section is mainly used for manual debugging */

        __forceinline size_t __fastcall work_queue_entry_count() {
            lock_vector_watch();

            return regions.size();

            unlock_vector_watch();
        }

#pragma endregion

#pragma region Watch Control

          __forceinline uintptr_t __fastcall append_memory_region(void* address, size_t size) {
            unlock_vector_watch();

            qregion_t region{};
            region.address = reinterpret_cast<void_t>(address);
            region.size = size;
            region.index = regions.size();
            region.region_lock = true;
            region.watcher = this;
            
            regions.push_back(region);

            update_hash(region);

            lock_vector_watch();
            return region.index;
         }

         __forceinline void __fastcall remove_memory_region(void* address, size_t size) {
             unlock_vector_watch();

            auto count = regions.size();
            if (!count) {
                lock_vector_watch();
                return;
            }

            uintptr_t remove_index = NULL;
            bool index_set = false;

            for (auto i = 0; i < regions.size(); ++i) {
                if (index_set) {
                    --regions[i].index; // relocate index table
                    continue;
                }
                if (regions[i].address  == reinterpret_cast<void_t>(address)) {
                    remove_index = i;
                    index_set = true;
                    continue;
                }
                
            }

            try {
                if(index_set)
                    if(remove_index < count)
                        regions.erase(regions.begin() + remove_index);
            }
            catch (std::exception& except) {}

            lock_vector_watch();
         }

         void __fastcall remove_memory_region(uintptr_t index) {
             unlock_vector_watch();

             auto count = regions.size();

             if (!count || count <= index){
                 lock_vector_watch();
                 return;
             }

             if(count >= (index + 1))
                for (auto i = (index+1); i < regions.size(); ++i)
                    --regions[i].index; // relocate index table

             try {
                 if(index < count)
                    regions.erase(regions.begin() + index);
             }
             catch (std::exception& except) { }

             lock_vector_watch();
         }

#pragma endregion

#pragma region Global Vector Accesors

        __forceinline void unlock_vector_watch() {
            /* region vector is flagged for modification, wait until this is complete in order to continue */
            std::lock_guard<std::mutex> lock(mtx);

            /* pause memory scan */
            watch_lock = false;
        }

        __forceinline void lock_vector_watch() {
            /* region vector is flagged for modification, wait until this is complete in order to continue */
            std::lock_guard<std::mutex> lock(mtx);

            /* pause memory scan */
            watch_lock = true;
        }

#pragma endregion

#pragma region Global Work Accessors

        __forceinline void unlock_region_watch(qregion_t& region ) {
            unlock_vector_watch();

            /* region data is flagged for modification, wait until this is complete in order to continue */
            std::lock_guard<std::mutex> lock_i(region.region_mutex);

            /* pause regions memory scan */
            region.region_lock = false;
        }

        __forceinline void __fastcall unlock_region_watch(uintptr_t index) {
            unlock_region_watch(regions[index]);
        }

        __forceinline void __fastcall lock_region_watch(qregion_t& instance, bool update) {
            /* region list flagged for modification, wait until this is complete in order to continue */
            lock_vector_watch();
            /* aquire region lock for instance */
            std::lock_guard<std::mutex> lock_i(instance.region_mutex);

            if(update)
                update_hash(instance);

            /* resume scanning memory */
            instance.region_lock = true;
        }
        /* size of allocation changed */
        __forceinline void __fastcall lock_region_watch(qregion_t& instance, size_t size ) {
            instance.size = size;

            lock_region_watch(instance, true);
        }

        /* pointer to allocation changed */
        __forceinline void __fastcall lock_region_watch(qregion_t& instance, void* allocation_ ) {
            instance.address = reinterpret_cast<void_t>(allocation_);

            lock_region_watch(instance, true);
        }

        /* both of the above have changed */
        __forceinline void __fastcall lock_region_watch(qregion_t& instance, void* allocation_, size_t size) {


            instance.address = reinterpret_cast<void_t>(allocation_);
            instance.size = size;

            lock_region_watch(instance, true);
        }

        __forceinline void __fastcall lock_region_watch(uintptr_t index, bool update) {
            lock_region_watch(regions[index], update);
        }

        __forceinline void __fastcall lock_region_watch(uintptr_t index, void* allocation_, size_t size) {
            lock_region_watch(regions[index], allocation_, size);
        }

#pragma endregion

#pragma region Dtor

        __forceinline void destroy_watch() {

            watch_lock = false;

            watch_state = WATCH_INACTIVE;

            for (auto& entry : regions)
                entry.region_lock = false;

        }

#pragma endregion

#pragma region Ctor

        __forceinline qwatcher() {}

        __forceinline __fastcall qwatcher(qcallback::qmem_exception_rogue_c& callback_) {

            callback = callback_;

            watch_state = WATCH_ACTIVE;
            watch_lock = true;

            watch_thr = std::thread(&qwatcher::do_watch, this);
            watch_thr.detach();
        }

        __forceinline __fastcall qwatcher(qcallback::qmem_exception_rogue_c&& callback_) {

            callback = callback_;

            watch_state = WATCH_ACTIVE;
            watch_lock = true;

            watch_thr = std::thread(&qwatcher::do_watch, this);
            watch_thr.detach();
        }

        __forceinline __fastcall qwatcher(const qmem::qwatcher& other) { 
            if (this == &other)
                return;

            std::lock_guard<std::mutex> lock(mtx);
            std::lock_guard<std::mutex> lockinst(*const_cast<std::mutex*>(&other.mtx));

            // Copy other member variables
            regions = other.regions;
            watch_state = other.watch_state;
            watch_lock = other.watch_lock;
            callback = other.callback;

        }

        __forceinline qwatcher& __fastcall operator=(const qwatcher& other){
            if (this == &other)
                return *this;

            std::lock_guard<std::mutex> lock(mtx);
            std::lock_guard<std::mutex> lockinst(*const_cast<std::mutex*>(&other.mtx));

            // Copy other member variables
            regions = other.regions;
            watch_state = other.watch_state;
            watch_lock = other.watch_lock;
            callback = other.callback;

            return *this;
        }

#pragma endregion
        __forceinline ~qwatcher() {
            try {
                std::lock_guard<std::mutex> lock(mtx);

                for (auto& entry : regions)
                    std::lock_guard<std::mutex> lock_r(entry.region_mutex); // aquire all mutex's to avoid invalid access during destruction

                destroy_watch();

            }catch(std::exception& except){}
        }
    };

}

#endif