#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cmath>
#include <cctype>
#include <cstring>
#include <algorithm>
using namespace std;

const int ID_LEN = 5;
const int ID_HURUF = 2;
const int NAMA_MAX = 24;
const int HRG_MAX = 10000000;
const int KAP = 100;

struct tanaman{
    char id[ID_LEN + 1];
    char nama[NAMA_MAX + 1];
    int hrg;
    float stk_kg;
};

struct katalog{
    tanaman data[KAP];
    int n;
};


struct tanaman_lama{
    string id;
    string nama;
    int hrg;
    float stk_kg;
};

int int_w(int val) {
    if (val == 0) return 1;
    int digit = 0;
    while (val > 0) {
        val /= 10;
        digit++;
    }
    return digit;
}

string besar(string s) {
    for (auto& c : s) c = toupper((unsigned char)c);
    return s;
}

string kecil(string s) {
    for (auto& c : s) c = tolower((unsigned char)c);
    return s;
}

string rapikan_nama(const string& s) {
    string out;
    bool kata_baru = true;
    for (unsigned char c : s) {
        if (isspace(c)) {
            kata_baru = true;
            continue;
        }
        if (kata_baru && !out.empty()) out += ' ';
        out += kata_baru ? toupper(c) : tolower(c);
        kata_baru = false;
    }
    return out;
}

bool id_valid(const string& id) {
    if (id.length() != ID_LEN) return false;
    for (int i = 0; i < ID_LEN; i++) {
        unsigned char c = id[i];
        if (i < ID_HURUF ? !isalpha(c) : !isdigit(c)) return false;
    }
    return true;
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

void width(const tanaman* data, int n,
    int& w_id, int& w_nama, int& w_hrg, int& w_stk_kg, 
    string& id_ktlg, string& nama_ktlg, string& hrg_ktlg, string& stk_kg_ktlg) {

    for (int i = 0; i < n; i++) {
        int i_w_hrg = int_w(data[i].hrg);
        ostringstream s_stk_kg;
        s_stk_kg << data[i].stk_kg;

        if (w_nama < int(strlen(data[i].nama))) w_nama = int(strlen(data[i].nama));
        if (w_hrg < i_w_hrg) w_hrg = i_w_hrg;
        if (w_stk_kg < int(s_stk_kg.str().length())) w_stk_kg = int(s_stk_kg.str().length());
    }
    if (w_id < id_ktlg.length()) w_id = id_ktlg.length();
    if (w_nama < nama_ktlg.length()) w_nama = nama_ktlg.length(); 
    if (w_hrg < hrg_ktlg.length()) w_hrg = hrg_ktlg.length(); 
    if (w_stk_kg < stk_kg_ktlg.length()) w_stk_kg = stk_kg_ktlg.length(); 
}

void load_data(katalog& k, const string& path, const float& stk_kg_max) {
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
            try {
                size_t pos1, pos2;
                hrg = stoi(str_hrg, &pos1);
                stk_kg = stof(str_stk_kg, &pos2);
                if (pos1 != str_hrg.size() || pos2 != str_stk_kg.size())
                    throw invalid_argument("sisa karakter");
            } catch (...) {
                cout << "Baris ke-" << i << " ditolak karena format angka salah" << endl;
                x++;
                continue;
            }

            bool hrg_salah = hrg < 0 || hrg > HRG_MAX;
            bool stk_salah = !isfinite(stk_kg) || stk_kg < 0 || stk_kg > stk_kg_max;
            nama = rapikan_nama(nama);

            if (hrg_salah && stk_salah) {
                cout << "Baris ke-" << i << " harga dan stk/kg di luar rentang (" << str_hrg << "," << str_stk_kg << ")" << endl;
                x++;
            } else if (hrg_salah || stk_salah) {
                string invalid_val = hrg_salah ? str_hrg : str_stk_kg;
                string invalid = hrg_salah ? "harga" : "stok/kg";
                cout << "Baris ke-" << i << " " << invalid << " di luar rentang (" << invalid_val << ")" << endl;
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
        }else {
        cout << "Baris ke-" << i << " ditolak karena kolom tidak lengkap" << endl;
        x++;
        }
    }
    if (i == 0) {
        cout << "File kosong (" << path << ")" << endl;
        return;
    } else if (i-x == 0)
    {
        cout << "Tidak ada data valid di file (" << path << ")" << endl;
        return;
    }
    
    cout << "Berhasil memuat " << i-x << " dari " << i << " baris (" << x <<" ditolak)" << endl;
}

void tampilkan_semua(const tanaman* data, int n, int& w_id, int& border,
    string& id_ktlg, string& nama_ktlg, string& hrg_ktlg, string& stk_kg_ktlg, string& judul_ktlg) {

    int w_nama = 1, w_hrg = 1, w_stk_kg = 1;
    width(data, n, w_id, w_nama, w_hrg, w_stk_kg, id_ktlg, nama_ktlg, hrg_ktlg, stk_kg_ktlg);

    border = 2 + w_id + 3 + w_nama + 3 + w_hrg + 3 + w_stk_kg + 2;
    if (judul_ktlg.length() >= border) judul_ktlg = "Biji2an tnm";

    cout << string(border, '=') << endl 

    << string((border-judul_ktlg.length()-1)/2, ' ') << "(" << judul_ktlg << ")"  << endl
    
    << string(border, '=') << endl

    << "| "
    << left << setw(w_id) << id_ktlg << " | "
    << left << setw(w_nama) << nama_ktlg << " | "
    << left << setw(w_hrg) << hrg_ktlg << " | "
    << left << setw(w_stk_kg) << stk_kg_ktlg << " |" << endl

    << string(border, '=') << endl;

    for (int i = 0; i < n; i++) {
        cout << "| " 
        << left << setw(w_id) << data[i].id << " | " 
        << left << setw(w_nama) << data[i].nama << " | " 
        << left << setw(w_hrg) << data[i].hrg << " | "
        << left << setw(w_stk_kg)<< data[i].stk_kg << " |" << endl;
    }
    cout << string(border, '=') << endl;
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

void tambah_bibit_baru(katalog& k, const float& stk_kg_max) {
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
    id = besar(id);
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
    nama = rapikan_nama(nama);
    if (nama.empty() || nama.length() > NAMA_MAX) {
        cout << "Ditolak, nama kosong atau lebih dari " << NAMA_MAX << " karakter" << endl;
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
    if (stk_kg < 0 || stk_kg > stk_kg_max) {
        cout << "Ditolak, stok/kg di luar rentang (" << str_stk_kg << ")" << endl;
        return;
    }

    isi_data(k, id, nama, hrg, stk_kg);
    cout << "Bibit " << nama << " berhasil ditambahkan" << endl;
}

void cari_bibit(const katalog& k, string header[], int& w_id) {
    string input, judul = "Hasil cari";
    int border;

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
    tampilkan_semua(hasil, n, w_id, border, header[0], header[1], header[2], header[3], judul);
}

void urutkan_katalog(katalog& k) {
    for (int i = 0; i < k.n - 1; i++) {
        int i_min = i;
        for (int j = i + 1; j < k.n; j++) {
            if (strcmp(k.data[j].id, k.data[i_min].id) < 0) i_min = j;
        }
        swap(k.data[i], k.data[i_min]);
    }
    cout << "Katalog berhasil diurutkan berdasarkan ID" << endl;
}

void ubah_hapus_bibit(katalog& k, string header[], int& w_id, const float& stk_kg_max) {
    string id, input, judul = "Bibit terpilih";
    int border;

    cout << "Ubah / hapus bibit :\n";
    cout << "Masukan ID (Blank to exit) : ";
    getline(cin, id);
    if (id.empty()) return;

    int i = cari_id(k, id);
    if (i == -1) {
        cout << "ID " << id << " tidak ditemukan" << endl;
        return;
    }

    tampilkan_semua(&k.data[i], 1, w_id, border, header[0], header[1], header[2], header[3], judul);
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
            nama = rapikan_nama(nama);
            if (nama.empty() || nama.length() > NAMA_MAX) {
                cout << "Ditolak, nama kosong atau lebih dari " << NAMA_MAX << " karakter" << endl;
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
            if (!ke_float(str_stk_kg, stk_kg) || stk_kg < 0 || stk_kg > stk_kg_max) {
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

void ubah_stok(katalog& k, const float& stk_kg_max) {
    string id, str_delta;
    float delta;

    cout << "Tambah / kurangi stok :\n";
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
    if (baru < 0 || baru > stk_kg_max) {
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
    << "Rata-rata harga    : " << total_hrg / k.n << endl
    << "Harga tertinggi    : " << d[i_max_hrg].nama << " (" << d[i_max_hrg].hrg << ")" << endl
    << "Harga terendah     : " << d[i_min_hrg].nama << " (" << d[i_min_hrg].hrg << ")" << endl
    << "Stok terbanyak     : " << d[i_max_stk].nama << " (" << d[i_max_stk].stk_kg << " KG)" << endl
    << "Stok tersedikit    : " << d[i_min_stk].nama << " (" << d[i_min_stk].stk_kg << " KG)" << endl;
    cout << defaultfloat << setprecision(6);
}

void info_memori(const katalog& k) {
    size_t s_baru = sizeof(tanaman);
    size_t s_lama = sizeof(tanaman_lama);

    cout << "Info memori :\n"
    << "sizeof(tanaman)       : " << s_baru << " byte" << endl
    << "sizeof(katalog)       : " << sizeof(katalog) << " byte" << endl
    << "Kapasitas array       : " << KAP << " data" << endl
    << "Jumlah data terisi    : " << k.n << endl
    << "Memori array total    : " << sizeof(k.data) << " byte (" << KAP << " x " << s_baru << ")" << endl
    << "Memori terpakai       : " << k.n * s_baru << " byte" << endl
    << "Memori sisa           : " << (KAP - k.n) * s_baru << " byte" << endl
    << "--- sebelum vs sesudah optimasi ---\n"
    << "Sebelum (string)      : " << s_lama << " byte/data, array " << KAP * s_lama << " byte" << endl
    << "Sesudah (char[])      : " << s_baru << " byte/data, array " << KAP * s_baru << " byte" << endl
    << "Hemat                 : " << s_lama - s_baru << " byte/data, " << KAP * (s_lama - s_baru) << " byte total" << endl
    << "Catatan: sizeof(tanaman_lama) belum menghitung teks string di heap (jika > SSO)\n";
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

void command(katalog& k, string header[], const int& border, bool& run, int& w_id, const float& stk_kg_max){
    string input;

    cout << "1. Tambah bibit baru\n" << "2. Cari bibit\n" << "3. Urutkan katalog\n" 
    << "4. Ubah / hapus bibit\n" << "5. Tambah / kurangi stok\n" << "6. Statistik\n" 
    << "7. Info memori\n" << "8. Simpan dan keluar\n" << string(border, '=') << endl;

    cout << "Pilih menu (Blank to exit without save): ";
    getline(cin, input);

    if (input.empty()) {
        cout << "Keluar tanpa menyimpan perubahan\n";
        run = false;
        return;
    }

    char choice = input[0];

    switch (choice) {
        case '1':
            tambah_bibit_baru(k, stk_kg_max);
            break;
        case '2':
            cari_bibit(k, header, w_id);
            break;
        case '3':
            urutkan_katalog(k);
            break;
        case '4':
            ubah_hapus_bibit(k, header, w_id, stk_kg_max);
            break;
        case '5':
            ubah_stok(k, stk_kg_max);
            break;
        case '6':
            statistik(k);
            break;
        case '7':
            info_memori(k);
            break;
        case '8':
            cout << "PERINGATAN: data di bibit.txt akan ditimpa saat disimpan. Lanjutkan? (y/n): ";
            getline(cin, input);
            if (input != "y" && input != "Y") {
                cout << "Penyimpanan dibatalkan" << endl;
                break;
            }
            if (simpan_data(k, "bibit.txt")) {
                cout << "Keluar dari program" << endl;
                run = false;
            }
            break;
        default:
            cout << "Invalid input. Please enter 1-8 or leave blank to quit.\n";
            break;
    }
}

int main() {
    int w_id = ID_LEN;
    float stk_kg_max = 1000; 
    int border;
    bool run = true;
    string header[5] = {"ID", "Nama tanaman", "Harga/KG", "Stok/KG", "Biji-bijian tanaman"};

    katalog k{};
    load_data(k, "bibit.txt", stk_kg_max);

    while(run) {
        tampilkan_semua(k.data, k.n, w_id, border, header[0], header[1], header[2], header[3], header[4]);
        command(k, header, border, run, w_id, stk_kg_max);
    }
    return 0;
}