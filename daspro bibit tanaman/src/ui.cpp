#include "ui.h"
#include "fileio.h"
#include <iostream>
#include <iomanip>
using namespace std;

void tampilkan_semua(const tanaman* data, int n, const string& judul) {
    int border = 60;

    cout << string(border, '=') << endl
    << string((border - judul.length() - 2)/2, ' ') << "(" << judul << ")" << endl
    << string(border, '=') << endl
    << "| "
    << left << setw(ID_LEN) << "ID" << " | "
    << left << setw(NAMA_MAX) << "Nama tanaman" << " | "
    << left << setw(10) << "Harga/KG" << " | "
    << left << setw(8) << "Stok/KG" << " |" << endl
    << string(border, '=') << endl;

    for (int i = 0; i < n; i++) {
        cout << "| "
        << left << setw(ID_LEN) << data[i].id << " | "
        << left << setw(NAMA_MAX) << data[i].nama << " | "
        << left << setw(10) << data[i].hrg << " | "
        << left << setw(8) << data[i].stk_kg << " |" << endl;
    }
    cout << string(border, '=') << endl;
}

void command(katalog& k, bool& run) {
    string input;

    cout << "1. Tambah bibit baru\n" << "2. Cari bibit\n" << "3. Urutkan katalog\n"
    << "4. Ubah/hapus bibit\n" << "5. Tambah/kurangi stok\n" << "6. Statistik\n"
    << "7. Info memori\n" << "8. Simpan dan keluar\n" << string(60, '=') << endl;

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
            tambah_bibit_baru(k);
            break;
        case '2':
            cari_bibit(k);
            break;
        case '3':
            urutkan_katalog(k);
            break;
        case '4':
            ubah_hapus_bibit(k);
            break;
        case '5':
            ubah_stok(k);
            break;
        case '6':
            statistik(k);
            break;
        case '7':
            info_memori(k);
            break;
        case '8':
            cout << "PERINGATAN: data di data/bibit.txt akan ditimpa saat disimpan. Lanjutkan? (y/n): ";
            getline(cin, input);
            if (input != "y" && input != "Y") {
                cout << "Penyimpanan dibatalkan" << endl;
                break;
            }
            if (simpan_data(k, "data/bibit.txt")) {
                cout << "Keluar dari program" << endl;
                run = false;
            }
            break;
        default:
            cout << "Invalid input. Please enter 1-8 or leave blank to quit.\n";
            break;
    }
}
