#ifndef __SPIFF_FNS_H__
#define __SPIFF_FNS_H__

int file_exists(char *);
void build_acc_pts_file (void);
int get_list_ssid(char *, int);
int delete_access_point(char *);
void append_new_access_point (char *, char *, uint32_t);
int read_acc_pts_file_and_compare_with_SSIDs (struct WiFiAp *);

#if SPIFF_WORKS_WITH_config_dot_h
void test_spiffs(void);
void listDir(fs::FS &fs, const char *, uint8_t );
void appendFile(fs::FS &fs, const char *, const char *);
void readFile(fs::FS &fs, const char *);
void renameFile(fs::FS &fs, const char *, const char *);
void writeFile(fs::FS &fs, const char *, const char *);
void deleteFile(fs::FS &fs, const char *);
void testFileIO(fs::FS &fs, const char *);
#endif

#endif
