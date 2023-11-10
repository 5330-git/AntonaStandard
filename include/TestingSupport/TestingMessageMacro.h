#ifndef TESTINGMESSAGEMACRO_H
#define TESTINGMESSAGEMACRO_H

    #ifndef TESTING_MESSAGE
        #define TESTING_MESSAGE 
    #endif

    #if defined(ASD_TEST_VERSION)
        #undef TESTING_MESSAGE
        #define TESTING_MESSAGE                     \
        public:                                     \
            static int& getAllocatedObjectCount() {     \
                static int allocated_object_count = 0;     \
                return allocated_object_count;      \
            }                                       \
            void* operator new(size_t size) {       \
                ++getAllocatedObjectCount();        \
                return ::operator new(size);                 \
            }                                       \
            void operator delete(void* ptr) {       \
                --getAllocatedObjectCount();         \
                ::operator delete(ptr);             \
            }                                       \
        private:                                    \


        
    #endif 

#endif