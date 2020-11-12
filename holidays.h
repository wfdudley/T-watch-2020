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

typedef struct reminder {
    uint16_t month;
    uint16_t fixed_day;
    uint16_t min_day;
    uint16_t max_day;
    uint16_t day_of_week;
    char *name;
} REMINDER;

// edit these to add any holidays you celebrate
EXTERN REMINDER Holidays[]
#ifdef __HOLIMAIN__
= {
    {  1,  1,  0,  0, 0, "New Year's Day" },
    {  1,  0, 15, 21, 1, "Martin Luther King Jr. Day" },
    {  2,  0, 15, 21, 1, "Washington's Birthday" },
    {  5,  0, 24, 31, 1, "Memorial Day" },
    {  7,  4,  0,  0, 0, "Independence Day" },
    {  9,  0,  1,  7, 1, "Labor Day" },
    { 10,  0,  8, 14, 1, "Columbus Day" },
    { 11, 11,  0,  0, 4, "Veteran's Day" },
    { 11,  0, 22, 28, 4, "Thanksgiving" },
    { 12, 25,  0,  0, 0, "Christmas Day" }
}
#endif
;

/* week day range
      1  1- 7
      2  8-14
      3 15-21
      4 22-28
 */

// Edit these to put in your own birthdays/anniversarys/etc.
EXTERN REMINDER Birthdays[]
#ifdef __HOLIMAIN__
= {
    {  2, 25,  0,  0, 0, "George Harrison's Birthday" },
    {  4, 10,  0,  0, 0, "Steven Seagal's Birthay" },
    {  4, 12,  0,  0, 0, "Beverly Cleary's Birthay" },
    {  5, 17,  0,  0, 0, "Bob Saget's Birthay" },
    {  6,  1,  0,  0, 0, "Marilyn Monroe's Birthay" },
    {  9, 21,  0,  0, 0, "Bill Murray's Birthday" }
}
#endif
;

EXTERN int sizeof_Holidays INIT(sizeof(Holidays)/sizeof(REMINDER));
EXTERN int sizeof_Birthdays INIT(sizeof(Birthdays)/sizeof(REMINDER));

#endif
