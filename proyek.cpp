#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>
using namespace std;

struct tanaman{
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

bool cek_id(const string& id) {
    static vector<string> seen;
    for (const string& id_seen : seen) {
        if (id_seen == id) {
            return false;
        }
    }
    seen.push_back(id);
    return true;
}

void width(const vector<tanaman>& data, 
    int& w_id, int& w_nama, int& w_hrg, int& w_stk_kg, 
    string& id_ktlg, string& nama_ktlg, string& hrg_ktlg, string& stk_kg_ktlg) {

    for (const auto& m : data) {
        int i_w_hrg = int_w(m.hrg);
        ostringstream s_stk_kg;
        s_stk_kg << m.stk_kg;

        if (w_nama < int(m.nama.length())) w_nama = int(m.nama.length());
        if (w_hrg < i_w_hrg) w_hrg = i_w_hrg;
        if (w_stk_kg < int(s_stk_kg.str().length())) w_stk_kg = int(s_stk_kg.str().length());
    }
    if (w_id < id_ktlg.length()) w_id = id_ktlg.length();
    if (w_nama < nama_ktlg.length()) w_nama = nama_ktlg.length(); 
    if (w_hrg < hrg_ktlg.length()) w_hrg = hrg_ktlg.length(); 
    if (w_stk_kg < stk_kg_ktlg.length()) w_stk_kg = stk_kg_ktlg.length(); 
}

vector<tanaman> load_data(const string& path, const float& stk_kg_max, const int& w_id) {
    vector<tanaman> out;
    ifstream file(path);
    if (!file) {
        cerr << "Tidak ada file di folder data (" << path << ")\n";
        return out;
    }

    string line;
    int i = 0;
    int x = 0;
    while (getline(file, line)) {
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

            if (hrg < 0 && (!isfinite(stk_kg) || stk_kg < 0 || stk_kg > stk_kg_max)) {
                cout << "Baris ke-" << i << " harga dan stk/kg di luar rentang (" << str_hrg << "," << str_stk_kg << ")" << endl;
                x++;
            }else if (hrg < 0 || !isfinite(stk_kg) || stk_kg < 0 || stk_kg > stk_kg_max) {
                string invalid_val = (hrg < 0) ? str_hrg : str_stk_kg;
                string invalid = (hrg < 0) ? "harga" : "stok/kg";
                cout << "Baris ke-" << i << " " << invalid << " di luar rentang (" << invalid_val << ")" << endl;
                x++;
            } else if (id.length() != w_id) {
                cout << "Baris ke-" << i << " ditolak karna ID tidak sesuai format" << endl;
                x++;
            } else if (!cek_id(id)) {
                cout << "Baris ke-" << i << " ditolak karna duplikat id" << endl;
                x++;
            } else {
                out.push_back({id, nama, hrg, stk_kg});
            }
        }else {
        cout << "Baris ke-" << i << " ditolak karena kolom tidak lengkap" << endl;
        x++;
        }
    }
    if (i == 0) {
        cout << "File kosong (" << path << ")" << endl;
        return out;
    } else if (i-x == 0)
    {
        cout << "Tidak ada data valid di file (" << path << ")" << endl;
        return out;
    }
    
    cout << "Berhasil memuat " << i-x << " dari " << i << " baris (" << x <<" ditolak)" << endl;
    return out;
}

void tampilkan_semua(const vector<tanaman>& data, int& w_id,
    string& id_ktlg, string& nama_ktlg, string& hrg_ktlg, string& stk_kg_ktlg, string& judul_ktlg) {

    int w_nama = 1, w_hrg = 1, w_stk_kg = 1;
    width(data, w_id, w_nama, w_hrg, w_stk_kg, id_ktlg, nama_ktlg, hrg_ktlg, stk_kg_ktlg);

    int border = 2 + w_id + 3 + w_nama + 3 + w_hrg + 3 + w_stk_kg + 2;
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

    for (const auto& m : data) {
        cout << "| " 
        << left << setw(w_id) << m.id << " | " 
        << left << setw(w_nama) << m.nama << " | " 
        << left << setw(w_hrg) << m.hrg << " | "
        << left << setw(w_stk_kg)<< m.stk_kg << " |" << endl;
    }
    cout << string(border, '=') << endl;
}

int main() {
    int w_id = 5;
    float stk_kg_max = 1000; 
    string header[5] = {"ID", "Nama tanaman", "Harga/KG", "Stok/KG", "Biji-bijian tanaman"};

    auto data = load_data("bibit.txt", stk_kg_max, w_id);
    if (data.empty()) return 1;

    tampilkan_semua(data, w_id, header[0], header[1], header[2], header[3], header[4]);
    return 0;
}
