#include <exception>
#include <iostream>
#include <iomanip>
#include <cstring>

std::string datasetPath;

std::string getFullPath(const std::string& name)
{
    return datasetPath + "/" + name;
}

#define ADD_TEST(test_name) \
extern void test_name();

#include "add_tests.h"

#define TERMINAL_RED        "\033[91m"
#define TERMINAL_DEFAULT    "\033[39m"

#undef ADD_TEST
#define ADD_TEST(test_name) " - " << #test_name << std::endl <<

void usage(const char* program)
{
    std::cout << "Usage: " << program << " test_case_name [dataset_path]" << std::endl;
    std::cout << "Known test cases:" << std::endl <<
#include "add_tests.h"
    std::endl;
}

#undef ADD_TEST
#define ADD_TEST(test_name)                                     \
    do {                                                        \
        if (argc >= 2 && !std::strcmp(argv[1], #test_name)) {   \
            try {                                               \
                test_name();                                    \
            } catch (std::exception& e) {                       \
                std::cout << TERMINAL_RED;                      \
                std::cout << e.what() << std::endl;             \
                std::cout << TERMINAL_DEFAULT;                  \
                return -1;                                      \
            }                                                   \
            std::cout << "SUCCESS" << std::endl;                \
            return 0;                                           \
        }                                                       \
    } while (false);


int main(int argc, char** argv)
{
    if (argc < 2) {
        usage(argv[0]);
        return -1;
    }

    if (argc >= 3) {
        datasetPath = argv[2];
    }

    std::cout << std::setprecision(20);

#include "add_tests.h"

    std::cout << TERMINAL_RED;
    std::cout << "Test not found: " << argv[1] << std::endl;
    std::cout << TERMINAL_DEFAULT;
    return -1;
}
