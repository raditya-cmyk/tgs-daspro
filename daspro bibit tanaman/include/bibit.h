#ifndef BIBIT_H
#define BIBIT_H

#include <string>
using std::string;

const int ID_LEN = 5;
const int ID_HURUF = 2;
const int NAMA_MAX = 24;
const int HRG_MAX = 10000000;
const float STK_KG_MAX = 1000;
const int KAP = 100;

struct tanaman {
    char id[ID_LEN + 1];
    char nama[NAMA_MAX + 1];
    int hrg;
    float stk_kg;
};

struct katalog {
    tanaman data[KAP];
    int n;
};

string besar(string s);
string kecil(string s);
bool id_valid(const string& id);
bool ke_int(const string& str, int& hasil);
bool ke_float(const string& str, float& hasil);
int cari_id(const katalog& k, const string& id);
void isi_data(katalog& k, const string& id, const string& nama, int hrg, float stk_kg);
void tambah_bibit_baru(katalog& k);
void cari_bibit(const katalog& k);
void urutkan_katalog(katalog& k);
void ubah_hapus_bibit(katalog& k);
void ubah_stok(katalog& k);
void statistik(const katalog& k);
void info_memori(const katalog& k);

#endif
