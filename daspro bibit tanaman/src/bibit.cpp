#include "bibit.h"
#include "ui.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cctype>
#include <cstring>
using namespace std;

string besar(string s) {
    for (auto& c : s) c = toupper((unsigned char)c);
    return s;
}

string kecil(string s) {
    for (auto& c : s) c = tolower((unsigned char)c);
    return s;
}

bool id_valid(const string& id) {
    if (id.length() != ID_LEN) return false;
    for (int i = 0; i < ID_LEN; i++) {
        unsigned char c = id[i];
        if (i < ID_HURUF ? !isalpha(c) : !isdigit(c)) return false;
    }
    return true;
}

bool ke_int(const string& str, int& hasil) {
    try {
        size_t pos;
        int v = stoi(str, &pos);
        if (pos != str.size()) return false;
        hasil = v;
        return true;
    } catch (...) {
        return false;
    }
}

bool ke_float(const string& str, float& hasil) {
    try {
        size_t pos;
        float v = stof(str, &pos);
        if (pos != str.size() || !isfinite(v)) return false;
        hasil = v;
        return true;
    } catch (...) {
        return false;
    }
}

int cari_id(const katalog& k, const string& id) {
    string cari = besar(id);
    for (int i = 0; i < k.n; i++) {
        if (cari == k.data[i].id) return i;
    }
    return -1;
}

void isi_data(katalog& k, const string& id, const string& nama, int hrg, float stk_kg) {
    tanaman& t = k.data[k.n++];
    strncpy(t.id, besar(id).c_str(), ID_LEN);
    t.id[ID_LEN] = '\0';
    strncpy(t.nama, nama.c_str(), NAMA_MAX);
    t.nama[NAMA_MAX] = '\0';
    t.hrg = hrg;
    t.stk_kg = stk_kg;
}

void tambah_bibit_baru(katalog& k) {
    string id, nama, str_hrg, str_stk_kg;
    int hrg;
    float stk_kg;

    cout << "Tambah bibit baru :\n";
    if (k.n >= KAP) {
        cout << "Ditolak, katalog penuh (maks " << KAP << " data)" << endl;
        return;
    }

    cout << "Masukan ID, 2 huruf + 3 angka (Blank to exit) : ";
    getline(cin, id);
    if (id.empty()) return;
    if (!id_valid(id)) {
        cout << "Ditolak, ID harus 2 huruf + 3 angka, contoh PD001" << endl;
        return;
    }
    if (cari_id(k, id) != -1) {
        cout << "Ditolak, duplikat id" << endl;
        return;
    }

    cout << "Masukan nama (Blank to exit) : ";
    getline(cin, nama);
    if (nama.empty()) return;
    if (nama.find(',') != string::npos) {
        cout << "Ditolak, nama mengandung koma" << endl;
        return;
    }
    if (nama.length() > NAMA_MAX) {
        cout << "Ditolak, nama lebih dari " << NAMA_MAX << " karakter" << endl;
        return;
    }

    cout << "Masukan harga/KG (Blank to exit) : ";
    getline(cin, str_hrg);
    if (str_hrg.empty()) return;
    if (!ke_int(str_hrg, hrg)) {
        cout << "Ditolak, format angka salah" << endl;
        return;
    }
    if (hrg < 0 || hrg > HRG_MAX) {
        cout << "Ditolak, harga di luar rentang (" << str_hrg << ")" << endl;
        return;
    }

    cout << "Masukan stok/KG (Blank to exit) : ";
    getline(cin, str_stk_kg);
    if (str_stk_kg.empty()) return;
    if (!ke_float(str_stk_kg, stk_kg)) {
        cout << "Ditolak, format angka salah" << endl;
        return;
    }
    if (stk_kg < 0 || stk_kg > STK_KG_MAX) {
        cout << "Ditolak, stok/kg di luar rentang (" << str_stk_kg << ")" << endl;
        return;
    }

    isi_data(k, id, nama, hrg, stk_kg);
    cout << "Bibit " << nama << " berhasil ditambahkan" << endl;
}

void cari_bibit(const katalog& k) {
    string input;

    cout << "Cari bibit :\n";
    cout << "Masukan ID atau nama, huruf besar/kecil bebas (Blank to exit) : ";
    getline(cin, input);
    if (input.empty()) return;

    string key = kecil(input);
    tanaman hasil[KAP];
    int n = 0;
    for (int i = 0; i < k.n; i++) {
        if (kecil(k.data[i].id).find(key) != string::npos ||
            kecil(k.data[i].nama).find(key) != string::npos) {
            hasil[n++] = k.data[i];
        }
    }

    if (n == 0) {
        cout << "\"" << input << "\" tidak ditemukan" << endl;
        return;
    }
    tampilkan_semua(hasil, n, "Hasil cari");
}

void urutkan_katalog(katalog& k) {
    for (int i = 0; i < k.n - 1; i++) {
        int i_min = i;
        for (int j = i + 1; j < k.n; j++) {
            if (strcmp(k.data[j].id, k.data[i_min].id) < 0) i_min = j;
        }
        tanaman temp = k.data[i];
        k.data[i] = k.data[i_min];
        k.data[i_min] = temp;
    }
    cout << "Katalog berhasil diurutkan berdasarkan ID" << endl;
}

void ubah_hapus_bibit(katalog& k) {
    string id, input;

    cout << "Ubah/hapus bibit :\n";
    cout << "Masukan ID (Blank to exit) : ";
    getline(cin, id);
    if (id.empty()) return;

    int i = cari_id(k, id);
    if (i == -1) {
        cout << "ID " << id << " tidak ditemukan" << endl;
        return;
    }

    tampilkan_semua(&k.data[i], 1, "Bibit terpilih");
    cout << "1. Ubah nama\n" << "2. Ubah harga\n" << "3. Ubah stok\n" << "4. Hapus bibit\n";
    cout << "Pilih (Blank to exit) : ";
    getline(cin, input);
    if (input.empty()) return;

    switch (input[0]) {
        case '1': {
            string nama;
            cout << "Masukan nama baru (Blank to exit) : ";
            getline(cin, nama);
            if (nama.empty()) return;
            if (nama.find(',') != string::npos) {
                cout << "Ditolak, nama mengandung koma" << endl;
                return;
            }
            if (nama.length() > NAMA_MAX) {
                cout << "Ditolak, nama lebih dari " << NAMA_MAX << " karakter" << endl;
                return;
            }
            strncpy(k.data[i].nama, nama.c_str(), NAMA_MAX);
            k.data[i].nama[NAMA_MAX] = '\0';
            break;
        }
        case '2': {
            string str_hrg;
            int hrg;
            cout << "Masukan harga baru (Blank to exit) : ";
            getline(cin, str_hrg);
            if (str_hrg.empty()) return;
            if (!ke_int(str_hrg, hrg) || hrg < 0 || hrg > HRG_MAX) {
                cout << "Ditolak, harga salah atau di luar rentang (" << str_hrg << ")" << endl;
                return;
            }
            k.data[i].hrg = hrg;
            break;
        }
        case '3': {
            string str_stk_kg;
            float stk_kg;
            cout << "Masukan stok baru (Blank to exit) : ";
            getline(cin, str_stk_kg);
            if (str_stk_kg.empty()) return;
            if (!ke_float(str_stk_kg, stk_kg) || stk_kg < 0 || stk_kg > STK_KG_MAX) {
                cout << "Ditolak, stok salah atau di luar rentang (" << str_stk_kg << ")" << endl;
                return;
            }
            k.data[i].stk_kg = stk_kg;
            break;
        }
        case '4':
            cout << "Yakin hapus " << k.data[i].nama << "? (y/n, Blank to exit) : ";
            getline(cin, input);
            if (input != "y" && input != "Y") {
                cout << "Batal menghapus" << endl;
                return;
            }
            for (int j = i; j < k.n - 1; j++) k.data[j] = k.data[j + 1];
            k.n--;
            cout << "Bibit berhasil dihapus" << endl;
            return;
        default:
            cout << "Pilihan tidak valid" << endl;
            return;
    }
    cout << "Data berhasil diubah" << endl;
}

void ubah_stok(katalog& k) {
    string id, str_delta;
    float delta;

    cout << "Tambah/kurangi stok :\n";
    cout << "Masukan ID (Blank to exit) : ";
    getline(cin, id);
    if (id.empty()) return;

    int i = cari_id(k, id);
    if (i == -1) {
        cout << "ID " << id << " tidak ditemukan" << endl;
        return;
    }

    cout << k.data[i].nama << ", stok saat ini " << k.data[i].stk_kg << " KG" << endl;
    cout << "Masukan perubahan, + tambah / - kurangi (Blank to exit) : ";
    getline(cin, str_delta);
    if (str_delta.empty()) return;
    if (!ke_float(str_delta, delta)) {
        cout << "Ditolak, format angka salah" << endl;
        return;
    }

    float baru = k.data[i].stk_kg + delta;
    if (baru < 0 || baru > STK_KG_MAX) {
        cout << "Ditolak, stok/kg di luar rentang (" << baru << ")" << endl;
        return;
    }
    k.data[i].stk_kg = baru;
    cout << "Stok " << k.data[i].nama << " sekarang " << baru << " KG" << endl;
}

void statistik(const katalog& k) {
    cout << "Statistik :\n";
    if (k.n == 0) {
        cout << "Katalog kosong" << endl;
        return;
    }

    const tanaman* d = k.data;
    int i_max_hrg = 0, i_min_hrg = 0, i_max_stk = 0, i_min_stk = 0;
    double total_stk = 0, total_nilai = 0, total_hrg = 0;

    for (int i = 0; i < k.n; i++) {
        total_stk += d[i].stk_kg;
        total_nilai += (double)d[i].hrg * d[i].stk_kg;
        total_hrg += d[i].hrg;
        if (d[i].hrg > d[i_max_hrg].hrg) i_max_hrg = i;
        if (d[i].hrg < d[i_min_hrg].hrg) i_min_hrg = i;
        if (d[i].stk_kg > d[i_max_stk].stk_kg) i_max_stk = i;
        if (d[i].stk_kg < d[i_min_stk].stk_kg) i_min_stk = i;
    }

    cout << fixed << setprecision(2)
    << "Jumlah jenis bibit : " << k.n << endl
    << "Total stok         : " << total_stk << " KG" << endl
    << "Total nilai stok   : " << total_nilai << endl
    << "Rata-rata harga    : " << total_hrg/k.n << endl
    << "Harga tertinggi    : " << d[i_max_hrg].nama << " (" << d[i_max_hrg].hrg << ")" << endl
    << "Harga terendah     : " << d[i_min_hrg].nama << " (" << d[i_min_hrg].hrg << ")" << endl
    << "Stok terbanyak     : " << d[i_max_stk].nama << " (" << d[i_max_stk].stk_kg << " KG)" << endl
    << "Stok tersedikit    : " << d[i_min_stk].nama << " (" << d[i_min_stk].stk_kg << " KG)" << endl;
    cout << defaultfloat << setprecision(6);
}

void info_memori(const katalog& k) {
    cout << "Info memori :\n"
    << "sizeof(tanaman)       : " << sizeof(tanaman) << " byte" << endl
    << "sizeof(katalog)       : " << sizeof(katalog) << " byte" << endl
    << "Kapasitas array       : " << KAP << " data" << endl
    << "Jumlah data terisi    : " << k.n << endl
    << "Memori array total    : " << sizeof(k.data) << " byte (" << KAP << " x " << sizeof(tanaman) << ")" << endl
    << "Memori terpakai       : " << k.n * sizeof(tanaman) << " byte" << endl
    << "Memori sisa           : " << (KAP - k.n) * sizeof(tanaman) << " byte" << endl;
}
