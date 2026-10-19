#include "fileio.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

void load_data(katalog& k, const string& path) {
    ifstream file(path);
    if (!file) {
        cerr << "Tidak ada file di folder data (" << path << "), mulai dengan katalog kosong\n";
        return;
    }

    string line;
    int i = 0;
    int x = 0;
    while (getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        istringstream iss(line);
        i++;

        if (line.empty()) {
            cout << "Baris kosong pada baris ke-" << i << endl;
            x++;
            continue;
        }

        string id, nama, str_hrg, str_stk_kg;
        if (getline(iss, id, ',') &&
            getline(iss, nama, ',') &&
            getline(iss, str_hrg, ',') &&
            getline(iss, str_stk_kg))
        {
            int hrg;
            float stk_kg;
            if (!ke_int(str_hrg, hrg) || !ke_float(str_stk_kg, stk_kg)) {
                cout << "Baris ke-" << i << " ditolak karena format angka salah" << endl;
                x++;
            } else if (hrg < 0 || hrg > HRG_MAX || stk_kg < 0 || stk_kg > STK_KG_MAX) {
                cout << "Baris ke-" << i << " ditolak karena harga atau stok di luar rentang" << endl;
                x++;
            } else if (!id_valid(id)) {
                cout << "Baris ke-" << i << " ditolak karna ID tidak sesuai format (2 huruf + 3 angka)" << endl;
                x++;
            } else if (nama.empty() || nama.length() > NAMA_MAX) {
                cout << "Baris ke-" << i << " ditolak karna nama kosong atau lebih dari " << NAMA_MAX << " karakter" << endl;
                x++;
            } else if (cari_id(k, id) != -1) {
                cout << "Baris ke-" << i << " ditolak karna duplikat id" << endl;
                x++;
            } else if (k.n >= KAP) {
                cout << "Baris ke-" << i << " ditolak karna katalog penuh (maks " << KAP << ")" << endl;
                x++;
            } else {
                isi_data(k, id, nama, hrg, stk_kg);
            }
        } else {
            cout << "Baris ke-" << i << " ditolak karena kolom tidak lengkap" << endl;
            x++;
        }
    }
    if (i == 0) {
        cout << "File kosong (" << path << ")" << endl;
        return;
    } else if (i - x == 0) {
        cout << "Tidak ada data valid di file (" << path << ")" << endl;
        return;
    }

    cout << "Berhasil memuat " << i - x << " dari " << i << " baris (" << x << " ditolak)" << endl;
}

bool simpan_data(const katalog& k, const string& path) {
    ofstream file(path);
    if (!file) {
        cerr << "Gagal membuka file untuk menyimpan data (" << path << ")" << endl;
        return false;
    }

    for (int i = 0; i < k.n; i++) {
        file << k.data[i].id << ',' << k.data[i].nama << ',' << k.data[i].hrg << ',' << k.data[i].stk_kg << '\n';
    }
    file.close();
    if (!file) {
        cerr << "Gagal menyimpan data ke file (" << path << ")" << endl;
        return false;
    }

    cout << "Data berhasil disimpan ke " << path << endl;
    return true;
}
