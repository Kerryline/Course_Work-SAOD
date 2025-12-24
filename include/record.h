#ifndef RECORD_H
#define RECORD_H

struct Record {
    char author[12];        
    char title[32];         
    char publisher[16];
    short year;
    short pages;
};

#endif