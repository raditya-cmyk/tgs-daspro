#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
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

void width(const vector<tanaman>& data, int& w_nama, int& w_hrg, int& w_stk_kg ) {

    for (const auto& m : data) {
        int i_w_hrg = int_w(m.hrg);
        ostringstream s_stk_kg;
        s_stk_kg << m.stk_kg;

        if (w_nama < int(m.nama.length())) w_nama = int(m.nama.length());
        if (w_hrg < i_w_hrg) w_hrg = i_w_hrg;
        if (w_stk_kg < int(s_stk_kg.str().length())) w_stk_kg = int(s_stk_kg.str().length());
    }
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

vector<tanaman> load_data(const string& path, const int& stk_kg_max) {
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
            getline(iss, str_stk_kg, ','))
        {
            int hrg;
            float stk_kg;
            try {
                hrg = stoi(str_hrg);
                stk_kg = stof(str_stk_kg);
            } catch (...) {
                cout << "Baris ke-" << i << " ditolak karna format angka salah" << endl;
                x++;
                continue;
            }

            if (hrg < 0 || stk_kg < 0 || stk_kg > stk_kg_max) {
                string invalid_val = (hrg < 0) ? str_hrg : str_stk_kg;
                string invalid = (hrg < 0) ? "harga" : "stok/kg";
                cout << "Baris ke-" << i << " " << invalid << " di luar rentang (" << invalid_val << ")" << endl;
                x++;
            } else if (!cek_id(id)) {
                cout << "Baris ke-" << i << " ditolak karna duplikat id" << endl;
                x++;
            } else {
                out.push_back({id, nama, hrg, stk_kg});
            }
        }
    }
    cout << "Berhasil memuat " << i-x << " dari " << i << " baris (" << x <<" ditolak)" << endl;
    return out;
}

void tampilkan_semua(const vector<tanaman>& data) {
    int w_nama = 1, w_hrg = 1, w_stk_kg = 1;
    width(data, w_nama, w_hrg, w_stk_kg);

    for (const auto& m : data) {
        cout << "| " << m.id << " | " 
        << left << setw(w_nama) << m.nama << " | " 
        << left << setw(w_hrg) << m.hrg << " | "
        << left << setw(w_stk_kg)<< m.stk_kg << " |" << endl;
    }
}

int main() {
    float stk_kg_max = 1000; 
    auto data = load_data("bibit.txt", stk_kg_max);
    tampilkan_semua(data);
    return 0;
}
