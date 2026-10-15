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

int W_nama(const vector<tanaman>& data) {
    int width = 0;
    for (const auto& m : data) {
        if (width < int(m.nama.length())) width = int(m.nama.length());
    }
    return width;
}

int W_hrg(const vector<tanaman>& data) {
    int width = 0;
    for (const auto& m : data) {
        string s_hrg = to_string(m.hrg);
        if (width < int(s_hrg.length())) width = int(s_hrg.length());
    }
    return width;
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

vector<tanaman> load_data(const string& path) {
    vector<tanaman> out;
    ifstream file(path);
    if (!file) {
        cerr << "Tidak ada file di folder data: " << path << '\n';
        return out;
    }
    string line;
    int i = 1;
    while (getline(file, line)) {
        istringstream iss(line);
        if (line.empty()) {
            cout << "Baris kosong pada baris ke-" << i << endl;
            i++;
            continue;
        }

        string id, nama, hrg, stk_kg;
        if (getline(iss, id, ',') && 
            getline(iss, nama, ',') && 
            getline(iss, hrg, ',') && 
            getline(iss, stk_kg, ','))
        {
            if (cek_id(id)) {
            out.push_back({id, nama, stoi(hrg), stof(stk_kg) });
            } else {
                cout << "Baris ke-" << i << " ditolak karna duplikat id"<< endl;
            }
        } else {
            cout << "Baris ke-" << i << " ditolak karna format salah"<< endl;
        }
        i++;
    }
    return out;
}

void tampilkan_semua(const vector<tanaman>& data) {
    int w_hrg = W_hrg(data);
    int w_nama = W_nama(data);
    for (const auto& m : data) {
        cout << m.id << " | " 
        << left << setw(w_nama) << m.nama << " | " 
        << left << setw(w_hrg) << m.hrg << " | " 
        << m.stk_kg << endl;
    }
}

int main() {
    auto data = load_data("bibit.txt");
    tampilkan_semua(data);
    return 0;
}
