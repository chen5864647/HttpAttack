#include <iostream>
#include <fstream>
#include <argtable2.h>
#include <string>
#include "run.hpp"

void mymain(int n, int e, int E, const char** strings, int nstrings) {

    std::cout << "option -n = " << ((n) ? "YES" : "NO") << std::endl;
    std::cout << "option -e = " << ((e) ? "YES" : "NO") << std::endl;
    std::cout << "option -E = " << ((E) ? "YES" : "NO") << std::endl;
    
    
    for (int j = 0; j < nstrings; j++)
        std::cout << strings[j] << " ";

    std::cout << std::endl;
}

int main(int argc, char **argv) {

    struct arg_lit *n     = arg_lit0("n", NULL,         "do not output the trailing newline");
    struct arg_lit *e     = arg_lit0("e", NULL,         "enable interpretation of the backslash-escaped characters listed below");
    struct arg_lit *E     = arg_lit0("E", NULL,         "disable interpretation of those sequences in <string>s");
    struct arg_lit *help  = arg_lit0(NULL,"help",       "print this help and exit");
    struct arg_lit *vers  = arg_lit0(NULL,"version",    "print version information and exit");
    struct arg_str *strs  = arg_strn(NULL,NULL,"STRING",0,argc+2,NULL);
    struct arg_end *end   = arg_end(20);

    work();

    return 0;
}