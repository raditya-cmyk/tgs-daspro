// Sistem Katalog dan Stok Bibit
// Kompilasi: g++ -std=c++11 -o proyek proyek.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <cmath>
#include <cctype>
#include <iomanip>
using namespace std;

// ====================== KONSTANTA ======================
const int    KAPASITAS    = 100;       // kapasitas array katalog
const int    STOK_MAX     = 1000;
const long   HARGA_MAX    = 10000000;
const int    PANJANG_NAMA = 23;        // termasuk '\0' (maks 22 karakter)
const int    JML_KAT      = 11;
const string FILE_UTAMA   = "data/bibit.txt";
const string FILE_CADANGAN = "bibit.txt";

const char* MERAH = "\033[31m";
const char* HIJAU = "\033[32m";
const char* ABU   = "\033[90m";
const char* RESET = "\033[0m";

const char* PREFIX_KAT[JML_KAT] = {
    "SR", "KC", "BH", "SY", "IN", "AT", "RM", "UB", "ST", "OB", "HI"
};
const char* NAMA_KAT[JML_KAT] = {
    "Serealia", "Kacang-kacangan", "Tanaman buah", "Tanaman sayuran",
    "Tanaman industri", "Minyak atsiri", "Tanaman rempah",
    "Tanaman umbi-umbian", "Tanaman serat", "Tanaman obat-obatan",
    "Tanaman hias"
};

// ====================== STRUCT ======================
// Struct pertama: versi boros (tipe besar, array char terlalu panjang)
struct BibitAwal {
    char   kode[20];
    char   nama[100];
    double harga;
    double stok;
    char   kategori[50];
};

// Struct kedua: versi hemat (tipe disesuaikan rentang data, urutan field dirapatkan)
struct Bibit {
    int            harga;               // maks 10.000.000
    short          stok;                // maks 1000
    unsigned char  kat;                 // indeks kategori 0..10
    char           kode[6];             // 5 karakter + '\0'
    char           nama[PANJANG_NAMA];  // 22 karakter + '\0'
};

Bibit katalog[KAPASITAS];
int   jumlah = 0;

// ====================== UTILITAS STRING ======================
string trim(const string& s) {
    size_t a = 0, b = s.size();
    while (a < b && isspace((unsigned char)s[a])) a++;
    while (b > a && isspace((unsigned char)s[b - 1])) b--;
    return s.substr(a, b - a);
}

string huruf_besar(string s) {
    for (size_t i = 0; i < s.size(); i++) s[i] = toupper((unsigned char)s[i]);
    return s;
}

string huruf_kecil(string s) {
    for (size_t i = 0; i < s.size(); i++) s[i] = tolower((unsigned char)s[i]);
    return s;
}

// "sORGUM" -> "Sorgum", "kacang HIJAU" -> "Kacang Hijau"
string rapikan_nama(const string& s) {
    string r = s;
    bool awal_kata = true;
    for (size_t i = 0; i < r.size(); i++) {
        if (isspace((unsigned char)r[i])) {
            awal_kata = true;
        } else {
            r[i] = awal_kata ? toupper((unsigned char)r[i]) : tolower((unsigned char)r[i]);
            awal_kata = false;
        }
    }
    return r;
}

void salin(char* dst, size_t n, const string& src) {
    strncpy(dst, src.c_str(), n - 1);
    dst[n - 1] = '\0';
}

bool parse_long(const string& s, long& out) {
    if (s.empty()) return false;
    char* end;
    errno = 0;
    long v = strtol(s.c_str(), &end, 10);
    if (errno != 0 || *end != '\0') return false;
    out = v;
    return true;
}

bool parse_double(const string& s, double& out) {
    if (s.empty()) return false;
    char* end;
    errno = 0;
    double v = strtod(s.c_str(), &end);
    if (errno != 0 || *end != '\0') return false;
    out = v;
    return true;
}

void pesan_merah(const string& s) { cout << MERAH << s << RESET << endl; }
void pesan_hijau(const string& s) { cout << HIJAU << s << RESET << endl; }

// ====================== KODE & KATEGORI ======================
bool format_kode_valid(const string& k) {
    if (k.size() != 5) return false;
    for (int i = 0; i < 2; i++) if (!isalpha((unsigned char)k[i])) return false;
    for (int i = 2; i < 5; i++) if (!isdigit((unsigned char)k[i])) return false;
    return true;
}

int idx_kategori(const string& kode) {
    if (kode.size() < 2) return -1;
    for (int i = 0; i < JML_KAT; i++)
        if (kode[0] == PREFIX_KAT[i][0] && kode[1] == PREFIX_KAT[i][1]) return i;
    return -1;
}

int cari_kode(const string& kode) {
    for (int i = 0; i < jumlah; i++)
        if (kode == katalog[i].kode) return i;
    return -1;
}

string daftar_awalan() {
    string r;
    for (int i = 0; i < JML_KAT; i++) {
        if (i) r += " ";
        r += PREFIX_KAT[i];
    }
    return r;
}

// ====================== INPUT ======================
// false jika EOF atau (kosong dan tidak diizinkan) -> artinya "keluar"
bool tanya(const string& prompt, string& s, bool boleh_kosong = false) {
    cout << prompt;
    if (!getline(cin, s)) { cout << endl; return false; }
    s = trim(s);
    if (s.empty() && !boleh_kosong) return false;
    return true;
}

int pilih(const string& prompt, int lo, int hi) {
    string s;
    while (true) {
        if (!tanya(prompt, s)) return 0;
        long v;
        if (parse_long(s, v) && v >= lo && v <= hi) return (int)v;
        cout << MERAH << "  ! Pilihan harus " << lo << "-" << hi << "." << RESET << endl;
    }
}

bool input_nama(const string& prompt, string& hasil, bool& dirapikan) {
    string s;
    while (true) {
        if (!tanya(prompt, s, true)) return false;
        if (s.empty()) { pesan_merah("  ! Nama tidak boleh kosong."); continue; }
        if ((int)s.size() > PANJANG_NAMA - 1) {
            pesan_merah("  ! Nama maksimal 22 karakter."); continue;
        }
        if (s.find(',') != string::npos) {
            pesan_merah("  ! Nama tidak boleh mengandung koma."); continue;
        }
        hasil = rapikan_nama(s);
        dirapikan = (hasil != s);
        return true;
    }
}

bool input_harga(const string& prompt, int& hasil) {
    string s;
    while (true) {
        if (!tanya(prompt, s)) return false;
        long v;
        if (parse_long(s, v) && v >= 0 && v <= HARGA_MAX) { hasil = (int)v; return true; }
        pesan_merah("  ! Harga harus berupa angka dalam rentang yang diizinkan.");
    }
}

bool input_stok(const string& prompt, int& hasil) {
    string s;
    while (true) {
        if (!tanya(prompt, s)) return false;
        long v;
        if (parse_long(s, v) && v >= 0 && v <= STOK_MAX) { hasil = (int)v; return true; }
        cout << MERAH << "  ! Stok harus bilangan bulat 0-" << STOK_MAX << "." << RESET << endl;
    }
}

// ====================== MUAT FILE ======================
void tolak(int baris, const string& alasan) {
    cout << MERAH << "Baris " << setw(2) << baris << " ditolak: " << alasan << RESET << endl;
}

void muat_file() {
    string path = FILE_UTAMA;
    ifstream f(path.c_str());
    if (!f) {
        f.clear();
        f.open(FILE_CADANGAN.c_str());
        if (f) path = FILE_CADANGAN;
    }
    if (!f) {
        pesan_merah(FILE_UTAMA + " tidak ditemukan. Mulai dengan katalog kosong.");
        return;
    }

    cout << "Membaca " << path << " ..." << endl;

    string line;
    int total = 0, ditolak = 0;
    while (getline(f, line)) {
        line = trim(line);
        if (line.empty()) continue;      // baris kosong diabaikan
        total++;

        // pecah berdasarkan koma
        vector<string> kol;
        size_t mulai = 0;
        while (true) {
            size_t p = line.find(',', mulai);
            if (p == string::npos) { kol.push_back(trim(line.substr(mulai))); break; }
            kol.push_back(trim(line.substr(mulai, p - mulai)));
            mulai = p + 1;
        }

        if (kol.size() < 4) { tolak(total, "format salah (kolom kurang)"); ditolak++; continue; }
        if (kol.size() > 4) { tolak(total, "format salah (kolom berlebih)"); ditolak++; continue; }

        string kode = huruf_besar(kol[0]);
        if (!format_kode_valid(kode) || idx_kategori(kode) < 0) {
            tolak(total, "kode tidak valid (" + kol[0] + ")"); ditolak++; continue;
        }
        if (kol[1].empty()) { tolak(total, "nama kosong"); ditolak++; continue; }
        if ((int)kol[1].size() > PANJANG_NAMA - 1) {
            tolak(total, "nama terlalu panjang (" + kol[1] + ")"); ditolak++; continue;
        }

        long harga;
        if (!parse_long(kol[2], harga)) {
            tolak(total, "harga bukan angka (" + kol[2] + ")"); ditolak++; continue;
        }
        if (harga < 0 || harga > HARGA_MAX) {
            tolak(total, "harga di luar rentang (" + kol[2] + ")"); ditolak++; continue;
        }

        double stok;
        if (!parse_double(kol[3], stok) || !isfinite(stok)) {
            tolak(total, "stok bukan angka (" + kol[3] + ")"); ditolak++; continue;
        }
        if (stok < 0 || stok > STOK_MAX) {
            tolak(total, "stok di luar rentang (" + kol[3] + ")"); ditolak++; continue;
        }

        if (cari_kode(kode) >= 0) {
            tolak(total, "kode duplikat (" + kode + ")"); ditolak++; continue;
        }
        if (jumlah >= KAPASITAS) {
            tolak(total, "kapasitas penuh"); ditolak++; continue;
        }

        Bibit& b = katalog[jumlah++];
        salin(b.kode, sizeof(b.kode), kode);
        salin(b.nama, sizeof(b.nama), rapikan_nama(kol[1]));
        b.harga = (int)harga;
        b.stok  = (short)(int)stok;       // desimal dipotong (40.25 -> 40)
        b.kat   = (unsigned char)idx_kategori(kode);
    }

    if (total == 0) {
        pesan_merah("File kosong. Mulai dengan katalog kosong.");
        return;
    }
    cout << HIJAU << "Berhasil dimuat: " << (total - ditolak) << " dari " << total
         << " baris (" << ditolak << " ditolak)" << RESET << endl;
}

// ====================== TAMPILAN ======================
void garis(char c, int n) { cout << string(n, c) << endl; }

void tampil_menu() {
    cout << endl;
    garis('=', 47);
    cout << "        SISTEM KATALOG DAN STOK BIBIT" << endl;
    garis('=', 47);
    cout << "1. Tampilkan katalog\n"
            "2. Tambah bibit baru\n"
            "3. Cari bibit\n"
            "4. Urutkan katalog\n"
            "5. Ubah / hapus bibit\n"
            "6. Tambah / kurangi stok\n"
            "7. Statistik\n"
            "8. Info memori\n";
    garis('=', 47);
}

void cetak_baris(const Bibit& b) {
    cout << left << setw(7) << b.kode << setw(20) << b.nama
         << setw(9) << b.harga << b.stok << endl;
}

void cetak_flat() {
    for (int i = 0; i < jumlah; i++) cetak_baris(katalog[i]);
}

bool cek_ada_data() {
    if (jumlah == 0) { pesan_merah("Belum ada data bibit."); return false; }
    return true;
}

// ====================== MENU 1: KATALOG ======================
void menu_katalog() {
    cout << endl;
    cout << left << setw(7) << "Kode" << setw(20) << "Nama Bibit" << setw(9) << "Harga" << "Stok" << endl;
    garis('-', 45);
    long total_stok = 0;
    for (int k = 0; k < JML_KAT; k++) {
        bool judul = false;
        for (int i = 0; i < jumlah; i++) {
            if (katalog[i].kat != k) continue;
            if (!judul) {
                cout << "[ " << huruf_besar(NAMA_KAT[k]) << " ]" << endl;
                judul = true;
            }
            cetak_baris(katalog[i]);
        }
    }
    for (int i = 0; i < jumlah; i++) total_stok += katalog[i].stok;
    garis('-', 45);
    cout << "Total: " << jumlah << " jenis bibit | Total stok: " << total_stok << endl;
}

// ====================== MENU 2: TAMBAH ======================
void menu_tambah() {
    if (jumlah >= KAPASITAS) {
        pesan_merah("Katalog penuh (maks " + to_string(KAPASITAS) + " bibit). Hapus data dulu.");
        return;
    }
    cout << endl;
    string kode;
    while (true) {
        if (!tanya("Kode bibit (blank to quit): ", kode)) return;
        kode = huruf_besar(kode);
        if (!format_kode_valid(kode)) {
            pesan_merah("  ! Format kode: 2 huruf awalan + 3 angka (contoh SR004).");
        } else if (idx_kategori(kode) < 0) {
            pesan_merah("  ! Awalan tidak dikenal (" + daftar_awalan() + ").");
        } else if (cari_kode(kode) >= 0) {
            pesan_merah("  ! Kode sudah dipakai.");
        } else break;
    }

    string nama; bool dirapikan = false;
    if (!input_nama("Nama bibit: ", nama, dirapikan)) return;
    int harga, stok;
    if (!input_harga("Harga (Rp): ", harga)) return;
    if (!input_stok("Stok awal: ", stok)) return;

    Bibit& b = katalog[jumlah++];
    salin(b.kode, sizeof(b.kode), kode);
    salin(b.nama, sizeof(b.nama), nama);
    b.harga = harga;
    b.stok  = (short)stok;
    b.kat   = (unsigned char)idx_kategori(kode);

    pesan_hijau("Tersimpan: " + kode + "  " + nama + "  [" + NAMA_KAT[b.kat] + "]");
    if (dirapikan) cout << ABU << "(nama dirapikan otomatis: huruf awal kapital)" << RESET << endl;
}

// ====================== MENU 3: CARI ======================
void menu_cari() {
    cout << endl;
    string q;
    while (true) {
        if (!tanya("Cari nama (blank to quit): ", q)) return;
        string kecil = huruf_kecil(q);
        int ketemu = 0;
        for (int i = 0; i < jumlah; i++)
            if (huruf_kecil(katalog[i].nama).find(kecil) != string::npos) ketemu++;

        if (ketemu == 0) {
            pesan_merah("Bibit \"" + q + "\" tidak ditemukan.");
        } else {
            cout << "Ditemukan " << ketemu << " bibit:" << endl;
            for (int i = 0; i < jumlah; i++)
                if (huruf_kecil(katalog[i].nama).find(kecil) != string::npos)
                    cetak_baris(katalog[i]);
        }
        cout << endl;
    }
}

// ====================== MENU 4: URUTKAN (sort buatan sendiri) ======================
int banding(const Bibit& a, const Bibit& b, int kriteria) {
    switch (kriteria) {
        case 1: return strcmp(a.kode, b.kode);
        case 2: return huruf_kecil(a.nama).compare(huruf_kecil(b.nama));
        case 3: return (a.harga > b.harga) - (a.harga < b.harga);
        default: return (a.stok > b.stok) - (a.stok < b.stok);
    }
}

// insertion sort (stabil)
void urutkan(int kriteria, bool turun) {
    for (int i = 1; i < jumlah; i++) {
        Bibit tmp = katalog[i];
        int j = i - 1;
        while (j >= 0) {
            int c = banding(katalog[j], tmp, kriteria);
            if (turun) c = -c;
            if (c <= 0) break;
            katalog[j + 1] = katalog[j];
            j--;
        }
        katalog[j + 1] = tmp;
    }
}

void menu_urut() {
    cout << endl;
    cout << "Urutkan berdasarkan:\n1. Kode   2. Nama   3. Harga   4. Stok" << endl;
    int kriteria = pilih("Pilihan (blank to quit): ", 1, 4);
    if (!kriteria) return;
    cout << "Urutan: 1. Naik   2. Turun" << endl;
    int arah = pilih("Pilihan (blank to quit): ", 1, 2);
    if (!arah) return;

    urutkan(kriteria, arah == 2);

    const char* nama_kriteria[] = {"", "kode", "nama", "harga", "stok"};
    pesan_hijau(string("Katalog diurutkan: ") + nama_kriteria[kriteria] + ", " +
                (arah == 1 ? "naik" : "turun") + ".");
    cout << endl;
    cetak_flat();
}

// ====================== MENU 5: UBAH / HAPUS ======================
void menu_ubah_hapus() {
    cout << endl;
    string kode;
    if (!tanya("Kode bibit (blank to quit): ", kode)) return;
    kode = huruf_besar(kode);
    int idx = cari_kode(kode);
    if (idx < 0) { pesan_merah("Bibit dengan kode " + kode + " tidak ditemukan."); return; }

    cetak_baris(katalog[idx]);
    cout << "1. Ubah nama   2. Ubah harga   3. Hapus" << endl;
    int p = pilih("Pilihan (blank to quit): ", 1, 3);
    if (!p) return;

    if (p == 1) {
        string nama; bool d;
        if (!input_nama("Nama baru: ", nama, d)) return;
        salin(katalog[idx].nama, sizeof(katalog[idx].nama), nama);
        pesan_hijau("Nama diubah.");
    } else if (p == 2) {
        int harga;
        if (!input_harga("Harga baru (Rp): ", harga)) return;
        katalog[idx].harga = harga;
        pesan_hijau("Harga diubah.");
    } else {
        string yn;
        string tanda = string("Hapus ") + katalog[idx].kode + " " + katalog[idx].nama + "? (y/n): ";
        while (true) {
            if (!tanya(tanda, yn, true)) return;
            yn = huruf_kecil(yn);
            if (yn == "y" || yn == "n") break;
            pesan_merah("  ! Jawab y atau n.");
        }
        if (yn == "y") {
            for (int i = idx; i < jumlah - 1; i++) katalog[i] = katalog[i + 1];
            jumlah--;
            pesan_hijau("Bibit dihapus.");
        } else {
            cout << "Dibatalkan." << endl;
        }
    }
}

// ====================== MENU 6: STOK ======================
void menu_stok() {
    cout << endl;
    string kode;
    if (!tanya("Kode bibit (blank to quit): ", kode)) return;
    kode = huruf_besar(kode);
    int idx = cari_kode(kode);
    if (idx < 0) { pesan_merah("Bibit dengan kode " + kode + " tidak ditemukan."); return; }

    Bibit& b = katalog[idx];
    cout << b.nama << ", stok saat ini: " << b.stok << endl;
    cout << "1. Tambah   2. Kurangi" << endl;
    int p = pilih("Pilihan (blank to quit): ", 1, 2);
    if (!p) return;

    string s;
    while (true) {
        if (!tanya("Jumlah (blank to quit): ", s)) return;
        long v;
        if (!parse_long(s, v) || v <= 0) {
            pesan_merah("  ! Jumlah harus bilangan bulat positif.");
            continue;
        }
        if (p == 2 && v > b.stok) {
            pesan_merah("Ditolak: stok hanya " + to_string(b.stok) +
                        ", tidak bisa dikurangi " + to_string(v) + ".");
            continue;
        }
        if (p == 1 && b.stok + v > STOK_MAX) {
            pesan_merah("Ditolak: stok melebihi batas " + to_string(STOK_MAX) + ".");
            continue;
        }
        b.stok = (short)(p == 1 ? b.stok + v : b.stok - v);
        pesan_hijau(string("Stok ") + b.nama + " sekarang " + to_string(b.stok) + ".");
        return;
    }
}

// ====================== MENU 7: STATISTIK ======================
void menu_statistik() {
    long total = 0, sum_harga = 0;
    long per_kat[JML_KAT] = {0};
    int i_max_h = 0, i_min_h = 0, i_max_s = 0, i_min_s = 0;

    for (int i = 0; i < jumlah; i++) {
        total += katalog[i].stok;
        sum_harga += katalog[i].harga;
        per_kat[katalog[i].kat] += katalog[i].stok;
        if (katalog[i].harga > katalog[i_max_h].harga) i_max_h = i;
        if (katalog[i].harga < katalog[i_min_h].harga) i_min_h = i;
        if (katalog[i].stok  > katalog[i_max_s].stok)  i_max_s = i;
        if (katalog[i].stok  < katalog[i_min_s].stok)  i_min_s = i;
    }
    double rata = (double)sum_harga / jumlah;

    cout << endl;
    cout << left << setw(19) << "Jumlah jenis bibit" << ": " << jumlah << endl;
    cout << left << setw(19) << "Total stok" << ": " << total << endl;
    cout << left << setw(19) << "Rata-rata harga" << ": ";
    if (fabs(rata - floor(rata)) < 1e-9) cout << (long long)rata << endl;
    else cout << fixed << setprecision(2) << rata << defaultfloat << endl;

    auto baris = [](const char* label, int nilai, const Bibit& b) {
        cout << left << setw(19) << label << ": " << setw(7) << nilai
             << " (" << b.kode << " " << b.nama << ")" << endl;
    };
    baris("Harga tertinggi", katalog[i_max_h].harga, katalog[i_max_h]);
    baris("Harga terendah",  katalog[i_min_h].harga, katalog[i_min_h]);
    baris("Stok terbanyak",  katalog[i_max_s].stok,  katalog[i_max_s]);
    baris("Stok tersedikit", katalog[i_min_s].stok,  katalog[i_min_s]);

    cout << endl << "Stok per kategori:" << endl;
    int setengah = (JML_KAT + 1) / 2;
    for (int i = 0; i < setengah; i++) {
        cout << left << setw(18) << NAMA_KAT[i] << right << setw(4) << per_kat[i] << "     ";
        int j = i + setengah;
        if (j < JML_KAT)
            cout << left << setw(19) << NAMA_KAT[j] << right << setw(4) << per_kat[j];
        cout << endl;
    }
    cout << left;
}

// ====================== MENU 8: INFO MEMORI ======================
void menu_memori() {
    cout << endl;
    cout << left << setw(24) << "sizeof(struct pertama)" << ": " << sizeof(BibitAwal) << " byte" << endl;
    cout << left << setw(24) << "sizeof(struct kedua)"   << ": " << sizeof(Bibit)     << " byte" << endl;
    cout << left << setw(24) << "Kapasitas array"        << ": " << KAPASITAS         << " elemen" << endl;
    cout << left << setw(24) << "Total memori array"     << ": " << sizeof(katalog)   << " byte" << endl;
    cout << left << setw(24) << ("Terpakai (" + to_string(jumlah) + " elemen)")
         << ": " << jumlah * sizeof(Bibit) << " byte" << endl;
    cout << endl;
    cout << left << setw(24) << "Sebelum optimasi" << ": " << KAPASITAS * sizeof(BibitAwal) << " byte" << endl;
    cout << left << setw(24) << "Sesudah optimasi" << ": " << KAPASITAS * sizeof(Bibit)     << " byte" << endl;
}

// ====================== MAIN ======================
int main() {
    muat_file();

    string s;
    while (true) {
        tampil_menu();
        if (!tanya("Pilih menu (blank to quit): ", s)) break;

        long m;
        if (!parse_long(s, m) || m < 1 || m > 8) {
            pesan_merah("Menu tidak valid. Pilih 1-8.");
            continue;
        }

        switch (m) {
            case 1: if (cek_ada_data()) menu_katalog();    break;
            case 2: menu_tambah();                          break;
            case 3: if (cek_ada_data()) menu_cari();        break;
            case 4: if (cek_ada_data()) menu_urut();        break;
            case 5: if (cek_ada_data()) menu_ubah_hapus();  break;
            case 6: if (cek_ada_data()) menu_stok();        break;
            case 7: if (cek_ada_data()) menu_statistik();   break;
            case 8: menu_memori();                          break;
        }
    }
    cout << "Terima kasih." << endl;
    return 0;
}
