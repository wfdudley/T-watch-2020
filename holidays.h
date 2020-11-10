// holidays.h
// edit this to add your own holidays
// William F. Dudley Jr. 2020 11 09
#ifndef __MYHOLI_H__
#define __MYHOLI_H__

#ifdef __HOLIMAIN__
#define EXTERN
#define INIT(x) = x
#else
#define EXTERN extern
#define INIT(x)
#endif

typedef struct holiday {
    uint16_t month;
    uint16_t fixed_day;
    uint16_t min_day;
    uint16_t max_day;
    uint16_t day_of_week;
    char *name;
} HOLIDAY;

EXTERN HOLIDAY Holidays[]
#ifdef __HOLIMAIN__
= {
    {  1,  1,  0,  0, 0, "New Year's Day"   },
    {  2,  0, 15, 21, 1, "President's Day" },
    {  5,  0, 24, 30, 1, "Memorial Day" },
    {  7,  4,  0,  0, 0, "Independence Day" },
    {  9,  0,  1,  7, 1, "Labor Day" },
    { 11,  0, 22, 28, 4, "Thanksgiving" },
    { 12, 25,  0,  0, 0, "Christmas Day" }
}
#endif
;

EXTERN int sizeof_Holidays INIT(sizeof(Holidays)/sizeof(HOLIDAY));

#endif
